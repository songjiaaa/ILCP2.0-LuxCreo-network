#ifndef _TIM_H_
#define _TIM_H_

//#include "stm32f4xx.h"
//#include "main.h"

#include "app.h"

#define UV_POWER_PER1   (TIM4->CCR3)    //光源1
#define UV_POWER_PER2   (TIM4->CCR4)    //光源2
#define HEAT_POWER_PER  (TIM5->CCR2)

#define ON_TIMER        (TIM_Cmd(TIM3,ENABLE))
#define OFF_TIMER       (TIM_Cmd(TIM3,DISABLE))

void timer1_pwm_config(u32 arr,u32 psc);
void timer3_time_config(u16 arr,u16 psc);
void timer4_pwm_config(u32 arr,u32 psc);
void timer5_pwm_config(u32 arr,u32 psc);

void motor_ctrl_on(u8 dir);

#endif


