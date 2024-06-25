#ifndef _WIFI_USE_H_
#define _WIFI_USE_H_

#include "app.h"

#define SHOW_WIFI_LINE       5  
#define SHOW_MAX_WIFI_LIST   32
#define WIFI_NAME_BUF_SIZE   64




#pragma pack(1)
typedef struct 
{
	u8  execution_step;           //��ʼ������  0:��ʼ��ʼ�� 1:WiFi�б�ˢ�� 2:����Ƿ���Ҫ�Զ����� 3:����  4;����NTPʱ��У׼ 5:ִ��OTA���� 6:���в�ִ�в���
	u8  connect_state     :1,     //����״̬    0:δ����  1:����
		connect_flag      :1,     //���ӱ�־        0:��ִ������  1:ִ��һ������
		auto_connect      :1,     //0 Ϊ�״�����  1;�Ѿ����ӹ�
		sntp_cal 	      :1,     //1:ִ��NTPʱ��У׼
		trans_mode	      :4;     //����״̬ 0:��������  1:UDP͸��  2:TCP����
	u8  ota_run_step;             //OTA����ִ�в���
	u8  file_down_step;           //�ļ�����ִ�в�
	u32 down_timeout;             //���س�ʱ
}network_flow_type;
#pragma pack()


//wifi��Ϣ
#pragma pack(1)
typedef struct
{
	int enc;         //���ܷ�ʽ
	char ssid[WIFI_NAME_BUF_SIZE];     //ap ��
	int rssi;        //�ź�ǿ��
	char mac[32];    //mac��ַ
	int channel;     //ͨ��
	char password[WIFI_NAME_BUF_SIZE]; //����
	char ip[16];     //IP
}ap_info;
#pragma pack()


//wifi�б�ҳ
#pragma pack(1)
typedef struct
{
	u8 cur_page;         //��ǰҳ
	u8 total_page;       //��ҳ��
	u8 total_line;       //������
	u8 select_num;       //ѡ����
}wifi_page;
#pragma pack()


//extern wifi_user wifi_app_user;
extern network_flow_type wifi_flow;
extern ap_info cur_connected_ap;  //��ǰ���ӵ�AP
extern ap_info record_ap[SHOW_MAX_WIFI_LIST];

u8 wifi_init_process(void);
void wifi_module_run(void);
u8 wifi_list_refresh(void);
u8 wifi_link_ctrl(void);
void parse_ap_name(u8 *str);
void remove_duplicates(void);
void show_wifi_list(u8 cur_page);
void wifi_list_ctrl(u16 touch_id);
void wifi_input_pro(u16 touch_id,u8 *p,int len);

void wifi_check_box_icon(u16 touch_id);

#endif



