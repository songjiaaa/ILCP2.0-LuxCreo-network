#include "rfid.h"

rfid_state rf_sta;

static u8 rfid1_pack_buf[100];
static u8 rfid2_pack_buf[100];

static u8 syn_buf[]={0x43,0x4D};

//RFID2
COMM_HEAD rfid1_pack= //组包对象
{
	rfid1_pack_buf,
	sizeof(rfid1_pack_buf),
	(u8 *)syn_buf,
	sizeof(syn_buf),
	sizeof(rfid_rx),
	2,
	0,
	rfid1_pre_cb,
	rfid1_pro
};


//RFID2
COMM_HEAD rfid2_pack= //组包对象
{
	rfid2_pack_buf,
	sizeof(rfid2_pack_buf),
	(u8 *)syn_buf,
	sizeof(syn_buf),
	sizeof(rfid_rx),
	2,
	0,
	rfid2_pre_cb,
	rfid2_pro
};



//rfid 1
void uart6_tx(u8 *addr,u16 len)
{
	for(int i=0;i<len;i++)
	{
		while(USART_GetFlagStatus(USART6,USART_FLAG_TXE)==RESET);//等待发送寄存器空
		USART_SendData(USART6, addr[i]);    
		while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET); //等待发送完成		
	}
}


//rfid 2
void uart3_tx(u8 *addr,u16 len)
{
	for(int i=0;i<len;i++)
	{
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);//等待发送寄存器空
		USART_SendData(USART3, addr[i]);  
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //等待发送完成		
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
		case 0x78:
			return 10;			
	}
	return 0;
}

//根据包类型返回包长度
int rfid2_pre_cb(u8 *b,int len)   //返回整包长度
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
		case 0x77:              //读接收
			return 26;
		case 0x78:              //写接收
			return 10;			
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


int rfid2_pro(u8 *b,int len)//返回是否正确接收
{
	u8 c_xor = 0;
	if(len < 0) return 0;
	if(b[0] == 0x43 && b[1] == 0x4D)
	{
		c_xor = check_xor(b+6,len - 7);
		if( c_xor != b[len - 1] || c_xor == b[len - 1] == 0)   
			return 1;

		rfid2_read_process(b,len);    //处理数据
	}
	return 0;
}


//rfid1指令处理
int rfid1_read_process(u8 *b,int len)
{
	if(len < 0) return 0;
	switch(b[2])
	{
		case 0x0B:
			m_data_t.io_input_state.bucket_rfid_stete = 1;      //标签第一次接触模块自动返回数据 视为读取正常
			rf_sta.rf1_connect_cnt = 5;
			break;
		case 0x3C:
			rf_sta.rf1_init = 1;
			break;
		case 0x70:              //切换到ISO14443A成功
			rf_sta.rf1_init = 2;
			break;
		case 0x72:              //打开射频开关成功
			rf_sta.rf1_init = 3;
			break;
		case 0x77:              //读取RFID的数据
		  //若有接收
			rf_sta.rf1_read_flag = 0;       //本次得到回应标记清空
			if(b[8] == 0)        //若读取到数据
			{
				memcpy(&rf_sta.rf1_data_buf[rf_sta.rf1_read_offset],b+9,16);    //拷贝一个块的数据  
				rf_sta.rf1_read_addr += 4;
				rf_sta.rf1_read_offset += 16;
				rf_sta.rf1_connect_cnt = 5;	
				m_data_t.io_input_state.bucket_rfid_stete = 1;   //料筒到位检测
			}
			else                 //若是未读取到数据
			{
				if( rf_sta.rf1_connect_cnt-- < 0 )
					m_data_t.io_input_state.bucket_rfid_stete = 0;    //无连接
			}
			break;
		case 0x78:
			rf_sta.rf1_write_flag = 0;    
			if(b[8] == 0)
			{
				rf_sta.rf1_write_addr += 1;
				rf_sta.rf1_write_offset = 4;
				rf_sta.rf1_connect_cnt = 5;
				
				m_data_t.io_input_state.bucket_rfid_stete = 1; 
			}
			else
			{
				if( rf_sta.rf1_connect_cnt-- < 0 )
					m_data_t.io_input_state.bucket_rfid_stete = 0;    //无连接
			}
			break;
	}
	return 0;
}

//rfid2指令处理
int rfid2_read_process(u8 *b,int len)
{
	if(len < 0) return 0;
	switch(b[2])
	{
		case 0x0B:
			m_data_t.io_input_state.material_rfid_stete = 1;      //标签第一次接触模块自动返回数据 视为读取正常
			rf_sta.rf2_connect_cnt = 5;
			break;
		case 0x3C:
			rf_sta.rf2_init = 1;
			break;
		case 0x70:              //切换到ISO14443A成功
			rf_sta.rf2_init = 2;
			break;
		case 0x72:              //打开射频开关成功
			rf_sta.rf2_init = 3;
			break;
		case 0x77:              //读取RFID的数据
		  //若有接收
			rf_sta.rf2_read_flag = 0;       //本次得到回应标记清空
			if(b[8] == 0)        //若读取到数据
			{
				memcpy(&rf_sta.rf2_data_buf[rf_sta.rf2_read_offset],b+9,16);    //拷贝一个块的数据  
				rf_sta.rf2_read_addr += 4;
				rf_sta.rf2_read_offset += 16;
				rf_sta.rf2_connect_cnt = 5;	
				m_data_t.io_input_state.material_rfid_stete = 1;   //料盒到位检测
			}
			else                 //若是未读取到数据
			{
				if( rf_sta.rf2_connect_cnt-- < 0 )
					m_data_t.io_input_state.material_rfid_stete = 0;    //无连接
			}
			break;
		case 0x78:
			rf_sta.rf2_write_flag = 0;    
			if(b[8] == 0)
			{
				rf_sta.rf2_write_addr += 1;
				rf_sta.rf2_write_offset = 4;
				rf_sta.rf2_connect_cnt = 5;
				
				m_data_t.io_input_state.material_rfid_stete = 1; 
			}
			else
			{
				if( rf_sta.rf2_connect_cnt-- < 0 )
					m_data_t.io_input_state.material_rfid_stete = 0;    //无连接
			}
			break;
	}
	return 0;
}



//初始化高频模块，切换为ISO14443A协议
void rfid_init(void)
{
//	43 4D 3C 02 02 00 00 00 00
//	const u8 reset_cmd[] = {0x43,0x4D,0x04,0x02,0x02,0x00,0x00,0x00,0x00};  //复位模块
	const u8 cmd0[] = {0x43,0x4D,0x3C,0x02,0x02,0x00,0x00,0x00,0x00};       //识别模块0x00,0x00,0x00,0x00
	const u8 cmd1[] = {0x43,0x4D,0x70,0x02,0x03,0x00,0x00,0x00,0x00,0x00};  //切换到ISO14443A协议 0x00,0x00,0x00,0x00,0x00
	const u8 cmd2[] = {0x43,0x4D,0x72,0x02,0x02,0x00,0x00,0x00,0x00};       //打开射频天线 0x00,0x00,0x00,0x00
//	u8 read_cmd[] = {0x43,0x4D,0x77,0x02,0x03,0x00,0x00,0x00,0x04,0x04};    //读命令
	
	
	GPIO_Set(GPIOD,PIN1,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);  //RFID1
	GPIO_Set(GPIOE,PIN11,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU); //RFID2
	PDout(1) = 0;
	PEout(11) = 0;
	
	while(1)          
	{
		//rfid1
		if(rf_sta.rf1_init == 0)
		{
			send_rfid1_cmd((u8*)cmd0,sizeof(cmd0));
			vTaskDelay(500);
		}
		else if(rf_sta.rf1_init == 1)
		{
			send_rfid1_cmd((u8*)cmd1,sizeof(cmd1));
			vTaskDelay(500);			
		}
		else if(rf_sta.rf1_init == 2)
		{
			send_rfid1_cmd((u8*)cmd2,sizeof(cmd2));
			vTaskDelay(500);			
		}
		else if(rf_sta.rf1_init == 3)
			break;
	}
	while(1)
	{		
		//rfid2
		if(rf_sta.rf2_init == 0)
		{
			send_rfid2_cmd((u8*)cmd0,sizeof(cmd0));
			vTaskDelay(500);
		}
		else if(rf_sta.rf2_init == 1)
		{
			send_rfid2_cmd((u8*)cmd1,sizeof(cmd1));
			vTaskDelay(500);			
		}
		else if(rf_sta.rf2_init == 2)
		{
			send_rfid2_cmd((u8*)cmd2,sizeof(cmd2));
			vTaskDelay(500);			
		}
		else if(rf_sta.rf2_init == 3)
			break;
	}
	
	m_data_t.io_input_state.bucket_rfid_stete = 0;  
	m_data_t.io_input_state.material_rfid_stete = 0;
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
}

void send_rfid2_cmd(u8 *buf, int len)
{
	while(uart3.que_rx.dlen != 0)  //等待接收处理完成
	{
		int i = 0;
		if(i++ > 100) break;
		vTaskDelay(10);
	}
	RFID2_EN = 1;                  //改为发送
	uart3_tx(buf,len);	
	RFID2_EN = 0;
}


//从addr地址开始读取len个字节,addr 需要从12 开始   
//前3个块共 16*3 = 48 字节   48/4 = 12  前16个页不写数据
int rfid1_read_data(u8 addr,u8 len)
{
	u8 cmd_read[] = {0x43,0x4D,0x77,0x02,0x03,0x00,0x00,0x00,0x04,0xFF};
	
	if(addr <= 6 || addr > 39)
		return 1;
	if( (addr*4 + len) >= RFID_MAX_STORE_LEN )  
		return 1;
	
	rf_sta.rf1_read_addr = addr;
	rf_sta.rf1_read_offset = 0;
	memset(rf_sta.rf1_data_buf,0x00,sizeof(rf_sta.rf1_data_buf));

	if(len % 16 == 0)           //需要读多少次
		rf_sta.rf1_read_num = len / 16;
	else
		rf_sta.rf1_read_num = len / 16 + 1;
	
	
	rf_sta.rf1_read_flag = 0;
	while(rf_sta.rf1_read_num)
	{
		u32 count = 0;
		if(rf_sta.rf1_read_flag == 0)
		{
			rf_sta.rf1_read_flag = 1;
			cmd_read[8] = rf_sta.rf1_read_addr;            //地址
			send_rfid1_cmd(cmd_read,sizeof(cmd_read));  
			rf_sta.rf1_read_num--;			
		}
		
		vTaskDelay(10);
		if(count++ > 300)        //读取超时
		{	
			m_data_t.io_input_state.bucket_rfid_stete = 0;    //无连接			
			return 1;
		}
	}
	return 0;
}


//从addr地址开始读取len个字节,addr 需要从12 开始   
//前3个块共 16*3 = 48 字节   48/4 = 12  前16个页不写数据
int rfid2_read_data(u8 addr,u8 len)
{
	u8 cmd_read[] = {0x43,0x4D,0x77,0x02,0x03,0x00,0x00,0x00,0x04,0xFF};

	if(addr <= 6 || addr > 39)
		return 1;
	if( (addr*4 + len) >= RFID_MAX_STORE_LEN )  
		return 1;
	
	rf_sta.rf2_read_addr = addr;
	rf_sta.rf2_read_offset = 0;
	memset(rf_sta.rf2_data_buf,0x00,sizeof(rf_sta.rf2_data_buf));

	if(len % 16 == 0)           //需要读多少次
		rf_sta.rf2_read_num = len / 16;
	else
		rf_sta.rf2_read_num = len / 16 + 1;
	
	
	rf_sta.rf2_read_flag = 0;
	while(rf_sta.rf2_read_num)
	{
		u32 count = 0;
		if(rf_sta.rf2_read_flag == 0)
		{
			rf_sta.rf2_read_flag = 1;
			cmd_read[8] = rf_sta.rf2_read_addr;            //地址
			send_rfid2_cmd(cmd_read,sizeof(cmd_read));  
			rf_sta.rf2_read_num--;			
		}
		
		vTaskDelay(10);
		if(count++ > 300)        //读取超时
		{	
			m_data_t.io_input_state.material_rfid_stete = 0;    //料盒rfid 无连接		
			return 1;
		}
	}
	return 0;
}

//把buf内容写入len字节到addr地址
int rfid1_write_data(u8* buf, u8 addr,u8 len)
{
	u8 cmd_write[] = {0x43,0x4D,0x78,0x02,0x13,0x00,0x00,0x00, 0x0C, \
	0x12,0x34,0x50,0x0C,  0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,00,0x00, 0xFF};
	
	if(addr <= 6 || addr > 39)
		return 1;
	if( (addr*4 + len) >= RFID_MAX_STORE_LEN )  
		return 1;

	rf_sta.rf1_write_addr = addr;
	rf_sta.rf1_write_offset = 0;
	
	if(len % 16 == 0)           //需要写多少次
		rf_sta.rf1_write_num = len / 4;
	else
		rf_sta.rf1_write_num = len / 4 + 1;
	
	rf_sta.rf1_write_flag = 0;
	while(rf_sta.rf1_write_num)
	{
		u32 count = 0;
		if( rf_sta.rf1_write_flag == 0)
		{
			rf_sta.rf1_write_flag = 1;
			cmd_write[8] = rf_sta.rf1_write_addr;            //地址
			memcpy(&cmd_write[9],buf+rf_sta.rf1_write_offset,4);
			send_rfid1_cmd(cmd_write,sizeof(cmd_write));  
			rf_sta.rf1_write_num--;			
		}
		
		vTaskDelay(10);
		if(count++ > 300) 
		{
			m_data_t.io_input_state.bucket_rfid_stete = 0;    //无连接	
			return 1;
		}
	}
	return 0;
}


//把buf内容写入len字节到addr地址
int rfid2_write_data(u8* buf, u8 addr,u8 len)
{
	u8 cmd_write[] = {0x43,0x4D,0x78,0x02,0x13,0x00,0x00,0x00, 0x0C, \
	0x12,0x34,0x50,0x0C,  0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,00,0x00, 0xFF};
	
	if(addr <= 6 || addr > 39)
		return 1;
	if( (addr*4 + len) >= RFID_MAX_STORE_LEN )  
		return 1;

	rf_sta.rf2_write_addr = addr;
	rf_sta.rf2_write_offset = 0;
	
	if(len % 16 == 0)           //需要写多少次
		rf_sta.rf2_write_num = len / 4;
	else
		rf_sta.rf2_write_num = len / 4 + 1;
	
	rf_sta.rf2_write_flag = 0;
	while(rf_sta.rf2_write_num)
	{
		u32 count = 0;
		if( rf_sta.rf2_write_flag == 0)
		{
			rf_sta.rf2_write_flag = 1;
			cmd_write[8] = rf_sta.rf2_write_addr;            //地址
			memcpy(&cmd_write[9],buf+rf_sta.rf2_write_offset,4);
			send_rfid2_cmd(cmd_write,sizeof(cmd_write));  
			rf_sta.rf2_write_num--;			
		}
		
		vTaskDelay(10);
		if(count++ > 300) 
		{
			m_data_t.io_input_state.material_rfid_stete = 0;    //料盒rfid 无连接	
			return 1;
		}
	}
	return 0;
}


//品牌名 -- 树脂名 -- 料瓶容量（L） -- 树脂密度(g/cm3) -- 树脂规格（出厂状态1kg/5kg） --  树脂余量(g)
// [LuxCreo]--[DMR Ⅲ]--[5]--[1.0]--[1]--[1000]
//读取料筒数据
int get_bucket_data(void)
{
	if( 0 == rfid1_read_data(READ_TAG_START_ADDR,RFID_MAX_STORE_LEN) )  //读取标签1数据
	{
		
		if( 6 == sscanf((const char*)rf_sta.rf1_data_buf,"[%[^]]]--[%[^]]]--[%d]--[%f]--[%d]--[%d]",(char*)m_data_t.resin_band,(char*)m_data_t.resin_name,\
				&m_data_t.resin_volume,&m_data_t.resin_density,&m_data_t.resin_weight,&m_data_t.resin_remain) )
			return 0;
		else
			return 1;
	}
	return 1;
}


//品牌名 -- 离型膜型号 --  可使用次数（使用寿命） --  剩余寿命
//[LuxCreo]--[LEAP-C+]--[50000]--[50000]
//读取料盒数据
int get_material_data(void)
{
	if( 0 == rfid2_read_data(READ_TAG_START_ADDR,RFID_MAX_STORE_LEN) )  //读取标签1数据
	{
		if( 6 == sscanf((const char*)rf_sta.rf2_data_buf,"[%[^]]]--[%[^]]]--[%d]--[%d]",(char*)m_data_t.leap_band,(char*)m_data_t.leap_model,\
				&m_data_t.leap_max_num,&m_data_t.leap_remain_num) )
			return 0;
		else
			return 1;
	}
	return 1;
}




