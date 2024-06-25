#ifndef F4_LED_H
#define F4_LED_H

#include "main.h"
#include "stm32f4_sys.h"

typedef struct
{
	GPIO_TypeDef *port; //led的端口
	u8 pin; //led的引脚号(0~15)
	u8 light_lev; //亮的电平
	u8 duty; //亮时间（tick数）
	u8 T; //周期（tick数），若小于duty，则为单次触发
	u8 tick; //实际时间
} S_LED_CTRL; //led 控制结构

void led_ini(S_LED_CTRL *pled,u8 n);
void led_tick(void);
void led_toggle(int offset); //对于非周期的，触发led
void led_period(int offset,int t); //设置led周期，单位tick，若为0则为关闭

#endif

