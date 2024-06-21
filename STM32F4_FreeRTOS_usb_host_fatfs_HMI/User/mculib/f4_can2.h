
#ifndef F4_CAN2_H
#define F4_CAN2_H

#include "f4_can1.h"

extern CAN_MSG can2_rx_buf; //接收缓存
int can2_mode_init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp);
int can2_rx(void); //接收，返回收到的数据数量
int can2_can_send(void); //是否可以发送
int can2_tx(CAN_MSG* buf); //发送,返回0成功
void can2_check_err(void); //检查是否有错误

#endif

