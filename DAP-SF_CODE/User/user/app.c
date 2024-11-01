#include "app.h"
#include "uart_device.h"
#include "modbus.h"
#include "errno.h"

USBH_HOST USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;
SD_Error SD_state;


TaskHandle_t debug_task_handler;
TaskHandle_t rfid1_task_handler;
TaskHandle_t rfid2_task_handler;
TaskHandle_t modbus_pro_task_handler;

TaskHandle_t start_task_handler;
TaskHandle_t software_timer_handler;
TaskHandle_t get_sensor_data_handler;
TaskHandle_t run_task_handler;
TaskHandle_t init_task_handler;


modbus_data_t m_data_t = {0};

void get_unique_id(uint32_t *id) 
{
    id[0] = *(uint32_t *)(0x1FFFF7AC); // ��ȡUID�ĵ�1-3����
    id[1] = *(uint32_t *)(0x1FFFF7B0);
    id[2] = *(uint32_t *)(0x1FFFF7B4);
}

//��������
void start_task(void *pvParameters)
{
	xTaskCreate( init_task, "init_task", 512, NULL, 9, &init_task_handler );	
	xTaskCreate( run_task, "run_task", 512, NULL, 8, &run_task_handler );		
	xTaskCreate( rfid1_task, "rfid1_task", 512, NULL, 7, &rfid1_task_handler);
	xTaskCreate( rfid2_task, "rfid2_task", 512, NULL, 6, &rfid2_task_handler);
	xTaskCreate( get_sensor_data_task, "get_sensor_data_task", 256, NULL, 5, &get_sensor_data_handler );
	xTaskCreate( software_timer_task, "software_timer_task", 256, NULL, 4, &software_timer_handler );
	xTaskCreate( modbus_pro_task, "modbus_pro_task", 256, NULL, 3, &modbus_pro_task_handler );

	vTaskDelete(start_task_handler);
}


//��ʼ������
void init_task(void * pvParameters)
{
//	u8 pwr_tick = 0;
	RCC->AHB1ENR|=0x7ff;
	bsp_InitDWT();                //��ʼ��DWT������
	gpio_init();
	
	uart_initial(&uart1,115200);  //dbug
	uart_initial(&uart3,115200);  //RFID 2 
	uart_initial(&uart4,115200);  //�ⲿ�ӿ� MODBUSЭ��
	uart_initial(&uart6,115200);  //RFID 1
	
	hx711_io_init();
	ws2812_init();
	
	get_unique_id((u32*)save_config.sn);
	
	cfg_get();                  //��ȡ��������
//	ADC1_Config();                //ע���ͻ����
	pump_motor_init();
	rfid_init();
	cmd_ini(); 

	
	while(1)
	{	
//		if(!KEY_DET)       //����
//		{
//			if(pwr_tick++ > 50)
//			{
////				rfid_init();
//			}
			
//			if(PWR_HOLD == 0)
//			{
//				if(pwr_tick++ > 50)
//				{
//					pwr_tick = 0;
//					PWR_HOLD = 1;
//				}
//			}
//			else 
//			{
////				if(pwr_tick++ > 50)
////				{
////					pwr_tick = 0;
////					PWR_HOLD = 0;
////				}
//			}
//		}
		led_single_show(yellow,RGB_LED_NUM);  
		vTaskDelay(10);
	}
}



//u8 modbus_rx_data[MODBUS_RTU_MAX_ADU_LENGTH];
//modbus_mapping_t *mb_mapping = NULL;
void modbus_pro_task(void * pvParameters)
{
	int rc;
	u8 *query;
	modbus_t *ctx = NULL;
	modbus_mapping_t *mb_mapping = NULL;
	ctx = modbus_new_st_rtu("uart4", 115200, 'N', 8, 1);
	modbus_set_slave(ctx, 2);
	
	query = pvPortMalloc(MODBUS_RTU_MAX_ADU_LENGTH);
	
	mb_mapping = modbus_mapping_new_start_address(0,          //��Ȧ�Ĵ���
												  0,          //��Ȧ����
												  0,          //��ɢ����Ĵ���
												  0,          //�Ĵ�������
												  REG_START_ADDR,          //����Ĵ���    03 �� 06д 10 ��д 
												  1000,
												  0,          
												  0);
	
	rc = modbus_connect(ctx);
	if (rc == -1) 
	{
		modbus_free(ctx);
		vTaskDelete(NULL);
	}
//	memcpy(m_data_t.program_version,cfg_dft.version,sizeof(cfg_dft.version));
	for(int i = 0; i< sizeof(m_data_t.program_version)/2; i++)             //��С��ת��
	{
		m_data_t.program_version[i] = CHANGE_END16( ((u16*)cfg_dft.version)[i] );
	}
	
    while(1)
    {
		do
		{
			rc = modbus_receive(ctx, query);
			/* Filtered queries return 0 */
		} while (rc == 0);
		
		if (rc == -1 || errno == EMBBADCRC)    //֡�����crc�����򲻻ظ�
		{
			continue;
		}
//		memcpy(modbus_rx_data, query, MODBUS_RTU_MAX_ADU_LENGTH);
		
		
		memcpy((u8*)&mb_mapping->tab_registers[BAND_REG],m_data_t.resin_band,sizeof(m_data_t.resin_band));  //����Ʒ����
		memcpy((u8*)&mb_mapping->tab_registers[RESIN_REG],m_data_t.resin_name,sizeof(m_data_t.resin_name)); //������֬��
		
		memcpy((u8*)&mb_mapping->tab_registers[LEAP_NAME_REG],m_data_t.leap_name,sizeof(m_data_t.leap_name)); //����Ĥ����
		
		memcpy((u8*)&mb_mapping->tab_registers[GET_SN_REG],m_data_t.sn,sizeof(m_data_t.sn));     //�豸SN
		memcpy( (u8*)&mb_mapping->tab_registers[PROGRAM_VERSION_REG],m_data_t.program_version,sizeof(m_data_t.program_version));  //�汾��Ϣ
		
		mb_mapping->tab_registers[WEIGHT_REG] = m_data_t.resin_weight;         //��֬����
		
		mb_mapping->tab_registers[IO_INPUT_STATE_REG] = *(u16*)&m_data_t.io_input_state;     //IO����״̬
		mb_mapping->tab_registers[IO_OUT_STATE_REG]	= *(u16*)&m_data_t.io_out_state;        //IO���״̬

		rc = modbus_reply(ctx,query,rc,mb_mapping);
		if (rc == -1) {
			break;
		}
		
		m_data_t.ip_camera_power_on = mb_mapping->tab_registers[IP_CAMERA_POWER_REG];
		m_data_t.liquid_in_on = mb_mapping->tab_registers[LIQUID_IN_SWITCH_REG];
		
		m_data_t.weight_zero = mb_mapping->tab_registers[WEIGHT_ZERO_REG];       //���ع���ָ��

		
	}
	modbus_mapping_free(mb_mapping);
	vPortFree(query);
	modbus_close(ctx);
	modbus_free(ctx);

	vTaskDelete(NULL);
}





TimerHandle_t  timer_1000ms;
TimerHandle_t  timer_10ms;
static void read_rfid_callback(TimerHandle_t xTimer)
{
//	static u32 tick = 0;
//	tick ++;
//	rfid1_read_data(0,100);
}

//static void motor_runtime_callback(TimerHandle_t xTimer)
//{
//}

u8 test_flag = 0;
void software_timer_task( void * pvParameters )
{
	timer_1000ms = xTimerCreate("count_timer",1000,pdTRUE,NULL,read_rfid_callback);
	xTimerStart(timer_1000ms, 0);
//	timer_10ms = xTimerCreate("count_timer",10,pdTRUE,NULL,motor_runtime_callback);
//	xTimerStart(timer_10ms, 0);
    while(1)
    {
		LED_IND ^= 1;	
        vTaskDelay(500);
		if(test_flag == 1)
		{
			test_flag = 0;
			rfid1_read_data(0,64);
		}
    }
	//xTimerDelete(timer, portMAX_DELAY);
}


//��ȡ�����������Լ���ʾ����
void get_sensor_data_task( void * pvParameters )
{
	u32 tick = 0;
	while(1)
    {	
		if(tick++ % 10 == 1)
		{
			m_data_t.resin_weight = (int)get_weight() + save_config.weight_offset;    //10Hz��ȡ
			if(m_data_t.weight_zero == 1)   //ִ�г��ع��㹦��
			{
				m_data_t.weight_zero = 0;
				save_config.weight_offset = 0 - m_data_t.resin_weight;
				cfg_save();
			}
			
			MINMAX(m_data_t.resin_weight,0,0xFFFF);
		}
		
        vTaskDelay(10);
    }
}



//UBaseType_t runstack_size = 0;
//UBaseType_t getrunstack_size = 0;
//UBaseType_t softwarestack_size = 0;
//UBaseType_t initstack_size = 0;
void run_task( void * pvParameters )
{
    while(1)
    {
		//��ѯ�����ջ���
//		runstack_size = uxTaskGetStackHighWaterMark(run_task_handler);
//		getrunstack_size = uxTaskGetStackHighWaterMark(get_sensor_data_handler);
//		softwarestack_size = uxTaskGetStackHighWaterMark(software_timer_handler);
//		initstack_size = uxTaskGetStackHighWaterMark(init_task_handler);
		
		//IO���
		if(m_data_t.ip_camera_power_on == 1)
			CAM12V_PWR_EN = ON;
		else
			CAM12V_PWR_EN = OFF;
		m_data_t.io_out_state.ip_camera_power = CAM12V_PWR_EN;
		
		if(m_data_t.liquid_in_on == 1)
			liquid_injection();
		else
		{
			pump_motor_dis();
		}
		LED_G = ~VK36_OUT1;
		LED_R = ~VK36_OUT0;
		//IO����
		m_data_t.io_input_state.material = m_data_t.io_input_state.material_rfid_stete;
		m_data_t.io_input_state.bucket = m_data_t.io_input_state.bucket_rfid_stete;
		m_data_t.io_input_state.liquid_max = VK36_OUT0;
		m_data_t.io_input_state.liquid_overflow = VK36_OUT1;
		if(m_data_t.resin_weight > 50)
			m_data_t.io_input_state.surplus_material = 1;
		else
			m_data_t.io_input_state.surplus_material = 0;
		m_data_t.io_input_state.empty_cantilever = ~GL_8F_OUT;
		m_data_t.io_input_state.reserved = 0;
		
		
		
        vTaskDelay(10);
    }
}


//DUG����
void debug_task(void *pvParameters)
{
	u8 tt;
	u32 notify_value = 0;
	while(1)
	{
		notify_value = ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		if( notify_value == 1 )
		{
			while( get_que_data(&tt,&uart1.que_rx) == 0 )
			{
				
			}
		}
		else
		{
			vTaskDelay(10);
		}
	}
	
}


//RFID1 
void rfid1_task(void *pvParameters)
{
	u8 tt;
	u32 notify_value = 0;
	while(1)
	{
		notify_value = ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		if( notify_value == 1 )
		{
			while( get_que_data(&tt,&uart6.que_rx) == 0 )   //uart6
			{
				rec_head(tt,&rfid1_pack);
			}
		}
		else
		{
			vTaskDelay(10);
		}
	}
	
}




//rfid2
void rfid2_task(void *pvParameters)
{
	u8 tt;
	u32 notify_value = 0;
	while(1)
	{
		notify_value = ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		if( notify_value == 1 )
		{
			while( get_que_data(&tt,&uart3.que_rx) == 0 )
			{
				rec_head(tt,&rfid2_pack);
			}
		}
		else
		{
			vTaskDelay(10);
		}
	}
	
}



