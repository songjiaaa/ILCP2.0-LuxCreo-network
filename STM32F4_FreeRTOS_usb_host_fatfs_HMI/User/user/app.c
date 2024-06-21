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
			set_screen(0);                       //��Ļ�ص�������
			switch_icon(0,6,3);	                 //U��ͼ�����
			steady_green_light();                //�̵Ƴ���
			cfg_dft.init_step = 1;	
		}
		else if(cfg_dft.init_step ==1)
		{
			mem_mount();                         //�ڴ����
			cfg_get();                           //�õ��ļ�����
			
			if(save_config.upgrade_state != 0)
			{
				save_config.upgrade_state = 0;   //���±�־��ʼ��Ϊ0
				cfg_save();
			}
			if(0 != my_rtc_init())               //RTC��ʼ��
			{
				pop_tips(30,(u8*)WARNING,(u8*)RTC_INIT_ERROR);    //��ʼ��ʧ����ʾ
			}
			if(save_config.set_server == 0)      //����������ѡ�� 0 ���� 1 ����
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
		else if(cfg_dft.init_step == 2)          //��̫��ģ���ʼ�� 
		{
			int i = 0;
			pop_tips(28,(u8*)INITIALIZE,(u8*)ETHERNET_INIT);   //��̫��ģ���ʼ������

			while(1)
			{
				if(0 == usr_k5_init())           //��ʼ���ɹ�
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
						pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_INIT_FAIL);    //��ʼ��ʧ����ʾ
						break;
					}
				}
			}
			enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
			cfg_dft.network_selet = 0;          //��̫������״̬����
			set_screen(0);
			cfg_dft.init_step = 3;          //ִ��WiFiģ���ʼ��
			memset(&wifi_flow,0x00,sizeof(wifi_flow));
		}
		else if(cfg_dft.init_step == 3)          //WiFiģ���ʼ�� 
		{
			int i = 0;
			pop_tips(28,(u8*)INITIALIZE,(u8*)WIFI_INIT);   //WiFi ģ���ʼ������
			while(1)
			{
				if( 0 == wifi_init_process() )      //wifi��ʼ��
				{
					wifi_flow.execution_step = 1;
					break;
				}
				else	
				{
					if(i++ < 3)
					{
						WIFI_PWR_EN = 0;            //WiFiģ�����
						vTaskDelay(3000);           //�ӳ�3��
						WIFI_PWR_EN = 1;
					}
					else
					{
						set_screen(0);
						pop_tips(30,(u8*)WARNING,(u8*)WIFI_INIT_ERROR);    //��ʼ��ʧ����ʾ    
						break;
					}
				}					
			}
			set_screen(0);
			cfg_dft.init_step = 4;
		}
		else if(cfg_dft.init_step == 4)        //��̫����WiFiģ������
		{
			ethernet_module_run();
			wifi_module_run();
			if(cfg_dft.network_selet == 1)
				switch_icon(0,5,2);		       //  0δ����  1wifi����  2��������	
			else
				switch_icon(0,5,wifi_flow.connect_state);  //wifi_flow.connect_state   0��WiFiδ���� 1��������	
		}
		else if(cfg_dft.init_step == 5)        //��������
		{
			if(cfg_dft.network_selet == 1)     //������������ͨ���������
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



//��̫�����ڽ���
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
	if(ui_id.cur_picture == 0)       //�����׽���
	{
		if(tick % 3 == 0)            //��ʱ3��һ��ѯ ����wifi ����״̬
		{
//			if(wifi_flow.init_state == 1)       //wiif�ѳ�ʼ��
//			{
//				if( 0 == esp8266_query_connected_ap(cur_connected_ap.ssid,cur_connected_ap.mac) ) //��ѯ�����ӵ�wifi����mac��ַ
//				{
//					wifi_app_user.connect_state = 1;
//					switch_icon(0,5,1);             //  0δ����  1wifi����  2��������
//				}
//				else
//				{
//					memset(&cur_connected_ap,0x00,sizeof(cur_connected_ap));  //��յ�ǰ����wifi����Ϣ
//					wifi_app_user.connect_state = 0;
//					switch_icon(0,5,0);             //������wifi����״̬��ʾ
//				}
//			}
		}
		if(home_tick++ >  60 * 30)	
		{
			set_screen(16);
		}	
//		if(cfg_dft.u_disk_state == 99)
//			switch_icon(0,6,1);       //��⵽U��
//		else
//			switch_icon(0,6,3);	      //δ��⵽U��	
	}
	else if(ui_id.cur_picture == 9)       //����ǰ����Ϊcontrol
	{
		u8 show_text[8] = {0};
		sprintf((char*)show_text,"%d",sensor_data.inside_t); 
		SetTextValue(9,5,show_text);      //��ʾ�ڲ��¶�
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
		if(cure.mode == 0 || cure.mode == 3)  //�����ڿ���
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


//��ȡ�����������Լ���ʾ����
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
		//��ѯ�����ջ���
		lcdstack_size = uxTaskGetStackHighWaterMark(lcd_data_pro_handler);
		wifistack_size = uxTaskGetStackHighWaterMark(wifi_data_pro_handler);
		usbstack_size = uxTaskGetStackHighWaterMark(usb_task_handler);
		runstack_size = uxTaskGetStackHighWaterMark(run_task_handler);
		getrunstack_size = uxTaskGetStackHighWaterMark(get_sensor_data_handler);
		softwarestack_size = uxTaskGetStackHighWaterMark(software_timer_handler);
		initstack_size = uxTaskGetStackHighWaterMark(init_task_handler);
		
		p_data_upload();   //�ϴ�����
        vTaskDelay(10);
    }
}


void door_task(void)
{
	if(cure.mode == 1)
	{
		if(DOOR_DET == 0)            //�Ź�
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





