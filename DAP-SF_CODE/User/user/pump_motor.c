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
	RLY_EN = ON;
	m_data_t.io_out_state.liquid_in_switch = ON;
}



//�ùر�
void pump_motor_dis(void)
{
	MOTOR_DIR = 0;
	TIM1->CCR3 = 0;
	RLY_EN = OFF;
	m_data_t.io_out_state.liquid_in_switch = OFF;
}



//�Զ�עҺעҺ��
void liquid_injection(void)
{
	if(GL_8F_OUT == 0)
	{
		if(VK36_OUT1 == 0)        //������
		{
			pump_motor_dis();
		}
		else
		{
			if(VK36_OUT0 == 0)   //עҺ�����
			{
				pump_motor_dis();
			}
			else
			{
				pump_motor_en();
			}
		}
	}	
}




