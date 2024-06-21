#include "tim.h"

//TIM5 CH2   PA1 
void timer5_pwm_config(u32 arr,u32 psc)
{		 					 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  	  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;          
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        
	GPIO_Init(GPIOA,&GPIO_InitStructure);              
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM5);

	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;    // 不输出           
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC2Init(TIM5, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM5, ENABLE);

	TIM_Cmd(TIM5, ENABLE);  								  
}  


//TIM4 CH3  CH4       PD14   PD15      LED_PWM
void timer4_pwm_config(u32 arr,u32 psc)
{		 					 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  	  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;          
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        
	GPIO_Init(GPIOD,&GPIO_InitStructure);              
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_TIM4);
	
	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;                
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM4, ENABLE);

	TIM_Cmd(TIM4, ENABLE);  								  
}  


//TIM1 CH3  PE13   
void timer1_pwm_config(u32 arr,u32 psc)
{		 					 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);  	  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;          
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        
	GPIO_Init(GPIOE,&GPIO_InitStructure);              
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_TIM1);

	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = (arr+1)/2;                
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM1, ENABLE);

	TIM_Cmd(TIM1, ENABLE);  								  
}

//������׼��ʱ
//100-1,8400-1 ��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����100��Ϊ10ms 
void timer3_time_config(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 	
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); 
	TIM_Cmd(TIM3,DISABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x03; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; 
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}



void TIM3_IRQHandler(void)  //周期 10ms
{
//	uint32_t ulReturn;
//	ulReturn = taskENTER_CRITICAL_FROM_ISR();	/* 进入临界段，临界段可以嵌套 */
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) 
	{
		if(cure.mode == 1)
		{
			cure.cure_tick ++;
			if(cure.cure_tick % 100 == 0)
			{				
				cure.r_time--;     
				cure.step_cnt_time++;
			}
			
			if(cure.r_time == 0)   
			{
//				cure_stop();
				curing_pause();
				cure.mode = 3;
			}
			else if(cure.step_cnt_time >= exe_parameter.time[cure.step]*60)
			{
				cure.step++;
				cure.exp_temp = exe_parameter.heat[cure.step];
				cure.step_cnt_time = 0;
				UV_POWER_PER1 = save_config.uv_power_max * save_config.uv_cal_k * save_config.up_uv_k * exe_parameter.up_uv[cure.step] * 10 ;
				UV_POWER_PER2 = save_config.uv_power_max * save_config.uv_cal_k * save_config.down_uv_k * exe_parameter.down_uv[cure.step] * 10;
				//刷新显示参数步骤
//				cure_show_text();
			}			
		}
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); 
//	taskEXIT_CRITICAL_FROM_ISR( ulReturn ); 	/* 退出临界段 */
}




void motor_ctrl_on(u8 dir)
{
	if(dir == 1)
	{
		MOTOR_DIR = 1;
		TIM1->CCR3 = 300;
	}
	else if(dir == 2)
	{
		MOTOR_DIR = 1;
		TIM1->CCR3 = 600;
	}
	else
	{
		MOTOR_DIR = 0;
		TIM1->CCR3 = 1000;
	}
}


