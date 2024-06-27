#ifndef DWT_DELAY_H
#define DWT_DELAY_H

#include "stm32f4xx.h" 
#include "main.h"

void bsp_InitDWT(void);
void bsp_DelayUS(uint32_t _ulDelayTime);
void bsp_DelayMS(uint32_t _ulDelayTime);

#endif




