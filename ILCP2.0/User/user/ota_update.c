#include "ota_update.h"

u32 ui_file_totalsize = 0;
u32 mcu_file_totalsize = 0;
u8 ota_file_number = 0;      //1: 只有UI文件  2:只有MCU文件  3:两个文件都有


void ota_run_process(void)
{
	static u32 connect_cnt = 0;
	if( wifi_flow.connect_state == 1 )        //wifi网络已连接
	{
		if(wifi_flow.ota_run_step == 0)
		{
			connect_cnt = 0;
			if( 0 != esp8266_at_test() )      //若AT指令不成功
			{
				if(0 == esp8266_exit_unvarnished())        //退出透传模式
				{
					if(0 == esp8266_disconnect_server())   //断开服务连接
					{
						wifi_flow.ota_run_step = 1;
					}
					else
					{}
				}
				else
				{}
			}
			else
				wifi_flow.ota_run_step = 1;
		}
		else if(wifi_flow.ota_run_step == 1)
		{	
			if( 0 == esp8266_connect_tcp_server(cfg_dft.ali_domain_name,PORT_NUMBER))    //进行Web TCP连接
			{
				if( 0 == esp8266_enter_unvarnished() )     //进入TCP透传模式
				{
					wifi_flow.ota_run_step = 2;
					wifi_flow.file_down_step = 0;
				}
			}
			else
			{
				connect_cnt++;
				if(connect_cnt > 5)
				{
					//服务器连接失败提示
					pop_tips(30,(u8*)"Connection fail",(u8*)"TCP connection timed out!");
					wifi_flow.ota_run_step = 0;
					wifi_flow.execution_step = 4;   //转转成UDP透传连接
					wifi_flow.sntp_cal = 1;         //执行一次NTP时间校准
				}
			}
		}
		else if(wifi_flow.ota_run_step == 2)      //检查版本文件
		{
			FRESULT result = FR_OK;
			static FIL version_file = {0};
			UINT r;
			version_s v_r = {0};
			char txt_buf[50] = {0}; 
		
			download_check_file(VERSION_FILE_PATH,MEM_VERSION_FILE_PATH); //下载一次版本文件
			if(wifi_flow.file_down_step == 4)
			{
				result = f_open(&version_file,MEM_VERSION_FILE_PATH,FA_READ); 
				if(FR_OK == result)
				{
					result = f_read(&version_file,&v_r,sizeof(version_s),&r);
					if(FR_OK == result)
					{
						u16 crc = 0;
						crc = cal_crc((u8*)&v_r,sizeof(v_r) - 4);
						if(v_r.head == 0xaa55 && v_r.end == 0xffff && v_r.self_crc == crc)
						{
//							u32 current_v = 0,latest_v = 0;
//							sscanf((const char*)cfg_dft.version,"V2.0.%d",&current_v);  //当前版本转换为整数
//							latest_v = atoi(v_r.v_buf);                                 //服务器存储的版本转换为整数
							
							set_screen(35);                //展示版本信息界面
							sprintf(txt_buf,"%s%s","Latest Version:V2.0.1",v_r.v_buf);
							SetTextValue(35,6,(unsigned char*)txt_buf);
							memset(txt_buf,0x00,sizeof(txt_buf));
							sprintf(txt_buf,"%s%s","Current Version:",cfg_dft.version);
							SetTextValue(35,8,(unsigned char*)txt_buf); 
							SetTextValue(37,3,(unsigned char*)v_r.release_note); 
//							if(latest_v > current_v)       //若存在新版本
//							{
//								set_screen(35);            //展示版本信息
//								sprintf(txt_buf,"%s%s","Latest Version:V2.0.",v_r.v_buf);
//								SetTextValue(35,6,(unsigned char*)txt_buf);
//								memset(txt_buf,0x00,sizeof(txt_buf));
//								sprintf(txt_buf,"%s%s","Current Version:",cfg_dft.version);
//								SetTextValue(35,6,(unsigned char*)txt_buf); 
//							}
//							else
//							{
//								set_screen(36);                 //提示当前已是最新版本
//								wifi_flow.ota_run_step = 0;
//								wifi_flow.execution_step = 4;   //转转成UDP透传连接
//								wifi_flow.sntp_cal = 1;         //执行一次NTP时间校准
//							}
						}
						
					}
				}
				f_close(&version_file);      //关闭文件

				wifi_flow.ota_run_step = 99;
			}
			
			if(wifi_flow.down_timeout ++ > 500)
			{
				set_screen(20);
				pop_tips(30,(u8*)WARNING,(u8*)NO_REQYEST_FIRMWARE);  //提示： 内存错误，无法进行OTA数据下载
				wifi_flow.ota_run_step = 0;
				wifi_flow.execution_step = 4;   //转转成UDP透传连接
				wifi_flow.sntp_cal = 1;         //执行一次NTP时间校准
			}
		}
		else if(wifi_flow.ota_run_step == 99)
		{
			if(0 == update_cure_parameter_file())
			{
				wifi_flow.ota_run_step = 0xFF;
				wifi_flow.down_timeout = 0;
				pop_tips(27,(u8*)HMI_SUCCEEDED,(u8*)FILE_IM_SUCCEEDED);
			}
			if(wifi_flow.down_timeout++ > 5000)
			{
				wifi_flow.ota_run_step = 66;
				set_screen(20);
				pop_tips(30,(u8*)HMI_FAILED,(u8*)"Cure parameter update failed!");   //固化参数更新失败
			}
		}
		else if(wifi_flow.ota_run_step == 3)              //检测内存和清除
		{	
			FRESULT result = FR_OK;
			FILINFO fno = {0};
			u32 mem_total_size = 0,mem_free_size = 0;
			
			result = f_stat(MEM_FOLDER_PATH,&fno);                   //检测内存是否存在更新文件，若存在可能是上次下载被中断的残留
			if(FR_OK == result)
			{
				result = DeleteDirectoryFiles(MEM_FOLDER_PATH);      //递归删除文件夹以及路径下文件
				if(FR_OK != result)
				{
					set_screen(20);
					pop_tips(30,(u8*)WARNING,(u8*)OTA_DOWNLOAD_MEM_ERROR);  //提示： 内存错误，无法进行OTA数据下载
					wifi_flow.ota_run_step = 0;
					wifi_flow.execution_step = 4;   //转转成UDP透传连接
					wifi_flow.sntp_cal = 1;         //执行一次NTP时间校准
					return;
				}
			}
			
			//HTTP 方式访问服务器，判断服务器是否存在更新文件若存在则得到文件大小 
			ota_file_number = 0;
			if( 0 == file_header_request(cfg_dft.ali_domain_name,UI_OTA_FILE_PATH,&ui_file_totalsize,5000))
			{
				ota_file_number |= 1;
			}
			if( 0 == file_header_request(cfg_dft.ali_domain_name,MCU_OTA_FILE_PATH,&mcu_file_totalsize,5000))
			{
				ota_file_number |= 2;
			}
			
			if( ota_file_number == 0 )
			{
				pop_tips(30,(u8*)"Error",(u8*)F_UPDATE_REQUEST_FAIL);   //没有请求到要下载的文件
				wifi_flow.ota_run_step = 0;
				wifi_flow.execution_step = 4;   //转转成UDP透传连接
				wifi_flow.sntp_cal = 1;         //执行一次NTP时间校准
				return;
			}
			
			exf_getfree((u8*)"1:",&mem_total_size,&mem_free_size);              //得到磁盘剩余空间大小
			if( (mem_free_size*1024) < (ui_file_totalsize + mcu_file_totalsize) )
			{
				//内存空间不足
				set_screen(20);
				pop_tips(30,(u8*)"Error",(u8*)MEM_SPACE_INSUFFF);
				wifi_flow.ota_run_step = 0;
				wifi_flow.execution_step = 4;   //转成UDP透传连接
				wifi_flow.sntp_cal = 1;         //执行一次NTP时间校准
				return;
			}
			wifi_flow.ota_run_step = 4;
			wifi_flow.file_down_step = 0;
		}
		else if(wifi_flow.ota_run_step == 4)          //下载MCU校验文件
		{
			if(ota_file_number == 3 || ota_file_number == 2)
			{
				download_check_file(MCU_CHECK_FILE_PATH,MEM_MCU_CHECK_FILE_PATH);
				if(wifi_flow.file_down_step == 4)
				{
					if(ota_file_number == 2)
					{
						wifi_flow.ota_run_step = 6;
						wifi_flow.file_down_step = 0;
					}
					if(ota_file_number == 3)
					{
						wifi_flow.ota_run_step = 5;
						wifi_flow.file_down_step = 0;
					}
				}
			}
			else if(ota_file_number == 1)
			{
				wifi_flow.ota_run_step = 4;
				wifi_flow.file_down_step = 0;
			}
			else
			{
				//没有更新文件
			}
		}
		else if(wifi_flow.ota_run_step == 5)         //下载UI的校验文件
		{
			download_check_file(UI_CHECK_FILE_PATH,MEM_UI_CHECK_FILE_PATH);
			if(wifi_flow.file_down_step == 4)
			{
				wifi_flow.ota_run_step = 6;
				wifi_flow.file_down_step = 0;
			}
		}
		else if(wifi_flow.ota_run_step == 6)
		{
			if(ota_file_number == 3 || ota_file_number == 2)
			{
				download_ota_file(MCU_OTA_FILE_PATH,MEM_MCU_WRITE_PATH,MEM_MCU_CHECK_FILE_PATH,mcu_file_totalsize); //下载MCU文件
				if(wifi_flow.file_down_step == 4)
				{
					if(ota_file_number == 3)
					{
						wifi_flow.ota_run_step = 7;
						wifi_flow.file_down_step = 0;
					}
				}
			}
			else if(ota_file_number == 1)
				wifi_flow.ota_run_step = 7;
		}
		else if(wifi_flow.ota_run_step == 7)
		{
			download_ota_file(UI_OTA_FILE_PATH,MEM_UI_WRITE_PATH,MEM_UI_CHECK_FILE_PATH,ui_file_totalsize);         //下载UI文件
			if(wifi_flow.file_down_step == 4)
			{
				wifi_flow.ota_run_step = 8;
				wifi_flow.file_down_step = 0;
			}
		}
		else if(wifi_flow.ota_run_step == 8)
		{
			save_config.upgrade_state = 2;    //内存更新标志
			if( 0 == cfg_save() )
			{
				vTaskDelay(100);
				Sys_Soft_Reset();           //系统软复位
			}
			else
			{
				//内存错误，无法升级
				printf("app:Upgrade write error!\r\n");  
				set_screen(20);
				pop_tips(30,(u8*)"Error",(u8*)MEM_WRITE_UPGRADE_ERROR);
			}
		}
	}
	else
	{

	}
}


//下载校验文件
void download_check_file(char* s_fpath,char* mem_fpath)
{
	FRESULT result = FR_OK;
	static FIL dowm_check_file = {0};
	static s_file_download check_file = {0};
	UINT c_r, c_w;
	FILINFO check_fno = {0};
	
	if(wifi_flow.file_down_step == 0)
	{
		result = f_stat(mem_fpath,&check_fno);                   //检测内存是否存在，若存在可能是上次下载被中断的残留
		if(FR_OK == result)
			f_unlink(mem_fpath);
		
		memset(&check_file,0x00,sizeof(s_file_download));
		memset(&dowm_check_file,0x00,sizeof(FIL));
		if( 0 != file_header_request(cfg_dft.ali_domain_name,s_fpath,&check_file.file_size,5000))    //请求文件头部信息
		{
			pop_tips(30,(u8*)WARNING,(u8*)DATA_LOAD_FAILED);        //数据加载失败，退出OTA。
			wifi_flow.ota_run_step = 0;
			wifi_flow.execution_step = 4;   //转转成UDP透传连接
			wifi_flow.sntp_cal = 1;         //执行一次NTP时间校准
			cfg_dft.init_step = 4;          //退出OTA下载
			return;
		}
		wifi_flow.file_down_step = 1;
	}
	else if(wifi_flow.file_down_step == 1)        //第一包数据下载
	{
		check_file.start = 0;
		check_file.end = DOWNLOAD_DATA_LEN -1;

		if( check_file.file_size <= DOWNLOAD_DATA_LEN ) 
			check_file.end = check_file.file_size -1;
		
		if( 0 == server_download_request(cfg_dft.ali_domain_name,s_fpath,\
			&check_file.start,&check_file.end,check_file.file_data,&check_file.file_size,3000)) 
		{
			result = f_open(&dowm_check_file,mem_fpath,FA_OPEN_ALWAYS|FA_WRITE|FA_READ);      //在内存路径下新建一个校验文件
			if(FR_OK == result)
			{
				if( check_file.file_size <= DOWNLOAD_DATA_LEN )    //若文件总大小不足1k
				{
					result = f_write(&dowm_check_file,check_file.file_data,check_file.end - check_file.start + 1,&c_w);
					if(FR_OK == result)
					{
						check_file.write_byte += check_file.end - check_file.start + 1;
						if(check_file.write_byte == check_file.file_size)
						{
							f_close(&dowm_check_file);      //关闭文件
							wifi_flow.file_down_step = 3;
							return;
						}
						else
						{
							f_unlink(mem_fpath);
							wifi_flow.file_down_step = 0;             //重新下载
							return;
						}
					}
					else
					{
						f_unlink(mem_fpath);
						wifi_flow.file_down_step = 0;             //重新下载
						return;						
					}					
				}
				else
				{
					result = f_write(&dowm_check_file,check_file.file_data,DOWNLOAD_DATA_LEN,&c_w);
					if(FR_OK == result)
					{
						check_file.write_byte = DOWNLOAD_DATA_LEN;
						wifi_flow.file_down_step = 2;
						check_file.repeated_req = 0;
					}
					else
					{
						f_unlink(mem_fpath);
						wifi_flow.file_down_step = 0;             //重新下载
						return;
					}
				}
			}
			else
			{
				f_unlink(mem_fpath);
				wifi_flow.file_down_step = 0;             //重新下载
				return;			
			}
		}
		else
		{
			check_file.repeated_req ++;
			if(check_file.repeated_req > 5)
			{
				set_screen(20);
				pop_tips(30,(u8*)WARNING,(u8*)DATA_LOAD_FAILED);        //数据加载失败，退出OTA。

				wifi_flow.ota_run_step = 0;
				wifi_flow.execution_step = 4;   //转转成UDP透传连接
				wifi_flow.sntp_cal = 1;         //执行一次NTP时间校准
				cfg_dft.init_step = 4;          //退出OTA下载
			}
		}		
	}
	else if(wifi_flow.file_down_step == 2)
	{
		if(check_file.repeated_req == 0)
		{
			check_file.start = check_file.end + 1;
			check_file.end = check_file.start + DOWNLOAD_DATA_LEN - 1;
		}
		if(check_file.end < check_file.file_size - 1)
		{
			if( 0 == server_download_request(cfg_dft.ali_domain_name,s_fpath,\
				&check_file.start,&check_file.end,check_file.file_data,&check_file.file_size,3000)) 
			{
				result = f_write(&dowm_check_file,check_file.file_data,DOWNLOAD_DATA_LEN,&c_w);
				if(FR_OK == result)
				{
					check_file.write_byte += DOWNLOAD_DATA_LEN;
					check_file.repeated_req = 0;
				}
				else
				{
				}
			}
			else
			{
				check_file.repeated_req ++;
				if(check_file.repeated_req > 5)
				{
					set_screen(20);
					pop_tips(30,(u8*)WARNING,(u8*)DATA_LOAD_FAILED);        //数据加载失败，退出OTA。

					wifi_flow.ota_run_step = 0;
					wifi_flow.execution_step = 4;   //转转成UDP透传连接
					wifi_flow.sntp_cal = 1;         //执行一次NTP时间校准
					cfg_dft.init_step = 4;          //退出OTA下载
				}
			}
		}
		else   //若是最后一包
		{
			check_file.end = check_file.file_size - 1;
			if( 0 == server_download_request(cfg_dft.ali_domain_name,s_fpath,\
				&check_file.start,&check_file.end,check_file.file_data,&check_file.file_size,3000)) 
			{
				result = f_write(&dowm_check_file,check_file.file_data,check_file.end - check_file.start + 1,&c_w);
				if(FR_OK == result)
				{
					check_file.write_byte += check_file.end - check_file.start + 1;
					if(check_file.write_byte == check_file.file_size)
					{
						f_close(&dowm_check_file);      //关闭文件
						wifi_flow.file_down_step = 3;
					}
					else
					{
					}
				}
				else
				{
				}
			}
			else
			{
				check_file.repeated_req ++;
				if(check_file.repeated_req > 5)
				{
					set_screen(20);
					pop_tips(30,(u8*)WARNING,(u8*)DATA_LOAD_FAILED);        //数据加载失败，退出OTA。
		
					wifi_flow.ota_run_step = 0;
					wifi_flow.execution_step = 4;   //转转成UDP透传连接
					wifi_flow.sntp_cal = 1;         //执行一次NTP时间校准
					cfg_dft.init_step = 4;          //退出OTA下载
				}				
			}
		}
	}
	else if(wifi_flow.file_down_step == 3)          //检查下载的文件
	{
		check_s check_data_r = {0}; 
		result = f_stat(mem_fpath,&check_fno);
		if(FR_OK == result)
		{
			if( check_fno.fsize != check_file.file_size )
			{
				f_unlink(mem_fpath);
				wifi_flow.file_down_step = 0;             //重新下载
				return;
			}
		}
		else
		{
			wifi_flow.file_down_step = 0;             //重新下载
			return;
		}
		
		result = f_open(&dowm_check_file,mem_fpath,FA_READ); //打开内存中下载的校验文件
		if(FR_OK != result)
		{
			f_unlink(mem_fpath);
			wifi_flow.file_down_step = 0;             //重新下载
			return;
		}
		//判断文件类型若为 txt格式的版本文件
		if( 0 == memcmp(mem_fpath,MEM_VERSION_FILE_PATH,sizeof(MEM_VERSION_FILE_PATH)) )   //若是版本信息文件
		{
			version_s version_r = {0};
			result = f_read(&dowm_check_file,&version_r,sizeof(version_s),&c_r);
			if(FR_OK == result)
			{
				u16 crc = 0;
				crc = cal_crc((u8*)&version_r,sizeof(version_r) - 4);
				if(version_r.head != 0xaa55 || version_r.end != 0xFFFF || version_r.self_crc != crc)
				{
					f_close(&dowm_check_file);                //关闭文件
					f_unlink(mem_fpath);                      //删除文件   
					wifi_flow.file_down_step = 0;             //重新下载
					return;
				}
			}
		}
		else
		{
			while(!f_eof(&dowm_check_file))
			{
				memset(&check_data_r,0x00,sizeof(check_s));
				result = f_read(&dowm_check_file,&check_data_r,sizeof(check_s),&c_r);
				if(FR_OK == result)
				{
					u16 crc = 0;
					crc = cal_crc((u8*)&check_data_r,sizeof(check_s) - 4);
					if( check_data_r.head != 0xaa55 || check_data_r.end != 0xFFFF || check_data_r.self_crc !=  crc )
					{
						f_close(&dowm_check_file);                //关闭文件
						f_unlink(mem_fpath);   
						wifi_flow.file_down_step = 0;             //重新下载
						return;
					}
				}
				else
				{
					f_close(&dowm_check_file);                //关闭文件
					f_unlink(mem_fpath);   
					wifi_flow.file_down_step = 0;                     //重新下载
					return;
				}
				vTaskDelay(1);                                        //避免长时间占用cpu
			}
		}
		
		f_close(&dowm_check_file);      //关闭文件
		wifi_flow.file_down_step = 4;
	}
}





//执行OTA  s_fpath : 服务器文件下载路径   mem_fpath: 内存文件路径
void download_ota_file(char* s_fpath,char* mem_fpath,char* mem_check_fpath,u32 file_size)
{
	FRESULT result = FR_OK;
	static FIL dowm_ota_file = {0},down_check_file = {0};
	static s_file_download file_ota = {0};
	FILINFO mem_fno = {0};

	static check_s check_data_r = {0};                       //校验数据读
	u16 crc = 0;
	
	if(wifi_flow.file_down_step == 0)
	{
		result = f_stat(MEM_FOLDER_PATH,&mem_fno);   //若文件夹不存在则创建文件夹
		if( result != FR_OK )
		{
			result = f_mkdir(MEM_FOLDER_PATH);
			if( result != FR_OK )
			{
				set_screen(20);
				pop_tips(30,(u8*)"Error",(u8*)MEMORY_UPGRADE_ERR);
				
				wifi_flow.ota_run_step = 0;
				wifi_flow.execution_step = 4;   //转转成UDP透传连接
				wifi_flow.sntp_cal = 1;         //执行一次NTP时间校准
				cfg_dft.init_step = 4;          //退出OTA下载
				return;
			}
		}
		memset(&file_ota,0x00,sizeof(s_file_download));
		memset(&dowm_ota_file,0x00,sizeof(FIL));
		wifi_flow.file_down_step = 1;
	}
	else if(wifi_flow.file_down_step == 1)        //第一包数据下载
	{
		file_ota.start = 0;
		file_ota.end = DOWNLOAD_DATA_LEN -1;
		if( 0 == server_download_request(cfg_dft.ali_domain_name,s_fpath,\
			&file_ota.start,&file_ota.end,file_ota.file_data,&file_ota.file_size,3000)) 
		{
			if(file_ota.file_size == file_size)
			{
				result = f_open(&down_check_file,mem_check_fpath,FA_READ);      //打开内存下载的校验文件
				if(FR_OK == result)
				{		
					result = f_read(&down_check_file,&check_data_r,sizeof(check_s),&b_read);
					if(FR_OK == result)
					{
						crc = cal_crc(file_ota.file_data,DOWNLOAD_DATA_LEN);              //对下载数据进行校验
						if(check_data_r.file_crc == crc && check_data_r.head == 0xaa55 && check_data_r.end == 0xFFFF)                                  //校验成功则写入数据
						{
							result = f_open(&dowm_ota_file,mem_fpath,FA_OPEN_ALWAYS|FA_WRITE|FA_READ);       //创建OTA文件并写入下载数据
							if(FR_OK == result)
							{
								result = f_write(&dowm_ota_file,file_ota.file_data,DOWNLOAD_DATA_LEN,&b_write);
								if(FR_OK == result)
								{
									file_ota.write_byte = DOWNLOAD_DATA_LEN;
									wifi_flow.file_down_step = 2;
									file_ota.repeated_req = 0;
									file_ota.crc_err = 0;
								}
							}								
						}
						else
						{
							f_close(&down_check_file);                  //关闭校验文件文件  下次开始重新打开
							file_ota.crc_err ++;
							if(file_ota.crc_err > 10)
							{
								set_screen(20);
								pop_tips(30,(u8*)WARNING,(u8*)DATA_LOAD_FAILED);        //数据加载失败，退出OTA。
						
								wifi_flow.ota_run_step = 0;
								wifi_flow.execution_step = 4;   //转转成UDP透传连接
								wifi_flow.sntp_cal = 1;         //执行一次NTP时间校准
								cfg_dft.init_step = 4;          //退出OTA下载
								return;
							}
						}
					}
				}
				else
				{}
			}
			else
			{
			}
		}
		else
		{
			file_ota.repeated_req ++;
			vTaskDelay(500);
			if(file_ota.repeated_req > 5)
			{
				set_screen(20);
				pop_tips(30,(u8*)WARNING,(u8*)DATA_LOAD_FAILED);        //数据加载失败，退出OTA。
				
				wifi_flow.ota_run_step = 0;
				wifi_flow.execution_step = 4;   //转转成UDP透传连接
				wifi_flow.sntp_cal = 1;         //执行一次NTP时间校准
				cfg_dft.init_step = 4;          //退出OTA下载
				return;
			}
		}
	}
	else if(wifi_flow.file_down_step == 2)
	{
		if(file_ota.repeated_req == 0)
		{
			file_ota.start = file_ota.end + 1;
			file_ota.end = file_ota.start + DOWNLOAD_DATA_LEN - 1;
		}
		if(file_ota.end < file_size - 1)
		{
			if( 0 == server_download_request(cfg_dft.ali_domain_name,s_fpath,\
				&file_ota.start,&file_ota.end,file_ota.file_data,&file_ota.file_size,3000)) 
			{
				if(file_ota.crc_err == 0)
				{
					result = f_read(&down_check_file,&check_data_r,sizeof(check_s),&b_read);   //读取校验文件
				}
				else
				{
					result = FR_OK;  
				}
				
				if(FR_OK == result)
				{
					crc = cal_crc(file_ota.file_data,DOWNLOAD_DATA_LEN);                      //对下载数据进行校验
					if(check_data_r.file_crc == crc && check_data_r.head == 0xaa55 && check_data_r.end == 0xFFFF)                                  //校验成功则写入数据
					{
						result = f_write(&dowm_ota_file,file_ota.file_data,DOWNLOAD_DATA_LEN,&b_write);
						if(FR_OK == result)
						{
							file_ota.write_byte += DOWNLOAD_DATA_LEN;
							file_ota.repeated_req = 0;
							file_ota.crc_err = 0;
						}
						else
						{
						}							
					}
					else
					{
						file_ota.crc_err ++;
						file_ota.repeated_req ++;
						vTaskDelay(500);
						if(file_ota.crc_err > 5)
						{
							set_screen(20);
							pop_tips(30,(u8*)WARNING,(u8*)DATA_LOAD_FAILED);        //数据加载失败，退出OTA。
				
							wifi_flow.ota_run_step = 0;
							wifi_flow.execution_step = 4;   //转转成UDP透传连接
							wifi_flow.sntp_cal = 1;         //执行一次NTP时间校准
							cfg_dft.init_step = 4;          //退出OTA下载
							return;
						}
					}
				}
			}
			else
			{
				file_ota.repeated_req ++;
				vTaskDelay(500);
				if(file_ota.repeated_req > 5)
				{
					set_screen(20);
					pop_tips(30,(u8*)WARNING,(u8*)DATA_LOAD_FAILED);        //数据加载失败，退出OTA。
		
					wifi_flow.ota_run_step = 0;
					wifi_flow.execution_step = 4;   //转转成UDP透传连接
					wifi_flow.sntp_cal = 1;         //执行一次NTP时间校准
					cfg_dft.init_step = 4;          //退出OTA下载
					return;
				}
			}
		}
		else   //若是最后一包
		{
			file_ota.end = file_size - 1;
			if( 0 == server_download_request(cfg_dft.ali_domain_name,s_fpath,\
				&file_ota.start,&file_ota.end,file_ota.file_data,&file_ota.file_size,3000)) 
			{				
				if(file_ota.crc_err == 0)
				{
					result = f_read(&down_check_file,&check_data_r,sizeof(check_s),&b_read);   //读取校验文件
				}
				else
				{
					result = FR_OK;  
				}
				if(FR_OK == result)
				{
					if(check_data_r.head == 0xaa55 && check_data_r.end == 0xFFFF)
					{
						crc = cal_crc(file_ota.file_data,file_ota.end - file_ota.start + 1);              //对下载数据进行校验
						if(check_data_r.file_crc == crc)                                  //校验成功则写入数据
						{
							result = f_write(&dowm_ota_file,file_ota.file_data,file_ota.end - file_ota.start + 1,&b_write);
							if(FR_OK == result)
							{
								file_ota.write_byte += file_ota.end - file_ota.start + 1;
								if(file_ota.write_byte == file_size)
								{
									f_close(&dowm_ota_file);        //关闭下载文件
									f_close(&down_check_file);      //关闭校验文件
									file_ota.crc_err = 0;
									file_ota.repeated_req = 0;
									wifi_flow.file_down_step = 3;
								}
								else
								{
								}
							}	
							else	
							{}								
						}
						else
						{
							file_ota.crc_err ++;
							file_ota.repeated_req ++;
							vTaskDelay(500);
							if(file_ota.crc_err > 5)
							{
								set_screen(20);
								pop_tips(30,(u8*)WARNING,(u8*)DATA_LOAD_FAILED);        //数据加载失败，退出OTA。
				
								wifi_flow.file_down_step = 0;
								wifi_flow.ota_run_step = 0;
								wifi_flow.execution_step = 4;   //转转成UDP透传连接
								wifi_flow.sntp_cal = 1;         //执行一次NTP时间校准
								cfg_dft.init_step = 4;          //退出OTA下载
								return;
							}
						}
					}
				}
			}
			else
			{
				file_ota.repeated_req ++;
				vTaskDelay(500);
				if(file_ota.repeated_req > 3)
				{
					set_screen(20);
					pop_tips(30,(u8*)WARNING,(u8*)DATA_LOAD_FAILED);        //数据加载失败，退出OTA。

					wifi_flow.file_down_step = 0;
					wifi_flow.ota_run_step = 0;
					wifi_flow.execution_step = 4;   //转转成UDP透传连接
					wifi_flow.sntp_cal = 1;         //执行一次NTP时间校准
					cfg_dft.init_step = 4;          //退出OTA下载
					return;
				}				
			}
		}
		if(1)
		{
			float permil = 0,num1 = 0,num2 = 0;
			char per_buf[100] = {0};
			num1 = file_ota.write_byte * 1.0f;
			num2 = file_size *1.0f;
			permil = (num1/num2) * 100.0f;
			memset(per_buf,0x00,sizeof(per_buf));
			if( 0 == memcmp(s_fpath,MCU_OTA_FILE_PATH,strlen(MCU_OTA_FILE_PATH)) )
				sprintf(per_buf, "%s %.2f %%","MCU firmware download progress:",permil);
			else if( 0 == memcmp(s_fpath,UI_OTA_FILE_PATH,strlen(UI_OTA_FILE_PATH)) )
				sprintf(per_buf, "%s %.2f %%","UI firmware download progress:",permil);
			
			pop_tips(30,(u8*)"Loading",(u8*)per_buf);
		}
	}
	else if(wifi_flow.file_down_step == 3)
	{
		pop_tips(30,(u8*)"Loading",(u8*)"Firmware download complete!");
		
		wifi_flow.file_down_step = 4;
	}
}



//更新固化参数相关文件使用，down_path:下载路径  mem_path:本地内存路径
//返回值 0  成功  1数据请求错误  2 内存错误
u8 download_parameter_file(char* down_path,char* mem_path,u8 d_mode)
{
	FRESULT result = FR_OK;
	static FIL dowm_cure_file = {0};
	static s_file_download cure_file = {0};
	u32 write_len = 0,file_size = 0;
	UINT w;

	memset(&cure_file,0x00,sizeof(cure_file));
	if(d_mode == 1)                           //若是以太网传输 优先使用以太网
	{
		if( 0 == usr_file_header_request(cfg_dft.ali_domain_name,down_path,&cure_file.file_size,3000))   //请求文件信息，确认文件大小
		{
			result = f_open(&dowm_cure_file,mem_path,FA_OPEN_ALWAYS|FA_WRITE|FA_READ);            //打开的方式创建一个本地文件
			if(FR_OK == result)
			{
				cure_file.end = -1;                               //第一包从0开始
				while(cure_file.write_byte < cure_file.file_size)
				{
					if(cure_file.file_size - cure_file.write_byte >= DOWNLOAD_DATA_LEN)        //若剩余字节大于1k 则按1k大小写入
						write_len = DOWNLOAD_DATA_LEN;
					else
						write_len = cure_file.file_size - cure_file.write_byte;                //写入剩余大小
					
					if(cure_file.repeated_req == 0)
					{
						cure_file.start = cure_file.end + 1;
						cure_file.end = cure_file.start + write_len - 1;
					}
					if( 0 == usr_server_download_request(cfg_dft.ali_domain_name,down_path,\
						&cure_file.start,&cure_file.end,cure_file.file_data,&file_size,3000) )    //分段下载数据
					{
						result = f_write(&dowm_cure_file,cure_file.file_data,DOWNLOAD_DATA_LEN,&w);
						if(FR_OK == result)
						{
							cure_file.write_byte += write_len;
							cure_file.repeated_req = 0;
						}
						else
							return 2;                      //内存错误
					}
					else
					{
						cure_file.repeated_req++;
						if(cure_file.repeated_req > 5)
							return 1;                      //数据请求错误
					}
				}
			}
			else                                           //打开失败
				return 2;                                  //内存错误
			f_close(&dowm_cure_file);  
		}
		else
		{
			return 1;                      //数据请求错误
		}
	}
	else                                                    //若是使用WiFi传输
	{
		if( 0 == file_header_request(cfg_dft.ali_domain_name,down_path,&cure_file.file_size,3000))   //请求文件信息，确认文件大小
		{
			result = f_open(&dowm_cure_file,mem_path,FA_OPEN_ALWAYS|FA_WRITE|FA_READ);            //打开的方式创建一个本地文件
			if(FR_OK == result)
			{
				cure_file.end = -1;
				while(cure_file.write_byte < cure_file.file_size)
				{
					if(cure_file.file_size - cure_file.write_byte >= DOWNLOAD_DATA_LEN)        //若剩余字节大于1k 则按1k大小写入
						write_len = DOWNLOAD_DATA_LEN;
					else
						write_len = cure_file.file_size - cure_file.write_byte;                //写入剩余大小
					
					if(cure_file.repeated_req == 0)
					{
						cure_file.start = cure_file.end + 1;
						cure_file.end = cure_file.start + write_len - 1;
					}
					if( 0 == server_download_request(cfg_dft.ali_domain_name,down_path,\
						&cure_file.start,&cure_file.end,cure_file.file_data,&file_size,3000))    //分段下载数据
					{
						result = f_write(&dowm_cure_file,cure_file.file_data,write_len,&w);
						if(FR_OK == result)
						{
							cure_file.write_byte += write_len;
							cure_file.repeated_req = 0;
						}
						else
							return 2;                      //内存错误
					}
					else
					{
						cure_file.repeated_req++;
						if(cure_file.repeated_req > 5)
							return 1;                      //数据请求错误
					}
				}
			}
			else                                           //打开失败
				return 2;                                  //内存错误
			f_close(&dowm_cure_file);  
		}
		else
		{
			return 1;                      //数据请求错误
		}
	}
	return 0;
}


u8 update_cure_parameter_file(void)
{
	static FILINFO file_info = {0};
	FRESULT result = FR_OK;
	static FIL dowm_check_file = {0},dowm_file = {0};
	static u32 file_size = 0;      //文件总大小
	check_s check_data_w = {0},check_data_r = {0}; 
	UINT r;
	u8 read_file_buf[1024] = {0},res = 0;
	if( 0 == download_parameter_file(CURE_CHECK_FILE_PATH,MEM_CURE_CHECK_FILE_PATH,cfg_dft.network_selet) )	    //先下载固化参数的校验文件
	{
		result = f_stat(MEM_CURE_CHECK_FILE_PATH,&file_info);                         //确认下载的本地文件存在                    
		if(FR_OK == result)       //若本地内存存在文件
		{
			if(cfg_dft.network_selet)   //若是网络连接
				res = usr_file_header_request(cfg_dft.ali_domain_name,CURE_CHECK_FILE_PATH,&file_size,3000);
			else
				res = file_header_request(cfg_dft.ali_domain_name,CURE_CHECK_FILE_PATH,&file_size,3000);
			
			if( 0 == res )   //请求文件信息，确认文件大小
			{
				if(file_info.fsize == file_size)    //若文件大小正确
				{
					result = f_open(&dowm_check_file,MEM_CURE_CHECK_FILE_PATH,FA_READ);          //打开本地下载的校验文件
					if(FR_OK == result) 
					{
						while(!f_eof(&dowm_check_file))         //校验文件数据自身
						{
							memset(&check_data_r,0x00,sizeof(check_s));
							result = f_read(&dowm_check_file,&check_data_r,sizeof(check_s),&r);
							if(FR_OK == result)
							{
								u16 crc = 0;
								crc = cal_crc((u8*)&check_data_r,sizeof(check_s) - 4);
								if( check_data_r.head != 0xaa55 || check_data_r.end != 0xFFFF || check_data_r.self_crc !=  crc )
								{
									f_close(&dowm_check_file);                //校验成功 关闭文件
									return 1;
								}
							}
							else
							{
								f_close(&dowm_check_file);                    //校验成功 关闭文件
								return 1;
							}
							vTaskDelay(1);                                        //避免长时间占用cpu
						}
						f_close(&dowm_check_file);                //校验成功 关闭文件
						if( 0 == download_parameter_file(CURE_OTA_FILE_PATH,MEM_CURE_FILE_WRITE_PATH,cfg_dft.network_selet) )	    //下载固化文件
						{
							result = f_stat(MEM_CURE_FILE_WRITE_PATH,&file_info);                         //确认下载的本地文件存在                    
							if(FR_OK == result)       //若本地内存存在文件
							{
								if(cfg_dft.network_selet)   //若是网络连接
									res = usr_file_header_request(cfg_dft.ali_domain_name,CURE_OTA_FILE_PATH,&file_size,3000);     //请求头文件信息
								else
									res = file_header_request(cfg_dft.ali_domain_name,CURE_OTA_FILE_PATH,&file_size,3000);
								
								if( 0 == res )   //请求文件信息，确认文件大小
								{
									if(file_info.fsize == file_size)    //若文件大小正确
									{
										result = f_open(&dowm_file,MEM_CURE_FILE_WRITE_PATH,FA_READ);          //打开本地下载的参数文件
										if(FR_OK == result) 
										{
											result = f_open(&dowm_check_file,MEM_CURE_CHECK_FILE_PATH,FA_READ);          //打开本地下载的校验文件
											if(FR_OK == result) 
											{
												while(!f_eof(&dowm_file))                        //校验下载文件
												{
													memset(read_file_buf,0x00,sizeof(read_file_buf));
													result = f_read(&dowm_file,read_file_buf,sizeof(read_file_buf),&r);
													if(FR_OK == result)
													{
														check_data_w.file_crc = cal_crc(read_file_buf,r);            //对读取的文件做校验
														result = f_read(&dowm_check_file,&check_data_r,sizeof(check_s),&r);    //读取校验文件
														if(FR_OK == result)
														{
															if(check_data_r.file_crc != check_data_w.file_crc)
															{
																f_close(&dowm_file); 
																f_close(&dowm_check_file); 
																return 1;
															}
														}
													}
													vTaskDelay(1);                                        //避免长时间占用cpu
												}
												f_close(&dowm_file); 
												f_close(&dowm_check_file); 
												return 0;
											}
										}											
									}
								}
							}
						}
					}
				}
			}			
		}
	}
	f_close(&dowm_file); 
	f_close(&dowm_check_file); 
	return 1;	
}

//https://luxlink.oss-cn-beijing.aliyuncs.com/Cure/ILCP_UPGRADE_FILE/iLuxCurePro.bin
//https://luxlink.oss-cn-beijing.aliyuncs.com/Cure/ILCP_UPGRADE_FILE/ota.bin

//GET /Cure/ILCP_UPGRADE_FILE/iLuxCurePro.bin HTTP/1.1
//Host: lux-link-usa.oss-us-west-1.aliyuncs.com
//Range: bytes=0-99

































