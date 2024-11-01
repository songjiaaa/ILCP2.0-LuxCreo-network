#ifndef CONFIG_H
#define CONFIG_H

#include "main.h"
#include "avflash.h"
#include "f4_flash.h"
#include "stm32f4xx.h"
#include "app.h"

//参数
#pragma pack(1)
typedef struct
{
	u8  language      : 3,           //语言
	    light_strip   : 1;           //rgb灯开关
	u8  sn[30];                      //SN号
	u8 	version[30];                 //程序版本
}s_cfg;
#pragma pack()


//芯片内部存储
#pragma pack(1)
typedef struct
{
	u16  id;
	u8 sn[30];
	int weight_offset;              //重量偏移值
}file_save_config;
#pragma pack()

#define CFG_FLASH_ADDR	0x08060000     //512k的flash 最后一个扇区（128k大小）的起始地址

int cfg_save(void); //
void cfg_get(void); //
void cfg_print(void);//打印cfg到调试口

extern s_cfg cfg_dft; 
extern file_save_config save_config;

#endif


