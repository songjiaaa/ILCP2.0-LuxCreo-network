#include "rfid.h"

 u8 rfid1_pack_buf[100];
static u8 syn_buf[]={0x43,0x4D};

COMM_HEAD rfid1_pack= //组包对象
{
	rfid1_pack_buf,
	sizeof(rfid1_pack_buf),
	(u8 *)syn_buf,
	sizeof(syn_buf),
	sizeof(rfid1_rx),
	2,
	0,
	rfid1_pre_cb,
	rfid1_pro
};






int rfid1_pre_cb(u8 *b,int len)//返回整包长度
{
//	if( b[2] == 0x0B )
//	{
//		return 22;
//	}
	switch(b[2])
	{
		case 0x0B:
			return 22;
		case 0x70:
			return 10;
		
	}
	return 0;
}



int rfid1_pro(u8 *b,int len)//返回是否正确接收
{
	u8 c_xor = 0;
	if(b[0] == 0x43 && b[1] == 0x4D)
	{
		c_xor = check_xor(b+6,len - 7);
		if( c_xor != b[len - 1] && c_xor == b[len - 1] == 0)   
			return 1;

		rfid1_read_process(b,len);    //处理数据
	}
	return 0;
}



//指令处理
void rfid1_read_process(u8 *b,int len)
{

	switch(b[2])
	{
		case 0x0B:
			m_data_t.io_input_state.material_rfid_stete = 1;
			break;
		case 0x70:
			break;
	}
	return;
}



//初始化高频模块，切换为ISO14443A协议
void rfid_init(void)
{
	const u8 cmd1[] = {0x43,0x4D,0x70,0x02,0x03,0x00,0x00,0x00,0x00,0x00}; //切换到ISO14443A协议
	const u8 cmd2[] = {0x43,0x4D,0x72,0x02,0x02,0x00,0x00,0x00,0x00};      //打开射频天线
	
	send_rfid1_cmd((u8*)cmd1,sizeof(cmd1));
	vTaskDelay(200);
	send_rfid1_cmd((u8*)cmd2,sizeof(cmd1));
	vTaskDelay(200);
}


void send_rfid1_cmd(u8 *buf, int len)
{
	while(uart6.que_rx.dlen != 0)  //等待接收处理完成
	{
		int i = 0;
		if(i++ > 100) break;
		vTaskDelay(10);
	}
	RFID1_EN  = 1;                  //方向改为发送
	uart_send(buf,len,&uart6);		
}



void send_rfid2_cmd(u8 *buf, int len)
{

}







