#ifndef _DAP_MOTOR_H
#define _DAP_MOTOR_H

#include "app.h"

#define M_MOTOR_L         PDin(0)         //主体正限位，触发为0   靠近打印机
#define M_MOTOR_R         PDin(1)         //主体负限位，触发为0
#define S_MOTOR_L         PDin(2)         //铲刀正限位 触发为1   靠近打印机
#define S_MOTOR_R         PDin(3)         //铲刀负限位 触发为1   
#define HOPPER_L          PDin(4)         //料斗正限位 触发 低电平
#define HOPPER_R          PDin(5)         //料斗负限位 触发 低电平
#define EG_STOP           PDin(6)         //急停 松开是低电平  按下为高
#define CB_H_DET          PDin(7)         //料筐 检测牙膜高度 触发低电平
#define CB_LOC       	  PDin(8)         //料筐 安装到位检测 触发为底电平
#define PRINTER_LOC       PDin(9)         //打印机旋转到位检测 触发低电平
#define IN_MATERIALS      PDin(10)        //加料中检测
#define S_SENSOR_L        PDin(11)        //铲刀边传感器 触发为底  目前无法触发  目前结构原因无法触发
#define S_SENSOR_R        PDin(12)        //铲刀边传感器 触发为底  目前无法触发  目前结构原因无法触发
#define FRAME_DOOR        PDin(13)        //外框门到位检测 触发低电平
#define S_MOTOR_ALM       PDin(14)        //刮刀电机报警
#define M_MOTOR_ALM       PDin(15)        //主体电机报警

#define LED_R             PGout(13)       //红
#define LED_G		  	  PGout(14)       //绿
#define LED_Y	 		  PGout(15)       //黄


//#define MAIN_MOTOR_DIR       PEout(8)        //主体平台电机方向引脚   1 前进  0后退
//#define SCRAPER_MOTOR_DIR    PGout(3)        //刮刀平台电机方向引脚   0 前进  1后退

#define MAIN_MOTOR_DIR       PEout(9)        //主体平台电机方向引脚   1 前进  0后退
#define SCRAPER_MOTOR_DIR    PEout(8)        //刮刀平台电机方向引脚   0 前进  1后退

#define MAIN_FORWAED            1                //前进
#define MAIN_BACKWARD           0                //后退

#define SCRAPER_FORWAED         1                //前进
#define SCRAPER_BACKWARD        0                //后退

#define IN_MATERIALS_SW         PGout(4)           //加料开关  0 拉低允许加料 1不运行加料

#define SCRAPER_STEP_LEN       (1)          // (2000/2000) 1   刮刀电机 一个脉冲丝杆上平台移动的距离  8000/圈
#define PLATFORM_STEP_LEN      (20)             // (4000/200) um 平台电机 一个脉冲丝杆上平台移动的距离  200/圈   导程 电机旋转一圈行走的距离

#define PLATFORM_MOTOR          0              //平台电机
#define SCRAPER_MOTOR           1              //刮刀电机

#define HOPPER_DUMP      do{ PGout(1) = 1;PGout(2) = 0; }while(0)     //倒料   HOPPER_R == 0
#define HOPPER_RESET     do{ PGout(1) = 0;PGout(2) = 1; }while(0)     //回位   HOPPER_L == 0
#define HOPPER_STATIC    do{ PGout(1) = 0;PGout(2) = 0; }while(0) 

#define LOG_COLOUR_RED      do{ LED_R = 1; LED_G = 0;LED_Y = 0; }while(0) 
#define LOG_COLOUR_GREEN    do{ LED_R = 0; LED_G = 1;LED_Y = 0; }while(0) 
#define LOG_COLOUR_YELLOW   do{ LED_R = 0; LED_G = 0;LED_Y = 1; }while(0) 
#define LOG_COLOUR_BLACK    do{ LED_R = 0; LED_G = 0;LED_Y = 0; }while(0) 

#define TOP_LOCK_ON       do{ PGout(3) = 1;PGout(12) = 1; }while(0) 
#define TOP_LOCK_OFF      do{ PGout(3) = 0;PGout(12) = 0; }while(0) 

#pragma pack(1)
typedef struct 
{
	u8  motor_sn;        //电机号
	u8  run  :1,         //运行标志  0：静止  1：运行
		dir  :1,         //运动方向
		stat :6;         //电机状态  
	u32 max_len;         //最大允许行程
	u32 pulse_num;       //根据移动距离需要发送的脉冲数
	u32 pulse_cnt;       //当前发送的脉冲数     
	s32 pos;  		     //电机位置
	u32 dis;             //移动距离
	u32 acc_tm;          //加速时间  s
	u32 dec_tm;          //减速时间  s
	float start_v;       //启动速度 mm/s
	float max_v;         //最高速度 mm/s
	float end_v;         //停止速度 mm/s
}motor_parameter;
#pragma pack()

//平台电机的步速
#define PLATFORM_MOTOR_STEPPS(ums)   (ums / PLATFORM_STEP_LEN)    //根据电机转速（um/s），计算电机步速（step/s）
//铲刀电机的速度
#define SCRAPER_MOTOR_STEPPS(ums)    (ums / SCRAPER_STEP_LEN)     //根据电机转速（um/s），计算电机步速（step/s）

#define ACCEL_TIME(t)                ( (t) / 2 )                             /* 加加速段和减加速段的时间是相等的 */
#define INCACCELSTEP(j,t)            ( ( (j) * pow( (t) , 3 ) ) / 6.0f )     /* 加加速段的位移量(步数)  S = 1/6 * J * t^3 */
#define SPEED_MIN                   (84000000/300 / (65535.0f))                  /* 最低频率/速度 */

#pragma pack(1)
typedef struct
{
    int32_t vo;             /*  初速度 单位 step/s */
    int32_t vt;             /*  末速度 单位 step/s */
    int32_t accel_step;     /*  加速段的步数单位 step */
    int32_t decel_step;     /*  加速段的步数单位 step */
    float   *accel_tab;     /*  速度表格 单位 step/s 步进电机的脉冲频率 */
    float   *decel_tab;     /*  速度表格 单位 step/s 步进电机的脉冲频率 */
    float   *ptr;           /*  速度指针 */
    int32_t dec_point;      /*  减速点 */
    int32_t step;
    int32_t step_pos;
}speed_calc_t;
#pragma pack()


extern motor_parameter scraper_motor;
extern motor_parameter platform_motor;

void limit_sensor_init(void);
void scraper_motor_init(void);
void platform_motor_init(void);

void set_motor_speed(u8 motor_sn,float fre_v);
u8 motor_move(motor_parameter *m);
void motor_stop(u8 motor_sn);
void pause_motor_move(void);
void continue_motor_move(void);

u8 hopper_dump(u8 sta);

uint8_t calc_speed(u8 motor_sn, int32_t vo, int32_t vt, float time);

#endif








