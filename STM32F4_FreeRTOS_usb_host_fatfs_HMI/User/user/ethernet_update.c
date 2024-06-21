#include "ethernet_update.h"

ethernet enthernet_flow = {0};

void eth_update_process(void)
{
	if(enthernet_flow.update_step == 0)
	{
		u8 ret = 0;
		ret = usr_enter_at_mode();      //进入AT指令模式      
		ret += usr_set_tcp_server(eth_server_ip,PORT_NUMBER);    //设置OTA下载文件的服务器
		ret += usr_restart();           //重启模块
		if(ret == 0)
		{
			int i= 0;
			while(i++ < 10)
			{
				ret = usr_cal_rtc_time(cfg_dft.ali_domain_name,MCU_OTA_FILE_PATH,2000);
				if(ret == 0)
					break;
			}
			if(ret == 0)
			{
				enthernet_flow.update_step = 1;
				enthernet_flow.file_down_step = 0;
			}
			else
			{
				pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //数据请求失败，退出更新。
				//退出下载
				enthernet_flow.update_step = 0;
				enthernet_flow.execution_step = 1;  //重新连接上传的服务器
				cfg_dft.network_selet = 0;          //以太网连接状态重置
				cfg_dft.init_step = 4;              //执行上传
			}
		}
	}
	else if(enthernet_flow.update_step == 1)     //检查版本文件
	{
		FRESULT result = FR_OK;
		static FIL version_file = {0};
		UINT r;
		version_s v_r = {0};
		char txt_buf[50] = {0}; 
	
		eth_download_check_file(VERSION_FILE_PATH,MEM_VERSION_FILE_PATH); //下载一次版本文件
		if(enthernet_flow.file_down_step == 4)
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
//						u32 current_v = 0,latest_v = 0;
//						sscanf((const char*)cfg_dft.version,"V2.0.%d",&current_v);  //当前版本转换为整数
//						latest_v = atoi(v_r.v_buf);                                 //服务器存储的版本转换为整数
						
						set_screen(35);                //展示版本信息界面
						sprintf(txt_buf,"%s%s","Latest Version:V2.0.",v_r.v_buf);
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
								//退出下载
//								enthernet_flow.update_step = 0;
	
//								cfg_dft.init_step = 4;          //执行上传
//							}
					}
					else
					{
						f_close(&version_file);               //关闭文件
						f_unlink(MEM_VERSION_FILE_PATH);      //删除文件
						enthernet_flow.file_down_step = 0;    //重新下载
					}
				}
			}
			f_close(&version_file);      //关闭文件
			if(0 != update_cure_parameter_file())
			{
				pop_tips(30,(u8*)HMI_FAILED,(u8*)"Cure parameter update failed!");   //固化参数更新失败
			}
			enthernet_flow.update_step = 0xFF;          //等待屏幕UI操作
		}
		
		if(enthernet_flow.down_timeout ++ > 500)        //若请求超时
		{
			pop_tips(30,(u8*)WARNING,(u8*)REQUEST_TIMED_OUT);        //请求超时
			//退出下载
			enthernet_flow.update_step = 0;
			enthernet_flow.execution_step = 1;  //重新连接上传的服务器
			cfg_dft.network_selet = 0;          //以太网连接状态重置
			cfg_dft.init_step = 4;              //执行上传
		}
	}
	else if(enthernet_flow.update_step == 2)              //检测内存和清除
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
				pop_tips(30,(u8*)WARNING,(u8*)OTA_DOWNLOAD_MEM_ERROR);  //提示： 内存错误，无法进行OTA数据下载
				//退出下载
				enthernet_flow.update_step = 0;
				enthernet_flow.execution_step = 1;  //重新连接上传的服务器
				cfg_dft.network_selet = 0;          //以太网连接状态重置
				cfg_dft.init_step = 4;              //执行上传
				return;
			}
		}
		
		//HTTP 方式访问服务器，判断服务器是否存在更新文件若存在则得到文件大小 
		ota_file_number = 0;
		if( 0 == usr_file_header_request(cfg_dft.ali_domain_name,UI_OTA_FILE_PATH,&ui_file_totalsize,5000))
		{
			ota_file_number |= 1;
		}
		if( 0 == usr_file_header_request(cfg_dft.ali_domain_name,MCU_OTA_FILE_PATH,&mcu_file_totalsize,5000))
		{
			ota_file_number |= 2;
		}
		
		if( ota_file_number == 0 )
		{
			pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);            //数据请求失败，退出更新。
			//退出下载
			enthernet_flow.update_step = 0;
			enthernet_flow.execution_step = 1;  //重新连接上传的服务器
			cfg_dft.network_selet = 0;          //以太网连接状态重置
			cfg_dft.init_step = 4;              //执行上传
			return;
		}
		
		exf_getfree((u8*)"1:",&mem_total_size,&mem_free_size);              //得到磁盘剩余空间大小
		if( (mem_free_size*1024) < (ui_file_totalsize + mcu_file_totalsize) )
		{
			//内存空间不足
			pop_tips(30,(u8*)"Error",(u8*)MEM_SPACE_INSUFFF);
			enthernet_flow.update_step = 0;
			enthernet_flow.execution_step = 1;  //重新连接上传的服务器
			cfg_dft.network_selet = 0;          //以太网连接状态重置
			cfg_dft.init_step = 4;              //执行上传
			return;
		}

		enthernet_flow.update_step = 3;
		enthernet_flow.file_down_step = 0;
	}
	else if(enthernet_flow.update_step == 3)             
	{
		if(ota_file_number == 3 || ota_file_number == 2)
		{
			eth_download_check_file(MCU_CHECK_FILE_PATH,MEM_MCU_CHECK_FILE_PATH);    //通过以太网下载 MCU校验文件
			if(enthernet_flow.file_down_step == 4)
			{
				if(ota_file_number == 2)
				{
					enthernet_flow.update_step = 5;       //下载MCU文件
					enthernet_flow.file_down_step = 0;
				}
				if(ota_file_number == 3)
				{
					enthernet_flow.update_step = 4;       //下载UI校验文件
					enthernet_flow.file_down_step = 0;
				}
			}
		}
		else if(ota_file_number == 1)                //若只有UI文件
		{
			enthernet_flow.update_step = 4;          //下载UI校验文件
			enthernet_flow.file_down_step = 0;
		}
	}
	else if(enthernet_flow.update_step == 4)         //下载UI的校验文件
	{
		eth_download_check_file(UI_CHECK_FILE_PATH,MEM_UI_CHECK_FILE_PATH);
		if(enthernet_flow.file_down_step == 4)
		{
			enthernet_flow.file_down_step = 0;
			if(ota_file_number == 3)
				enthernet_flow.update_step = 5;      //下载MCU文件    
			else
				enthernet_flow.update_step = 6;      //下载UI文件  
		}
	}
	else if(enthernet_flow.update_step == 5)         //下载MCU文件
	{
		eth_download_update_file(MCU_OTA_FILE_PATH,MEM_MCU_WRITE_PATH,MEM_MCU_CHECK_FILE_PATH,mcu_file_totalsize); //下载MCU文件
		if(enthernet_flow.file_down_step == 4)
		{
			enthernet_flow.file_down_step = 0;
			if(ota_file_number == 2)                //若只有MCU文件则直接进入更新
				enthernet_flow.update_step = 7;
			if(ota_file_number == 3)                //若两个文件都有则继续下载UI文件
				enthernet_flow.update_step = 6;
		}
	}
	else if(enthernet_flow.update_step == 6)        //下载UI文件
	{
		eth_download_update_file(UI_OTA_FILE_PATH,MEM_UI_WRITE_PATH,MEM_UI_CHECK_FILE_PATH,ui_file_totalsize);        //下载UI文件
		if(enthernet_flow.file_down_step == 4)
		{
			enthernet_flow.file_down_step = 0;
			enthernet_flow.update_step = 7;
		}
	}
	else if(enthernet_flow.update_step == 7) 
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


////////////////////////////////////////////////////////////////////
//通过以太网下载固件的校验文件
void eth_download_check_file(char* s_fpath,char* mem_fpath)
{
	FRESULT result = FR_OK;
	static FIL dowm_check_file = {0};
	static s_file_download check_file = {0};
	UINT c_r, c_w;
	FILINFO check_fno = {0};
	
	if(enthernet_flow.file_down_step == 0)
	{
		result = f_stat(mem_fpath,&check_fno);                          //检测内存是否存在，若存在可能是上次下载被中断的残留
		if(FR_OK == result)
			f_unlink(mem_fpath);
		
		memset(&check_file,0x00,sizeof(s_file_download));
		memset(&dowm_check_file,0x00,sizeof(FIL));
		if( 0 != usr_file_header_request(cfg_dft.ali_domain_name,s_fpath,&check_file.file_size,5000))    //请求文件头部信息
		{
			pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //数据请求失败，退出更新。
			//退出下载
			enthernet_flow.update_step = 0;
			enthernet_flow.execution_step = 1;  //重新连接上传的服务器
			cfg_dft.network_selet = 0;          //以太网连接状态重置
			cfg_dft.init_step = 4;              //改为执行上传
			return;
		}
		enthernet_flow.file_down_step = 1;
	}
	else if(enthernet_flow.file_down_step == 1)        //第一包数据下载
	{
		check_file.start = 0;
		check_file.end = DOWNLOAD_DATA_LEN -1;

		if( check_file.file_size <= DOWNLOAD_DATA_LEN ) 
			check_file.end = check_file.file_size -1;
		
		if( 0 == usr_server_download_request(cfg_dft.ali_domain_name,s_fpath,\
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
							f_close(&dowm_check_file);             //关闭文件
							enthernet_flow.file_down_step = 3;     //执行校验步骤
							return;
						}
						else
						{
							f_close(&dowm_check_file);             //关闭文件
							f_unlink(mem_fpath);
							enthernet_flow.file_down_step = 0;             //重新下载
							return;
						}
					}
					else
					{
						f_close(&dowm_check_file);             //关闭文件
						f_unlink(mem_fpath);
						enthernet_flow.file_down_step = 0;             //重新下载
						return;						
					}					
				}
				else
				{
					result = f_write(&dowm_check_file,check_file.file_data,DOWNLOAD_DATA_LEN,&c_w);
					if(FR_OK == result)
					{
						check_file.write_byte = DOWNLOAD_DATA_LEN;
						enthernet_flow.file_down_step = 2;
						check_file.repeated_req = 0;
					}
					else
					{
						f_close(&dowm_check_file);             //关闭文件
						f_unlink(mem_fpath);
						enthernet_flow.file_down_step = 0;             //重新下载
						return;
					}
				}
			}
			else
			{
				f_close(&dowm_check_file);                     //关闭文件
				f_unlink(mem_fpath);
				enthernet_flow.file_down_step = 0;             //重新下载
				return;			
			}
		}
		else
		{
			check_file.repeated_req ++;
			if(check_file.repeated_req == 3)
			{
				if(0 == usr_enter_at_mode())      //进入AT指令模式
					usr_restart();	              //设备重启
			}
			if(check_file.repeated_req > 10)
			{
				pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //数据请求失败，退出更新。
				//退出下载
				enthernet_flow.update_step = 0;
				enthernet_flow.execution_step = 1;  //重新连接上传的服务器
				cfg_dft.network_selet = 0;          //以太网连接状态重置
				cfg_dft.init_step = 4;              //执行上传
			}
		}		
	}
	else if(enthernet_flow.file_down_step == 2)
	{
		if(check_file.repeated_req == 0)
		{
			check_file.start = check_file.end + 1;
			check_file.end = check_file.start + DOWNLOAD_DATA_LEN - 1;
		}
		if(check_file.end < check_file.file_size - 1)
		{
			if( 0 == usr_server_download_request(cfg_dft.ali_domain_name,s_fpath,\
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
				if(check_file.repeated_req == 3)
				{
					if(0 == usr_enter_at_mode())      //进入AT指令模式
						usr_restart();	              //设备重启
				}
				if(check_file.repeated_req > 10)
				{
					pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //数据请求失败，退出更新。
					//退出下载
					enthernet_flow.update_step = 0;
					enthernet_flow.execution_step = 1;  //重新连接上传的服务器
					cfg_dft.network_selet = 0;          //以太网连接状态重置
					cfg_dft.init_step = 4;              //执行上传
				}
			}
		}
		else   //若是最后一包
		{
			check_file.end = check_file.file_size - 1;
			if( 0 == usr_server_download_request(cfg_dft.ali_domain_name,s_fpath,\
				&check_file.start,&check_file.end,check_file.file_data,&check_file.file_size,3000)) 
			{
				result = f_write(&dowm_check_file,check_file.file_data,check_file.end - check_file.start + 1,&c_w);
				if(FR_OK == result)
				{
					check_file.write_byte += check_file.end - check_file.start + 1;
					if(check_file.write_byte == check_file.file_size)
					{
						f_close(&dowm_check_file);      //关闭文件
						enthernet_flow.file_down_step = 3;
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
				if(check_file.repeated_req == 3)
				{
					if(0 == usr_enter_at_mode())      //进入AT指令模式
						usr_restart();	              //设备重启
				}
				if(check_file.repeated_req > 10)
				{
					pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //数据请求失败，退出更新。
					//退出下载
					enthernet_flow.update_step = 0;
					enthernet_flow.execution_step = 1;  //重新连接上传的服务器
					cfg_dft.network_selet = 0;          //以太网连接状态重置
					cfg_dft.init_step = 4;              //执行上传
				}				
			}
		}
	}
	else if(enthernet_flow.file_down_step == 3)          //检查下载的文件
	{
		check_s check_data_r = {0}; 
		result = f_stat(mem_fpath,&check_fno);
		if(FR_OK == result)
		{
			if( check_fno.fsize != check_file.file_size )
			{
				f_close(&dowm_check_file);             //关闭文件
				f_unlink(mem_fpath);
				enthernet_flow.file_down_step = 0;             //重新下载
				return;
			}
		}
		else
		{
			enthernet_flow.file_down_step = 0;             //重新下载
			return;
		}
		
		result = f_open(&dowm_check_file,mem_fpath,FA_READ); //打开内存中下载的校验文件
		if(FR_OK != result)
		{
			f_close(&dowm_check_file);             //关闭文件
			f_unlink(mem_fpath);
			enthernet_flow.file_down_step = 0;             //重新下载
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
					enthernet_flow.file_down_step = 0;             //重新下载
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
						enthernet_flow.file_down_step = 0;             //重新下载
						return;
					}
				}
				else
				{
					f_close(&dowm_check_file);                //关闭文件
					f_unlink(mem_fpath);   
					enthernet_flow.file_down_step = 0;                     //重新下载
					return;
				}
				vTaskDelay(1);                                        //避免长时间占用cpu
			}
		}
		
		f_close(&dowm_check_file);      //关闭文件
		enthernet_flow.file_down_step = 4;
	}
}

//下载文件 s_fpath : 服务器文件下载路径   mem_fpath: 内存文件路径
void eth_download_update_file(char* s_fpath,char* mem_fpath,char* mem_check_fpath,u32 file_size)
{
	FRESULT result = FR_OK;
	static FIL dowm_ota_file = {0},down_check_file = {0};
	static s_file_download file_ota = {0};
	FILINFO mem_fno = {0};

	static check_s check_data_r = {0};                       //校验数据读
	u16 crc = 0;
	
	if(enthernet_flow.file_down_step == 0)
	{
		result = f_stat(MEM_FOLDER_PATH,&mem_fno);            //打开内存文件夹，若内存文件夹不存在则需要创建
		if( result != FR_OK )
		{
			result = f_mkdir(MEM_FOLDER_PATH);
			if( result != FR_OK )
			{
				set_screen(20);
				pop_tips(30,(u8*)"Error",(u8*)MEMORY_UPGRADE_ERR);
				//退出下载
				enthernet_flow.update_step = 0;
				enthernet_flow.execution_step = 1;  //重新连接上传的服务器
				cfg_dft.network_selet = 0;          //以太网连接状态重置
				cfg_dft.init_step = 4;              //执行上传
				return;
			}
		}
		memset(&file_ota,0x00,sizeof(s_file_download));
		memset(&dowm_ota_file,0x00,sizeof(FIL));
		enthernet_flow.file_down_step = 1;
	}
	else if(enthernet_flow.file_down_step == 1)        //第一包数据下载
	{
		file_ota.start = 0;
		file_ota.end = DOWNLOAD_DATA_LEN -1;
		if( 0 == usr_server_download_request(cfg_dft.ali_domain_name,s_fpath,\
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
						if(check_data_r.head == 0xaa55 && check_data_r.end == 0xFFFF)
						{
							crc = cal_crc(file_ota.file_data,DOWNLOAD_DATA_LEN);              //对下载数据进行校验
							if(check_data_r.file_crc == crc)                                  //校验成功则写入数据
							{
								result = f_open(&dowm_ota_file,mem_fpath,FA_OPEN_ALWAYS|FA_WRITE|FA_READ);       //创建OTA文件并写入下载数据
								if(FR_OK == result)
								{
									result = f_write(&dowm_ota_file,file_ota.file_data,DOWNLOAD_DATA_LEN,&b_write);
									if(FR_OK == result)
									{
										file_ota.write_byte = DOWNLOAD_DATA_LEN;
										enthernet_flow.file_down_step = 2;
										file_ota.repeated_req = 0;
										file_ota.crc_err = 0;
									}
								}								
							}
							else
							{
								f_close(&down_check_file);
								file_ota.crc_err ++;								
								file_ota.repeated_req ++;
								if(file_ota.crc_err > 10)
								{
									set_screen(20);
									pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //数据请求失败，退出更新。
									//退出下载
									enthernet_flow.update_step = 0;
									enthernet_flow.execution_step = 1;  //重新连接上传的服务器
									cfg_dft.network_selet = 0;          //以太网连接状态重置
									cfg_dft.init_step = 4;              //执行上传
									return;
								}
							}
						}
					}
				}
			}
		}
		else
		{
			file_ota.repeated_req ++;
			if(file_ota.repeated_req == 3)
			{
				if(0 == usr_enter_at_mode())      //进入AT指令模式
					usr_restart();	              //设备重启
			}
			if(file_ota.repeated_req > 10)
			{
				set_screen(20);
				pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //数据请求失败，退出更新。
				//退出下载
				enthernet_flow.update_step = 0;
				enthernet_flow.execution_step = 1;  //重新连接上传的服务器
				cfg_dft.network_selet = 0;          //以太网连接状态重置
				cfg_dft.init_step = 4;              //执行上传
				return;
			}
		}
	}
	else if(enthernet_flow.file_down_step == 2)
	{
		if(file_ota.repeated_req == 0)
		{
			file_ota.start = file_ota.end + 1;
			file_ota.end = file_ota.start + DOWNLOAD_DATA_LEN - 1;
		}
		if(file_ota.end < file_size - 1)
		{
			if( 0 == usr_server_download_request(cfg_dft.ali_domain_name,s_fpath,\
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
						crc = cal_crc(file_ota.file_data,DOWNLOAD_DATA_LEN);              //对下载数据进行校验
						if(check_data_r.file_crc == crc)                                  //校验成功则写入数据
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
							if(file_ota.repeated_req > 10)
							{
								f_close(&dowm_ota_file);        //关闭下载文件
								f_close(&down_check_file);      //关闭校验文件
								set_screen(20);
								pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //数据请求失败，退出更新。
								//退出下载
								enthernet_flow.update_step = 0;
								enthernet_flow.execution_step = 1;  //重新连接上传的服务器
								cfg_dft.network_selet = 0;          //以太网连接状态重置
								cfg_dft.init_step = 4;              //执行上传
								return;
							}
						}
					}
				}
			}
			else
			{
				file_ota.repeated_req ++;
				if(file_ota.repeated_req == 3)
				{
					if(0 == usr_enter_at_mode())      //进入AT指令模式
						usr_restart();	              //设备重启
				}
				if(file_ota.repeated_req > 10)
				{
					f_close(&down_check_file);       //关闭校验文件 
					f_close(&dowm_ota_file);         //关闭下载文件
					set_screen(20);
					pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //数据请求失败，退出更新。
					//退出下载
					enthernet_flow.update_step = 0;
					enthernet_flow.execution_step = 1;  //重新连接上传的服务器
					cfg_dft.network_selet = 0;          //以太网连接状态重置
					cfg_dft.init_step = 4;              //执行上传
					return;
				}
			}
		}
		else   //若是最后一包
		{
			file_ota.end = file_size - 1;
			if( 0 == usr_server_download_request(cfg_dft.ali_domain_name,s_fpath,\
				&file_ota.start,&file_ota.end,file_ota.file_data,&file_ota.file_size,3000)) 
			{				
				if(file_ota.repeated_req == 0)
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
									enthernet_flow.file_down_step = 3;
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
							file_ota.crc_err++;
							file_ota.repeated_req ++;

							if(file_ota.crc_err > 5)
							{
								f_close(&dowm_ota_file);        //关闭下载文件
								f_close(&down_check_file);      //关闭校验文件
								set_screen(20);
								pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //数据请求失败，退出更新。
								//退出下载
								enthernet_flow.update_step = 0;
								enthernet_flow.execution_step = 1;  //重新连接上传的服务器
								cfg_dft.network_selet = 0;          //以太网连接状态重置
								cfg_dft.init_step = 4;              //执行上传
								return;
							}
						}
					}
				}
			}
			else
			{
				file_ota.repeated_req ++;
				if(file_ota.repeated_req == 3)
				{
					if(0 == usr_enter_at_mode())      //进入AT指令模式
						usr_restart();	              //设备重启
				}
				if(file_ota.repeated_req > 10)
				{
					f_close(&dowm_ota_file);        //关闭下载文件
					f_close(&down_check_file);      //关闭校验文件
					set_screen(20);
					pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //数据请求失败，退出更新。
					//退出下载
					enthernet_flow.execution_step = 1;  //重新连接上传的服务器
					cfg_dft.network_selet = 0;          //以太网连接状态重置
					cfg_dft.init_step = 4;              //执行上传
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
				sprintf(per_buf, "%s %.2f %%","MCU file download progress:",permil);
			else if( 0 == memcmp(s_fpath,UI_OTA_FILE_PATH,strlen(UI_OTA_FILE_PATH)) )
				sprintf(per_buf, "%s %.2f %%","UI file download progress:",permil);
			
			pop_tips(30,(u8*)"Loading",(u8*)per_buf);
		}
	}
	else if(enthernet_flow.file_down_step == 3)
	{
		pop_tips(30,(u8*)"Loading",(u8*)"Firmware download complete!");
		
		enthernet_flow.file_down_step = 4;
	}
}







