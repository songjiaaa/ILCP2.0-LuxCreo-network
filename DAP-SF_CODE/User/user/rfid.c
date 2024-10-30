#include "rfid.h"

rf_init rf_ini;

static u8 rfid1_pack_buf[100];
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




//43 4D 3C 03 08 00 20 00 00 00 04 01 01 00 24
//根据包类型返回包长度
int rfid1_pre_cb(u8 *b,int len)   //返回整包长度
{
	switch(b[2])
	{
		case 0x0B:
			return 22;
		case 0x3C:              //识别模块
			return 15;
		case 0x70:              //切换到ISO14443A
			return 10;
		case 0x72:              //打开射频开关
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
			m_data_t.io_input_state.bucket_rfid_stete = 1;      //料筒标签是否正常读取
			break;
		case 0x3C:
			rf_ini.rfid1_state = 1;
			break;
		case 0x70:              //切换到ISO14443A成功
			rf_ini.rfid1_state = 2;
			break;
		case 0x72:              //打开射频开关成功
			rf_ini.rfid1_state = 3;
			break;
	}
	return;
}



//初始化高频模块，切换为ISO14443A协议
void rfid_init(void)
{
//	43 4D 3C 02 02 00 00 00 00
	const u8 cmd0[] = {0x43,0x4D,0x3C,0x02,0x02,0x00,0x00,0x00,0x00};      //识别模块0x00,0x00,0x00,0x00
	const u8 cmd1[] = {0x43,0x4D,0x70,0x02,0x03,0x00,0x00,0x00,0x00,0x00}; //切换到ISO14443A协议 0x00,0x00,0x00,0x00,0x00
	const u8 cmd2[] = {0x43,0x4D,0x72,0x02,0x02,0x00,0x00,0x00,0x00};      //打开射频天线 0x00,0x00,0x00,0x00
	
	GPIO_Set(GPIOD,PIN1,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);  //RFID1
	GPIO_Set(GPIOE,PIN11,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU); //RFID2
	PDout(1) = 0;
	PEout(11) = 0;
	
	while(1)          
	{
		if(rf_ini.rfid1_state == 0)
		{
			send_rfid1_cmd((u8*)cmd0,sizeof(cmd0));
			vTaskDelay(1000);
		}
		else if(rf_ini.rfid1_state == 1)
		{
			send_rfid1_cmd((u8*)cmd1,sizeof(cmd1));
			vTaskDelay(1000);			
		}
		else if(rf_ini.rfid1_state == 2)
		{
			send_rfid1_cmd((u8*)cmd2,sizeof(cmd2));
			vTaskDelay(1000);			
		}
		else if(rf_ini.rfid1_state == 3)
			break;
	}
	m_data_t.io_input_state.bucket_rfid_stete  = 0;
}


void send_rfid1_cmd(u8 *buf, int len)
{
	while(uart6.que_rx.dlen != 0)  //等待接收处理完成
	{
		int i = 0;
		if(i++ > 100) break;
		vTaskDelay(10);
	}
	RFID1_EN  = 1;                  //改为发送
	uart_send(buf,len,&uart6);	
	
	RFID2_EN = 1;
	uart_send(buf,len,&uart3);		
}





void rfid1_read_data(u8* buf,u8 addr,u32 len)
{
	const u8 cmd_read[] = {0x43,0x4D,0x77,0x02,0x03,0x00,0x00,00};
}






