#include "ethernet_update.h"

ethernet enthernet_flow = {0};

void eth_update_process(void)
{
	if(enthernet_flow.update_step == 0)
	{
		u8 ret = 0;
		ret = usr_enter_at_mode();      //����ATָ��ģʽ      
		ret += usr_set_tcp_server(eth_server_ip,PORT_NUMBER);    //����OTA�����ļ��ķ�����
		ret += usr_restart();           //����ģ��
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
				pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //��������ʧ�ܣ��˳����¡�
				//�˳�����
				enthernet_flow.update_step = 0;
				enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
				cfg_dft.network_selet = 0;          //��̫������״̬����
				cfg_dft.init_step = 4;              //ִ���ϴ�
			}
		}
	}
	else if(enthernet_flow.update_step == 1)     //���汾�ļ�
	{
		FRESULT result = FR_OK;
		static FIL version_file = {0};
		UINT r;
		version_s v_r = {0};
		char txt_buf[50] = {0}; 
	
		eth_download_check_file(VERSION_FILE_PATH,MEM_VERSION_FILE_PATH); //����һ�ΰ汾�ļ�
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
//						sscanf((const char*)cfg_dft.version,"V2.0.%d",&current_v);  //��ǰ�汾ת��Ϊ����
//						latest_v = atoi(v_r.v_buf);                                 //�������洢�İ汾ת��Ϊ����
						
						set_screen(35);                //չʾ�汾��Ϣ����
						sprintf(txt_buf,"%s%s","Latest Version:V2.0.",v_r.v_buf);
						SetTextValue(35,6,(unsigned char*)txt_buf);
						memset(txt_buf,0x00,sizeof(txt_buf));
						sprintf(txt_buf,"%s%s","Current Version:",cfg_dft.version);
						SetTextValue(35,8,(unsigned char*)txt_buf); 
						SetTextValue(37,3,(unsigned char*)v_r.release_note); 
//							if(latest_v > current_v)       //�������°汾
//							{
//								set_screen(35);            //չʾ�汾��Ϣ
//								sprintf(txt_buf,"%s%s","Latest Version:V2.0.",v_r.v_buf);
//								SetTextValue(35,6,(unsigned char*)txt_buf);
//								memset(txt_buf,0x00,sizeof(txt_buf));
//								sprintf(txt_buf,"%s%s","Current Version:",cfg_dft.version);
//								SetTextValue(35,6,(unsigned char*)txt_buf); 
//							}
//							else
//							{
//								set_screen(36);                 //��ʾ��ǰ�������°汾
								//�˳�����
//								enthernet_flow.update_step = 0;
	
//								cfg_dft.init_step = 4;          //ִ���ϴ�
//							}
					}
					else
					{
						f_close(&version_file);               //�ر��ļ�
						f_unlink(MEM_VERSION_FILE_PATH);      //ɾ���ļ�
						enthernet_flow.file_down_step = 0;    //��������
					}
				}
			}
			f_close(&version_file);      //�ر��ļ�
			if(0 != update_cure_parameter_file())
			{
				pop_tips(30,(u8*)HMI_FAILED,(u8*)"Cure parameter update failed!");   //�̻���������ʧ��
			}
			enthernet_flow.update_step = 0xFF;          //�ȴ���ĻUI����
		}
		
		if(enthernet_flow.down_timeout ++ > 500)        //������ʱ
		{
			pop_tips(30,(u8*)WARNING,(u8*)REQUEST_TIMED_OUT);        //����ʱ
			//�˳�����
			enthernet_flow.update_step = 0;
			enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
			cfg_dft.network_selet = 0;          //��̫������״̬����
			cfg_dft.init_step = 4;              //ִ���ϴ�
		}
	}
	else if(enthernet_flow.update_step == 2)              //����ڴ�����
	{	
		FRESULT result = FR_OK;
		FILINFO fno = {0};
		u32 mem_total_size = 0,mem_free_size = 0;
		
		result = f_stat(MEM_FOLDER_PATH,&fno);                   //����ڴ��Ƿ���ڸ����ļ��������ڿ������ϴ����ر��жϵĲ���
		if(FR_OK == result)
		{
			result = DeleteDirectoryFiles(MEM_FOLDER_PATH);      //�ݹ�ɾ���ļ����Լ�·�����ļ�
			if(FR_OK != result)
			{
				pop_tips(30,(u8*)WARNING,(u8*)OTA_DOWNLOAD_MEM_ERROR);  //��ʾ�� �ڴ�����޷�����OTA��������
				//�˳�����
				enthernet_flow.update_step = 0;
				enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
				cfg_dft.network_selet = 0;          //��̫������״̬����
				cfg_dft.init_step = 4;              //ִ���ϴ�
				return;
			}
		}
		
		//HTTP ��ʽ���ʷ��������жϷ������Ƿ���ڸ����ļ���������õ��ļ���С 
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
			pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);            //��������ʧ�ܣ��˳����¡�
			//�˳�����
			enthernet_flow.update_step = 0;
			enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
			cfg_dft.network_selet = 0;          //��̫������״̬����
			cfg_dft.init_step = 4;              //ִ���ϴ�
			return;
		}
		
		exf_getfree((u8*)"1:",&mem_total_size,&mem_free_size);              //�õ�����ʣ��ռ��С
		if( (mem_free_size*1024) < (ui_file_totalsize + mcu_file_totalsize) )
		{
			//�ڴ�ռ䲻��
			pop_tips(30,(u8*)"Error",(u8*)MEM_SPACE_INSUFFF);
			enthernet_flow.update_step = 0;
			enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
			cfg_dft.network_selet = 0;          //��̫������״̬����
			cfg_dft.init_step = 4;              //ִ���ϴ�
			return;
		}

		enthernet_flow.update_step = 3;
		enthernet_flow.file_down_step = 0;
	}
	else if(enthernet_flow.update_step == 3)             
	{
		if(ota_file_number == 3 || ota_file_number == 2)
		{
			eth_download_check_file(MCU_CHECK_FILE_PATH,MEM_MCU_CHECK_FILE_PATH);    //ͨ����̫������ MCUУ���ļ�
			if(enthernet_flow.file_down_step == 4)
			{
				if(ota_file_number == 2)
				{
					enthernet_flow.update_step = 5;       //����MCU�ļ�
					enthernet_flow.file_down_step = 0;
				}
				if(ota_file_number == 3)
				{
					enthernet_flow.update_step = 4;       //����UIУ���ļ�
					enthernet_flow.file_down_step = 0;
				}
			}
		}
		else if(ota_file_number == 1)                //��ֻ��UI�ļ�
		{
			enthernet_flow.update_step = 4;          //����UIУ���ļ�
			enthernet_flow.file_down_step = 0;
		}
	}
	else if(enthernet_flow.update_step == 4)         //����UI��У���ļ�
	{
		eth_download_check_file(UI_CHECK_FILE_PATH,MEM_UI_CHECK_FILE_PATH);
		if(enthernet_flow.file_down_step == 4)
		{
			enthernet_flow.file_down_step = 0;
			if(ota_file_number == 3)
				enthernet_flow.update_step = 5;      //����MCU�ļ�    
			else
				enthernet_flow.update_step = 6;      //����UI�ļ�  
		}
	}
	else if(enthernet_flow.update_step == 5)         //����MCU�ļ�
	{
		eth_download_update_file(MCU_OTA_FILE_PATH,MEM_MCU_WRITE_PATH,MEM_MCU_CHECK_FILE_PATH,mcu_file_totalsize); //����MCU�ļ�
		if(enthernet_flow.file_down_step == 4)
		{
			enthernet_flow.file_down_step = 0;
			if(ota_file_number == 2)                //��ֻ��MCU�ļ���ֱ�ӽ������
				enthernet_flow.update_step = 7;
			if(ota_file_number == 3)                //�������ļ��������������UI�ļ�
				enthernet_flow.update_step = 6;
		}
	}
	else if(enthernet_flow.update_step == 6)        //����UI�ļ�
	{
		eth_download_update_file(UI_OTA_FILE_PATH,MEM_UI_WRITE_PATH,MEM_UI_CHECK_FILE_PATH,ui_file_totalsize);        //����UI�ļ�
		if(enthernet_flow.file_down_step == 4)
		{
			enthernet_flow.file_down_step = 0;
			enthernet_flow.update_step = 7;
		}
	}
	else if(enthernet_flow.update_step == 7) 
	{
		save_config.upgrade_state = 2;    //�ڴ���±�־
		if( 0 == cfg_save() )
		{
			vTaskDelay(100);
			Sys_Soft_Reset();           //ϵͳ��λ
		}
		else
		{
			//�ڴ�����޷�����
			printf("app:Upgrade write error!\r\n");  
			set_screen(20);
			pop_tips(30,(u8*)"Error",(u8*)MEM_WRITE_UPGRADE_ERROR);
		}
	}
}


////////////////////////////////////////////////////////////////////
//ͨ����̫�����ع̼���У���ļ�
void eth_download_check_file(char* s_fpath,char* mem_fpath)
{
	FRESULT result = FR_OK;
	static FIL dowm_check_file = {0};
	static s_file_download check_file = {0};
	UINT c_r, c_w;
	FILINFO check_fno = {0};
	
	if(enthernet_flow.file_down_step == 0)
	{
		result = f_stat(mem_fpath,&check_fno);                          //����ڴ��Ƿ���ڣ������ڿ������ϴ����ر��жϵĲ���
		if(FR_OK == result)
			f_unlink(mem_fpath);
		
		memset(&check_file,0x00,sizeof(s_file_download));
		memset(&dowm_check_file,0x00,sizeof(FIL));
		if( 0 != usr_file_header_request(cfg_dft.ali_domain_name,s_fpath,&check_file.file_size,5000))    //�����ļ�ͷ����Ϣ
		{
			pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //��������ʧ�ܣ��˳����¡�
			//�˳�����
			enthernet_flow.update_step = 0;
			enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
			cfg_dft.network_selet = 0;          //��̫������״̬����
			cfg_dft.init_step = 4;              //��Ϊִ���ϴ�
			return;
		}
		enthernet_flow.file_down_step = 1;
	}
	else if(enthernet_flow.file_down_step == 1)        //��һ����������
	{
		check_file.start = 0;
		check_file.end = DOWNLOAD_DATA_LEN -1;

		if( check_file.file_size <= DOWNLOAD_DATA_LEN ) 
			check_file.end = check_file.file_size -1;
		
		if( 0 == usr_server_download_request(cfg_dft.ali_domain_name,s_fpath,\
			&check_file.start,&check_file.end,check_file.file_data,&check_file.file_size,3000)) 
		{
			result = f_open(&dowm_check_file,mem_fpath,FA_OPEN_ALWAYS|FA_WRITE|FA_READ);      //���ڴ�·�����½�һ��У���ļ�
			if(FR_OK == result)
			{
				if( check_file.file_size <= DOWNLOAD_DATA_LEN )    //���ļ��ܴ�С����1k
				{
					result = f_write(&dowm_check_file,check_file.file_data,check_file.end - check_file.start + 1,&c_w);
					if(FR_OK == result)
					{
						check_file.write_byte += check_file.end - check_file.start + 1;
						if(check_file.write_byte == check_file.file_size)
						{
							f_close(&dowm_check_file);             //�ر��ļ�
							enthernet_flow.file_down_step = 3;     //ִ��У�鲽��
							return;
						}
						else
						{
							f_close(&dowm_check_file);             //�ر��ļ�
							f_unlink(mem_fpath);
							enthernet_flow.file_down_step = 0;             //��������
							return;
						}
					}
					else
					{
						f_close(&dowm_check_file);             //�ر��ļ�
						f_unlink(mem_fpath);
						enthernet_flow.file_down_step = 0;             //��������
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
						f_close(&dowm_check_file);             //�ر��ļ�
						f_unlink(mem_fpath);
						enthernet_flow.file_down_step = 0;             //��������
						return;
					}
				}
			}
			else
			{
				f_close(&dowm_check_file);                     //�ر��ļ�
				f_unlink(mem_fpath);
				enthernet_flow.file_down_step = 0;             //��������
				return;			
			}
		}
		else
		{
			check_file.repeated_req ++;
			if(check_file.repeated_req == 3)
			{
				if(0 == usr_enter_at_mode())      //����ATָ��ģʽ
					usr_restart();	              //�豸����
			}
			if(check_file.repeated_req > 10)
			{
				pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //��������ʧ�ܣ��˳����¡�
				//�˳�����
				enthernet_flow.update_step = 0;
				enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
				cfg_dft.network_selet = 0;          //��̫������״̬����
				cfg_dft.init_step = 4;              //ִ���ϴ�
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
					if(0 == usr_enter_at_mode())      //����ATָ��ģʽ
						usr_restart();	              //�豸����
				}
				if(check_file.repeated_req > 10)
				{
					pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //��������ʧ�ܣ��˳����¡�
					//�˳�����
					enthernet_flow.update_step = 0;
					enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
					cfg_dft.network_selet = 0;          //��̫������״̬����
					cfg_dft.init_step = 4;              //ִ���ϴ�
				}
			}
		}
		else   //�������һ��
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
						f_close(&dowm_check_file);      //�ر��ļ�
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
					if(0 == usr_enter_at_mode())      //����ATָ��ģʽ
						usr_restart();	              //�豸����
				}
				if(check_file.repeated_req > 10)
				{
					pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //��������ʧ�ܣ��˳����¡�
					//�˳�����
					enthernet_flow.update_step = 0;
					enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
					cfg_dft.network_selet = 0;          //��̫������״̬����
					cfg_dft.init_step = 4;              //ִ���ϴ�
				}				
			}
		}
	}
	else if(enthernet_flow.file_down_step == 3)          //������ص��ļ�
	{
		check_s check_data_r = {0}; 
		result = f_stat(mem_fpath,&check_fno);
		if(FR_OK == result)
		{
			if( check_fno.fsize != check_file.file_size )
			{
				f_close(&dowm_check_file);             //�ر��ļ�
				f_unlink(mem_fpath);
				enthernet_flow.file_down_step = 0;             //��������
				return;
			}
		}
		else
		{
			enthernet_flow.file_down_step = 0;             //��������
			return;
		}
		
		result = f_open(&dowm_check_file,mem_fpath,FA_READ); //���ڴ������ص�У���ļ�
		if(FR_OK != result)
		{
			f_close(&dowm_check_file);             //�ر��ļ�
			f_unlink(mem_fpath);
			enthernet_flow.file_down_step = 0;             //��������
			return;
		}
		//�ж��ļ�������Ϊ txt��ʽ�İ汾�ļ�
		if( 0 == memcmp(mem_fpath,MEM_VERSION_FILE_PATH,sizeof(MEM_VERSION_FILE_PATH)) )   //���ǰ汾��Ϣ�ļ�
		{
			version_s version_r = {0};
			result = f_read(&dowm_check_file,&version_r,sizeof(version_s),&c_r);
			if(FR_OK == result)
			{
				u16 crc = 0;
				crc = cal_crc((u8*)&version_r,sizeof(version_r) - 4);
				if(version_r.head != 0xaa55 || version_r.end != 0xFFFF || version_r.self_crc != crc)
				{
					f_close(&dowm_check_file);                //�ر��ļ�
					f_unlink(mem_fpath);                      //ɾ���ļ�   
					enthernet_flow.file_down_step = 0;             //��������
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
						f_close(&dowm_check_file);                //�ر��ļ�
						f_unlink(mem_fpath);   
						enthernet_flow.file_down_step = 0;             //��������
						return;
					}
				}
				else
				{
					f_close(&dowm_check_file);                //�ر��ļ�
					f_unlink(mem_fpath);   
					enthernet_flow.file_down_step = 0;                     //��������
					return;
				}
				vTaskDelay(1);                                        //���ⳤʱ��ռ��cpu
			}
		}
		
		f_close(&dowm_check_file);      //�ر��ļ�
		enthernet_flow.file_down_step = 4;
	}
}

//�����ļ� s_fpath : �������ļ�����·��   mem_fpath: �ڴ��ļ�·��
void eth_download_update_file(char* s_fpath,char* mem_fpath,char* mem_check_fpath,u32 file_size)
{
	FRESULT result = FR_OK;
	static FIL dowm_ota_file = {0},down_check_file = {0};
	static s_file_download file_ota = {0};
	FILINFO mem_fno = {0};

	static check_s check_data_r = {0};                       //У�����ݶ�
	u16 crc = 0;
	
	if(enthernet_flow.file_down_step == 0)
	{
		result = f_stat(MEM_FOLDER_PATH,&mem_fno);            //���ڴ��ļ��У����ڴ��ļ��в���������Ҫ����
		if( result != FR_OK )
		{
			result = f_mkdir(MEM_FOLDER_PATH);
			if( result != FR_OK )
			{
				set_screen(20);
				pop_tips(30,(u8*)"Error",(u8*)MEMORY_UPGRADE_ERR);
				//�˳�����
				enthernet_flow.update_step = 0;
				enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
				cfg_dft.network_selet = 0;          //��̫������״̬����
				cfg_dft.init_step = 4;              //ִ���ϴ�
				return;
			}
		}
		memset(&file_ota,0x00,sizeof(s_file_download));
		memset(&dowm_ota_file,0x00,sizeof(FIL));
		enthernet_flow.file_down_step = 1;
	}
	else if(enthernet_flow.file_down_step == 1)        //��һ����������
	{
		file_ota.start = 0;
		file_ota.end = DOWNLOAD_DATA_LEN -1;
		if( 0 == usr_server_download_request(cfg_dft.ali_domain_name,s_fpath,\
			&file_ota.start,&file_ota.end,file_ota.file_data,&file_ota.file_size,3000)) 
		{
			if(file_ota.file_size == file_size)
			{
				result = f_open(&down_check_file,mem_check_fpath,FA_READ);      //���ڴ����ص�У���ļ�
				if(FR_OK == result)
				{		
					result = f_read(&down_check_file,&check_data_r,sizeof(check_s),&b_read);
					if(FR_OK == result)
					{
						if(check_data_r.head == 0xaa55 && check_data_r.end == 0xFFFF)
						{
							crc = cal_crc(file_ota.file_data,DOWNLOAD_DATA_LEN);              //���������ݽ���У��
							if(check_data_r.file_crc == crc)                                  //У��ɹ���д������
							{
								result = f_open(&dowm_ota_file,mem_fpath,FA_OPEN_ALWAYS|FA_WRITE|FA_READ);       //����OTA�ļ���д����������
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
									pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //��������ʧ�ܣ��˳����¡�
									//�˳�����
									enthernet_flow.update_step = 0;
									enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
									cfg_dft.network_selet = 0;          //��̫������״̬����
									cfg_dft.init_step = 4;              //ִ���ϴ�
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
				if(0 == usr_enter_at_mode())      //����ATָ��ģʽ
					usr_restart();	              //�豸����
			}
			if(file_ota.repeated_req > 10)
			{
				set_screen(20);
				pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //��������ʧ�ܣ��˳����¡�
				//�˳�����
				enthernet_flow.update_step = 0;
				enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
				cfg_dft.network_selet = 0;          //��̫������״̬����
				cfg_dft.init_step = 4;              //ִ���ϴ�
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
					result = f_read(&down_check_file,&check_data_r,sizeof(check_s),&b_read);   //��ȡУ���ļ�
				}
				else
				{
					result = FR_OK;  
				}
				
				if(FR_OK == result)
				{
					if(check_data_r.head == 0xaa55 && check_data_r.end == 0xFFFF)
					{
						crc = cal_crc(file_ota.file_data,DOWNLOAD_DATA_LEN);              //���������ݽ���У��
						if(check_data_r.file_crc == crc)                                  //У��ɹ���д������
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
								f_close(&dowm_ota_file);        //�ر������ļ�
								f_close(&down_check_file);      //�ر�У���ļ�
								set_screen(20);
								pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //��������ʧ�ܣ��˳����¡�
								//�˳�����
								enthernet_flow.update_step = 0;
								enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
								cfg_dft.network_selet = 0;          //��̫������״̬����
								cfg_dft.init_step = 4;              //ִ���ϴ�
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
					if(0 == usr_enter_at_mode())      //����ATָ��ģʽ
						usr_restart();	              //�豸����
				}
				if(file_ota.repeated_req > 10)
				{
					f_close(&down_check_file);       //�ر�У���ļ� 
					f_close(&dowm_ota_file);         //�ر������ļ�
					set_screen(20);
					pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //��������ʧ�ܣ��˳����¡�
					//�˳�����
					enthernet_flow.update_step = 0;
					enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
					cfg_dft.network_selet = 0;          //��̫������״̬����
					cfg_dft.init_step = 4;              //ִ���ϴ�
					return;
				}
			}
		}
		else   //�������һ��
		{
			file_ota.end = file_size - 1;
			if( 0 == usr_server_download_request(cfg_dft.ali_domain_name,s_fpath,\
				&file_ota.start,&file_ota.end,file_ota.file_data,&file_ota.file_size,3000)) 
			{				
				if(file_ota.repeated_req == 0)
				{
					result = f_read(&down_check_file,&check_data_r,sizeof(check_s),&b_read);   //��ȡУ���ļ�
				}
				else
				{
					result = FR_OK;  
				}
				if(FR_OK == result)
				{
					if(check_data_r.head == 0xaa55 && check_data_r.end == 0xFFFF)
					{
						crc = cal_crc(file_ota.file_data,file_ota.end - file_ota.start + 1);              //���������ݽ���У��
						if(check_data_r.file_crc == crc)                                  //У��ɹ���д������
						{
							result = f_write(&dowm_ota_file,file_ota.file_data,file_ota.end - file_ota.start + 1,&b_write);
							if(FR_OK == result)
							{
								file_ota.write_byte += file_ota.end - file_ota.start + 1;
								if(file_ota.write_byte == file_size)
								{
									f_close(&dowm_ota_file);        //�ر������ļ�
									f_close(&down_check_file);      //�ر�У���ļ�
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
								f_close(&dowm_ota_file);        //�ر������ļ�
								f_close(&down_check_file);      //�ر�У���ļ�
								set_screen(20);
								pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //��������ʧ�ܣ��˳����¡�
								//�˳�����
								enthernet_flow.update_step = 0;
								enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
								cfg_dft.network_selet = 0;          //��̫������״̬����
								cfg_dft.init_step = 4;              //ִ���ϴ�
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
					if(0 == usr_enter_at_mode())      //����ATָ��ģʽ
						usr_restart();	              //�豸����
				}
				if(file_ota.repeated_req > 10)
				{
					f_close(&dowm_ota_file);        //�ر������ļ�
					f_close(&down_check_file);      //�ر�У���ļ�
					set_screen(20);
					pop_tips(30,(u8*)WARNING,(u8*)ETHERNET_UPDATE_FAIL);        //��������ʧ�ܣ��˳����¡�
					//�˳�����
					enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
					cfg_dft.network_selet = 0;          //��̫������״̬����
					cfg_dft.init_step = 4;              //ִ���ϴ�
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







