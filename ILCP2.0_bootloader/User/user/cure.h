#ifndef _CURE_H__
#define _CURE_H__

#include "cure.h"
#include "app.h"

/**********************************************************************************/
#define MAX_FILE_LINE           4096       
#define MAX_PARA_NUM            60

#define PROCESS_STEPS           5

#pragma pack(1)
typedef struct
{
	u8   resin[30];      
	u8   up_uv[PROCESS_STEPS];        
	u8   down_uv[PROCESS_STEPS];     
	s16  heat[PROCESS_STEPS];         
	u8   time[PROCESS_STEPS];         
//	bool n2[5];
}cure_para;
#pragma pack()

#pragma pack(1)
typedef struct
{
	u8  mode;       //模式  0空闲   1固化运行  2固化运行中检测到开门
	u8  step;       //流程步骤
	u32 step_cnt_time;   //每步的计数时间
	s16 exp_temp;   //期望温度
	u16 r_time;     //剩余时间
	u16 total_time; //总时间
}curing_pro;
#pragma pack()

#define CURE_FILE  "2:Cure.txt"     

#define DENTAL     "Dental Model"
#define SPLINT     "Splint"
#define ALIGNER    "Aligner/Retainer"

#define RESIN      "Resin:"
#define STEP       "Step:"
#define UP_UV      "Up_UV:"
#define DOWN_UV    "Down_UV:"
#define HEAT       "Heat:"
//#define N2         "N2:"
#define TIME       "Time:"

extern curing_pro cure;
extern cure_para cure_parameter[MAX_PARA_NUM],exe_parameter;
extern cure_para user_custom[MAX_PARA_NUM];


void my_gbktouni(char* destin,char* source,u32 len);
int get_str_num(char *buf,unsigned char len);

void get_cure_file(u16 scene);
void show_cur_cure_page(u16 cur_page);
void cure_page_info(u16 scene);
void analytical_curing_parameter(void);
void show_para(u16 screen_id,u16 control_id,cure_para *para);

/************************************************************************************/


/************************************************************************************/
#pragma pack(1)
typedef struct
{
	u16  cur_page;            
	u16  total_page;         
	u16  total_line;         
	s16  select_line;         
	s16  select_num;         
}page_record;
#pragma pack()

#define TOTAL_NUM_LINE_PAGE     4    
/************************************************************************************/

void cure_select(u16 id);

int cure_start(void);
int cure_stop(void);
void curing_pause(void);
void curing_continue(void);
void cure_state_process(void);
void show_hour_min_sec(u16 screen_id,u16 control_id,u32 seconds);
void cure_show_text(void);
int temperature_control(s16 exp_temp,u8 sta);


#endif


