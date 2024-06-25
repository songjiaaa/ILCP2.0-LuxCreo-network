#include "app.h"

RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

data_collection sensor_data;

//USB_OTG_CORE_HANDLE USB_OTG_dev;
USBH_HOST USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;
SD_Error SD_state;


TaskHandle_t usb_task_handler;
TaskHandle_t dap_data_pro_handler;
TaskHandle_t enthernet_data_pro_handler;
TaskHandle_t wifi_data_pro_handler; 
TaskHandle_t lcd_data_pro_handler;    
TaskHandle_t software_timer_handler;
TaskHandle_t get_sensor_data_handler;
TaskHandle_t run_task_handler;
TaskHandle_t init_task_handler;



void init_task(void * pvParameters)
{
	limit_sensor_init();
	scraper_motor_init();
	platform_motor_init();
	
//	LOG_COLOUR_GREEN; 
	while(1)
	{
		vTaskDelay(10);
	}
}


//DAP串口接收
void dap_data_pro_task(void *pvParameters)
{
	u8 tt;
	u32 NotifyValue = 0;
    while(1)
    {
		NotifyValue = ulTaskNotifyTake(pdTRUE,portMAX_DELAY); 
		if(NotifyValue == 1)
		{
			while(get_que_data(&tt,&uart1.que_rx)==0)
			{
				rec_head(tt,&dap_pack);
			}
		}
		else
		{
			vTaskDelay(10);
		}
	}
}



//以太网串口接收
void enthernet_data_pro_task(void *pvParameters)
{
	u8 tt;
	u32 NotifyValue = 0;
    while(1)
    {
		NotifyValue = ulTaskNotifyTake(pdTRUE,portMAX_DELAY); 
		if(NotifyValue == 1)
		{
			while(get_que_data(&tt,&uart2.que_rx)==0)
			{
//				uart_send(&tt,1,&uart1);
				ethernet_data_pack.rec_buff[ethernet_data_pack.rec_p++] = tt;
				ethernet_data_pack.rec_p %= ethernet_data_pack.buf_len;
			}
		}
		else
		{
			vTaskDelay(10);
		}
	}
}


void wifi_data_pro_task(void *pvParameters)
{
	u8 tt;
	u32 NotifyValue = 0;
    while(1)
    {
		NotifyValue = ulTaskNotifyTake(pdTRUE,portMAX_DELAY); 
		if(NotifyValue == 1)
		{
			while(get_que_data(&tt,&uart3.que_rx)==0)
			{
				wifi_data_pack.rec_buff[wifi_data_pack.rec_p++] = tt;
				wifi_data_pack.rec_p %= wifi_data_pack.buf_len;
			}
		}
		else
		{
			vTaskDelay(10);
		}
	}
}

void lcd_data_pro_task(void * pvParameters)
{
	u8 tt;
	u32 NotifyValue = 0;
    while(1)
    {
		NotifyValue = ulTaskNotifyTake(pdTRUE,portMAX_DELAY); 
		if(NotifyValue == 1)
		{
			while(get_que_data(&tt,&uart6.que_rx)==0)
			{
				rec_sync(tt,&button_ctrl_pack);				
			}
		}
		else
		{
			vTaskDelay(10);
		}
	}
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
	if(ui_id.cur_picture == 0)       //若在首界面
	{
		if(tick % 3 == 0)            //定时3秒一查询 更新wifi 连接状态
		{
//			if(wifi_flow.init_state == 1)       //wiif已初始化
//			{
//				if( 0 == esp8266_query_connected_ap(cur_connected_ap.ssid,cur_connected_ap.mac) ) //查询已连接的wifi名和mac地址
//				{
//					wifi_app_user.connect_state = 1;
//					switch_icon(0,5,1);             //  0未连接  1wifi连接  2网线连接
//				}
//				else
//				{
//					memset(&cur_connected_ap,0x00,sizeof(cur_connected_ap));  //清空当前连接wifi的信息
//					wifi_app_user.connect_state = 0;
//					switch_icon(0,5,0);             //主界面wifi连接状态显示
//				}
//			}
		}
		if(home_tick++ >  60 * 30)	
		{
			set_screen(16);
		}	
//		if(cfg_dft.u_disk_state == 99)
//			switch_icon(0,6,1);       //检测到U盘
//		else
//			switch_icon(0,6,3);	      //未检测到U盘	
	}

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

UBaseType_t lcdstack_size = 0;
UBaseType_t wifistack_size = 0;
UBaseType_t usbstack_size = 0;
UBaseType_t runstack_size = 0;
UBaseType_t getrunstack_size = 0;
UBaseType_t softwarestack_size = 0;
UBaseType_t initstack_size = 0;
void run_task( void * pvParameters )
{
	LOG_COLOUR_GREEN;
	vTaskDelay(1000);
	LOG_COLOUR_BLACK;
	vTaskDelay(800);
	LOG_COLOUR_GREEN;	
    while(1)
    {
		//查询任务堆栈情况
		lcdstack_size = uxTaskGetStackHighWaterMark(lcd_data_pro_handler);
		wifistack_size = uxTaskGetStackHighWaterMark(wifi_data_pro_handler);
		usbstack_size = uxTaskGetStackHighWaterMark(usb_task_handler);
		runstack_size = uxTaskGetStackHighWaterMark(run_task_handler);
		getrunstack_size = uxTaskGetStackHighWaterMark(get_sensor_data_handler);
		softwarestack_size = uxTaskGetStackHighWaterMark(software_timer_handler);
		initstack_size = uxTaskGetStackHighWaterMark(init_task_handler);
		automatic_shoveling_pro();
//		p_data_upload();   //上传数据
        vTaskDelay(10);
    }
}


void door_task(void)
{

}




int get_que_data(u8 *p,Queue *q)
{
	int rst=1;
//	OS_CLOSE_INT;
	taskENTER_CRITICAL();
	rst=Queue_get_1(p,q);
//	OS_OPEN_INT;
	taskEXIT_CRITICAL();
	return rst;
}





