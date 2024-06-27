#include "ui_upgrade.h"

upgrade_stat updata_state = {0};

//屏幕采用大端  需要注意大小端转换
ota_cmd upgrade_cmd =
{
	.head = CHANGE_END16(0xEEB6),
	.len = CHANGE_END16(17),
	.cmd = CHANGE_END16(0x8811),
	.baudrate = CHANGE_END32((u32)LCD_OTA_BAUDRATE),   //串口更新时串口波特率
	.filesize = CHANGE_END16(0),
	.down_name = "ota.bin",
	.end = CHANGE_END32(FEND)
};

ota_updata ui_updata =
{
	.head = CHANGE_END16(0xEEB6),
	.len = CHANGE_END16(0),
	.cmd = CHANGE_END16(0x8822),
	.sn = CHANGE_END16(0),
	.end = CHANGE_END32(FEND)
};


void ui_updata_tx(u8 *p,int n)
{
	uart_send(p,n,&uart6);
}

void set_lcd_baudrate(u32 b)
{
	uart_initial(&uart6,b);
}


//void read_file_test(void)
//{
//	static int a = 0;
//	static FIL ota_file;
//	static UINT ota_r;
//	DSTATUS fr;
//	fr = f_open(&ota_file,U_DISK_UI_FILE_PATH,FA_READ);
//	if(FR_OK == fr)
//	{
//		while( f_eof(&ota_file) == 0 )
//		{
//			fr = f_read(&ota_file,ui_updata.packet,OTA_WRITE_BYTE,&ota_r);
//			if(FR_OK == fr)
//			{
//				a ++;
//			}
//		}
//		f_close(&ota_file); 
//		updata_state.update_status_flow = 0;
//	}
//}

extern char* ui_upgrade_file_path;
extern char* mcu_upgrade_file_path;
//给屏幕更新  更新成功返回 0 失败返回 1
int upgrade_ui(void)
{
	static FIL ota_file;
	static UINT ota_r;
	DSTATUS fr;

	switch(updata_state.ui_ota_state)
	{
		case 0: 
			fr = f_open(&ota_file,ui_upgrade_file_path,FA_READ);
			printf("ui_open_ota_file fr = %d\r\n",fr);
			if(FR_OK == fr)
			{
				upgrade_cmd.filesize = CHANGE_END32( f_size(&ota_file) );
				ui_updata_tx((u8*)&upgrade_cmd,sizeof(ota_cmd));
				
//				uart_send((u8*)&upgrade_cmd,sizeof(ota_cmd),&uart1);  //测试串口
				while(USART_GetFlagStatus(USART6,USART_FLAG_TXE)==RESET);
				while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET);
				
				set_lcd_baudrate(LCD_OTA_BAUDRATE);    //设置串口波特率921600
				
//				bsp_DelayMS(1000);   //延迟两秒
//				//再发送一次
//				upgrade_cmd.filesize = CHANGE_END32( f_size(&ota_file) );
//				ui_updata_tx((u8*)&upgrade_cmd,sizeof(ota_cmd));				
//				uart_send((u8*)&upgrade_cmd,sizeof(ota_cmd),&uart1); 
				
				updata_state.ui_ota_state = 1;
				updata_state.ota_ack = OTA_WAIT_ACK_IDLE;
			}
			else
			{
				f_close(&ota_file); 
				//文件打开失败
				pop_tips(28,(u8*)TIPS,(u8*) "open file failed!");
				printf("%s  open file failed!",ui_upgrade_file_path);
				updata_state.update_status_flow = 0;
				while(1);
//				return 1;
			}
			break;
		case 1:
			if(updata_state.ota_ack == OTA_WAIT_START_ACK)      //启动更新应答允许
			{
				//启动更新
				memset(ui_updata.packet,0x00,sizeof(ui_updata.packet));
				fr = f_read(&ota_file,ui_updata.packet,OTA_WRITE_BYTE,&ota_r);
				if(FR_OK == fr)
				{
					ui_updata.len = CHANGE_END16(517);   //2 + 1 + 512 + 2
					ui_updata.sn = 0;
					ui_updata.checksum = check_sum16((u8*)&ui_updata.head + 1,6 + OTA_WRITE_BYTE) - ui_updata.sn;
					ui_updata.checksum =  ~CHANGE_END16(ui_updata.checksum);
					ui_updata_tx((u8*)&ui_updata,sizeof(ui_updata));
									
					updata_state.ui_ota_state = 2;
				}

				updata_state.timeout = 0;
				updata_state.sent_cnt = 0;
				updata_state.ota_ack = OTA_WAIT_ACK_IDLE;
			}
			else
			{
				updata_state.timeout++;
				if(updata_state.sent_cnt < 3)
				{
					if(updata_state.timeout%100 == 1 && updata_state.timeout > 100)
					{
						updata_state.sent_cnt++;
						upgrade_cmd.filesize = CHANGE_END32( f_size(&ota_file) );
						ui_updata_tx((u8*)&upgrade_cmd,sizeof(ota_cmd));				
//						uart_send((u8*)&upgrade_cmd,sizeof(ota_cmd),&uart1); 
					}
				}
				if(updata_state.timeout > 500)
				{
					f_close(&ota_file); 
					pop_tips(28,(u8*)TIPS,(u8*) "UI update response timed out!");
					updata_state.update_status_flow = 0;
					while(1);
//					return 1;
				}
			}
			break;
		case 2:
			if(updata_state.ota_ack == OTA_WAIT_DOWNLOAD_ACK)
			{
				if( f_eof(&ota_file) == 0 )
				{
					memset(ui_updata.packet,0x00,sizeof(ui_updata.packet));
					fr = f_read(&ota_file,ui_updata.packet,OTA_WRITE_BYTE,&ota_r); //读取一帧数据
					if(FR_OK == fr)
					{
						if(ota_r < 0x200)   //若是最后一帧不足512字节单独组包
						{
							u8 end_fream[600]= {0xEE,0xB6,0x00,0x00,0x88,0x22};
							ui_updata.len = CHANGE_END16(5 + ota_r);
							ui_updata.sn++;
							memcpy(end_fream,(u8*)&ui_updata,7+ota_r);
							*(u16*)&end_fream[7+ota_r] = check_sum16(end_fream+1,6+ota_r);
							*(u16*)&end_fream[7+ota_r] = ~CHANGE_END16(*(u16*)&end_fream[7+ota_r]);
							*(u32*)&end_fream[7+ota_r+2] = CHANGE_END32(FEND);
							ui_updata_tx(end_fream,7+ota_r+2+4);
						}
						else
						{
							ui_updata.len = CHANGE_END16(517);   //2 + 1 + 512 + 2
							ui_updata.sn ++;
							ui_updata.checksum = check_sum16((u8*)&ui_updata.head + 1,6 + OTA_WRITE_BYTE);
							ui_updata.checksum =  ~CHANGE_END16(ui_updata.checksum);
							ui_updata_tx((u8*)&ui_updata,sizeof(ui_updata));
						}
					}
					else
					{
						return 1;
					}
				}
				else
				{
					//更新数据完成 给屏幕发送升级命令
					u8 ui_upgrade_cmd[] = {0xEE,0xB6,0x00,0x03,0x88,0x33,0x01,0xFF,0xFC,0xFF,0xFF};
					ui_updata_tx((u8*)ui_upgrade_cmd,sizeof(ui_upgrade_cmd));
					f_close(&ota_file); 
					updata_state.ui_ota_state = 3;
				}
				updata_state.ota_ack = OTA_WAIT_ACK_IDLE;
				updata_state.timeout = 0;
				updata_state.sent_cnt = 0;
			}
			else if(updata_state.ota_ack == UI_UPDATA_SN_ERR)  //SN错误
			{
				updata_state.ota_ack = OTA_WAIT_ACK_IDLE;
				if(updata_state.sent_cnt++ < 5)
				{
					if(ota_r < 0x200)                         //若是最后一帧不足512字节单独组包
					{
						u8 end_fream[600]= {0xEE,0xB6,0x00,0x00,0x88,0x22};
						ui_updata.len = CHANGE_END16(5 + ota_r);
//						ui_updata.sn++;
						ui_updata.sn = ota_file.fptr/512 - 2 + updata_state.sent_cnt;
						memcpy(end_fream,(u8*)&ui_updata,7+ota_r);
						*(u16*)&end_fream[7+ota_r] = check_sum16(end_fream+1,6+ota_r);
						*(u16*)&end_fream[7+ota_r] = ~CHANGE_END16(*(u16*)&end_fream[7+ota_r]);
						*(u32*)&end_fream[7+ota_r+2] = CHANGE_END32(FEND);
						ui_updata_tx(end_fream,7+ota_r+2+4);
					}
					else
					{
						ui_updata.len = CHANGE_END16(517);   //2 + 1 + 512 + 2
						ui_updata.sn = ota_file.fptr/512 - 1 + updata_state.sent_cnt;
						ui_updata.checksum = check_sum16((u8*)&ui_updata.head + 1,6 + OTA_WRITE_BYTE);
						ui_updata.checksum =  ~CHANGE_END16(ui_updata.checksum);
						ui_updata_tx((u8*)&ui_updata,sizeof(ui_updata));
					}
				}
				if(	updata_state.timeout++ > 500)
				{
					f_close(&ota_file); 
					pop_tips(28,(u8*)TIPS,(u8*) "UI update response timed out!");
					updata_state.update_status_flow = 0;
					while(1);
//					return 1;
				}
			}
			else if(updata_state.ota_ack == UI_UPDATA_CHECK_ERR) //校验错误
			{
				updata_state.ota_ack = OTA_WAIT_ACK_IDLE;
				if(ota_r < 0x200)   //若是最后一帧不足512字节单独组包
				{
					u8 end_fream[600]= {0xEE,0xB6,0x00,0x00,0x88,0x22};
					ui_updata.len = CHANGE_END16(5 + ota_r);
					ui_updata.sn++;
					memcpy(end_fream,(u8*)&ui_updata,7+ota_r);
					*(u16*)&end_fream[7+ota_r] = check_sum16(end_fream+1,6+ota_r);
					*(u16*)&end_fream[7+ota_r] = ~CHANGE_END16(*(u16*)&end_fream[7+ota_r]);
					*(u32*)&end_fream[7+ota_r+2] = CHANGE_END32(FEND);
					ui_updata_tx(end_fream,7+ota_r+2+4);
				}
				else
				{
					ui_updata.len = CHANGE_END16(517);   //2 + 1 + 512 + 2
					ui_updata.sn = ota_file.fptr/512 - 1;
					ui_updata.checksum = check_sum16((u8*)&ui_updata.head + 1,6 + OTA_WRITE_BYTE);
					ui_updata.checksum =  ~CHANGE_END16(ui_updata.checksum);
					ui_updata_tx((u8*)&ui_updata,sizeof(ui_updata));
				}
			}
			else
			{
				updata_state.timeout++;
				if(updata_state.sent_cnt < 3)
				{
					if(updata_state.timeout%100 == 1 && updata_state.timeout > 100)
					{
						updata_state.sent_cnt++;
						updata_state.ota_ack = OTA_WAIT_ACK_IDLE;
						if(ota_r < 0x200)   //若是最后一帧不足512字节单独组包
						{
							u8 end_fream[600]= {0xEE,0xB6,0x00,0x00,0x88,0x22};
							ui_updata.len = CHANGE_END16(5 + ota_r);
							ui_updata.sn++;
							memcpy(end_fream,(u8*)&ui_updata,7+ota_r);
							*(u16*)&end_fream[7+ota_r] = check_sum16(end_fream+1,6+ota_r);
							*(u16*)&end_fream[7+ota_r] = ~CHANGE_END16(*(u16*)&end_fream[7+ota_r]);
							*(u32*)&end_fream[7+ota_r+2] = CHANGE_END32(FEND);
							ui_updata_tx(end_fream,7+ota_r+2+4);
						}
						else
						{
							ui_updata.len = CHANGE_END16(517);   //2 + 1 + 512 + 2
							ui_updata.sn = ota_file.fptr/512 - 1;
							ui_updata.checksum = check_sum16((u8*)&ui_updata.head + 1,6 + OTA_WRITE_BYTE);
							ui_updata.checksum =  ~CHANGE_END16(ui_updata.checksum);
							ui_updata_tx((u8*)&ui_updata,sizeof(ui_updata));
						}
					}
				}

				if(updata_state.timeout > 500)     //超时
				{
					f_close(&ota_file); 
					pop_tips(28,(u8*)TIPS,(u8*) "UI update response timed out!");
					updata_state.update_status_flow = 0;
					while(1);
//					return 1;
				}
			}
			break;
		case 3:
			if(updata_state.ota_ack == UI_UPDATA_YES)               //屏幕升级应答OK
			{
//				pop_tips(28,(u8*)TIPS,(u8*) "ota.bin Upgrade response OK!");
			}
			else if(updata_state.ota_ack == UPDATA_FILE_CHECK_YES)  //屏幕反馈文件校验成功
			{
//				pop_tips(28,(u8*)TIPS,(u8*) "The ota.bin file is successfully verified!");
			}
			else if(updata_state.ota_ack == UPDATA_FILE_CHECK_NO)   //屏幕反馈文件校验失败
			{
				pop_tips(28,(u8*)TIPS,(u8*) "The screen reports that the ota.bin file verification fails!");
				return 1;
			}
			else if(updata_state.ota_ack == UPDATA_FILE_ZIP_YES)    //文件解压成功
			{
				pop_tips(28,(u8*)TIPS,(u8*) "The ota.bin file is decompressed successfully!");
				set_lcd_baudrate(115200);            //恢复波特率
				if(updata_state.update_file_mode == 1)   //若只有一个UI文件则更新完UI后直接启动应用程序
				{
					memset(&updata_state,0x00,sizeof(updata_state));
					start_app();                     //直接进入应用程序
//					Sys_Soft_Reset();                //系统软复位
					return 0;
				}
				else if(updata_state.update_file_mode == 3)  //若是两个文件都存在则改变状态更新mcu文件
				{
					updata_state.update_status_flow = 2;
					return 0;					
				}

			}
			else if(updata_state.ota_ack == UPDATA_FILE_ZIP_NO)
			{
//				pop_tips(28,(u8*)TIPS,(u8*) "The ota.bin file failed to be decompressed!");
				return 1;
			}
			else
			{
				if(updata_state.timeout++ > 500)
				{
					f_close(&ota_file); 
					//数据帧应答超时
					pop_tips(28,(u8*)TIPS,(u8*) "UI update response timed out!");
					return 1;
				}
			}
			break;
	}
	return 0;
}

//只更新MCU
u32 write_addr_offset = APP_START_ADDR;
int upgrade_mcu(void)
{
	static FIL ota_mcu_file;
	static UINT ota_mcu_r;
	DSTATUS fr;
	u32 flash_addr_offset = 0;
	u8 read_mcu[1024] = {0};
	fr = f_open(&ota_mcu_file,mcu_upgrade_file_path,FA_READ);
	if(FR_OK == fr)
	{
		//擦除320K
		if(f_size(&ota_mcu_file) > 320*1024)  
		{
			//升级文件过大
			f_close(&ota_mcu_file);
			pop_tips(30,(u8*)TIPS,(u8*) "The file exceeds the flash memory!");
			return 1;
		}
			
		flash_addr_offset = flash_erase((void*)APP_START_ADDR);
		flash_addr_offset += flash_erase((void*)(APP_START_ADDR+ flash_addr_offset));
		flash_addr_offset += flash_erase((void*)(APP_START_ADDR+ flash_addr_offset));

		while( f_eof(&ota_mcu_file) == 0 )
		{
			memset(read_mcu,0x00,1024);
			fr = f_read(&ota_mcu_file,read_mcu,1024,&ota_mcu_r);
			if(FR_OK == fr)
			{
				if( 0 == iap_flash_write((u32*)read_mcu,ota_mcu_r,(void*)write_addr_offset ) )//写入app区
					write_addr_offset += ota_mcu_r;
				else
				{
					f_close(&ota_mcu_file);
					return 1;
				}
			}
			else
			{
				f_close(&ota_mcu_file);
				return 1;
			}
		}
		f_close(&ota_mcu_file);
		memset(&updata_state,0x00,sizeof(updata_state));
		start_app();                     //直接进入应用程序
	}
	f_close(&ota_mcu_file); 
	return 0;
}



//更新任务，以一定频率调用
int updating_task(upgrade_stat *s)
{
	if(s->update_status_flow ==1)           //1  进入更新流程
	{
		if(updata_state.update_file_mode == 2)  //若只有mcu文件
		{
			s->update_status_flow = 2;      //则进入MCU更新
			return 0;
		}
		printf("upgrade_ui\r\n");
		if(1 == upgrade_ui() )
		{
			pop_tips(30,(u8*)TIPS,(u8*) "UI update failure!");
			s->update_status_flow = 0;
		}
	}
	else if(s->update_status_flow == 2)
	{
		if(1 == upgrade_mcu())
		{
			pop_tips(30,(u8*)TIPS,(u8*) "MCU update failure!");
			s->update_status_flow = 0;
		}
	}
	return 0;
}




































