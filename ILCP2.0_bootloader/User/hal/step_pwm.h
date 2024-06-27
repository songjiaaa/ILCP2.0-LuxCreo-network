#ifndef STEP_PWM_H_
#define STEP_PWM_H_

#include "main.h"

#define MOTOR_DIR  PCout(7)
#define MOTOR_EN   PDout(2)


void step_io_init(void);

void step_pwm(u16 arr,u16 psc);
void TIM3_PWM_Init(u32 arr,u32 psc);


void TIM4_config(u32 PulseNum);
void TIM8_config(u32 Cycle);
void Pulse_output(u32 Cycle,u32 PulseNum);


#endif


