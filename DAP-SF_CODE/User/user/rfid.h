#ifndef _RFID_H
#define _RFID_H

#include "app.h"
//#include "stm32f4_sys.h"
//#include "stm32f4xx.h" 
//#include "main.h"
//#include "comm_rec.h"
//#include "f4_uart.h"
//#include "my_queue.h"

//数据包头部字段，不含固定包头
#pragma pack(1)
typedef struct
{
	u8 cmd_val;     //指示设备执行动作的命令码
	u8 eigen_val;   //bit[7.6] 表示通信协议版本；bit[5.4]扩展的命令；bit[3.0]帧序号
	u16 data_len;   //数据长度
	u16 device_number; //设备号
}d_h;
#pragma pack()

//数据包体
#pragma pack(1)
typedef struct
{
	u16 frame_header;
	d_h rf_data_header;
	u8* data;
	u8 c_xor;           //异或校验
}rfid1_rx;
#pragma pack()



//u8 rf1_data_buf[256] = {0};
//u8 read_num = 0;
//u8 read_flag = 0;
//u8 read_addr = 0;

//u8 write_num = 0;
//u8 write_flag = 0;
//u8 write_addr = 0;

//u32 connect_cnt = 0;
//高频模块初始化状态
#pragma pack(1)
typedef struct
{
	u8 rfid1_init;
	u8 rfid1_read;
	u32 rf1_read_num;
	
	u8 rfid2_init;
	u8 rfid2_read;
	u32 rf2_read_num;
}rfid_state;
#pragma pack()



extern COMM_HEAD rfid1_pack;

int rfid1_pre_cb(u8 *b,int len);
int rfid1_pro(u8 * b,int len);


int rfid1_read_process(u8 *b,int len);
void rfid_init(void);
void send_rfid1_cmd(u8 *buf, int len);
void send_rfid2_cmd(u8 *buf, int len);



int rfid1_read_data(u8 addr,u8 len);
int rfid1_write_data(u8* buf, u8 addr,u8 len);


#endif



