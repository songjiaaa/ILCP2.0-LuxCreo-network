#include "htu21d.h"
   
//��ʪ�ȳ�ʼ��
void htu21d_init(void)
{					     
	//PD4 SCL       PD3 SDA    
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;    
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        //�������
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

void HTU_SDA_OUT(void)        //SDA ���
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

void HTU_SDA_IN(void)        //SDA ����
{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;     
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 	 
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_Set(GPIOC,PIN4,GPIO_MODE_IN,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU); 
}


//��ʼ�ź�
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

//ֹͣ�ź�
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

//����һ���ֽ�
void I2c_SendOneByte(uint8_t _ucData)
{
	uint8_t ucCnt = 0;
	
	HTU_SDA_OUT();	//SDA����Ϊ���(��IOΪ��©,������з����л�)
	for(ucCnt = 0; ucCnt < 8; ucCnt++)
	{
		HTU_SCL = 0;		//SCL�͵�ƽ,�������ݸı�
		Delay_us(5);
		
		if(_ucData & 0x80)		//�Ӹ�λ��ʼ����
		{
			HTU_SDA = 1;		
		}
		else
		{
			HTU_SDA = 0;		
		}
		
		_ucData <<= 1;
		Delay_us(5);
		
		HTU_SCL = 1;		//�����ȶ�,���͸��ӻ�
		Delay_us(5);
	}
	HTU_SCL = 0;		//��9��ʱ��,SCL�͵�ƽ,�ȴ�Ӧ���ź�����
	Delay_us(5);
}

//����һ���ֽ� 0��Ӧ�� 1����Ӧ��
uint8_t I2c_RecvOneByte(uint8_t _ucAck)
{
	uint8_t ucCnt = 0, ucData = 0;
	
	HTU_SCL = 0;
	Delay_us(5);
	
	HTU_SDA = 1;
	
	HTU_SDA_IN();		//�л�SDA���䷽��
	
	for(ucCnt = 0; ucCnt < 8; ucCnt++)
	{
		HTU_SCL = 1;		//SCL�ߵ�ƽʱSDA�ϵ����ݴﵽ�ȶ�
		Delay_us(5);		//��ʱ�ȴ��ź��ȶ�
		
		ucData <<= 1;
		if(HTU_READ_SDA)
		{
			ucData |= 0x01;
		}
		else
		{
			ucData &= 0xfe;		
		}
		HTU_SCL = 0;		//�������ݸı�
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

//�ȴ�Ӧ��
uint8_t I2c_WaitAck(uint16_t _usErrTime)
{
	uint8_t ucAck = 0xFF;
	
	HTU_SDA = 1;
	Delay_us(5);
	HTU_SCL = 1;		//��ʱ�ж��Ƿ���Ӧ��
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

//�õ�Ӧ��
void I2c_GetAck(void)
{
	HTU_SCL = 0;
	Delay_us(5);
	HTU_SDA_OUT();
	HTU_SDA = 0;	   //�ھŸ�ʱ��,SDAΪ��Ӧ��
	Delay_us(5);
	HTU_SCL = 1;		//SCL�ߵ�ƽ,�ߵ�ƽʱ��ȡSDA������
	Delay_us(5);
	HTU_SCL = 0;
	Delay_us(5);
	HTU_SDA = 1;		//�ͷ�SDA
}

//�õ���Ӧ��
void I2c_GetNack(void)
{
	HTU_SCL = 0;
	Delay_us(5);
	HTU_SDA_OUT();
	HTU_SDA = 1;	    //�ھŸ�ʱ��,SDAΪ�߷�Ӧ��
	Delay_us(5);
	HTU_SCL = 1;		//SCCL�ߵ�ƽ,�ߵ�ƽʱ��ȡSDA������
	Delay_us(5);
	HTU_SCL = 0;
	Delay_us(5);
}

//д�����ֽ�����
int htu_write_some_bytes(uint8_t *pbdata, uint16_t write_length)
{
	HTU_Start();

	I2c_SendOneByte(0x40<<1);
	if(I2C_NACK == I2c_WaitAck(200))
	{
		return 0;
	}
	
	//forѭ�����Ͷ���ֽ�����
	for (uint16_t i = 0; i < write_length; i++)
	{
		I2c_SendOneByte(pbdata[i]);
		if (I2C_NACK == I2c_WaitAck(200))
		{
			return 0;
		}
	}

	//whileѭ�����Ͷ���ֽ�����
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

//��ȡ����ֽ�����
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
	*pbdata++ = I2c_RecvOneByte(I2C_NACK);	//�������һ���ֽڷ���NACK,���ߴӻ��������Ѿ����

	HTU_Stop();

	return 1;
}

//Ҫ��100hz����
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
				ucTmpVal = NOHOLD_TEMP_MEASURE;                         //�¶Ȳ���
				if( htu_write_some_bytes(&ucTmpVal, 1) )		//д��������
				{
					tim_tick_sta = 1;
					tim_tick = 0;
				}
			}
			break;
		case 1:
			if(tim_tick++ >= 10)
			{
				if( htu_read_some_bytes(ucaRecvBuf, 3))     //�������ֽ�����
				{
					crc = Get_CRC8(ucaRecvBuf,2);
					if(crc == ucaRecvBuf[2])
					{
						fRetVal = (ucaRecvBuf[0] << 8) | ucaRecvBuf[1];
						*fTemp = (175.72f) * fRetVal / 65536 - 46.85f; //�¶�:T= -46.85 + 175.72 * ST/2^16
					}
				}
				tim_tick_sta = 2;
				tim_tick = 0;
			}
			break;
		case 2:
			if(tim_tick++ >= 10)
			{
				ucTmpVal = NOHOLD_HUMI_MEASURE;                         //ʪ�Ȳ���
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
				if( htu_read_some_bytes(ucaRecvBuf, 3))     //�������ֽ�����
				{
					crc = Get_CRC8(ucaRecvBuf,2);
					if(crc == ucaRecvBuf[2])
					{
						fRetVal = (ucaRecvBuf[0] << 8) | ucaRecvBuf[1];
						*fHumi = (fRetVal * 125) / 65536 - 6.00f;     //ʪ��: RH%= -6 + 125 * SRH/2^16
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

/* ����HTU21D �û��Ĵ��� */
u8 HTU_Set_UserReg(u8 data)
{
	u8 cmd[2];
	
	cmd[0] = WRITE_USER_REG;
	cmd[1] = data;
	htu_write_some_bytes(cmd, 2);//����д�Ĵ���ָ��ʹ�д������
	return 0;
}

/* ��ȡHTU21D �û��Ĵ��� */
u8 HTU_Read_UserReg(void)
{
	u8 cmd = READ_USER_REG;
	u8 data = 0;
	
	htu_write_some_bytes(&cmd, 1);//���Ͷ��Ĵ���ָ��
	htu_read_some_bytes(&data, 1);//��ȡ�Ĵ�������
	
	return data; //���ؼĴ�������
}










