#ifndef _RFID_H
#define _RFID_H

#include "app.h"
//#include "stm32f4_sys.h"
//#include "stm32f4xx.h" 
//#include "main.h"
//#include "comm_rec.h"
//#include "f4_uart.h"
//#include "my_queue.h"


#pragma pack(1)
typedef struct
{
	u8 cmd_val;     //ָʾ�豸ִ�ж�����������
	u8 eigen_val;   //bit[7.6] ��ʾͨ��Э��汾��bit[5.4]��չ�����bit[3.0]֡���
	u16 data_len;   //���ݳ���
	u16 device_number; //�豸��
}d_h;
#pragma pack()




#pragma pack(1)
typedef struct
{
	u16 frame_header;
	d_h rf_data_header;
	u8* data;
	u8 c_xor;           //���У��
}rfid1_rx;
#pragma pack()






extern COMM_HEAD rfid1_pack;

int rfid1_pre_cb(u8 *b,int len);
int rfid1_pro(u8 * b,int len);


void rfid1_read_process(u8 *b,int len);

void send_rfid1_cmd(u8 *buf, int len);
void send_rfid2_cmd(u8 *buf, int len);

#endif



