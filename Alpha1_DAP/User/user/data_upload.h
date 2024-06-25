#ifndef _DATA_UPLOAD_H_
#define _DATA_UPLOAD_H_

#include "app.h"

#define  WIFI_UPLOAD_MODE               0x00      //WiFi上传
#define  ENTHERNET_UPLOAD_MODE          0x01      //以太网上传

#define  UPLOADING_DIRECTION            0x00      //传输方向  0x00 上传   0x01服务器下发

#define  IDLE_UPLOAD_TYPE               0x01      //空闲状态下上传数据
#define  CURED_UPLOAD_TYPE              0x02      //固化或暂停中上传数据
#define  DATA_IMPORT_TYPE               0x03      //数据导入请求帧 
#define  OTA_UPGRADE_TYPE               0x04      //OTA升级数据帧
#define  ERROR_UPLOAD_TYPE              0x05      //错误上传

#pragma pack(1)
typedef struct 
{
	u16 head;     //帧头
	u16 len;      //帧长度 指的是帧全长
	u8 run_num;   //序号
	u8 t_mode;    //传输方式   0x00 WiFi传输    0x01 enthernet 传输
	u8 t_dir;     //传输方向   0x00 上传        0x01 接收 
    u8 type;      //帧类型
	u8 version[30]; //设备版本号
}upload_frame_head;
#pragma pack()


//空闲或固化结束状态下上传数据包                  0x01
#pragma pack(1)
typedef struct 
{
	//数据包
	u8 device_name[20];  //设备名称
	u8 SN[30];    //设备SN码
	u8 status;    //设备状态
	float uv_power_cal;  //UV电源功率的补偿值
	float up_uv_k;    //上灯板调整系数
	float down_uv_k;  //下灯板调整系数
	s16  heat_cal;    //独立加热补偿
	s16  uv_heat_cal; //固化加热补偿 
	u8 door_state;    //门状态
	u8 mac_lock_sta;  //门锁状态
	u32 uv_total_time; //UV使用总时间
	u8 utc_time[30];   //任务启动和结束时utc时间
}idle_upload_frame;
#pragma pack()


//固化中上传的数据包                    0x02
#pragma pack(1)
typedef struct 
{
	u8 device_name[20];  //设备名称
	u8 SN[30];           //设备SN码
	u8 status;           //设备状态
}cure_upload_front;
#pragma pack()

extern char upload_cure_para_data[500];         //固化参数

#pragma pack(1)
typedef struct 
{
	//数据包
	u8 resin[30];      //参数名
	u8 app_area[30];   //应用领域
	u8 step;           //当前固化步骤
	u8 step_time;      //当前固化步骤下的时长
	u8 up_uv_per;      //上灯板光强百分比
	u8 down_uv_per;    //下灯板光强百分比
	s16 step_temp;     //当前步骤下设定温度
	u32 cure_total_time;    //当前参数固化总时间
	u32 r_time;        //剩余固化时间 
	s16 ptc_temp;      //ptc加热片的温度
	s16 up_uv_temp;    //上灯板散热片的温度
	s16 down_uv_temp;  //下灯板散热片的温度
	u8  head_lamp;     //照明灯状态
	u8  motor_state;   //电机状态
	u8 utc_time[30];
}cure_upload_tail;
#pragma pack()


////固化参数或配置导入数据包 OTA升级请求数据包              0x03  或 0x04
//#pragma pack(1)
//typedef struct 
//{
//	//数据包
//	u8 device_name[20];  //设备名称
//	u8 SN[30];           //设备SN码
//	u8 version[30];      //设备版本号
//	u8 d_file_name[30];  //请求加载的文件名 
//	u16 file_data_num;   //下载的文件帧序号
//}data_load_request;
//#pragma pack()

////错误上传                                              0x05
//#pragma pack(1)
//typedef struct 
//{
//	//数据包
//	u8 device_name[20];  //设备名称
//	u8 SN[30];           //设备SN码
//	u8 version[30];      //设备版本号
//	u8 error;            //下载的文件帧序号
//}error_load;
//#pragma pack()


//帧尾
#pragma pack(1)
typedef struct 
{
	u16 crc;
	u16 end;
}upload_end;
#pragma pack()

//应用场景
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

extern cure_upload_front cure_data_head;      //固话中或固化暂停上传的数据包
extern char upload_cure_para_data[500];       //固化中数据组包
extern cure_upload_tail cure_data_tail;       //固化格式结尾

//extern cure_upload_frame cure_data;
//extern data_load_request request_data;
//extern error_load error_code;


void netword_package_upload(u8 t_mode,u8 type);
void p_data_upload(void);


#endif























