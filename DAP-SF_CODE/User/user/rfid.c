#include "rfid.h"

static u8 rfid1_pack_buf = [100];
static u8 syn_buf[]={0x01};

COMM_HEAD rfid1_pack= //组包对象
{
	dap_pack_buf,
	sizeof(dap_pack_buf),
	(u8 *)syn_buf,
	sizeof(syn_buf),
	sizeof(dap_03cmd),
	1,
	0,
	rfid1_pre_cb,
	rfid1_pro
};




int rfid1_pre_cb(u8 *b,int len)//返回整包长度
{
	switch(b[1])
	{
		case 0x03: //传感器数据包共22字节
			return sizeof(dap_03cmd);
		case 0x10: //输出姿态数据，共26字节
			return sizeof(dap_10cmd);
		default:
			return sizeof(dap_10cmd);
	}
}


int rfid1_pro(u8 * b,int len)//返回是否正确接收
{
	u16 crc=GetModbusCRC16(b,len-2);
	if( crc != *(u16*)&b[len-2] ) return 1;
//		dap_cmd_process(b,len);

	return 0;
}


void rfid1_read_process(u8 *b,int len)
{
	return;
}


