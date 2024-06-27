#ifndef _UI_UPGRADE_H_
#define _UI_UPGRADE_H_

#include "app.h"

#define U_DISK_MCU_FILE_PATH                  "2:/ILCP_UPGRADE_FILE/iLuxCurePro.bin"
#define U_DISK_UI_FILE_PATH                   "2:/ILCP_UPGRADE_FILE/ota.bin"

#define OTA_MCU_BIN_MEM_FILE        "1:/ILCP_UPGRADE_FILE/iLuxCurePro.bin"
#define OTA_UI_MEM_FILE             "1:/ILCP_UPGRADE_FILE/ota.bin"

#define MEM_MCU_WRITE_PATH     "1:/ILCP_UPGRADE_FILE/iLuxCurePro.bin"
#define MEM_UI_WRITE_PATH      "1:/ILCP_UPGRADE_FILE/ota.bin"
#define MEM_FOLDER_PATH        "1:/ILCP_UPGRADE_FILE"

//У���ļ����ڴ�Ĵ洢·��
#define MEM_MCU_CHECK_FILE_PATH     "1:/iLuxCurePro_check.bin"
#define MEM_UI_CHECK_FILE_PATH      "1:/ota_check.bin"

#define OTA_WRITE_BYTE     512
#define FILE_NAME_LEN      (sizeof("ota.bin") - 1)    //Ҫȥ��\0
//#define LCD_OTA_BAUDRATE   (115200)
#define LCD_OTA_BAUDRATE   (921600)

#define UI_START_ACK_YES        0x8011         //0x1180    //��С�˷�ʽ  
#define UI_START_ACK_NO         0x8111         //0x1181     

#define UI_UPDATA_ACK_YES       0x8022         //ÿ֡���µ����ݵ�Ӧ��
#define UI_UPDATA_SN_ERR        0x8222         //����֡SN����
#define UI_UPDATA_CHECK_ERR     0x8322         //����֡У�����
 
#define UI_UPDATA_YES           0x8033         //��Ļ��������Ӧ��
#define UPDATA_FILE_CHECK_YES   0x0155         //ota.bin �ļ�У��ɹ�
#define UPDATA_FILE_CHECK_NO    0x0055         //ota.bin �ļ�У��ʧ��
#define UPDATA_FILE_ZIP_YES     0x0166         //ota.bin �ļ���ѹ�ɹ�
#define UPDATA_FILE_ZIP_NO      0x0066         //ota.bin �ļ���ѹʧ��

typedef enum
{
	OTA_WAIT_ACK_IDLE,
	OTA_WAIT_START_ACK,
	OTA_WAIT_DOWNLOAD_ACK,
}UPGRADE_STATE;


//����UI����
#pragma pack(1)
typedef struct 
{
	u16 head;
	u16 len;
	u16 cmd;
	u8 sn;         //���к�
	u8 packet[OTA_WRITE_BYTE];
	u16 checksum;
	u32 end;
}ota_updata;
#pragma pack()



//����ָ��
#pragma pack(1)
typedef struct 
{
	u16 head;
	u16 len;
	u16 cmd;
	u32 baudrate;
	u32 filesize;
	u8 down_name[FILE_NAME_LEN];
	u32 end;
}ota_cmd;
#pragma pack()


//����״̬
#pragma pack(1)
typedef struct 
{
	u8 update_status_flow;   //����״̬����
	u8 update_file_mode;     //�����ļ�ģʽ   0 ������  1 ����UI�ļ� 2 ����mcu�ļ� 3 �����ļ�������
	u8 ui_ota_state;         //UI����״̬
	u16 ota_ack;             //UIӦ���־
	u32 timeout;             //��Ļ��ʱ
	u8 sent_cnt;             //�������ݴ���
}upgrade_stat;
#pragma pack()


//UI��������Ӧ��
#pragma pack(1)
typedef struct 
{
	u16 head;
	u16 cmd;
	u32 end;
}ui_upgrade_start_ack;
#pragma pack()

//UI��������Ӧ��
#pragma pack(1)
typedef struct 
{
	u16 head;
	u16 cmd;
	u8  sn;
	u32 end;
}ui_upgrade_data_ack;
#pragma pack()


int upgrade_ui(void);
int updating_task(upgrade_stat *s);

extern upgrade_stat updata_state;

#endif




