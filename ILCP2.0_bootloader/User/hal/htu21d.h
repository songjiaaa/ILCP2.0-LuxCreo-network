#ifndef __HTU21D_H_
#define __HTU21D_H_

#include "app.h"

#define HTU_SCL        PEout(8) //SCL
#define HTU_SDA        PCout(4) //SDA ‰≥ˆ	 
#define HTU_READ_SDA   PCin(4)  //SDA ‰»Î

#define HTDU21D_ADDRESS           0x40
#define HOLD_TEMP_MEASURE         0xE3
#define HOLD_HUMD_MEASURE         0xE5
#define NOHOLD_TEMP_MEASURE       0xF3
#define NOHOLD_HUMI_MEASURE       0xF5
#define WRITE_USER_REG            0xE6
#define READ_USER_REG             0xE7
#define SOFT_RESET                0xFE


#define I2C_NACK   1
#define I2C_ACK    0

#define Delay_us(x)   bsp_DelayUS(x)

void htu21d_init(void);
void HTU_SDA_OUT(void);
void HTU_SDA_IN(void);
void HTU_Start(void);
void HTU_Stop(void);
void I2c_SendOneByte(uint8_t _ucData);
uint8_t I2c_RecvOneByte(uint8_t _ucAck);
uint8_t I2c_WaitAck(uint16_t _usErrTime);
void I2c_GetAck(void);
void I2c_GetNack(void);
int htu_write_some_bytes(uint8_t *pbdata, uint16_t write_length);
int htu_read_some_bytes(uint8_t *pbdata, uint16_t read_length);
void htu_measure(float *fTemp,float *fHumi);

u8 HTU_Set_UserReg(u8 data);
u8 HTU_Read_UserReg(void);

#endif



