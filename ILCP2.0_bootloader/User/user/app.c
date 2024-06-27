#include "app.h"


data_collection data_c;

//USB_OTG_CORE_HANDLE USB_OTG_dev;
USBH_HOST USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;
SD_Error SD_state;

char* ui_upgrade_file_path = NULL;
char* mcu_upgrade_file_path = NULL;

u8 u_disk_run = 0;
u32 app_tick=0;
void app_poll(void)
{
	app_tick++;
	if(app_tick % 10 == 0)
	{
	}
	if(app_tick % 2 == 1)
	{
		if(updata_state.update_status_flow == 0 && u_disk_run == 0)  //updata_state.update_status_flow
			USBH_Process(&USB_OTG_Core, &USB_Host);       //当读写文件依然轮训调用此函数时，会导致读写异常
	}
	if(app_tick % 10 == 2)
	{
		updating_task(&updata_state);  //更新流程
	}
	if(app_tick % 10 == 3)
	{
		boot_process();
	}
	if(app_tick % 20 == 4)
	{
		LED_IND ^= 1;
	}
}

//启动app程序
void start_app(void)
{
//	DSTATUS result = 0;
//	FILINFO check_fno = {0};

	save_config.upgrade_state = 0;  //更新状态清空
	if( 0 != cfg_save() )
	{
		//内存错误，无法升级
		set_screen(20);
		pop_tips(30,(u8*)"Error",(u8*)"Error: save error!!! ");
	}
	
//	result = f_stat(MEM_MCU_CHECK_FILE_PATH,&check_fno);    
//	if(result == FR_OK)
//	{
//		f_unlink(MEM_MCU_CHECK_FILE_PATH); 
//	}
//	result = f_stat(MEM_UI_CHECK_FILE_PATH,&check_fno);    
//	if(result == FR_OK)
//	{
//		f_unlink(MEM_UI_CHECK_FILE_PATH); 
//	}
//	
//	result = f_stat(MEM_MCU_WRITE_PATH,&check_fno);    
//	if(result == FR_OK)
//	{
//		f_unlink(MEM_MCU_WRITE_PATH); 
//	}
//	result = f_stat(MEM_UI_WRITE_PATH,&check_fno);    
//	if(result == FR_OK)
//	{
//		f_unlink(MEM_UI_WRITE_PATH); 
//	}
	
	if(((*(__IO u32*)APP_START_ADDR) & 0x2FFE0000U) == 0x20000000U) //若应用有效
	{
		DEBUG("start app\r\n");
		bsp_DelayMS(100);
		switch_2_addr((u32*)APP_START_ADDR); 
	}
	else
	{
		//应用程序启动错误无法运行，请使用u盘更新正确的应用程序!
		pop_tips(30,(u8*)TIPS,(u8*) "Application startup error can not run,   please use u disk to update the correct application!");
	}	
}
	

void boot_process(void)
{
	static DSTATUS result = FR_OK;
	static FILINFO fno = {0};
	static u16 light_num = 0;   //灯珠数
	static u32 led_tick = 0;    //控制速度 

	if(updata_state.update_status_flow != 0)                  //若程序正处于更新
		return;

	if(light_num < RGB_LED_NUM) 
	{
		led_tick ++;
		if(led_tick%4 == 1)
		{
			light_num++;
			progress_light(orange,light_num);
		}
	}
	else
	{
		led_single_show(black,RGB_LED_NUM);  //灯灭
		printf("boot:1:save_config.upgrade_state：%d\r\n",save_config.upgrade_state); 
		if(save_config.upgrade_state == 2)
		{
			result = f_stat(OTA_UI_MEM_FILE, &fno);               //检测内存中UI升级文件 
			if( FR_OK == result )
			{
				updata_state.update_file_mode |= 0x01;
				ui_upgrade_file_path = OTA_UI_MEM_FILE;
				printf("boot:1:UI：%s\r\n",ui_upgrade_file_path);  
			}
		
			result = f_stat(OTA_MCU_BIN_MEM_FILE, &fno);          //检测内存MCU升级文件 
			if( FR_OK == result )
			{
				updata_state.update_file_mode |= 0x02;
				mcu_upgrade_file_path = OTA_MCU_BIN_MEM_FILE;
				printf("boot:1:MCU：%s\r\n",mcu_upgrade_file_path);  
			}
			
			if(updata_state.update_file_mode) 
			{
				save_config.upgrade_state = 0;  //更新状态清空
				if( 0 != cfg_save() )
				{
					//内存错误，无法升级
					set_screen(20);
					pop_tips(30,(u8*)"Error",(u8*)"Error: memory write error!!!");
					return;
				}
				updata_state.update_status_flow = 1;              //进入更新流程
				printf("boot:1:update process OK\r\n"); 
				printf("boot:1:updata_state.update_file_mode: %d\r\n",updata_state.update_file_mode); 
				printf("boot:1:updata_state.update_status_flow: %d\r\n",updata_state.update_status_flow); 
				return;
			}
			else 
			{
				
			}
		}
		else if(updata_state.update_status_flow == 0 && u_disk_run == 0)
		{
			if(cfg_dft.u_disk_state == 99)       //检测到U盘
			{
				result = f_stat(U_DISK_UI_FILE_PATH, &fno);           //检测UI升级文件 
				if(FR_OK == result )
				{
					updata_state.update_file_mode |= 0x01;
					ui_upgrade_file_path = U_DISK_UI_FILE_PATH;
				}
				result = f_stat(U_DISK_MCU_FILE_PATH, &fno);          //检测U盘MCU升级文件 
				if(FR_OK == result )
				{
					updata_state.update_file_mode |= 0x02;
					mcu_upgrade_file_path = U_DISK_MCU_FILE_PATH;
				}
				
				if(updata_state.update_file_mode)  //有升级文件
				{
					if(save_config.upgrade_state == 1)  //若是应用程序发出更新 则直接进入更新流程
					{
						save_config.upgrade_state = 0;  //更新状态清空
						if( 0 != cfg_save() )
						{
							//内存错误，无法升级
							set_screen(20);
							pop_tips(30,(u8*)"Error",(u8*)"Error: memory write error!!!");
							return;
						}
						updata_state.update_status_flow = 1;    //进入更新流程
					}
					else
					{
						set_screen(33);                //若不是应用程序提出的更新，可能是插着U盘重启，弹框提示是否更新
						u_disk_run = 1;
					}
				}
				else
				{
					if(updata_state.update_status_flow == 0)
						start_app();                   //没有升级文件直接启动
				}

			}
			else
			{
				if(cfg_dft.u_disk_state == 0 || app_tick > 3000)      
				{
					if(updata_state.update_status_flow == 0)
						start_app(); 
				}				
			}
		}
	}

}




int get_que_data(u8 *p,Queue *q)
{
	int rst=1;
	OS_CLOSE_INT;
	rst=Queue_get_1(p,q);
	OS_OPEN_INT;
	return rst;
}

