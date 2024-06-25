#ifndef COM_FUN_H
#define COM_FUN_H

#include "main.h"


#ifdef __cplusplus
extern "C"
{
#endif

u8 check_sum(u8 *p,int n);
u16 check_sum16(u8 *p,int n);

u16 cal_crc(u8 *ptr,u32 len);
u16 GetModbusCRC16(u8 *cp,int leng);
u8 Get_CRC8(u8 *ptr, u8 len);
u64 crc64ecma(const unsigned char *data, int len);

#ifdef __cplusplus
}
#endif

#endif

