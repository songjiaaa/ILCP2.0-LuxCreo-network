#ifndef APP_H
#define APP_H
#include <stdio.h>

//#include "FreeRTOS.h"
//#include "task.h"
//#include "timers.h"

#include "stm32f4_sys.h"
#include "stm32f4xx.h" 
#include "main.h"
#include "comm_rec.h"
#include "com_fun.h"
#include "f4_uart.h"
#include "cmd.h"
#include "config.h"
#include "f4_led.h"
#include "led.h"
#include "dwt_delay.h"
#include "tim.h"
#include "hmi_driver.h"
//#include "adc.h"
//#include "ntc.h"  
//#include "htu21d.h"

#include "w25qxx.h"    
#include "sdio_sdcard.h"    

#include "usbh_usr.h" 

#include "ff.h"
#include "diskio.h"
#include "file_handing.h"


#include "ui_process.h"
#include "ui_upgrade.h"

//#include "cure.h"
//#include "custom.h"
#include "ws2812b.h"
#include "config.h"
//#include "setting.h"
//#include "version.h"

////传感器数据
#pragma pack(1)
typedef struct 
{
    s16 inside_t;            //内部温度
    s16 ptc_t;               //PTC 温度
    s16 uv_led1_t;           //灯板温度
    s16 uv_led2_t;           //灯板温度
    float ac_v;              //接入电压
    float pcb_t;             //电路板 温度
    float pcb_h;             //电路板 湿度
}data_collection;
#pragma pack()

extern data_collection data_c;

//USB_OTG_CORE_HANDLE USB_OTG_dev;
extern USBH_HOST USB_Host;
extern USB_OTG_CORE_HANDLE  USB_OTG_Core;
extern SD_Error SD_state;



#define APP_START_ADDR         ((uint32_t)0x08010000U)  //应用程序起始地址

void app_poll(void);
void start_app(void);
void boot_process(void);
int get_que_data(u8 *p,Queue *q);


#endif

