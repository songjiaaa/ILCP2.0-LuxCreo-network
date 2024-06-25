#ifndef _DAP_APP_H
#define _DAP_APP_H

#include "app.h"

//modbus 03֡
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

//modbus 10֡
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

////����IO״̬�ϴ�
//#pragma pack(1)
//typedef struct
//{
//	u8   addr;         //�豸��ַ
//	u8   cmd;          //������
//	u8   len;          //���ݳ���
//	u8	    scraper_r		:1,     //�����Ḻ��λ
//	        scraper_z		:1,     //������ԭ��λ
//		    scraper_l		:1,     //����������λ
//			main_r			:1,      //�����Ḻ��λ
//			main_z			:1,		//������ԭ��λ
//			main_l			:1,     //����������λ
//			cb_sensor_l		:1,     //�ӿ������λ
//			cb_sensor_r		:1,     //�ӿ�и���λ

//			printer_loc		:1,     //��ӡ����ת̨���
//			material_det	:1,     //0 ȱ�� 1 ����
//			in_material_det :1,     //0 ������ɣ� 1������
//			door_det		:1,     //��Χ�ż�� 0 δ�غ�  1 �غ�
//			e_stop  		:1,     //��ͣ��ť 0 ����  1����
//			res				:1,     //��
//			scraper_alm		:1,
//			main_alm   		:1;	
//	u16  crc;            
//}in_io_state1;
//#pragma pack()

//����IO״̬�ϴ�  ת����С��
#pragma pack(1)
typedef struct
{
	u8   addr;         //�豸��ַ
	u8   cmd;          //������
	u8   len;          //���ݳ���
	
	u16		printer_loc		:1,     //��ӡ����ת̨���
			material_det	:1,     //0 ȱ�� 1 ����
			in_material_det :1,     //0 ������ɣ� 1������
			door_det		:1,     //��Χ�ż�� 0 δ�غ�  1 �غ�
			e_stop  		:1,     //��ͣ��ť 0 ����  1����
			res				:1,     //��
			scraper_alm		:1,
			main_alm   		:1,	
	
		    scraper_r		:1,     //�����Ḻ��λ
	        scraper_z		:1,     //������ԭ��λ
		    scraper_l		:1,     //����������λ
			main_r			:1,      //�����Ḻ��λ
			main_z			:1,		//������ԭ��λ
			main_l			:1,     //����������λ
			cb_sensor_l		:1,     //�ӿ������λ
			cb_sensor_r		:1;     //�ӿ�и���λ
	
	u16  crc;            
}in_io_state1;
#pragma pack()

//����IO״̬�ϴ�
#pragma pack(1)
typedef struct
{
	u8   addr;         //�豸��ַ
	u8   cmd;          //������
	u8   len;          //���ݳ���
	u16	   	res2			:8,
	
			scr_self_l		:1,     //�����Ը�Ӧ��ഫ����
	        scr_self_r		:1,     //�����Ը�Ӧ�Ҳഫ����
		    cb_loc			:1,     //�Ͽ�װ��λ����  0  ��Ͳδ��λ  1 ��Ͳ��λ
			cb_h_det		:1,     //�Ͽ�������
			res   			:4;
	u16  crc;            
}in_io_state2;
#pragma pack()


//���IO״̬�ϴ�
#pragma pack(1)
typedef struct
{
	u8   addr;         //�豸��ַ
	u8   cmd;          //������
	u8   len;          //���ݳ���
	u16	   	res2			:1,	    //��
			run_button		:1,      //���а�ťָʾ��
			led_r			:1,			//���
			led_g			:1,			//�̵�
			led_b			:1,			//�Ƶ�
			res3   			:3,	    //��
	
			scraper_pwm		:1,     //�ν�������
	        scraper_dir		:1,     //�ν��᷽��
		    main_pwm		:1,     //���������壨��̨��
			main_dir		:1,     //�����᷽�򣨲�̨��
			rec_material    :1,     //���ϣ�0=���Ϻд�����1=���Ϻ�ȥ�ӽ�λ�У�
			pour_material	:1,     //���ϣ�0=���Ϻд�����1=���Ϻ�ȥ����λ�У�
			in_material     :1,     //עҺ�õ����1=������ϣ�0=��������ϣ�
			res1			:1;		//��
	
	u16  crc;            
}out_io_state;
#pragma pack()



#pragma pack(1)
typedef struct
{
	u8  dap_state;      //DAP�豸״̬    1:��λ״̬
	u8  auto_state;     //�Զ�����״̬   
	u8  reset_state;    //��λ״̬            
           
}dap_state;
#pragma pack()

extern COMM_HEAD dap_pack;

void dap_cmd_process(u8 * b,int len);
void responsde_cmd(u8 cmd,u8 state);
void automatic_shoveling_pro(void);
void lamp_flickering(void);
u8 reset_motor(void);
#endif




