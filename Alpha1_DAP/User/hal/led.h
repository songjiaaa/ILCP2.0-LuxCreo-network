#ifndef LED_H_
#define LED_H_

#include "stm32f4xx.h"
#include "stm32f4_sys.h"

#define PTC_FAN1_EN           PAout(4)
#define PTC_FAN2_EN           PAout(5)
#define BOX_FAN_EN            PAout(6)
#define EHT_RESET             PAout(7)
#define LED_IND               PAout(8)


#define MAC_DOOR_LOCK         PCout(1)
#define RGBS_PWR_EN           PCout(9)


#define WIFI_PWR_EN           PDout(0)
#define UV_FAN1_PWR_EN        PDout(3)
#define UV_FAN2_PWR_EN        PDout(4)
#define DOOR_BYPASS           PDout(6)

#define LCD_PWR_EN            PEout(0)
#define GP_PWR_EN             PEout(1)
#define UV_DAC_SW             PEout(10)
#define WLED_PWR_EN           PEout(11)
#define MOTOR_DIR             PEout(14)
#define WIFI_RESET            PEout(15)
#define DOOR_DET              PEin(2)            
 

void gpio_init(void);

#endif



