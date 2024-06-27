/*
flash.h
必须使用C的方式包含
*/
#ifndef F4_FLASH_H
#define F4_FLASH_H

#include "stm32f4_sys.h"

//#define FLASH_PAGE_407VE (2048)
//#define FLASH_PAGE 1024
//#define USER_FLASH_START 0x0800c800 //50K的位置

int flash_erase(void *pf); //擦MFLASH扇区,返回本扇区大小
int flash_write(u16 *p,int n,void *addr); //直接写入(按4字节,兼容接口用u16*)
int flash_eraseAwrite(u32 *p,int n,void *pf); //擦除并写入,输入数据指针，数据长度，MFLASH地址
int iap_flash_write(u32 *p,int n,void *pf);

#endif

