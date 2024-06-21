#include "app.h"

RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

data_collection sensor_data;

//USB_OTG_CORE_HANDLE USB_OTG_dev;
USBH_HOST USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;
SD_Error SD_state;

TaskHandle_t usb_task_handler;
TaskHandle_t enthernet_data_pro_handler;
TaskHandle_t wifi_data_pro_handler; 
TaskHandle_t lcd_data_pro_handler;    
TaskHandle_t software_timer_handler;
TaskHandle_t get_sensor_data_handler;
TaskHandle_t run_task_handler;
TaskHandle_t init_task_handler;


void init_task(void * pvParameters)
{
	while(1)
	{
		if(cfg_dft.init_step == 0)
		{
			set_screen(0);                       //屏幕回到主界面
			switch_icon(0,6,3);	                 //U盘图标清除
			steady_green_light();                //绿灯常亮
			cfg_dft.init_step = 1;	
		}
		else if(cfg_dft.init_step ==1)
		{
			mem_mount();                         //内存挂载
			cfg_get();                           //得到文件数据
			
			if(save_config.upgrade_state != 0)
			{
				save_config.upgrade_state = 0;   //更新标志初始化为0
				cfg_save();
			}
			if(0 != my_rtc_init())               //RTC初始化
			{
				pop_tips(30,(u8*)WARNING,(u8*)RTC_INIT_ERROR);    //初始化失败提示
			}
			if(save_config.set_server == 0)      //服务器域名选择 0 国内 1 国外
			{
				cfg_dft.ali_domain_name = OTA_DOMESTIC_DNS;
				cfg_dft.lux_domain_name = LUX_SERVER_URL_CN;
			}
			else
			{
				cfg_dft.ali_domain_name = OTA_FOREIGN_DNS;
				cfg_dft.lux_domain_name = LUX_SERVER_URL_COM;
			}
			cfg_dft.init_step = 2;		
		}
		else if(cfg_dft.init_step == 2)          //以太网模块初始化 
		{
			int i = 0;
			pop_tips(28,(u8*)INITIALIZE,(u8*)ETHERNET_INIT);   //以太网模块初始化弹框

			while(1)
			{
				if(0 == usr_k5_init())           //初始化成功
				{
					break;
				}
				else
				{
					if(i++ < 3)
					{
						usr_exit_at_mode();
						ethernet_h_reset();
						vTaskDelay(2000);   
					}
					else
					{
						set_screen(0);
						pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_INIT_FAIL);    //初始化失败提示
						break;
					}
				}
			}
			enthernet_flow.execution_step = 1;  //重新连接上传的服务器
			cfg_dft.network_selet = 0;          //以太网连接状态重置
			set_screen(0);
			cfg_dft.init_step = 3;          //执行WiFi模块初始化
			memset(&wifi_flow,0x00,sizeof(wifi_flow));
		}
		else if(cfg_dft.init_step == 3)          //WiFi模块初始化 
		{
			int i = 0;
			pop_tips(28,(u8*)INITIALIZE,(u8*)WIFI_INIT);   //WiFi 模块初始化弹框
			while(1)
			{
				if( 0 == wifi_init_process() )      //wifi初始化
				{
					wifi_flow.execution_step = 1;
					break;
				}
				else	
				{
					if(i++ < 3)
					{
						WIFI_PWR_EN = 0;            //WiFi模块掉电
						vTaskDelay(3000);           //延迟3秒
						WIFI_PWR_EN = 1;
					}
					else
					{
						set_screen(0);
						pop_tips(30,(u8*)WARNING,(u8*)WIFI_INIT_ERROR);    //初始化失败提示    
						break;
					}
				}					
			}
			set_screen(0);
			cfg_dft.init_step = 4;
		}
		else if(cfg_dft.init_step == 4)        //以太网和WiFi模块运行
		{
			ethernet_module_run();
			wifi_module_run();
			if(cfg_dft.network_selet == 1)
				switch_icon(0,5,2);		       //  0未连接  1wifi连接  2网线连接	
			else
				switch_icon(0,5,wifi_flow.connect_state);  //wifi_flow.connect_state   0：WiFi未联网 1：已联网	
		}
		else if(cfg_dft.init_step == 5)        //更新升级
		{
			if(cfg_dft.network_selet == 1)     //若网络连接则通过网络更新
			{
				eth_update_process();
			}
			else    
			{
				ota_run_process();			
			}				
		}
		vTaskDelay(10);
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
	USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);	
	vTaskDelay(500);
    while(1)
    {
		USBH_Process(&USB_OTG_Core, &USB_Host);
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
	else if(ui_id.cur_picture == 9)       //若当前界面为control
	{
		u8 show_text[8] = {0};
		sprintf((char*)show_text,"%d",sensor_data.inside_t); 
		SetTextValue(9,5,show_text);      //显示内部温度
	}
}

static void motor_runtime_callback(TimerHandle_t xTimer)
{
	static u32 motor_tick = 0;
	if(cfg_dft.m_ctrl == 1)
	{
		motor_ctrl_on(2); 
		if(motor_tick++ > 350)   //100hz   
		{
			cfg_dft.m_ctrl = 0;
			motor_tick = 0;
			switch_icon(11,5,cfg_dft.m_ctrl);
			motor_ctrl_on(0); 
		}
	}
	else
	{
		if(cure.mode == 0 || cure.mode == 3)  //若处于空闲
		{
			motor_ctrl_on(0); 
		}
		motor_tick = 0;
	}
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
		sensor_data.inside_t = get_temperature(3);
		sensor_data.ptc_t = get_temperature(0);
		sensor_data.uv_led1_t = get_temperature(2);
		sensor_data.uv_led2_t = get_temperature(4);
		sensor_data.ac_v = get_io_voltage();
//		htu_measure(&data_c.pcb_t,&data_c.pcb_h);	
		rgb_light_stat(rgb_light_state);
		door_task();
		
		RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
		RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);

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
	vTaskDelay(1000);
    while(1)
    {
		cure_state_process();
		//查询任务堆栈情况
		lcdstack_size = uxTaskGetStackHighWaterMark(lcd_data_pro_handler);
		wifistack_size = uxTaskGetStackHighWaterMark(wifi_data_pro_handler);
		usbstack_size = uxTaskGetStackHighWaterMark(usb_task_handler);
		runstack_size = uxTaskGetStackHighWaterMark(run_task_handler);
		getrunstack_size = uxTaskGetStackHighWaterMark(get_sensor_data_handler);
		softwarestack_size = uxTaskGetStackHighWaterMark(software_timer_handler);
		initstack_size = uxTaskGetStackHighWaterMark(init_task_handler);
		
		p_data_upload();   //上传数据
        vTaskDelay(10);
    }
}


void door_task(void)
{
	if(cure.mode == 1)
	{
		if(DOOR_DET == 0)            //门关
			MAC_DOOR_LOCK = 1;
	}
	else
	{
		if( UV_POWER_PER1 ==  0 && UV_POWER_PER2 == 0 )
			MAC_DOOR_LOCK = 0;
		else 
			MAC_DOOR_LOCK = 1;
	}
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





