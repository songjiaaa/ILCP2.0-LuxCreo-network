/*
avflash.h
必须使用C的方式包含
*/
#ifndef AVFLASH_H
#define AVFLASH_H

#include "main.h"

//均匀擦写flash
//Flash擦除后值为0xffff
extern u32 AV_FLASH_START; //50K的位置
extern u32 AV_FLASH_LEN; //均匀擦写配置区域总长度(字节)

void *av_flash_read(void); //在均匀擦写Flash中读取数据，返回有效数据指针
int av_flash_write(u16 *p16,int n); //写，输入数据首指针，长度单位字节,必须在规定的大小内

#endif

