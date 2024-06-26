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
	    light_strip   : 1,           //rgb灯开关
		heat_sw       : 1,           //加热开关
		w_data        : 1,           //写测试数据
		n2_enable     : 1,           //氮气使能
		head_lamp     : 1;           //顶部照明灯
	u8  init_step;                   //模块初始化步骤
	u8  network_selet;               //网络模块选择   0：WiFi 1：以太网  
	u8  m_ctrl        : 1,           //电机控制按钮
		res           : 7; 
	u8  u_disk_state;                //u盘状态  0：拔出 1：插入 2：无法识别
	s16 heat_temp;                   //加热温度 
	u8	res_null;
	u8  sn[30];                      //SN号
	u8 	version[30];                 //程序版本
	char* ali_domain_name;           //TCP连接服务器下载用
	char* lux_domain_name;           //公司服务器，上报数据使用
	char ip_buff[16];
}s_cfg;
#pragma pack()

#pragma pack(1)
typedef struct
{
	u32 uv_total_time;            //UV灯板使用总时长
	u32 cure_total_time;          //固化流程时间
	u32 run_total_time;           //开机运行总时间
}save_total_time;
#pragma pack()

//芯片内部存储
#pragma pack(1)
typedef struct
{
	u16   id;
	u8    upgrade_state;          //数据更新状态  1：重启从U盘读取文件升级  2：重启从闪存读取升级文件
	u8    set_ac      :1,         //输入电压选择  0：110V  1:220V
		  set_server  :1,         //服务器选择 0 国内服务器 1国外服务器
		  reserved    :6;         //预留位
	float uv_power_max;           //UV 最大光源输出功率
	float uv_cal_k;               //使用功率
	float up_uv_k;                //上灯板调整系数
	float down_uv_k;              //下灯板调整系数
	s16 heat_cal;                 //独立加热补偿
	s16 uv_heat_cal;              //固化和加热同开时的补偿
}file_save_config;
#pragma pack()

#define CFG_FLASH_ADDR	0x08060000     //512k的flash 最后一个扇区（128k大小）的起始地址

#define CONFIG_FILE       "1:CONFIG_FILE.TXT"
#define TT_FILE           "1:TOTAL_FILE.TXT"
#define SN_FILE           "1:SN_FILE.TXT"
#define WIFI_LINK_SAVE    "1:WIFI_LINK_FILE.TXT"

int cfg_save(void); //
void cfg_get(void); //
void cfg_print(void);//打印cfg到调试口

extern s_cfg cfg_dft; 
extern save_total_time save_time;
extern file_save_config save_config;

#endif


