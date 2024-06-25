#include "data_upload.h"
//整包包头
upload_frame_head head = {0};

//空闲状态下上传的数据
idle_upload_frame idle_data = {0};           //空闲或固化结束状态下上传数据包

//固话中上传的数据
cure_upload_front cure_data_head = {0};      //固话中或固化暂停上传的数据包
char upload_cure_para_data[500] = {0};       //固化中数据组包
cure_upload_tail cure_data_tail = {0};       //固化格式结尾

upload_end end = {0};

u8 upload_data_buf[1000] = {0};
//t_mode传输模式：0x00 WiFi上传  0x01 以太网上传
//type 帧类型：0x01 空闲状态下上传   0x02 固化或暂停状态下上传   0x03 固化参数或配置导入   0x04 OTA升级    0x05 错误上报
void netword_package_upload(u8 t_mode,u8 type)
{

	head.head = 0xa55a;
	head.len = 0;

	head.t_mode = t_mode;
	head.t_dir = UPLOADING_DIRECTION;
	head.type = type;
	memcpy(head.version,cfg_dft.version,sizeof(head.version));   //版本号
	
	memset(upload_data_buf,0x00,sizeof(upload_data_buf));
	if(type == 0x01)          //空闲或结束状态上传数据
	{

	}
	else if(type == 0x02)     //固化中或暂停 上传数据
	{

	}
	else if(type == 0x03 || type == 0x04)
	{

	}
	else if(type == 0x05)
	{
		
	}

	if(head.t_mode == 0)        //WiFi上传
	{
		esp8266_uart_tx(upload_data_buf,head.len);
		head.run_num ++;	
	}
	else if(head.t_mode == 1)   //以太网上传
	{
		usr_k5_tx(upload_data_buf,head.len);
		head.run_num ++;
	}
}


//上传数据
void p_data_upload(void)
{

}







































