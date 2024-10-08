#ifndef _DAP_MOTOR_H
#define _DAP_MOTOR_H

#include "app.h"

#define M_MOTOR_L         PDin(0)         //��������λ������Ϊ0   ������ӡ��
#define M_MOTOR_R         PDin(1)         //���帺��λ������Ϊ0
#define S_MOTOR_L         PDin(2)         //��������λ ����Ϊ1   ������ӡ��
#define S_MOTOR_R         PDin(3)         //��������λ ����Ϊ1   
#define HOPPER_L          PDin(4)         //�϶�����λ ���� �͵�ƽ
#define HOPPER_R          PDin(5)         //�϶�����λ ���� �͵�ƽ
#define EG_STOP           PDin(6)         //��ͣ �ɿ��ǵ͵�ƽ  ����Ϊ��
#define CB_H_DET          PDin(7)         //�Ͽ� �����Ĥ�߶� �����͵�ƽ
#define CB_LOC       	  PDin(8)         //�Ͽ� ��װ��λ��� ����Ϊ�׵�ƽ
#define PRINTER_LOC       PDin(9)         //��ӡ����ת��λ��� �����͵�ƽ
#define IN_MATERIALS      PDin(10)        //�����м��
#define S_SENSOR_L        PDin(11)        //�����ߴ����� ����Ϊ��  Ŀǰ�޷�����  Ŀǰ�ṹԭ���޷�����
#define S_SENSOR_R        PDin(12)        //�����ߴ����� ����Ϊ��  Ŀǰ�޷�����  Ŀǰ�ṹԭ���޷�����
#define FRAME_DOOR        PDin(13)        //����ŵ�λ��� �����͵�ƽ
#define S_MOTOR_ALM       PDin(14)        //�ε��������
#define M_MOTOR_ALM       PDin(15)        //����������

#define LED_R             PGout(13)       //��
#define LED_G		  	  PGout(14)       //��
#define LED_Y	 		  PGout(15)       //��


//#define MAIN_MOTOR_DIR       PEout(8)        //����ƽ̨�����������   1 ǰ��  0����
//#define SCRAPER_MOTOR_DIR    PGout(3)        //�ε�ƽ̨�����������   0 ǰ��  1����

#define MAIN_MOTOR_DIR       PEout(9)        //����ƽ̨�����������   1 ǰ��  0����
#define SCRAPER_MOTOR_DIR    PEout(8)        //�ε�ƽ̨�����������   0 ǰ��  1����

#define MAIN_FORWAED            1                //ǰ��
#define MAIN_BACKWARD           0                //����

#define SCRAPER_FORWAED         1                //ǰ��
#define SCRAPER_BACKWARD        0                //����

#define IN_MATERIALS_SW         PGout(4)           //���Ͽ���  0 ����������� 1�����м���

#define SCRAPER_STEP_LEN       (1)          // (2000/2000) 1   �ε���� һ������˿����ƽ̨�ƶ��ľ���  8000/Ȧ
#define PLATFORM_STEP_LEN      (20)             // (4000/200) um ƽ̨��� һ������˿����ƽ̨�ƶ��ľ���  200/Ȧ   ���� �����תһȦ���ߵľ���

#define PLATFORM_MOTOR          0              //ƽ̨���
#define SCRAPER_MOTOR           1              //�ε����

#define HOPPER_DUMP      do{ PGout(1) = 1;PGout(2) = 0; }while(0)     //����   HOPPER_R == 0
#define HOPPER_RESET     do{ PGout(1) = 0;PGout(2) = 1; }while(0)     //��λ   HOPPER_L == 0
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
	u8  motor_sn;        //�����
	u8  run  :1,         //���б�־  0����ֹ  1������
		dir  :1,         //�˶�����
		stat :6;         //���״̬  
	u32 max_len;         //��������г�
	u32 pulse_num;       //�����ƶ�������Ҫ���͵�������
	u32 pulse_cnt;       //��ǰ���͵�������     
	s32 pos;  		     //���λ��
	u32 dis;             //�ƶ�����
	u32 acc_tm;          //����ʱ��  s
	u32 dec_tm;          //����ʱ��  s
	float start_v;       //�����ٶ� mm/s
	float max_v;         //����ٶ� mm/s
	float end_v;         //ֹͣ�ٶ� mm/s
}motor_parameter;
#pragma pack()

//ƽ̨����Ĳ���
#define PLATFORM_MOTOR_STEPPS(ums)   (ums / PLATFORM_STEP_LEN)    //���ݵ��ת�٣�um/s�������������٣�step/s��
//����������ٶ�
#define SCRAPER_MOTOR_STEPPS(ums)    (ums / SCRAPER_STEP_LEN)     //���ݵ��ת�٣�um/s�������������٣�step/s��

#define ACCEL_TIME(t)                ( (t) / 2 )                             /* �Ӽ��ٶκͼ����ٶε�ʱ������ȵ� */
#define INCACCELSTEP(j,t)            ( ( (j) * pow( (t) , 3 ) ) / 6.0f )     /* �Ӽ��ٶε�λ����(����)  S = 1/6 * J * t^3 */
#define SPEED_MIN                   (84000000/300 / (65535.0f))                  /* ���Ƶ��/�ٶ� */

#pragma pack(1)
typedef struct
{
    int32_t vo;             /*  ���ٶ� ��λ step/s */
    int32_t vt;             /*  ĩ�ٶ� ��λ step/s */
    int32_t accel_step;     /*  ���ٶεĲ�����λ step */
    int32_t decel_step;     /*  ���ٶεĲ�����λ step */
    float   *accel_tab;     /*  �ٶȱ�� ��λ step/s �������������Ƶ�� */
    float   *decel_tab;     /*  �ٶȱ�� ��λ step/s �������������Ƶ�� */
    float   *ptr;           /*  �ٶ�ָ�� */
    int32_t dec_point;      /*  ���ٵ� */
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








