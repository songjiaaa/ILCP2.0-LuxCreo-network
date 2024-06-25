#ifndef _DATA_UPLOAD_H_
#define _DATA_UPLOAD_H_

#include "app.h"

#define  WIFI_UPLOAD_MODE               0x00      //WiFi�ϴ�
#define  ENTHERNET_UPLOAD_MODE          0x01      //��̫���ϴ�

#define  UPLOADING_DIRECTION            0x00      //���䷽��  0x00 �ϴ�   0x01�������·�

#define  IDLE_UPLOAD_TYPE               0x01      //����״̬���ϴ�����
#define  CURED_UPLOAD_TYPE              0x02      //�̻�����ͣ���ϴ�����
#define  DATA_IMPORT_TYPE               0x03      //���ݵ�������֡ 
#define  OTA_UPGRADE_TYPE               0x04      //OTA��������֡
#define  ERROR_UPLOAD_TYPE              0x05      //�����ϴ�

#pragma pack(1)
typedef struct 
{
	u16 head;     //֡ͷ
	u16 len;      //֡���� ָ����֡ȫ��
	u8 run_num;   //���
	u8 t_mode;    //���䷽ʽ   0x00 WiFi����    0x01 enthernet ����
	u8 t_dir;     //���䷽��   0x00 �ϴ�        0x01 ���� 
    u8 type;      //֡����
	u8 version[30]; //�豸�汾��
}upload_frame_head;
#pragma pack()


//���л�̻�����״̬���ϴ����ݰ�                  0x01
#pragma pack(1)
typedef struct 
{
	//���ݰ�
	u8 device_name[20];  //�豸����
	u8 SN[30];    //�豸SN��
	u8 status;    //�豸״̬
	float uv_power_cal;  //UV��Դ���ʵĲ���ֵ
	float up_uv_k;    //�ϵư����ϵ��
	float down_uv_k;  //�µư����ϵ��
	s16  heat_cal;    //�������Ȳ���
	s16  uv_heat_cal; //�̻����Ȳ��� 
	u8 door_state;    //��״̬
	u8 mac_lock_sta;  //����״̬
	u32 uv_total_time; //UVʹ����ʱ��
	u8 utc_time[30];   //���������ͽ���ʱutcʱ��
}idle_upload_frame;
#pragma pack()


//�̻����ϴ������ݰ�                    0x02
#pragma pack(1)
typedef struct 
{
	u8 device_name[20];  //�豸����
	u8 SN[30];           //�豸SN��
	u8 status;           //�豸״̬
}cure_upload_front;
#pragma pack()

extern char upload_cure_para_data[500];         //�̻�����

#pragma pack(1)
typedef struct 
{
	//���ݰ�
	u8 resin[30];      //������
	u8 app_area[30];   //Ӧ������
	u8 step;           //��ǰ�̻�����
	u8 step_time;      //��ǰ�̻������µ�ʱ��
	u8 up_uv_per;      //�ϵư��ǿ�ٷֱ�
	u8 down_uv_per;    //�µư��ǿ�ٷֱ�
	s16 step_temp;     //��ǰ�������趨�¶�
	u32 cure_total_time;    //��ǰ�����̻���ʱ��
	u32 r_time;        //ʣ��̻�ʱ�� 
	s16 ptc_temp;      //ptc����Ƭ���¶�
	s16 up_uv_temp;    //�ϵư�ɢ��Ƭ���¶�
	s16 down_uv_temp;  //�µư�ɢ��Ƭ���¶�
	u8  head_lamp;     //������״̬
	u8  motor_state;   //���״̬
	u8 utc_time[30];
}cure_upload_tail;
#pragma pack()


////�̻����������õ������ݰ� OTA�����������ݰ�              0x03  �� 0x04
//#pragma pack(1)
//typedef struct 
//{
//	//���ݰ�
//	u8 device_name[20];  //�豸����
//	u8 SN[30];           //�豸SN��
//	u8 version[30];      //�豸�汾��
//	u8 d_file_name[30];  //������ص��ļ��� 
//	u16 file_data_num;   //���ص��ļ�֡���
//}data_load_request;
//#pragma pack()

////�����ϴ�                                              0x05
//#pragma pack(1)
//typedef struct 
//{
//	//���ݰ�
//	u8 device_name[20];  //�豸����
//	u8 SN[30];           //�豸SN��
//	u8 version[30];      //�豸�汾��
//	u8 error;            //���ص��ļ�֡���
//}error_load;
//#pragma pack()


//֡β
#pragma pack(1)
typedef struct 
{
	u16 crc;
	u16 end;
}upload_end;
#pragma pack()

//Ӧ�ó���
#define DENTAL_MODEL                       "Dental Model"
#define SPLINT                             "Splint"
#define ALIGNER_RETAINER                   "Aligner/Retainer"
#define RESTORATION                        "Restoration"
#define DENTURE_BASE                       "Denture Base"
#define INDIRECT_BONDING_TRAY              "Indirect Bonding Tray"
#define DENTURE_TEETH                      "Denture Teeth"
#define CROWN_BRIDGE                       "Crown & Bridge"
#define IMPRESSION_TRAY                    "Impression Tray"
#define SURGICAL_GUIDE                     "Surgical Guide"
#define GINGIVA                            "Gingiva"

static const char resin_app_area[15][30] = 
{
	DENTAL_MODEL,
	SPLINT,
	ALIGNER_RETAINER,
	RESTORATION,
	DENTURE_BASE,
	INDIRECT_BONDING_TRAY,
	DENTURE_TEETH,
	CROWN_BRIDGE,
	IMPRESSION_TRAY,
	SURGICAL_GUIDE,
	GINGIVA,
};


extern idle_upload_frame idle_data;

extern cure_upload_front cure_data_head;      //�̻��л�̻���ͣ�ϴ������ݰ�
extern char upload_cure_para_data[500];       //�̻����������
extern cure_upload_tail cure_data_tail;       //�̻���ʽ��β

//extern cure_upload_frame cure_data;
//extern data_load_request request_data;
//extern error_load error_code;


void netword_package_upload(u8 t_mode,u8 type);
void p_data_upload(void);


#endif























