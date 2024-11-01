#ifndef _RFID_H
#define _RFID_H

#include "app.h"
//#include "stm32f4_sys.h"
//#include "stm32f4xx.h" 
//#include "main.h"
//#include "comm_rec.h"
//#include "f4_uart.h"
//#include "my_queue.h"

//���ݰ�ͷ���ֶΣ������̶���ͷ
#pragma pack(1)
typedef struct
{
	u8 cmd_val;     //ָʾ�豸ִ�ж�����������
	u8 eigen_val;   //bit[7.6] ��ʾͨ��Э��汾��bit[5.4]��չ�����bit[3.0]֡���
	u16 data_len;   //���ݳ���
	u16 device_number; //�豸��
}d_h;
#pragma pack()

//���ݰ���
#pragma pack(1)
typedef struct
{
	u16 frame_header;
	d_h rf_data_header;
	u8* data;
	u8 c_xor;           //���У��
}rfid_rx;
#pragma pack()


#define READ_TAG_START_ADDR  6

#define RFID_MAX_STORE_LEN   96
//��Ƶģ���ʼ��״̬
#pragma pack(1)
typedef struct
{
	u8 rf1_data_buf[RFID_MAX_STORE_LEN];
	u8 rf1_init;
	u8 rf1_read_num;
	u8 rf1_read_flag;
	u8 rf1_read_addr;
	u8 rf1_read_offset;

	u8 rf1_write_num;
	u8 rf1_write_flag;
	u8 rf1_write_addr;
	u8 rf1_write_offset;
	int rf1_connect_cnt;
	
	u8 rf2_data_buf[RFID_MAX_STORE_LEN];
	u8 rf2_init;
	u8 rf2_read_num;
	u8 rf2_read_flag;
	u8 rf2_read_addr;
	u8 rf2_read_offset;

	u8 rf2_write_num;
	u8 rf2_write_flag;
	u8 rf2_write_addr;
	u8 rf2_write_offset;
	int rf2_connect_cnt;
}rfid_state;
#pragma pack()



extern COMM_HEAD rfid1_pack;
extern COMM_HEAD rfid2_pack;

int rfid1_pre_cb(u8 *b,int len);
int rfid2_pre_cb(u8 *b,int len);
int rfid1_pro(u8 * b,int len);
int rfid2_pro(u8 *b,int len);

int rfid1_read_process(u8 *b,int len);
int rfid2_read_process(u8 *b,int len);

void rfid_init(void);
void send_rfid1_cmd(u8 *buf, int len);
void send_rfid2_cmd(u8 *buf, int len);

int rfid1_read_data(u8 addr,u8 len);
int rfid2_read_data(u8 addr,u8 len);
int rfid1_write_data(u8* buf, u8 addr,u8 len);
int rfid2_write_data(u8* buf, u8 addr,u8 len);


int get_bucket_data(void);
int get_material_data(void);


#endif



