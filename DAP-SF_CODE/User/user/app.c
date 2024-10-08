#include "app.h"

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


//启动任务
void start_task(void *pvParameters)
{

//	xTaskCreate( run_task, "run_task", 512, NULL, 6, &run_task_handler );
//	xTaskCreate( get_sensor_data_task, "get_sensor_data_task", 256, NULL, 5, &get_sensor_data_handler );
	xTaskCreate( software_timer_task, "software_timer_task", 256, NULL, 4, &software_timer_handler );
	xTaskCreate( init_task, "init_task",512, NULL, 2, &init_task_handler );	
	
	vTaskDelete(start_task_handler);
}


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



//以太网串口接收
//void enthernet_data_pro_task(void *pvParameters)
//{
//	u8 tt;
//	u32 NotifyValue = 0;
//    while(1)
//    {
//		NotifyValue = ulTaskNotifyTake(pdTRUE,portMAX_DELAY); 
//		if(NotifyValue == 1)
//		{
//			while(get_que_data(&tt,&uart2.que_rx)==0)
//			{
////				uart_send(&tt,1,&uart1);
////				ethernet_data_pack.rec_buff[ethernet_data_pack.rec_p++] = tt;
////				ethernet_data_pack.rec_p %= ethernet_data_pack.buf_len;
//			}
//		}
//		else
//		{
//			vTaskDelay(10);
//		}
//	}
//}


//void wifi_data_pro_task(void *pvParameters)
//{
//	u8 tt;
//	u32 NotifyValue = 0;
//    while(1)
//    {
//		NotifyValue = ulTaskNotifyTake(pdTRUE,portMAX_DELAY); 
//		if(NotifyValue == 1)
//		{
//			while(get_que_data(&tt,&uart3.que_rx)==0)
//			{
////				wifi_data_pack.rec_buff[wifi_data_pack.rec_p++] = tt;
////				wifi_data_pack.rec_p %= wifi_data_pack.buf_len;
//			}
//		}
//		else
//		{
//			vTaskDelay(10);
//		}
//	}
//}

//void lcd_data_pro_task(void * pvParameters)
//{
//	u8 tt;
//	u32 NotifyValue = 0;
//    while(1)
//    {
//		NotifyValue = ulTaskNotifyTake(pdTRUE,portMAX_DELAY); 
//		if(NotifyValue == 1)
//		{
//			while(get_que_data(&tt,&uart6.que_rx)==0)
//			{
////				rec_sync(tt,&button_ctrl_pack);				
//			}
//		}
//		else
//		{
//			vTaskDelay(10);
//		}
//	}
//}


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

void software_timer_task( void * pvParameters )
{
	timer_1000ms = xTimerCreate("count_timer",1000,pdTRUE,NULL,uv_total_time_callback);
	xTimerStart(timer_1000ms, 0);
	timer_10ms = xTimerCreate("count_timer",10,pdTRUE,NULL,motor_runtime_callback);
	xTimerStart(timer_10ms, 0);
    while(1)
    {
		uart_send((u8*)"test_ch340aaaaa\r\n",sizeof("test_ch340aaaaa\r\n"),&uart4);
		
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




int get_que_data(u8 *p,Queue *q)
{
	int rst=1;

	taskENTER_CRITICAL();
	rst=Queue_get_1(p,q);
	taskEXIT_CRITICAL();
	return rst;
}





