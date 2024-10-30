#ifndef APP_H
#define APP_H

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"


#include "stm32f4_sys.h"
#include "stm32f4xx.h" 
#include "main.h"
#include "comm_rec.h"
#include "com_fun.h"
#include "f4_uart.h"
#include "my_queue.h"

#include "cmd.h"
#include "config.h"
#include "f4_led.h"
#include "gpio_init.h"
#include "dwt_delay.h"
#include "tim.h"

#include "adc.h"
#include "ntc.h"  

//#include "rtc.h"


#include "w25qxx.h"  
#include "spi.h"  
#include "sdio_sdcard.h"    
#include "ws2812b.h"

#include "usbh_usr.h" 

#include "ff.h"
#include "diskio.h"
#include "file_handing.h"

#include "config.h"
#include "version.h"

#include "hx711.h"
#include "rfid.h"

extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef RTC_DateStruct;


extern USBH_HOST USB_Host;
extern USB_OTG_CORE_HANDLE  USB_OTG_Core;
extern SD_Error SD_state;




extern TaskHandle_t debug_task_handler;
void debug_task(void *pvParameters);

extern TaskHandle_t rfid1_task_handler;
void rfid1_task(void *pvParameters);

extern TaskHandle_t rfid2_task_handler;
void rfid2_task(void *pvParameters);

void modbus_pro_task(void * pvParameters);

extern TaskHandle_t start_task_handler;
void start_task(void *pvParameters);

extern TaskHandle_t software_timer_handler;
void software_timer_task( void * pvParameters );

extern TaskHandle_t get_sensor_data_handler;
void get_sensor_data_task( void * pvParameters );

extern TaskHandle_t init_task_handler;
void init_task(void * pvParameters);







//modbus addr  相应数据对应的modbus寄存器地址
#define  REG_START_ADDR   0x05A0          //起始地址

#define  BAND_ADDR        0x05A0          //材料品牌名
#define  RESIN_ADDR       0x05E0          //树脂名
#define  VOLUME_ADDR      0x061E          //料瓶容量

#define  WEIGHT_ADDR      0x0620          //树脂的重量
#define  LEAP_MAX_NUM_ADDR     0x0622          //离型膜最多使用次数
#define  LEAP_REMAIN_NUM_ADDR  0x0624          //离型膜剩余使用次数
#define  LEAP_NUM_ADDR    0x0626               //膜使用次数
#define  LEAP_NAME_ADDR   0x0630               //膜名字
#define  REMAIN_RESIN_WEIGHT   0x0660          //写入剩余树脂重量
#define  WEIGHT_ZERO_ADDR      0x0662          //称重归零
#define  GET_SN_ADDR           0x0670          //SN

#define  IO_INPUT_STATE_ADDR   0x0690          //IO输入检测
#define  IO_OUT_STATE_ADDR     0x0691          //IO输出检测

#define  HEARTBEAT_FRAME_ADDR  0x0688          //心跳检测

#define  PROGRAM_VERSION_ADDR  0x06A0          //版本信息查询

#define  FILE_TRANSFER_FLAGS_ADDR   0x0800          //文件传输标志   1发送文件   0 结束文件传输
#define  RECEIVED_FILE_SIZE_ADDR    0x06FF          //接收的文件大小
#define  RECEIVED_FILE_DATA_ADDR    0x0700          //接收文件数据
#define  START_UPDATE_COMMAND_ADDR  0x0801          //启动更新标志


//线圈寄存器
#define  IP_CAMERA_POWER_REG   0x0680        //IP相机电源
#define  LIQUID_IN_SWITCH_REG  0x0681        //注液进料开关



#define  BAND_REG        (BAND_ADDR - REG_START_ADDR)          //材料品牌名
#define  RESIN_REG        (RESIN_ADDR - REG_START_ADDR)          //树脂名
#define  VOLUME_REG     (VOLUME_ADDR - REG_START_ADDR)          //料瓶容量
#define  WEIGHT_REG       (WEIGHT_ADDR - REG_START_ADDR)          //树脂的重量
#define  REMAIN_RESIN_WEIGHT_REG    (REMAIN_RESIN_WEIGHT - REG_START_ADDR)          //写入剩余树脂重量


#define  LEAP_MAX_NUM_REG      (LEAP_MAX_NUM_ADDR - REG_START_ADDR)          //离型膜最多使用次数
#define  LEAP_REMAIN_NUM_REG   (LEAP_REMAIN_NUM_ADDR - REG_START_ADDR)          //离型膜剩余使用次数
#define  LEAP_NUM_REG     (LEAP_NUM_ADDR - REG_START_ADDR)               //写入膜使用次数
#define  LEAP_NAME_REG    (LEAP_NAME_ADDR - REG_START_ADDR)               //膜名字

#define  GET_SN_REG            (GET_SN_ADDR - REG_START_ADDR)           //SN
#define  WEIGHT_ZERO_REG       (WEIGHT_ZERO_ADDR - REG_START_ADDR)          //称重归零

#define  IO_INPUT_STATE_REG    (IO_INPUT_STATE_ADDR - REG_START_ADDR)          //IO输入检测)          //IO输入检测
#define  IO_OUT_STATE_REG      (IO_OUT_STATE_ADDR - REG_START_ADDR)          //IO输出检测

#define  HEARTBEAT_FRAME_REG   (HEARTBEAT_FRAME_ADDR - REG_START_ADDR)          //心跳检测

#define  PROGRAM_VERSION_REG   (PROGRAM_VERSION_ADDR - REG_START_ADDR)          //版本信息查询

#define  FILE_TRANSFER_FLAGS_REG    (FILE_TRANSFER_FLAGS_ADDR - REG_START_ADDR)          //文件传输标志   1发送文件   0 结束文件传输
#define  RECEIVED_FILE_SIZE_REG     (RECEIVED_FILE_SIZE_ADDR - REG_START_ADDR)          //接收的文件大小
#define  RECEIVED_FILE_DATA_REG     (RECEIVED_FILE_DATA_ADDR - REG_START_ADDR)          //接收文件数据
#define  START_UPDATE_COMMAND_REG   (START_UPDATE_COMMAND_ADDR - REG_START_ADDR)          //启动更新标志




//IO输入检测
#pragma pack(1)
typedef struct
{
    u16 material:             1,   //料盒到位检测
        bucket:               1,   //料筒到位检测
        liquid_max:           1,   //注液满检测
        liquid_overflow:      1,   //注液溢出检测
        surplus_material:     1,   //料盒剩余检测
        surplus_bucket:       1,   //料筒剩余检测
        empty_cantilever:     1,   //空悬臂检测
        bucket_rfid_stete:    1,   //料筒RFID状态
        material_rfid_stete:  1,   //料盒RFID状态
        reserved:             7;  //保留位
}in_state_t;
#pragma pack()

//IO输出检测
#pragma pack(1)
typedef struct
{
    u16 ip_camera_power:      1,   //IP相机电源
        liquid_in_switch:     1,   //注液进料开关
        reserved:             14;   //保留位
}out_state_t;
#pragma pack()


#pragma pack(1)
typedef struct
{
	int resin_weight;    //树脂重量
    u32 write_resin_weight;   //写入树脂重量
	u32 resin_volume;    //树脂容积
    u8 resin_band[30];   //树脂品牌名
	u8 resin_name[30];   //树脂名

	u32 leap_max_num;    //离型膜最多使用次数
	u32 leap_remain_num; //离型膜剩余使用次数
	u32 writeleap_num;   //写入膜使用次数
	u8 leap_name[30];    //膜名字

	u8 sn[30];   //SN
    int weight_zero;   //称重归零

	in_state_t io_input_state;   //IO输入检测
	out_state_t io_out_state;   //IO输出检测
	

	u8 heartbeat_frame;   //心跳检测
	u8 program_version[30];   //版本信息查询
	u8 file_transfer_flags;   //文件传输标志   1发送文件   0 结束文件传输
	u32 received_file_size;   //接收的文件大小
	u8 received_file_data[256];   //接收文件数据
	u8 start_update_command;   //启动更新标志
}modbus_data_t;
#pragma pack()

extern modbus_data_t m_data_t;

#endif

