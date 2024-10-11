#ifndef _HX711_H_
#define _HX711_H_

#include "stm32f4_sys.h"
#include "stm32f4xx.h" 
#include "main.h"

#include "dwt_delay.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "timers.h"

#define HX711_SDA   PDin(13)
#define HX711_SCK   PDout(14)

void hx711_io_init(void);
int hx711_recv(void);

#endif



