#include "htu21d.h"
   
//温湿度初始化
void htu21d_init(void)
{					     
	//PD4 SCL       PD3 SDA    
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;    
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        //推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;         
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
//	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	GPIO_Set(GPIOE,PIN8,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);   //SCL	
	GPIO_Set(GPIOC,PIN4,GPIO_MODE_OUT,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU);   //SDA

	
	HTU_SCL = 1;
	HTU_SDA = 1;
	
	HTU_Start();
	I2c_SendOneByte(0x40<<1);
	I2c_WaitAck(200);
	I2c_SendOneByte(SOFT_RESET);
	I2c_WaitAck(200);
	HTU_Stop();
}

void HTU_SDA_OUT(void)        //SDA 输出
{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;   
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;     
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
//	GPIO_Set(GPIOC,PIN4,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU); 
	GPIO_Set(GPIOC,PIN4,GPIO_MODE_OUT,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU);   //SDA
}

void HTU_SDA_IN(void)        //SDA 输入
{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;     
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 	 
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_Set(GPIOC,PIN4,GPIO_MODE_IN,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU); 
}


//起始信号
void HTU_Start(void)
{
	HTU_SDA_OUT(); 
	HTU_SDA=1;  
	HTU_SCL=1;
	Delay_us(5);   
 	HTU_SDA=0;
	Delay_us(5);
	HTU_SCL=0;
}	

//停止信号
void HTU_Stop(void)
{
	HTU_SDA_OUT();
	HTU_SCL=0;    
	HTU_SDA=0;
 	Delay_us(5);
	HTU_SCL=1;
	HTU_SDA=1; 
	Delay_us(5);							   	
}

//发送一个字节
void I2c_SendOneByte(uint8_t _ucData)
{
	uint8_t ucCnt = 0;
	
	HTU_SDA_OUT();	//SDA设置为输出(若IO为开漏,无需进行方向切换)
	for(ucCnt = 0; ucCnt < 8; ucCnt++)
	{
		HTU_SCL = 0;		//SCL低电平,允许数据改变
		Delay_us(5);
		
		if(_ucData & 0x80)		//从高位开始传输
		{
			HTU_SDA = 1;		
		}
		else
		{
			HTU_SDA = 0;		
		}
		
		_ucData <<= 1;
		Delay_us(5);
		
		HTU_SCL = 1;		//数据稳定,发送给从机
		Delay_us(5);
	}
	HTU_SCL = 0;		//第9个时钟,SCL低电平,等待应答信号来到
	Delay_us(5);
}

//接收一个字节 0给应答 1给非应答
uint8_t I2c_RecvOneByte(uint8_t _ucAck)
{
	uint8_t ucCnt = 0, ucData = 0;
	
	HTU_SCL = 0;
	Delay_us(5);
	
	HTU_SDA = 1;
	
	HTU_SDA_IN();		//切换SDA传输方向
	
	for(ucCnt = 0; ucCnt < 8; ucCnt++)
	{
		HTU_SCL = 1;		//SCL高电平时SDA上的数据达到稳定
		Delay_us(5);		//延时等待信号稳定
		
		ucData <<= 1;
		if(HTU_READ_SDA)
		{
			ucData |= 0x01;
		}
		else
		{
			ucData &= 0xfe;		
		}
		HTU_SCL = 0;		//允许数据改变
		Delay_us(5);
	}
	HTU_SDA_OUT();
	if(_ucAck)
	{
		I2c_GetNack();
	}
	else
	{
		I2c_GetAck();
	}
	
	return ucData;
}

//等待应答
uint8_t I2c_WaitAck(uint16_t _usErrTime)
{
	uint8_t ucAck = 0xFF;
	
	HTU_SDA = 1;
	Delay_us(5);
	HTU_SCL = 1;		//此时判断是否有应答
	Delay_us(5);
	HTU_SDA_IN();
	if(HTU_READ_SDA)
	{
		ucAck = I2C_NACK;	
	}
	else
	{
		ucAck = I2C_ACK;	
	}
	
	HTU_SCL = 0;
	Delay_us(5);
	
	return ucAck;
}

//得到应答
void I2c_GetAck(void)
{
	HTU_SCL = 0;
	Delay_us(5);
	HTU_SDA_OUT();
	HTU_SDA = 0;	   //第九个时钟,SDA为低应答
	Delay_us(5);
	HTU_SCL = 1;		//SCL高电平,高电平时读取SDA的数据
	Delay_us(5);
	HTU_SCL = 0;
	Delay_us(5);
	HTU_SDA = 1;		//释放SDA
}

//得到非应答
void I2c_GetNack(void)
{
	HTU_SCL = 0;
	Delay_us(5);
	HTU_SDA_OUT();
	HTU_SDA = 1;	    //第九个时钟,SDA为高非应答
	Delay_us(5);
	HTU_SCL = 1;		//SCCL高电平,高电平时读取SDA的数据
	Delay_us(5);
	HTU_SCL = 0;
	Delay_us(5);
}

//写入多个字节数据
int htu_write_some_bytes(uint8_t *pbdata, uint16_t write_length)
{
	HTU_Start();

	I2c_SendOneByte(0x40<<1);
	if(I2C_NACK == I2c_WaitAck(200))
	{
		return 0;
	}
	
	//for循环发送多个字节数据
	for (uint16_t i = 0; i < write_length; i++)
	{
		I2c_SendOneByte(pbdata[i]);
		if (I2C_NACK == I2c_WaitAck(200))
		{
			return 0;
		}
	}

	//while循环发送多个字节数据
//	while (write_length--)
//	{
//		I2c_SendOneByte(*pbdata++);
//		if (I2C_NACK == I2c_WaitAck(200))
//		{
//			return false;
//		}
//	}

	HTU_Stop();

	return 1;
}

//读取多个字节数据
int htu_read_some_bytes(uint8_t *pbdata, uint16_t read_length)
{
	HTU_Start();

	I2c_SendOneByte(0x40<<1 | 0x01);
	if (I2C_NACK == I2c_WaitAck(200))
	{
		return 0;
	}

	for (uint16_t i = 0; i < read_length - 1; i++)
	{
		*pbdata++ = I2c_RecvOneByte(I2C_ACK);
	}
	*pbdata++ = I2c_RecvOneByte(I2C_NACK);	//接收最后一个字节发送NACK,告诉从机读操作已经完成

	HTU_Stop();

	return 1;
}

//要求100hz调用
void htu_measure(float *fTemp,float *fHumi)
{
	static u8 tim_tick_sta = 0,tim_tick = 0;
	uint8_t ucaRecvBuf[3] = {0};
	uint8_t ucTmpVal = NOHOLD_TEMP_MEASURE;
	uint8_t crc = 0;
	volatile float fRetVal = 0.0;

	switch(tim_tick_sta)
	{
		case 0:
			if(tim_tick++ >= 10)
			{
				ucTmpVal = NOHOLD_TEMP_MEASURE;                         //温度测量
				if( htu_write_some_bytes(&ucTmpVal, 1) )		//写操作命令
				{
					tim_tick_sta = 1;
					tim_tick = 0;
				}
			}
			break;
		case 1:
			if(tim_tick++ >= 10)
			{
				if( htu_read_some_bytes(ucaRecvBuf, 3))     //接收三字节数据
				{
					crc = Get_CRC8(ucaRecvBuf,2);
					if(crc == ucaRecvBuf[2])
					{
						fRetVal = (ucaRecvBuf[0] << 8) | ucaRecvBuf[1];
						*fTemp = (175.72f) * fRetVal / 65536 - 46.85f; //温度:T= -46.85 + 175.72 * ST/2^16
					}
				}
				tim_tick_sta = 2;
				tim_tick = 0;
			}
			break;
		case 2:
			if(tim_tick++ >= 10)
			{
				ucTmpVal = NOHOLD_HUMI_MEASURE;                         //湿度测量
				if( htu_write_some_bytes(&ucTmpVal, 1) )	
				{				
					tim_tick_sta = 3;
					tim_tick = 0;
				}
			}
			break;
		case 3:
			if(tim_tick++ >= 10)
			{
				if( htu_read_some_bytes(ucaRecvBuf, 3))     //接收三字节数据
				{
					crc = Get_CRC8(ucaRecvBuf,2);
					if(crc == ucaRecvBuf[2])
					{
						fRetVal = (ucaRecvBuf[0] << 8) | ucaRecvBuf[1];
						*fHumi = (fRetVal * 125) / 65536 - 6.00f;     //湿度: RH%= -6 + 125 * SRH/2^16
					}
				}
				tim_tick_sta = 0;
				tim_tick = 0;
			}
			break;
		default:
			break;
	}
} 



/*******************************************************************************************************/

/* 设置HTU21D 用户寄存器 */
u8 HTU_Set_UserReg(u8 data)
{
	u8 cmd[2];
	
	cmd[0] = WRITE_USER_REG;
	cmd[1] = data;
	htu_write_some_bytes(cmd, 2);//发送写寄存器指令和待写入数据
	return 0;
}

/* 读取HTU21D 用户寄存器 */
u8 HTU_Read_UserReg(void)
{
	u8 cmd = READ_USER_REG;
	u8 data = 0;
	
	htu_write_some_bytes(&cmd, 1);//发送读寄存器指令
	htu_read_some_bytes(&data, 1);//读取寄存器数据
	
	return data; //返回寄存器数据
}










