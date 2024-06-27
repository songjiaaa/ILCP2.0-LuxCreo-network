#include "ui_process.h"


save_ui_id ui_id = {0};

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



button_control b_ctrl = {0};        //按钮数据
read_pic_id r_pic_id = {0};         //读取当前图片
ui_upgrade_start_ack upgrade_start_ack = {0};
ui_upgrade_data_ack  upgrade_data_ack = {0};
u8 touch_cmd_pro(u8 *b ,int len)    //一次只解析一帧的数据
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
			if( b_ctrl.pic_id == CHANGE_END16(33) )
			{
				if( b_ctrl.touch_id == CHANGE_END16(0x0003) )        //NO
				{
					set_screen(ui_id.pre_picture);
					//跳转到用户程序
					start_app();
				}
				else if( b_ctrl.touch_id == CHANGE_END16(0x0002) )   //YES
				{
					set_screen(31);
					updata_state.update_status_flow = 1;    //进入更新流程
				}
			}

		}
		return 0;
	}

	upgrade_start_ack = *(ui_upgrade_start_ack*)b;
	if(upgrade_start_ack.head == CHANGE_END16(0xEEA6) && upgrade_start_ack.end == CHANGE_END32( FEND ))
	{
		if(upgrade_start_ack.cmd == UI_START_ACK_YES)
		{
			updata_state.ota_ack = OTA_WAIT_START_ACK;
		}
		else if(upgrade_start_ack.cmd == UI_START_ACK_NO)
		{
			//文件大小或名称错误
			pop_tips(28,(u8*)TIPS,(u8*)"The size or name of the ota.bin file is incorrect!");
		}
		else if(upgrade_start_ack.cmd == UI_UPDATA_YES)         //升级命令应答
		{
			updata_state.ota_ack = UI_UPDATA_YES;
		}
		else if(upgrade_start_ack.cmd == UPDATA_FILE_CHECK_YES) //ota.bin文件校验成功
		{
			updata_state.ota_ack = UPDATA_FILE_CHECK_YES;
		}
		else if(upgrade_start_ack.cmd == UPDATA_FILE_ZIP_YES)   //解压成功
		{
			updata_state.ota_ack = UPDATA_FILE_ZIP_YES;
		}
		else if(upgrade_start_ack.cmd == UPDATA_FILE_CHECK_NO)  //ota.bin文件校验失败
		{
			updata_state.ota_ack = UPDATA_FILE_CHECK_NO;
		}
		else if(upgrade_start_ack.cmd == UPDATA_FILE_ZIP_NO)  //ota.bin文件解压失败
		{
			updata_state.ota_ack = UPDATA_FILE_ZIP_NO;
		}
		return 0;
	}
	upgrade_data_ack = *(ui_upgrade_data_ack*)b;
	if(upgrade_data_ack.head == CHANGE_END16(0xEEA6) && upgrade_data_ack.end == CHANGE_END32( FEND ))
	{
		if(upgrade_data_ack.cmd == UI_UPDATA_ACK_YES)
		{
			updata_state.ota_ack = OTA_WAIT_DOWNLOAD_ACK;
		}
		else if(upgrade_data_ack.cmd == UI_UPDATA_SN_ERR)    //sn错误
		{
			updata_state.ota_ack = UI_UPDATA_SN_ERR;
		}
		else if(upgrade_data_ack.cmd == UI_UPDATA_CHECK_ERR)  // 校验错误
		{
			updata_state.ota_ack = UI_UPDATA_CHECK_ERR;
		}
		return 0;
	}
	return 1;
}
//#undef B



//主界面弹框
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
}


//弹窗提示
void pop_tips(u16 screen_id,u8* t,u8* str)
{
	set_screen(screen_id);
	SetTextValue(screen_id,2,t); 
	SetTextValue(screen_id,3,str); 
}








