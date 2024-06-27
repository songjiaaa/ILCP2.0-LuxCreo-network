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
u8 Get_CRC8(u8 *ptr, u8 len);

#ifdef __cplusplus
}
#endif

#endif

