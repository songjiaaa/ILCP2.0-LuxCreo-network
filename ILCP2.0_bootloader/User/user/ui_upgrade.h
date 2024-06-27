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

//校验文件在内存的存储路径
#define MEM_MCU_CHECK_FILE_PATH     "1:/iLuxCurePro_check.bin"
#define MEM_UI_CHECK_FILE_PATH      "1:/ota_check.bin"

#define OTA_WRITE_BYTE     512
#define FILE_NAME_LEN      (sizeof("ota.bin") - 1)    //要去掉\0
//#define LCD_OTA_BAUDRATE   (115200)
#define LCD_OTA_BAUDRATE   (921600)

#define UI_START_ACK_YES        0x8011         //0x1180    //按小端方式  
#define UI_START_ACK_NO         0x8111         //0x1181     

#define UI_UPDATA_ACK_YES       0x8022         //每帧更新的数据的应答
#define UI_UPDATA_SN_ERR        0x8222         //数据帧SN错误
#define UI_UPDATA_CHECK_ERR     0x8322         //数据帧校验错误
 
#define UI_UPDATA_YES           0x8033         //屏幕升级命令应答
#define UPDATA_FILE_CHECK_YES   0x0155         //ota.bin 文件校验成功
#define UPDATA_FILE_CHECK_NO    0x0055         //ota.bin 文件校验失败
#define UPDATA_FILE_ZIP_YES     0x0166         //ota.bin 文件解压成功
#define UPDATA_FILE_ZIP_NO      0x0066         //ota.bin 文件解压失败

typedef enum
{
	OTA_WAIT_ACK_IDLE,
	OTA_WAIT_START_ACK,
	OTA_WAIT_DOWNLOAD_ACK,
}UPGRADE_STATE;


//更新UI数据
#pragma pack(1)
typedef struct 
{
	u16 head;
	u16 len;
	u16 cmd;
	u8 sn;         //序列号
	u8 packet[OTA_WRITE_BYTE];
	u16 checksum;
	u32 end;
}ota_updata;
#pragma pack()



//更新指令
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


//更新状态
#pragma pack(1)
typedef struct 
{
	u8 update_status_flow;   //更新状态流程
	u8 update_file_mode;     //更新文件模式   0 不更新  1 更新UI文件 2 更新mcu文件 3 两个文件都更新
	u8 ui_ota_state;         //UI更新状态
	u16 ota_ack;             //UI应答标志
	u32 timeout;             //屏幕超时
	u8 sent_cnt;             //发送数据次数
}upgrade_stat;
#pragma pack()


//UI更新启动应答
#pragma pack(1)
typedef struct 
{
	u16 head;
	u16 cmd;
	u32 end;
}ui_upgrade_start_ack;
#pragma pack()

//UI更新数据应答
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




