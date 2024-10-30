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

//��������
void start_task(void *pvParameters)
{
	xTaskCreate( init_task, "init_task", 512, NULL, 9, &init_task_handler );	
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
	u8 pwr_tick = 0;
	RCC->AHB1ENR|=0x7ff;
	bsp_InitDWT();                //��ʼ��DWT������
	gpio_init();
	
	uart_initial(&uart1,115200);  //dbug
	uart_initial(&uart3,115200);  //RFID 2 
	uart_initial(&uart4,115200);  //�ⲿ�ӿ� MODBUSЭ��
	uart_initial(&uart6,115200);  //RFID 1
	
	hx711_io_init();
	ws2812_init();
//	ADC1_Config();                //ע���ͻ����
	
	cmd_ini(); 
	
//	vTaskDelay(3000);
	rfid_init();
	
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



u8 modbus_rx_data[MODBUS_RTU_MAX_ADU_LENGTH];
modbus_mapping_t *mb_mapping = NULL;
void modbus_pro_task(void * pvParameters)
{
	int rc;
	u8 *query;
	modbus_t *ctx = NULL;
//	modbus_mapping_t *mb_mapping = NULL;
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
	
    while(1)
    {
		do
		{
			rc = modbus_receive(ctx, query);
			/* Filtered queries return 0 */
		} while (rc == 0);
		
		if (rc == -1 || errno == EMBBADCRC)    //֡�����crc�����򲻻ظ�
		{
//			vTaskDelay(10);
			continue;
		}
//		mb_mapping->tab_bits++;
//		mb_mapping->tab_input_bits++;
//		mb_mapping->tab_input_registers[0] ++;
//		mb_mapping->tab_input_registers[1] ++;
//		mb_mapping->tab_registers[0]++;
		memcpy(modbus_rx_data, query, MODBUS_RTU_MAX_ADU_LENGTH);
		
		
		mb_mapping->tab_registers[BAND_REG] = 6754;
		mb_mapping->tab_registers[WEIGHT_REG] = m_data_t.resin_weight;
		
	
//		if (modbus_write_bit(ctx, 0x0002, reg_value))
//		{
//			mb_mapping->tab_registers[LIQUID_IN_SWITCH_REG];
//		}
		
		rc = modbus_reply(ctx,query,rc,mb_mapping);
		if (rc == -1) {
			break;
		}
//		if(mb_mapping->start_bits[0] == )
//		{
//			
//		}
		
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
	static u32 tick = 0;
	tick ++;
	rfid1_read_data(0,100);
}

//static void motor_runtime_callback(TimerHandle_t xTimer)
//{
//}


void software_timer_task( void * pvParameters )
{
	timer_1000ms = xTimerCreate("count_timer",1000,pdTRUE,NULL,read_rfid_callback);
	xTimerStart(timer_1000ms, 0);
//	timer_10ms = xTimerCreate("count_timer",10,pdTRUE,NULL,motor_runtime_callback);
//	xTimerStart(timer_10ms, 0);
    while(1)
    {
		LED_IND ^= 1;	
        vTaskDelay(1000);
		rfid1_read_data(0,100);
    }
	//xTimerDelete(timer, portMAX_DELAY);
}


//��ȡ�����������Լ���ʾ����
void get_sensor_data_task( void * pvParameters )
{
	
	while(1)
    {	
		m_data_t.resin_weight = (int)get_weight();
		MINMAX(m_data_t.resin_weight,0,0xFFFF);
		
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
	
        vTaskDelay(5);
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



#define RFID2_MAX_LEN     50
u8 rfid2_rec_buf[RFID2_MAX_LEN];
u32 rfid2_rec_p = 0;
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
				rfid2_rec_buf[rfid2_rec_p++] = tt;
				rfid2_rec_p %= RFID2_MAX_LEN;
			}
		}
		else
		{
			vTaskDelay(10);
		}
	}
	
}



