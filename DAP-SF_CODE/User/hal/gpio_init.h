#ifndef LED_H_
#define LED_H_

#include "stm32f4xx.h"
#include "stm32f4_sys.h"

#define ON   1
#define OFF  0

#define RGBS_PWR_EN           PAout(1)
#define BUZ_PWM               PAout(3)
#define CAM12V_PWR_EN         PAout(5)
#define LED_G                 PAout(6)
#define LED_R                 PAout(7)
#define LED_IND               PAout(8)

#define HUB_PWR_EN 			  PAout(15)


#define PWR_HOLD              PBout(0)

#define EXT5V_PWR_EN          PEout(2)
#define USBCAM_PWR_EN         PEout(9)


#define KEY_DET               PBin(1)

#define RFID1_EN              PDout(1)
#define RFID2_EN              PEout(11)

#define GL_8F_OUT     		  PEin(12)   //×¢ÒºÐü±Û


#define MOTOR_DIR             PEout(14)
#define RLY_EN                PEout(15)

#define VK36_OUT0             PEin(8)    //ÒºÎ»¼ì²â
#define VK36_OUT1             PEin(7)    //Òç³ö¼ì²â


void gpio_init(void);

#endif



