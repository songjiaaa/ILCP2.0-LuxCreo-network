#ifndef _DAP_APP_H
#define _DAP_APP_H

#include "app.h"

//modbus 03帧
#pragma pack(1)
typedef struct
{
	u8   addr;     
	u8   cmd;                
	u16  data_addr;             
	u16  data; 
	u16  crc;
}dap_03cmd;
#pragma pack()

//modbus 10帧
#pragma pack(1)
typedef struct
{
	u8   addr;     
	u8   cmd;                
	u16  data_addr;             
	u16  data1;
	u16  data2;
	u8   data3;
	u16  crc;            
}dap_10cmd;
#pragma pack()

////输入IO状态上传
//#pragma pack(1)
//typedef struct
//{
//	u8   addr;         //设备地址
//	u8   cmd;          //功能码
//	u8   len;          //数据长度
//	u8	    scraper_r		:1,     //铲刀轴负限位
//	        scraper_z		:1,     //铲刀轴原点位
//		    scraper_l		:1,     //铲刀轴正限位
//			main_r			:1,      //主体轴负限位
//			main_z			:1,		//主体轴原点位
//			main_l			:1,     //主体轴正限位
//			cb_sensor_l		:1,     //接筐盒正限位
//			cb_sensor_r		:1,     //接筐盒负限位

//			printer_loc		:1,     //打印机旋转台检测
//			material_det	:1,     //0 缺料 1 有料
//			in_material_det :1,     //0 加料完成， 1加料中
//			door_det		:1,     //外围门检测 0 未关好  1 关好
//			e_stop  		:1,     //急停按钮 0 按下  1正常
//			res				:1,     //空
//			scraper_alm		:1,
//			main_alm   		:1;	
//	u16  crc;            
//}in_io_state1;
//#pragma pack()

//输入IO状态上传  转换大小端
#pragma pack(1)
typedef struct
{
	u8   addr;         //设备地址
	u8   cmd;          //功能码
	u8   len;          //数据长度
	
	u16		printer_loc		:1,     //打印机旋转台检测
			material_det	:1,     //0 缺料 1 有料
			in_material_det :1,     //0 加料完成， 1加料中
			door_det		:1,     //外围门检测 0 未关好  1 关好
			e_stop  		:1,     //急停按钮 0 按下  1正常
			res				:1,     //空
			scraper_alm		:1,
			main_alm   		:1,	
	
		    scraper_r		:1,     //铲刀轴负限位
	        scraper_z		:1,     //铲刀轴原点位
		    scraper_l		:1,     //铲刀轴正限位
			main_r			:1,      //主体轴负限位
			main_z			:1,		//主体轴原点位
			main_l			:1,     //主体轴正限位
			cb_sensor_l		:1,     //接筐盒正限位
			cb_sensor_r		:1;     //接筐盒负限位
	
	u16  crc;            
}in_io_state1;
#pragma pack()

//输入IO状态上传
#pragma pack(1)
typedef struct
{
	u8   addr;         //设备地址
	u8   cmd;          //功能码
	u8   len;          //数据长度
	u16	   	res2			:8,
	
			scr_self_l		:1,     //铲刀自感应左侧传感器
	        scr_self_r		:1,     //铲刀自感应右侧传感器
		    cb_loc			:1,     //料筐安装到位开关  0  料筒未到位  1 料筒到位
			cb_h_det		:1,     //料筐溢出检测
			res   			:4;
	u16  crc;            
}in_io_state2;
#pragma pack()


//输出IO状态上传
#pragma pack(1)
typedef struct
{
	u8   addr;         //设备地址
	u8   cmd;          //功能码
	u8   len;          //数据长度
	u16	   	res2			:1,	    //空
			run_button		:1,      //运行按钮指示灯
			led_r			:1,			//红灯
			led_g			:1,			//绿灯
			led_b			:1,			//黄灯
			res3   			:3,	    //空
	
			scraper_pwm		:1,     //刮胶轴脉冲
	        scraper_dir		:1,     //刮胶轴方向
		    main_pwm		:1,     //主体轴脉冲（铲台）
			main_dir		:1,     //主体轴方向（铲台）
			rec_material    :1,     //接料（0=收料盒待机；1=收料盒去接胶位中）
			pour_material	:1,     //倒料（0=收料盒待机；1=收料盒去倒胶位中）
			in_material     :1,     //注液泵电机（1=允许加料；0=不允许加料）
			res1			:1;		//空
	
	u16  crc;            
}out_io_state;
#pragma pack()



#pragma pack(1)
typedef struct
{
	u8  dap_state;      //DAP设备状态    1:复位状态
	u8  auto_state;     //自动铲料状态   
	u8  reset_state;    //复位状态            
           
}dap_state;
#pragma pack()

extern COMM_HEAD dap_pack;

void dap_cmd_process(u8 * b,int len);
void responsde_cmd(u8 cmd,u8 state);
void automatic_shoveling_pro(void);
void lamp_flickering(void);
u8 reset_motor(void);
#endif




