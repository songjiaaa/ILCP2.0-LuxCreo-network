#ifndef _SETTING_H_
#define _SETTING_H_

//#include "hmi_driver.h"
//#include "stm32f4_sys.h"
//#include "stm32f4xx.h" 
//#include "config.h"

//#include "version.h"

#include "app.h"

#define SETTTING_PAGE_ID1    20
#define SETTTING_PAGE_ID2    21

void setting_show(void);
void setting_ctrl(u16 touch_id);

void uvled_total_time_show(void);

#endif




















