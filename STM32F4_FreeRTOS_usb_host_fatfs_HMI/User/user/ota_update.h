#ifndef _OTA_UPDATE_H_
#define _OTA_UPDATE_H_

#include "app.h"
//国内阿里云服务器  home
//luxlink.oss-cn-beijing.aliyuncs.com

//国外阿里云服务器  abroad
//lux-link-usa.oss-us-west-1.aliyuncs.com

//#define  OTA_MCU_BIN_URL       "http://luxlink.oss-cn-beijing.aliyuncs.com/Cure/ILCP_UPGRADE_FILE/iLuxCurePro.bin"
//#define  OTA_UI_BIN_URL        "http://luxlink.oss-cn-beijing.aliyuncs.com/Cure/ILCP_UPGRADE_FILE/ota.bin"

//OTA下载文件的服务器域名
#define  OTA_DOMESTIC_DNS           "luxlink.oss-cn-beijing.aliyuncs.com"         //国内
#define  OTA_FOREIGN_DNS            "lux-link-usa.oss-us-west-1.aliyuncs.com"     //国外

#define  PORT_NUMBER                "80"
//OTA下载文件的服务器IP
#define  OTA_DOMESTIC_IP            "o.luxcreo.cn"                    //国内
#define  OTA_FOREIGN_IP             "47.88.111.29"                    //国外



//OTA升级文件在服务器存储路径    
#define  UI_OTA_FILE_PATH           "/Cure/ILCP_UPGRADE_FILE/ota.bin"
#define  MCU_OTA_FILE_PATH          "/Cure/ILCP_UPGRADE_FILE/iLuxCurePro.bin"
#define  CURE_OTA_FILE_PATH         "/Cure/ILCP_UPGRADE_FILE/Cure.txt"
  
//校验文件在服务器存储路径
#define  VERSION_FILE_PATH			"/Cure/ILCP_UPGRADE_FILE/Version.txt"
#define  UI_CHECK_FILE_PATH         "/Cure/ILCP_UPGRADE_FILE/ota_check.bin"
#define  MCU_CHECK_FILE_PATH        "/Cure/ILCP_UPGRADE_FILE/iLuxCurePro_check.bin"
#define  CURE_CHECK_FILE_PATH       "/Cure/ILCP_UPGRADE_FILE/Cure_check.bin"

//文件在内存的存储路径
#define MEM_FOLDER_PATH             "1:/ILCP_UPGRADE_FILE"
#define MEM_MCU_WRITE_PATH          "1:/ILCP_UPGRADE_FILE/iLuxCurePro.bin"
#define MEM_UI_WRITE_PATH           "1:/ILCP_UPGRADE_FILE/ota.bin"
#define MEM_CURE_FILE_WRITE_PATH    "1:/Cure.txt"


//校验文件在内存的存储路径
#define MEM_VERSION_FILE_PATH	    "1:/Version.txt"
#define MEM_MCU_CHECK_FILE_PATH     "1:/iLuxCurePro_check.bin"
#define MEM_UI_CHECK_FILE_PATH      "1:/ota_check.bin"
#define MEM_CURE_CHECK_FILE_PATH    "1:/Cure_check.bin"

//规定升级文件在U盘中的路径
#define U_DISK_MCU_FILE_PATH        "2:/ILCP_UPGRADE_FILE/iLuxCurePro.bin"
#define U_DISK_UI_FILE_PATH         "2:/ILCP_UPGRADE_FILE/ota.bin"

#define DOWNLOAD_DATA_LEN           1024



//ota 文件下载
#pragma pack(1)
typedef struct
{
	int start;
	int end;
	u8 repeated_req;      //重复请求
	u8 crc_err;           //校验出错
	u32 file_size;        //文件大小
	u32 write_byte;       //写入字节
	u8 file_data[DOWNLOAD_DATA_LEN];
}s_file_download;
#pragma pack()

//校验文件数据格式
#pragma pack(1)
typedef struct 
{
	u16 head;
	u16 file_crc;         //文件的校验
	u16 self_crc;         //自身的校验
	u16 end;
}check_s;
#pragma pack()

//版本信息数据格式
#pragma pack(1)
typedef struct 
{
	u16 head;
	u16 space;              //两个空格
	char v_buf[20];         //版本号 实际为日期
	char release_note[256]; //发布说明
	u16 self_crc;           //自身的校验
	u16 end;
}version_s;
#pragma pack()

extern u32 ui_file_totalsize;
extern u32 mcu_file_totalsize;
extern u8 ota_file_number;      //1: 只有UI文件  2:只有MCU文件  3:两个文件都有

void ota_date(void);

void download_check_file(char* s_fpath,char* mem_fpath);
void download_ota_file(char* s_fpath,char* mem_fpath,char* mem_check_fpath,u32 file_size);
void ota_run_process(void);

u8 download_parameter_file(char* down_path,char* mem_path,u8 d_mode);
u8 update_cure_parameter_file(void);
#endif



