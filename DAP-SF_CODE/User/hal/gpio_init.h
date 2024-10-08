#ifndef LED_H_
#define LED_H_

#include "stm32f4xx.h"
#include "stm32f4_sys.h"


#define LED_IND               PAout(8)
#define KEY_DET               PBin(1)

#define PWR_HOLD              PBout(0)

void gpio_init(void);

#endif



