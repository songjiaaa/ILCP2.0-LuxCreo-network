
#ifndef F4_CAN1_H
#define F4_CAN1_H

#include "stm32f4_sys.h"
#include "com_fun.h"

#pragma pack(1)
typedef struct
{
	u16 baud;
	u8 tsjw;
	u8 tbs1;
	u8 tbs2;
	u16 brp;
} S_CAN_BAUD_TAB;
#pragma pack()
extern const S_CAN_BAUD_TAB can_baud_tab[23];//can波特率表
int get_can_baud_offset(u16 t); //输入波特率，得到偏移位置

extern CAN_MSG can1_rx_buf; //接收缓存
int can1_mode_init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp);
int can1_rx(void); //接收，返回收到的数据数量
int can1_can_send(void); //是否可以发送
int can1_tx(CAN_MSG* buf); //发送,返回0成功
void can1_check_err(void); //检查是否有错误

#endif

