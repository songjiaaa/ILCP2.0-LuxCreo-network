#ifndef _OTA_UPDATE_H_
#define _OTA_UPDATE_H_

#include "app.h"
//���ڰ����Ʒ�����  home
//luxlink.oss-cn-beijing.aliyuncs.com

//���Ⱒ���Ʒ�����  abroad
//lux-link-usa.oss-us-west-1.aliyuncs.com

//#define  OTA_MCU_BIN_URL       "http://luxlink.oss-cn-beijing.aliyuncs.com/Cure/ILCP_UPGRADE_FILE/iLuxCurePro.bin"
//#define  OTA_UI_BIN_URL        "http://luxlink.oss-cn-beijing.aliyuncs.com/Cure/ILCP_UPGRADE_FILE/ota.bin"

//OTA�����ļ��ķ���������
#define  OTA_DOMESTIC_DNS           "luxlink.oss-cn-beijing.aliyuncs.com"         //����
#define  OTA_FOREIGN_DNS            "lux-link-usa.oss-us-west-1.aliyuncs.com"     //����

#define  PORT_NUMBER                "80"
//OTA�����ļ��ķ�����IP
#define  OTA_DOMESTIC_IP            "o.luxcreo.cn"                    //����
#define  OTA_FOREIGN_IP             "47.88.111.29"                    //����



//OTA�����ļ��ڷ������洢·��    
#define  UI_OTA_FILE_PATH           "/Cure/ILCP_UPGRADE_FILE/ota.bin"
#define  MCU_OTA_FILE_PATH          "/Cure/ILCP_UPGRADE_FILE/iLuxCurePro.bin"
#define  CURE_OTA_FILE_PATH         "/Cure/ILCP_UPGRADE_FILE/Cure.txt"
  
//У���ļ��ڷ������洢·��
#define  VERSION_FILE_PATH			"/Cure/ILCP_UPGRADE_FILE/Version.txt"
#define  UI_CHECK_FILE_PATH         "/Cure/ILCP_UPGRADE_FILE/ota_check.bin"
#define  MCU_CHECK_FILE_PATH        "/Cure/ILCP_UPGRADE_FILE/iLuxCurePro_check.bin"
#define  CURE_CHECK_FILE_PATH       "/Cure/ILCP_UPGRADE_FILE/Cure_check.bin"

//�ļ����ڴ�Ĵ洢·��
#define MEM_FOLDER_PATH             "1:/ILCP_UPGRADE_FILE"
#define MEM_MCU_WRITE_PATH          "1:/ILCP_UPGRADE_FILE/iLuxCurePro.bin"
#define MEM_UI_WRITE_PATH           "1:/ILCP_UPGRADE_FILE/ota.bin"
#define MEM_CURE_FILE_WRITE_PATH    "1:/Cure.txt"


//У���ļ����ڴ�Ĵ洢·��
#define MEM_VERSION_FILE_PATH	    "1:/Version.txt"
#define MEM_MCU_CHECK_FILE_PATH     "1:/iLuxCurePro_check.bin"
#define MEM_UI_CHECK_FILE_PATH      "1:/ota_check.bin"
#define MEM_CURE_CHECK_FILE_PATH    "1:/Cure_check.bin"

//�涨�����ļ���U���е�·��
#define U_DISK_MCU_FILE_PATH        "2:/ILCP_UPGRADE_FILE/iLuxCurePro.bin"
#define U_DISK_UI_FILE_PATH         "2:/ILCP_UPGRADE_FILE/ota.bin"

#define DOWNLOAD_DATA_LEN           1024



//ota �ļ�����
#pragma pack(1)
typedef struct
{
	int start;
	int end;
	u8 repeated_req;      //�ظ�����
	u8 crc_err;           //У�����
	u32 file_size;        //�ļ���С
	u32 write_byte;       //д���ֽ�
	u8 file_data[DOWNLOAD_DATA_LEN];
}s_file_download;
#pragma pack()

//У���ļ����ݸ�ʽ
#pragma pack(1)
typedef struct 
{
	u16 head;
	u16 file_crc;         //�ļ���У��
	u16 self_crc;         //�����У��
	u16 end;
}check_s;
#pragma pack()

//�汾��Ϣ���ݸ�ʽ
#pragma pack(1)
typedef struct 
{
	u16 head;
	u16 space;              //�����ո�
	char v_buf[20];         //�汾�� ʵ��Ϊ����
	char release_note[256]; //����˵��
	u16 self_crc;           //�����У��
	u16 end;
}version_s;
#pragma pack()

extern u32 ui_file_totalsize;
extern u32 mcu_file_totalsize;
extern u8 ota_file_number;      //1: ֻ��UI�ļ�  2:ֻ��MCU�ļ�  3:�����ļ�����

void ota_date(void);

void download_check_file(char* s_fpath,char* mem_fpath);
void download_ota_file(char* s_fpath,char* mem_fpath,char* mem_check_fpath,u32 file_size);
void ota_run_process(void);

u8 download_parameter_file(char* down_path,char* mem_path,u8 d_mode);
u8 update_cure_parameter_file(void);
#endif



