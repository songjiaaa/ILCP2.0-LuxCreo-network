#include "pump_motor.h"


//�����ʼ��
void pump_motor_init(void)
{
	timer1_pwm_config(1000-1,168-1);  //motor 1K hz
	MOTOR_DIR = 0;
//	TIM1->CCR3 = 1000;   //����
	TIM1->CCR3 = 0;      //����
}



//�ô�
void pump_motor_en(void)
{
	MOTOR_DIR = 0;
	TIM1->CCR3 = 1000;
}



//�ùر�
void pump_motor_dis(void)
{
	MOTOR_DIR = 0;
	TIM1->CCR3 = 0;
}



//�Զ�עҺעҺ��
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




