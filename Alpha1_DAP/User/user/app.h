#ifndef APP_H
#define APP_H
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
//#include "semphr.h"

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
#include "adc.h"
#include "ntc.h"  
#include "htu21d.h"
#include "rtc.h"


#include "w25qxx.h"  
#include "spi.h"  
#include "sdio_sdcard.h"    
#include "ws2812b.h"

#include "usbh_usr.h" 

#include "ff.h"
#include "diskio.h"
#include "file_handing.h"

#include "ui_process.h"


#include "config.h"
#include "setting.h"
#include "version.h"

#include "wifi_driver.h"
#include "wifi_use.h"
#include "ethernet_update.h"
#include "ethernet_driver.h"


#include "data_upload.h"
#include "ota_update.h"

#include "dap_motor.h"
#include "dap_app.h"

//传感器数据
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

extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef RTC_DateStruct;

extern data_collection sensor_data;
extern USBH_HOST USB_Host;
extern USB_OTG_CORE_HANDLE  USB_OTG_Core;
extern SD_Error SD_state;


extern TaskHandle_t usb_task_handler;
void usb_disk_task( void * pvParameters );

extern TaskHandle_t dap_data_pro_handler;
void dap_data_pro_task(void *pvParameters);

extern TaskHandle_t enthernet_data_pro_handler;
void enthernet_data_pro_task(void *pvParameters);

extern TaskHandle_t wifi_data_pro_handler;  
void wifi_data_pro_task( void * pvParameters );

extern TaskHandle_t lcd_data_pro_handler;  
void lcd_data_pro_task( void * pvParameters );

extern TaskHandle_t software_timer_handler;
void software_timer_task( void * pvParameters );

extern TaskHandle_t get_sensor_data_handler;
void get_sensor_data_task( void * pvParameters );

extern TaskHandle_t run_task_handler;
void run_task( void * pvParameters );

extern TaskHandle_t init_task_handler;
void init_task(void * pvParameters);

void door_task(void);

int get_que_data(u8 *p,Queue *q);


#endif

