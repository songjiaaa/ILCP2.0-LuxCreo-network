#include "app.h"
#include "uart_device.h"
#include "modbus.h"
#include "errno.h"


RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

//USB_OTG_CORE_HANDLE USB_OTG_dev;
USBH_HOST USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;
SD_Error SD_state;


TaskHandle_t start_task_handler;
TaskHandle_t usb_task_handler;
TaskHandle_t software_timer_handler;
TaskHandle_t get_sensor_data_handler;
TaskHandle_t run_task_handler;
TaskHandle_t init_task_handler;
TaskHandle_t modbus_pro_task_handler;

//启动任务
void start_task(void *pvParameters)
{
//	xTaskCreate( run_task, "run_task", 512, NULL, 6, &run_task_handler );
//	xTaskCreate( get_sensor_data_task, "get_sensor_data_task", 256, NULL, 5, &get_sensor_data_handler );
	
	xTaskCreate( init_task, "init_task",512, NULL, 9, &init_task_handler );	
	xTaskCreate( software_timer_task, "software_timer_task", 256, NULL, 4, &software_timer_handler );
	xTaskCreate( modbus_pro_task, "modbus_pro_task", 256, NULL, 3, &modbus_pro_task_handler );

	
	vTaskDelete(start_task_handler);
}

int weight_data = 0;
//初始化任务
void init_task(void * pvParameters)
{
	u8 pwr_tick = 0;
	RCC->AHB1ENR|=0x7ff;
	bsp_InitDWT();                //初始化DWT计数器
	gpio_init();
	
	uart_initial(&uart1,115200);  //dbug
	uart_initial(&uart3,115200);  
	uart_initial(&uart4,115200);  //外部接口 MODBUS协议
	
	hx711_io_init();
	
//	ADC1_Config();                //注意冲突引脚
	
	cmd_ini(); 
	
	while(1)
	{
		
		if(!KEY_DET)       //按下
		{
			if(PWR_HOLD == 0)
			{
				if(pwr_tick++ > 50)
				{
					pwr_tick = 0;
					PWR_HOLD = 1;
				}
			}
			else 
			{
//				if(pwr_tick++ > 50)
//				{
//					pwr_tick = 0;
//					PWR_HOLD = 0;
//				}
			}
		}
//		weight_data = hx711_recv();
		vTaskDelay(10);
	}
}


//DAP串口接收
//void dap_data_pro_task(void *pvParameters)
//{
//	u8 tt;
//	u32 NotifyValue = 0;
//    while(1)
//    {
//		NotifyValue = ulTaskNotifyTake(pdTRUE,portMAX_DELAY); 
//		if(NotifyValue == 1)
//		{
//			while(get_que_data(&tt,&uart1.que_rx)==0)
//			{
////				rec_head(tt,&dap_pack);
//			}
//		}
//		else
//		{
//			vTaskDelay(10);
//		}
//	}
//}







void modbus_pro_task(void * pvParameters)
{
	int rc;
	u8 *query;
	modbus_t *ctx = NULL;
	modbus_mapping_t *mb_mapping = NULL;
	ctx = modbus_new_st_rtu("uart4", 115200, 'N', 8, 1);
	modbus_set_slave(ctx, 2);
	
	query = pvPortMalloc(MODBUS_RTU_MAX_ADU_LENGTH);
	
	mb_mapping = modbus_mapping_new_start_address(0,
												  10,
												  0,
												  10,
												  0,
												  10,
												  0,
												  10);
	
	rc = modbus_connect(ctx);
	if (rc == -1) 
	{
		//fprintf(stderr, "Unable to connect %s\n", modbus_strerror(errno));
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
		
		if (rc == -1 || errno == EMBBADCRC)    //帧错误或crc错误则不回复
		{
			vTaskDelay(10);
			continue;
		}
		mb_mapping->tab_bits++;
		mb_mapping->tab_input_bits++;
		mb_mapping->tab_input_registers[0] ++;
		mb_mapping->tab_input_registers[1] ++;
		mb_mapping->tab_registers[0]++;
		mb_mapping->tab_registers[1] = 6754;
		rc = modbus_reply(ctx, query, rc, mb_mapping);
		if (rc == -1) {
			break;
		}
	}
	modbus_mapping_free(mb_mapping);
	vPortFree(query);
	/* For RTU */
	modbus_close(ctx);
	modbus_free(ctx);

	vTaskDelete(NULL);
}


void usb_disk_task( void * pvParameters )
{
//	USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);	
//	vTaskDelay(500);
    while(1)
    {
//		USBH_Process(&USB_OTG_Core, &USB_Host);
        vTaskDelay(20);
    }
}

u32 home_tick = 0;
TimerHandle_t  timer_1000ms;
TimerHandle_t  timer_10ms;
static void uv_total_time_callback(TimerHandle_t xTimer)
{
	static u32 tick = 0;
	tick ++;
	
	if(UV_POWER_PER1 > 10 || UV_POWER_PER2 > 10)
		save_time.uv_total_time ++;	
	

}

static void motor_runtime_callback(TimerHandle_t xTimer)
{


}

float weight_d = 0;
void software_timer_task( void * pvParameters )
{
	timer_1000ms = xTimerCreate("count_timer",1000,pdTRUE,NULL,uv_total_time_callback);
	xTimerStart(timer_1000ms, 0);
	timer_10ms = xTimerCreate("count_timer",10,pdTRUE,NULL,motor_runtime_callback);
	xTimerStart(timer_10ms, 0);
    while(1)
    {
//		uart_send((u8*)"test_ch340aaaaa\r\n",sizeof("test_ch340aaaaa\r\n"),&uart4);
		weight_data = hx711_recv();
		weight_d = weight_data * 3.3f / 16777216.0f;
		LED_IND ^= 1;	
        vTaskDelay(500);
    }
	//xTimerDelete(timer, portMAX_DELAY);
}


//获取传感器数据以及显示任务
void get_sensor_data_task( void * pvParameters )
{
	while(1)
    {
//		RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
//		RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);

        vTaskDelay(9);
    }
}


UBaseType_t usbstack_size = 0;
UBaseType_t runstack_size = 0;
UBaseType_t getrunstack_size = 0;
UBaseType_t softwarestack_size = 0;
UBaseType_t initstack_size = 0;
void run_task( void * pvParameters )
{
//	LOG_COLOUR_GREEN;
//	vTaskDelay(1000);
//	LOG_COLOUR_BLACK;
//	vTaskDelay(800);
//	LOG_COLOUR_GREEN;	
    while(1)
    {
		//查询任务堆栈情况
	
		usbstack_size = uxTaskGetStackHighWaterMark(usb_task_handler);
		runstack_size = uxTaskGetStackHighWaterMark(run_task_handler);
		getrunstack_size = uxTaskGetStackHighWaterMark(get_sensor_data_handler);
		softwarestack_size = uxTaskGetStackHighWaterMark(software_timer_handler);
		initstack_size = uxTaskGetStackHighWaterMark(init_task_handler);
		

        vTaskDelay(5);
    }
}


void door_task(void)
{

}









