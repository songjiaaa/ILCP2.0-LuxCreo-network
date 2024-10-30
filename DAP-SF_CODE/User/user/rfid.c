#include "rfid.h"

rfid_state rf_sta;

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


void uart3_tx(u8 *addr,u16 len)
{
	for(int i=0;i<len;i++)
	{
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);//等待发送寄存器空
		USART_SendData(USART3, addr[i]);  
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //等待发送完成		
	}
}

void uart6_tx(u8 *addr,u16 len)
{
	for(int i=0;i<len;i++)
	{
		while(USART_GetFlagStatus(USART6,USART_FLAG_TXE)==RESET);//等待发送寄存器空
		USART_SendData(USART6, addr[i]);    
		while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET); //等待发送完成		
	}
}

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
		case 0x77:
			return 26;
				
	}
	return 0;
}



int rfid1_pro(u8 *b,int len)//返回是否正确接收
{
	u8 c_xor = 0;
	if(len < 0) return 0;
	if(b[0] == 0x43 && b[1] == 0x4D)
	{
		c_xor = check_xor(b+6,len - 7);
		if( c_xor != b[len - 1] || c_xor == b[len - 1] == 0)   
			return 1;

		rfid1_read_process(b,len);    //处理数据
	}
	return 0;
}


u8 rf1_data_buf[256] = {0};
//int test_len = 0;
//指令处理
int rfid1_read_process(u8 *b,int len)
{
	if(len < 0) return 0;
	switch(b[2])
	{
		case 0x0B:
			m_data_t.io_input_state.bucket_rfid_stete = 1;      //料筒标签是否正常读取
			break;
		case 0x3C:
			rf_sta.rfid1_init = 1;
			break;
		case 0x70:              //切换到ISO14443A成功
			rf_sta.rfid1_init = 2;
			break;
		case 0x72:              //打开射频开关成功
			rf_sta.rfid1_init = 3;
			break;
		case 0x77:              //读取RFID的数据
			rf_sta.rfid1_read = 0;
			if(b[8] != 0)
			{
				memcpy(&rf1_data_buf[rf_sta.rf1_read_num],b+9,16);
				rf_sta.rf1_read_num += 16;
			}
			break;
	}
	return 0;
}



//初始化高频模块，切换为ISO14443A协议
void rfid_init(void)
{
//	43 4D 3C 02 02 00 00 00 00
//	const u8 reset_cmd[] = {0x43,0x4D,0x04,0x02,0x02,0x00,0x00,0x00,0x00};
	const u8 cmd0[] = {0x43,0x4D,0x3C,0x02,0x02,0x00,0x00,0x00,0x00};      //识别模块0x00,0x00,0x00,0x00
	const u8 cmd1[] = {0x43,0x4D,0x70,0x02,0x03,0x00,0x00,0x00,0x00,0x00}; //切换到ISO14443A协议 0x00,0x00,0x00,0x00,0x00
	const u8 cmd2[] = {0x43,0x4D,0x72,0x02,0x02,0x00,0x00,0x00,0x00};      //打开射频天线 0x00,0x00,0x00,0x00
//	u8 read_cmd[] = {0x43,0x4D,0x77,0x02,0x03,0x00,0x00,0x00,0x04,0x04};
	
	
	GPIO_Set(GPIOD,PIN1,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);  //RFID1
	GPIO_Set(GPIOE,PIN11,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU); //RFID2
	PDout(1) = 0;
	PEout(11) = 0;
	
	while(1)          
	{
		if(rf_sta.rfid1_init == 0)
		{
			send_rfid1_cmd((u8*)cmd0,sizeof(cmd0));
			vTaskDelay(1000);
		}
		else if(rf_sta.rfid1_init == 1)
		{
			send_rfid1_cmd((u8*)cmd1,sizeof(cmd1));
			vTaskDelay(1000);			
		}
		else if(rf_sta.rfid1_init == 2)
		{
			send_rfid1_cmd((u8*)cmd2,sizeof(cmd2));
			vTaskDelay(1000);			
		}
		else if(rf_sta.rfid1_init == 3)
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
	RFID1_EN = 1;                  //改为发送
	uart6_tx(buf,len);	
	RFID1_EN = 0;
	
	
//	RFID2_EN = 1;	
//	uart3_tx(buf,len);	
//	RFID2_EN = 0;
}




//从addr地址开始读取len个字节
void rfid1_read_data(u8 addr,int len)
{
	u8 cmd_read[] = {0x43,0x4D,0x77,0x02,0x03,0x00,0x00,0x00,0x04,0xFF};
	cmd_read[8] = addr + 12;    //前3个块共 16*3 = 48 字节   48/4 = 12  前12个页不能写数据
	rf_sta.rf1_read_num = 0;
	memset(rf1_data_buf,0x00,sizeof(rf1_data_buf));
	
	while(len > 0)
	{
		if(rf_sta.rfid1_read == 0)
		{
			rf_sta.rfid1_read = 1;
			send_rfid1_cmd(cmd_read,sizeof(cmd_read));
			cmd_read[8] += 4;
			len -= 16;
		}
		vTaskDelay(10);	
	}
}


//把buf内容写入len字节到addr地址
void rfid1_write_data(u8* buf, u8 addr,int len)
{
	u8 cmd_write[] = {0x43,0x4D,0x78,0x02,0x13,0x00,0x00,0x00, 0x0C, \
	0x12,0x34,0x50,0x0C,  0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,00,0x00, 0xFF};
	
	while(len > 0)
	{
		
			
	}
	
}





