#ifndef _SETTING_H_
#define _SETTING_H_
#include "app.h"


#pragma pack(1)
typedef struct
{
	u8 cal_set;       //校准界面处
	u8 eject_key;     //SN 弹出键盘
	u8 total_time;    //总时长
	u8 custom_para;   //客户参数
	u8 decive_para;   //设备参数弹框
	u8 u_disk;        //u盘参数导入
	u8 set_ac;        //设置ptc使用电压
}pop_type_flag;
#pragma pack()


#define SETTTING_PAGE_ID1    20
#define SETTTING_PAGE_ID2    21
 
extern pop_type_flag p_flag;

extern u8 hide_screen_click;

void setting_show(void);
void setting_ctrl1(u16 touch_id);
void setting_ctrl2(u16 touch_id);
void control_input_pro(u16 touch_id,u8 *p,int len);
void eject_input_pro(u8 *p,int len);
void cal_input_pro(u16 touch_id,u8 *p,int len);
void uvled_total_time_show(void);
void hidden_function_screen(u16 touch_id);
void hidden_function_screen_two(u16 touch_id);

#endif




















