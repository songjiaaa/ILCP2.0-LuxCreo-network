#include "rfid.h"

rfid_state rf_sta;

static u8 rfid1_pack_buf[100];
static u8 syn_buf[]={0x43,0x4D};

COMM_HEAD rfid1_pack= //�������
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
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);//�ȴ����ͼĴ�����
		USART_SendData(USART3, addr[i]);  
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //�ȴ��������		
	}
}

void uart6_tx(u8 *addr,u16 len)
{
	for(int i=0;i<len;i++)
	{
		while(USART_GetFlagStatus(USART6,USART_FLAG_TXE)==RESET);//�ȴ����ͼĴ�����
		USART_SendData(USART6, addr[i]);    
		while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET); //�ȴ��������		
	}
}

//43 4D 3C 03 08 00 20 00 00 00 04 01 01 00 24
//���ݰ����ͷ��ذ�����
int rfid1_pre_cb(u8 *b,int len)   //������������
{
	switch(b[2])
	{
		case 0x0B:
			return 22;
		case 0x3C:              //ʶ��ģ��
			return 15;
		case 0x70:              //�л���ISO14443A
			return 10;
		case 0x72:              //����Ƶ����
			return 10;
		case 0x77:
			return 26;
				
	}
	return 0;
}



int rfid1_pro(u8 *b,int len)//�����Ƿ���ȷ����
{
	u8 c_xor = 0;
	if(len < 0) return 0;
	if(b[0] == 0x43 && b[1] == 0x4D)
	{
		c_xor = check_xor(b+6,len - 7);
		if( c_xor != b[len - 1] || c_xor == b[len - 1] == 0)   
			return 1;

		rfid1_read_process(b,len);    //��������
	}
	return 0;
}


u8 rf1_data_buf[256] = {0};
u8 read_num = 0;
u8 read_flag = 0;
u8 read_addr = 0;

u8 write_num = 0;
u8 write_flag = 0;
u8 write_addr = 0;

u32 connect_cnt = 0;


//ָ���
int rfid1_read_process(u8 *b,int len)
{
	if(len < 0) return 0;
	switch(b[2])
	{
		case 0x0B:
			m_data_t.io_input_state.bucket_rfid_stete = 1;      //��ǩ��һ�νӴ�ģ���Զ��������� ��Ϊ��ȡ����
			connect_cnt = 5;
			break;
		case 0x3C:
			rf_sta.rfid1_init = 1;
			break;
		case 0x70:              //�л���ISO14443A�ɹ�
			rf_sta.rfid1_init = 2;
			break;
		case 0x72:              //����Ƶ���سɹ�
			rf_sta.rfid1_init = 3;
			break;
		case 0x77:              //��ȡRFID������
//			rf_sta.rfid1_read = 0;
//			if(b[8] != 0)       //����д�ɹ�
//			{
//				memcpy(&rf1_data_buf[rf_sta.rf1_read_num],b+9,16);
//				rf_sta.rf1_read_num += 16;
//			}
		  //���н���
			read_flag = 0;       //���εõ���Ӧ������
			if(b[8] == 0)        //����ȡ������
			{
				memcpy(&rf1_data_buf[read_addr],b+9,16);    //����һ���������
				read_addr += 4;      //��ȡ��ַƫ��
				connect_cnt = 5;
				m_data_t.io_input_state.bucket_rfid_stete = 1; 
			}
			else                 //����δ��ȡ������
			{
				if( connect_cnt-- == 0 )
					m_data_t.io_input_state.bucket_rfid_stete = 0;    //������
			}
			break;
		case 0x78:
			write_flag = 0;      
			if(b[8] == 0)
			{
				write_addr += 1;
				connect_cnt = 5;
				m_data_t.io_input_state.bucket_rfid_stete = 1; 
			}
			else
			{
				if( connect_cnt-- == 0 )
				m_data_t.io_input_state.bucket_rfid_stete = 0;    //������
			}
			break;
	}
	return 0;
}



//��ʼ����Ƶģ�飬�л�ΪISO14443AЭ��
void rfid_init(void)
{
//	43 4D 3C 02 02 00 00 00 00
//	const u8 reset_cmd[] = {0x43,0x4D,0x04,0x02,0x02,0x00,0x00,0x00,0x00};
	const u8 cmd0[] = {0x43,0x4D,0x3C,0x02,0x02,0x00,0x00,0x00,0x00};      //ʶ��ģ��0x00,0x00,0x00,0x00
	const u8 cmd1[] = {0x43,0x4D,0x70,0x02,0x03,0x00,0x00,0x00,0x00,0x00}; //�л���ISO14443AЭ�� 0x00,0x00,0x00,0x00,0x00
	const u8 cmd2[] = {0x43,0x4D,0x72,0x02,0x02,0x00,0x00,0x00,0x00};      //����Ƶ���� 0x00,0x00,0x00,0x00
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
			vTaskDelay(500);
		}
		else if(rf_sta.rfid1_init == 1)
		{
			send_rfid1_cmd((u8*)cmd1,sizeof(cmd1));
			vTaskDelay(500);			
		}
		else if(rf_sta.rfid1_init == 2)
		{
			send_rfid1_cmd((u8*)cmd2,sizeof(cmd2));
			vTaskDelay(500);			
		}
		else if(rf_sta.rfid1_init == 3)
			break;
	}
	m_data_t.io_input_state.bucket_rfid_stete  = 0;
}


void send_rfid1_cmd(u8 *buf, int len)
{
	while(uart6.que_rx.dlen != 0)  //�ȴ����մ������
	{
		int i = 0;
		if(i++ > 100) break;
		vTaskDelay(10);
	}
	RFID1_EN = 1;                  //��Ϊ����
	uart6_tx(buf,len);	
	RFID1_EN = 0;
	
	
//	RFID2_EN = 1;	
//	uart3_tx(buf,len);	
//	RFID2_EN = 0;
}




//��addr��ַ��ʼ��ȡlen���ֽ�,addr ��Ҫ��12 ��ʼ   
//ǰ3���鹲 16*3 = 48 �ֽ�   48/4 = 12  ǰ12��ҳ����д����
int rfid1_read_data(u8 addr,u8 len)
{
	u8 cmd_read[] = {0x43,0x4D,0x77,0x02,0x03,0x00,0x00,0x00,0x04,0xFF};
	   
	read_addr = addr;
	memset(rf1_data_buf,0x00,sizeof(rf1_data_buf));
	
	if(len % 16 == 0)           //��Ҫ�����ٴ�
		read_num = len / 16;
	else
		read_num = len / 16 + 1;
	
	read_flag = 0;
	while(read_num)
	{
		u32 count = 0;
		if(read_flag == 0)
		{
			read_flag = 1;
			cmd_read[8] = read_addr;            //��ַ
			send_rfid1_cmd(cmd_read,sizeof(cmd_read));  
			read_num--;			
		}
		
		vTaskDelay(10);
		if(count++ > 300) 
			return 1;
	}
	return 0;
}


//��buf����д��len�ֽڵ�addr��ַ
int rfid1_write_data(u8* buf, u8 addr,u8 len)
{
	u8 cmd_write[] = {0x43,0x4D,0x78,0x02,0x13,0x00,0x00,0x00, 0x0C, \
	0x12,0x34,0x50,0x0C,  0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,00,0x00, 0xFF};
	
	write_addr = addr;
	
	if(len % 16 == 0)           //��Ҫд���ٴ�
		write_num = len / 4;
	else
		write_num = len / 4 + 1;
	
	write_flag = 0;
	while(write_num)
	{
		u32 count = 0;
		if(write_flag == 0)
		{
			write_flag = 1;
			cmd_write[8] = write_addr;            //��ַ
			send_rfid1_cmd(cmd_write,sizeof(cmd_write));  
			write_num--;			
		}
		
		vTaskDelay(10);
		if(count++ > 300) 
			return 1;
	}
	return 0;
}





