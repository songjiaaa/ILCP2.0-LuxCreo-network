#include "pump_motor.h"


//电机初始化
void pump_motor_init(void)
{
	timer1_pwm_config(1000-1,168-1);  //motor 1K hz
	MOTOR_DIR = 0;
//	TIM1->CCR3 = 1000;   //拉高
	TIM1->CCR3 = 0;      //拉低
}



//泵打开
void pump_motor_en(void)
{
	MOTOR_DIR = 0;
	TIM1->CCR3 = 1000;
}



//泵关闭
void pump_motor_dis(void)
{
	MOTOR_DIR = 0;
	TIM1->CCR3 = 0;
}



//自动注液注液机
void liquid_injection(void)
{
	if(VK36_OUT1 == 0)
	{
		pump_motor_dis();
	}
	else
	{
		if(VK36_OUT0 == 0)
		{
			pump_motor_dis();
		}
		else
		{
			pump_motor_en();
		}
	}
}




