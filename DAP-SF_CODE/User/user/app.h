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

#include "config.h"
#include "version.h"


extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef RTC_DateStruct;


extern USBH_HOST USB_Host;
extern USB_OTG_CORE_HANDLE  USB_OTG_Core;
extern SD_Error SD_state;


extern TaskHandle_t start_task_handler;
void start_task(void *pvParameters);

extern TaskHandle_t usb_task_handler;
void usb_disk_task( void * pvParameters );

extern TaskHandle_t software_timer_handler;
void software_timer_task( void * pvParameters );

extern TaskHandle_t get_sensor_data_handler;
void get_sensor_data_task( void * pvParameters );

extern TaskHandle_t run_task_handler;
void run_task( void * pvParameters );

extern TaskHandle_t init_task_handler;
void init_task(void * pvParameters);

int get_que_data(u8 *p,Queue *q);


#endif

