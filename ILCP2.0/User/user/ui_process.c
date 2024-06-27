#include "ui_process.h"

u16 scene_mode = 0;
save_ui_id ui_id = {0};

void ui_updata_tx(u8 *p,int n)
{
	uart_send(p,n,&uart6);
}

static u8 hmi_pack_buf[100]={0};
//static u8 syn_buf[]={FHEAD};

COMM_SYNC button_ctrl_pack=
{
	hmi_pack_buf,//u8 *rec_buff;
	sizeof(hmi_pack_buf),//int buf_len;
	FEND,//u32 endc; 
	0,//int rec_p;
	touch_cmd_pro//u8 (*pro)(u8 * b,int len);
};


//#define B   (*(button_control*)b)
extern page_record custom_page;
button_control b_ctrl = {0};        //按钮数据
read_pic_id r_pic_id = {0};         //读取当前图片
//const char frame_end[] = {0xFF,0xFF,0xFC,0xFF};
u8 touch_cmd_pro(u8 *b ,int len)    
{
//	uart_send(b,len,&uart1);        //串口1回显
	r_pic_id = *(read_pic_id*)b;
	if(r_pic_id.head == FHEAD && r_pic_id.end == CHANGE_END32( FEND ) && r_pic_id.cmd == CHANGE_END16(0xB101) )   
	{
		if(ui_id.cur_picture != CHANGE_END16(r_pic_id.pic_id))
		{
			ui_id.pre_picture = ui_id.cur_picture;
			ui_id.cur_picture = CHANGE_END16(r_pic_id.pic_id);
		}
		return 0;
	}
	b_ctrl = *(button_control*)b;
	if(b_ctrl.head == FHEAD && b_ctrl.end == CHANGE_END32( FEND ) && b_ctrl.button_type == 0x10)      
	{
		if( b_ctrl.cmd == CHANGE_END16(0xB111) )                
		{
			if( 0x0001 <= CHANGE_END16(b_ctrl.pic_id) && CHANGE_END16(b_ctrl.pic_id) <= 0x0004 )  
			{
				if( 0x0001 <= CHANGE_END16(b_ctrl.touch_id) && CHANGE_END16(b_ctrl.touch_id) <= 0x0003)
				{
					scene_mode = (CHANGE_END16(b_ctrl.pic_id) - 1)*3 + CHANGE_END16(b_ctrl.touch_id);
					if(scene_mode > 20)
					{
						scene_mode = 0;
//						while(1);
					}
					cure_page_info(scene_mode); 
					
					switch_icon(0x0005,0x0009,scene_mode - 1);
			

					show_cur_cure_page(0);  
					set_screen(5);            
				}
				else if(0x0004 == CHANGE_END16(b_ctrl.touch_id))
				{
			
				}
			}
			else if( b_ctrl.pic_id == CHANGE_END16(0x0000) )         
			{
				if( b_ctrl.touch_id == CHANGE_END16(0x0001) )     //固化场景界面
				{					
				}
				if( b_ctrl.touch_id == CHANGE_END16(0x0002) )     //custom
				{
					read_custom_info();
					show_cur_custom_page(0);     
				}
				if( b_ctrl.touch_id == CHANGE_END16(0x0003) )     //设置界面    
				{
					setting_show();
				}
				if( b_ctrl.touch_id == CHANGE_END16(0x0004) )     //主界面弹框
				{
					switch_icon(11,3,cfg_dft.head_lamp);          //刷新内部照明灯的状态    
					switch_icon(11,4,cfg_dft.head_lamp);
				}
			}
			else if( b_ctrl.pic_id == CHANGE_END16(0x0002) )   
			{
//				if(b_ctrl.touch_id == CHANGE_END16(0x0004))
			}
			else if( b_ctrl.pic_id == CHANGE_END16(0x0003) )   
			{
//				if(b_ctrl.touch_id == CHANGE_END16(0x0004))
			}
			else if( b_ctrl.pic_id == CHANGE_END16(0x0005) )         //Cure ҳ��
			{
				cure_select(CHANGE_END16(b_ctrl.touch_id));
			}
			else if( b_ctrl.pic_id == CHANGE_END16(0x0006) )         //Custom ����
			{
				custom_select(CHANGE_END16(b_ctrl.touch_id));
			}
			else if( b_ctrl.pic_id == CHANGE_END16(0x0007) )         //custom 
			{
				if( b_ctrl.touch_id == CHANGE_END16(0x0001) )        
				{
					//清除选中状态，图标 
					custom_page.select_num = -1; 
					switch_icon(0x06,13,0);  
					switch_icon(0x06,14,0);  
					switch_icon(0x06,15,0);  
					clear_button_all(0x0006,0x0005,0x0006,0x0007,0x0008);
				}
				else if( b_ctrl.touch_id == CHANGE_END16(0x0002) )   
				{
					save_para();
					rgb_light_state = 4;    //橙灯闪烁告警
				}
				else if( CHANGE_END16(b_ctrl.touch_id) >= 20 && CHANGE_END16(b_ctrl.touch_id) <= 23 )  
				{
					user_custom[custom_page.select_num - 1].up_uv[CHANGE_END16(b_ctrl.touch_id) - 20] = 0;
					user_custom[custom_page.select_num - 1].down_uv[CHANGE_END16(b_ctrl.touch_id) - 20] = 0;
					user_custom[custom_page.select_num - 1].heat[CHANGE_END16(b_ctrl.touch_id) - 20] = 0;
					user_custom[custom_page.select_num - 1].time[CHANGE_END16(b_ctrl.touch_id) - 20] = 0;
					//清空显示
					clear_text_value(7,CHANGE_END16(b_ctrl.touch_id) - 16);	
					clear_text_value(7,CHANGE_END16(b_ctrl.touch_id) - 12);
					clear_text_value(7,CHANGE_END16(b_ctrl.touch_id) - 8);
					clear_text_value(7,CHANGE_END16(b_ctrl.touch_id) - 4);		
				}
			}
			else if( b_ctrl.pic_id == CHANGE_END16(9) )        
			{
				if(CHANGE_END16(b_ctrl.touch_id) == 1)
				{
					UV_POWER_PER1 = 0;      //上灯板关闭
					UV_POWER_PER2 = 0;      //下灯板关闭
					UV_FAN1_PWR_EN = 0;     //上光源风扇关闭
					UV_FAN2_PWR_EN = 0;     //下光源风扇关闭
					cfg_dft.heat_sw = 0;    //加热关闭
					cfg_dft.heat_temp = -300;
					set_screen(20);			
				}					
			}
			else if( b_ctrl.pic_id == CHANGE_END16(10) )        
			{
				if(CHANGE_END16(b_ctrl.touch_id) == 1)
					set_screen(20);	
				else if( CHANGE_END16(b_ctrl.touch_id) == 8 )   //加热补偿减
				{
					u8 show_text[8] = {0};
					if(save_config.heat_cal >= -40)
						save_config.heat_cal--;
					sprintf((char*)show_text,"%d",save_config.heat_cal); 
					SetTextValue(10,10,show_text); //显示加热补偿值
				}
				else if( CHANGE_END16(b_ctrl.touch_id) == 9 )   //加热补偿加
				{
					u8 show_text[8] = {0};
					if(save_config.heat_cal <= 60)
						save_config.heat_cal++;
					sprintf((char*)show_text,"%d",save_config.heat_cal); 
					SetTextValue(10,10,show_text); //显示加热补偿值
				}
				else if( CHANGE_END16(b_ctrl.touch_id) == 12 )  //UV和加热补偿减
				{
					u8 show_text[8] = {0};
					if(save_config.uv_heat_cal >= -40)
						save_config.uv_heat_cal--;
					sprintf((char*)show_text,"%d",save_config.uv_heat_cal); 
					SetTextValue(10,11,show_text); //显示UV加热补偿值
				}
				else if( CHANGE_END16(b_ctrl.touch_id) == 13 )  //UV和加热补偿加
				{
					u8 show_text[8] = {0};
					if(save_config.uv_heat_cal <= 60)
						save_config.uv_heat_cal++;
					sprintf((char*)show_text,"%d",save_config.uv_heat_cal); 
					SetTextValue(10,11,show_text); //显示UV加热补偿值
				}
				else if( CHANGE_END16(b_ctrl.touch_id) == 14 )  //保存设置
				{
					cfg_save();
					set_screen(13);                             //保存成功提示
					rgb_light_state = 4;    //橙灯闪烁告警
				}
			}
			else if( b_ctrl.pic_id == CHANGE_END16(11) )        
			{
				pop_up_main(CHANGE_END16(b_ctrl.touch_id));
			}
			else if( b_ctrl.pic_id == CHANGE_END16(12) )
			{
				if(CHANGE_END16(b_ctrl.touch_id) == 1)                //pause
				{
					if(cure.mode == 1)
					{
						cure.mode = 2;
						curing_pause();
					}
					else  
					{
						cure.mode = 1;
						curing_continue();
					}
					switch_icon(12,11,cure.mode-1);
				}
				else if(CHANGE_END16(b_ctrl.touch_id) == 2)           //结束固化
				{
					cure_stop();
					cure.mode = 3;
				}
				else if(CHANGE_END16(b_ctrl.touch_id) == 10)          //cure lamp
				{
					cfg_dft.head_lamp ^= 1;
					switch_icon(12,9,cfg_dft.head_lamp);
					WLED_PWR_EN = cfg_dft.head_lamp;
				}
			}
			else if( b_ctrl.pic_id == CHANGE_END16(13) )              //保存成功关闭
			{
				if(CHANGE_END16(b_ctrl.touch_id) == 3)
					set_screen(ui_id.pre_picture);
			}	
			else if( b_ctrl.pic_id == CHANGE_END16(14) )              //固化结束返回
			{
				if(CHANGE_END16(b_ctrl.touch_id) == 1)
				{
					if(FR_OK != write_user_parameter(TT_FILE,(const void*)&save_time,sizeof(save_time))) 	//存储一次固化使用时间
					{
						//弹框提示 文件写入错误可能会影响总时间记录
						pop_tips(30,(u8*)WARNING,(u8*)FILE_RW_ERR); 
						return 1;
					}
					set_screen(0);
				}
			}				
			else if( b_ctrl.pic_id == CHANGE_END16(15) )    
			{
				if(CHANGE_END16(b_ctrl.touch_id) == 17)
					set_screen(ui_id.pre_picture);
			}
			else if( b_ctrl.pic_id == CHANGE_END16(16) )	      //点击屏保
			{
				extern u32 home_tick;
				if(CHANGE_END16(b_ctrl.touch_id) == 1)
				{
					set_screen(0);//返回首页
 					home_tick = 0;
				}
			}				
			else if( b_ctrl.pic_id == CHANGE_END16(19) )          //删除参数弹框
			{
				if(CHANGE_END16(b_ctrl.touch_id) == 2)       
					set_screen(5);
				else if(CHANGE_END16(b_ctrl.touch_id) == 3)  
					delete_para();
			}
			else if( b_ctrl.pic_id == CHANGE_END16(20) )          //setting 
			{
				setting_ctrl1(CHANGE_END16(b_ctrl.touch_id));
			}
			else if( b_ctrl.pic_id == CHANGE_END16(21) )
			{
				setting_ctrl2(CHANGE_END16(b_ctrl.touch_id));
			}
			else if( b_ctrl.pic_id == CHANGE_END16(22) )          //wifi列表操作
			{
				wifi_list_ctrl(CHANGE_END16(b_ctrl.touch_id));
			}
			else if( b_ctrl.pic_id == CHANGE_END16(23) )
			{
				hidden_function_screen(CHANGE_END16(b_ctrl.touch_id));
			}
			else if( b_ctrl.pic_id == CHANGE_END16(24) )
			{
				hidden_function_screen_two(CHANGE_END16(b_ctrl.touch_id));
			}
			else if( b_ctrl.pic_id == CHANGE_END16(27) )
			{
				if(CHANGE_END16(b_ctrl.touch_id) == 4)  
				{
					if(p_flag.eject_key == 1 || p_flag.eject_key == 2 || p_flag.total_time == 1 || p_flag.decive_para == 1 || p_flag.custom_para == 1)
						set_screen(23);
					else
						set_screen(ui_id.pre_picture);
				}
			}
			else if( b_ctrl.pic_id == CHANGE_END16(29) )
			{
				if(CHANGE_END16(b_ctrl.touch_id) == 4)  
				{					
					if(p_flag.eject_key == 1 || p_flag.total_time == 1)
						set_screen(23);
					else
						set_screen(ui_id.pre_picture);	
				}
				else if(CHANGE_END16(b_ctrl.touch_id) == 5)
				{
					if(p_flag.eject_key == 1 || p_flag.eject_key == 2 || p_flag.total_time == 1 || p_flag.decive_para == 1 )
					{
						eject_keyboard_set(23);       //弹出全键盘
					}
					else if( p_flag.set_ac == 1 || p_flag.u_disk == 1)
					{
						eject_keyboard_set(24);
					}
					else if( p_flag.cal_set == 1 )
					{
						eject_keyboard_set(20);
					}
					else if(p_flag.custom_para == 1)
					{
						memset(user_custom,0x00,sizeof(user_custom));
		
						if(FR_OK == write_user_parameter(USER_FILE,user_custom,sizeof(user_custom)))
						{
							// 弹框 清空成功
							pop_tips(27,(u8*)TIPS,(u8*)USER_PARA_CLEARED); 
						}	
						else
						{
							pop_tips(30,(u8*)WARNING,(u8*)FILE_RW_ERR); 
						}
					}
				}					
			}
			else if(b_ctrl.pic_id == CHANGE_END16(30))
			{
				if(CHANGE_END16(b_ctrl.touch_id) == 4)
				{
					if(cure.mode != 0)  
					{
						set_screen(12);	 //固化中界面				
					}
					else if(p_flag.eject_key == 1)
					{
						set_screen(23);  //隐藏SN界面
					}
					else if(p_flag.u_disk == 1)
					{
						set_screen(24);  //隐藏界面2
					}
					else if(p_flag.cal_set == 1)
					{
						set_screen(20);  //设置界面
					}
					else
					{
						set_screen(ui_id.pre_picture);		
					}						
				}
			}
			else if(b_ctrl.pic_id == CHANGE_END16(32))
			{
				if(CHANGE_END16(b_ctrl.touch_id) == 2)
					set_screen(33);
				else if(CHANGE_END16(b_ctrl.touch_id) == 3)
				{
					if( cfg_dft.network_selet == 1 || wifi_flow.connect_state == 1 )  //若是以太网连接则走以太网路线
					{
						cfg_dft.init_step = 5;  //执行OTA更新
						set_screen(34);         //跳转到检查更新等待界面
					}
					else
					{
						if( wifi_flow.connect_state != 1 )                 //若WiFi网络未连接
						{
							wifi_flow.ota_run_step = 0;
							set_screen(20);
							pop_tips(30,(u8*)NO_NETWORK,(u8*)NO_OTA_UPDATES);
							return 0;
						}
					}
				}
			}
			else if(b_ctrl.pic_id == CHANGE_END16(33))   //u盘升级
			{
				if(CHANGE_END16(b_ctrl.touch_id) == 2)
				{
					//u盘升级
					if(cfg_dft.u_disk_state == 0)   //没有检测到u盘
					{
						pop_tips(28,(u8*)TIPS,(u8*)NO_USB_DISK); 
						vTaskDelay(1000);
						set_screen(20);							
					}
					else if(cfg_dft.u_disk_state == 99)  //检测到U盘插入
					{
						DSTATUS fr_ui = 0,fr_mcu = 0;
						FILINFO fno = {0};
						fr_ui = f_stat(U_DISK_UI_FILE_PATH, &fno);
						fr_mcu = f_stat(U_DISK_MCU_FILE_PATH, &fno);
						if( FR_OK == fr_ui || FR_OK == fr_mcu )
						{
							save_config.upgrade_state = 1;    //重启从U盘读取升级文件
							
							if( 0 == cfg_save() )
								Sys_Soft_Reset(); //系统软复位
							else
							{
								set_screen(20);
								pop_tips(30,(u8*)"Error",(u8*)MEM_WRITE_UPGRADE_ERROR);
							}
						}
						else
						{
							set_screen(20);
							pop_tips(30,(u8*)"Error",(u8*)"Failed to upgrade due to memory error.");
						}
					}
					else if(cfg_dft.u_disk_state == 2)    //无法识别的USB设备
					{
						pop_tips(28,(u8*)TIPS,(u8*)USB_DISK_UNKNOWN);
						vTaskDelay(1000);
						set_screen(20);	
					}
				}
				else if(CHANGE_END16(b_ctrl.touch_id) == 3) //取消升级
				{
					//network
					set_screen(20);
				}
			}
			else if(b_ctrl.pic_id == CHANGE_END16(34))
			{
				//关闭检查更新弹窗
				if(CHANGE_END16(b_ctrl.touch_id) == 2)
				{
					set_screen(20);
				}
			}
			else if(b_ctrl.pic_id == CHANGE_END16(35))      //弹出的版本信息框
			{
				if(CHANGE_END16(b_ctrl.touch_id) == 9)
				{
					if(cfg_dft.network_selet == 1)
					{
						enthernet_flow.update_step = 2;
					}
					else if(wifi_flow.connect_state == 1)
					{
						wifi_flow.ota_run_step = 3;
					}
					pop_tips(30,(u8*)"Loading",(u8*)EQYEST_UPDATE_FIRMWARE);
				}
				else if(CHANGE_END16(b_ctrl.touch_id) == 10)
				{
					if(cfg_dft.network_selet == 1)
					{
						enthernet_flow.execution_step = 1;  //重新连接上传数据的服务器
						cfg_dft.network_selet = 0;          //以太网连接状态重置
						cfg_dft.init_step = 4;              //执行上传
					}
					else if(wifi_flow.connect_state == 1)
					{
							//取消
						wifi_flow.ota_run_step = 0;
						wifi_flow.execution_step = 4;       //转转成UDP透传连接
						wifi_flow.sntp_cal = 1;             //执行一次NTP时间校准
						cfg_dft.init_step = 4;
					}

					set_screen(20);
				}
				else if(CHANGE_END16(b_ctrl.touch_id) == 11)
				{
					//发布说明
					set_screen(37);
				}
			}
			else if(b_ctrl.pic_id == CHANGE_END16(36))
			{
				if(CHANGE_END16(b_ctrl.touch_id) == 2)
				{
					set_screen(20);
				}
			}
			else if(b_ctrl.pic_id == CHANGE_END16(37))
			{
				if(CHANGE_END16(b_ctrl.touch_id) == 5)
				{
					set_screen(35);
				}				
			}
		}
		return 0;
	}
	if(b_ctrl.head == FHEAD && b_ctrl.cmd == CHANGE_END16(0xB111) && b_ctrl.button_type == 0x11)  //文本输入
	{
		if(b_ctrl.pic_id == CHANGE_END16(0x0007))         //custom
		{
			custom_input_pro(CHANGE_END16(b_ctrl.touch_id),b,len);
		}
		else if(b_ctrl.pic_id == CHANGE_END16(9))
		{
			control_input_pro(CHANGE_END16(b_ctrl.touch_id),b,len);
		}
		else if(b_ctrl.pic_id == CHANGE_END16(10))
		{
			cal_input_pro(CHANGE_END16(b_ctrl.touch_id),b,len);		
		}		
		else if(b_ctrl.pic_id == CHANGE_END16(22))       //wifi列表显示页面
		{
			wifi_input_pro(CHANGE_END16(b_ctrl.touch_id),b,len);
		}
		return 0;
	}
	if( b_ctrl.head == FHEAD && b_ctrl.cmd == CHANGE_END16(0x8601) )
	{
		eject_input_pro(b,len);
	}
	return 1;
}




void pop_up_main(u16 touch_id)
{
	if(touch_id == 0x0006)
		cfg_dft.head_lamp = 1;
	else if(touch_id == 0x0007)
		cfg_dft.head_lamp = 0;
	else if(touch_id == 0x0008)
	{
		cfg_dft.m_ctrl ^= 1;
		switch_icon(11,5,cfg_dft.m_ctrl);
	}
	switch_icon(11,3,cfg_dft.head_lamp);
	switch_icon(11,4,cfg_dft.head_lamp);
	WLED_PWR_EN = cfg_dft.head_lamp;
}


//弹窗提示
void pop_tips(u16 screen_id,u8* t,u8* str)
{
	set_screen(screen_id);
	SetTextValue(screen_id,2,t); 
	SetTextValue(screen_id,3,str); 
}








