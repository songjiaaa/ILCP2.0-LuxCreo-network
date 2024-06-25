#include "dap_motor.h"

speed_calc_t g_calc_t = {0};
float velocity_tab[5000] = {0} ;                 /* �ٶȱ��ָ�� */
float dec_velocity_tab[5000] = {0} ;             //����

//float main_acc_velocity_tab[5000] = {0};
//float main_dec_velocity_tab[5000] = {0};
//float scraper_acc_velocity_tab[5000] = {0}; 
//float scraper_dec_velocity_tab[5000] = {0}; 
//�ε�������� �ٶ�um/s  �ƶ�����um
motor_parameter scraper_motor = 
{
	.motor_sn = SCRAPER_MOTOR,     //�ε����
	.max_len = 300 * 1000,          //um
};

//ƽ̨���   303000 um  2500
motor_parameter platform_motor =
{
	.motor_sn = PLATFORM_MOTOR,    //ƽ̨���
	.max_len = 350 * 1000,         //����г� 350mm    
};
	
//�����źŴ�������ʼ��
void limit_sensor_init(void)
{
	GPIO_Set(GPIOD,PIN0,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //��������λ������Ϊ0
	GPIO_Set(GPIOD,PIN1,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //���帺��λ������Ϊ0
	GPIO_Set(GPIOD,PIN2,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //��������λ��������ӡ�����  ����Ϊ��
	GPIO_Set(GPIOD,PIN3,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //��������λ��Զ���ӡ��     ����Ϊ��
	GPIO_Set(GPIOD,PIN4,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //�϶�����λ ���� �͵�ƽ
	GPIO_Set(GPIOD,PIN5,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //�϶�����λ ���� �͵�ƽ
	GPIO_Set(GPIOD,PIN6,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //��ͣ �ɿ��ǵ͵�ƽ  ����Ϊ��
	GPIO_Set(GPIOD,PIN7,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //�϶������Ĥ�߶� �����͵�ƽ
	GPIO_Set(GPIOD,PIN8,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //��Ͱ��װ��λ��� ����Ϊ�׵�ƽ
	GPIO_Set(GPIOD,PIN9,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //��ӡ����ת��λ��� �����͵�ƽ
	GPIO_Set(GPIOD,PIN10,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //�����м��
	GPIO_Set(GPIOD,PIN11,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //�����ұߴ����� ����Ϊ��  Ŀǰ�޷�����  Ŀǰ�ṹԭ���޷�����
	GPIO_Set(GPIOD,PIN12,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //������ߴ����� ����Ϊ��  Ŀǰ�޷�����  Ŀǰ�ṹԭ���޷�����
	GPIO_Set(GPIOD,PIN13,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //����ŵ�λ���  �����͵�ƽ
	GPIO_Set(GPIOD,PIN14,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //�ε��������
	GPIO_Set(GPIOD,PIN15,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //����������
	
//	GPIO_Set(GPIOB,PIN6,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //�����������ӿ�
//	GPIO_Set(GPIOE,PIN5,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //����������ӿ�
	
	GPIO_Set(GPIOG,PIN1,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //�϶�
	GPIO_Set(GPIOG,PIN2,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //�϶�
	
	//���
	GPIO_Set(GPIOG,PIN4,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);  //�������
	IN_MATERIALS_SW = 1;  //���ϲ�����
	
	GPIO_Set(GPIOG,PIN13,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);   //R
	GPIO_Set(GPIOG,PIN14,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);   //G
	GPIO_Set(GPIOG,PIN15,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);   //Y
}




//TIM1��TIM8~TIM11��ʱ��ΪAPB2ʱ�ӵ�������168M��
//TIM2~TIM7��TIM12~TIM14��ʱ��ΪAPB1��ʱ�ӵ�������84M��
//������� 
void scraper_motor_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//�������˿�
	GPIO_Set(GPIOB,PIN6,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6, GPIO_AF_TIM4);//PB6

	//�������˿�
	GPIO_Set(GPIOE,PIN8,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
	//���ɲ���˿�
	GPIO_Set(GPIOG,PIN0,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
	
	//Tout =(ARR+1)*(PSC+1)/Tclk 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	
	TIM_TimeBaseInitStructure.TIM_Period = 300-1; 
	TIM_TimeBaseInitStructure.TIM_Prescaler = 8020-1;      //psc
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //TIMx_CNT<TIMx_CCRnʱ Ҳ����Ĭ������� 
	TIM_OCInitStructure.TIM_Pulse = 150;  
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      //��Ч��ƽΪ�͵�ƽ  Ҳ����Ĭ���������������͵�ƽ

	TIM_OC1Init(TIM4,&TIM_OCInitStructure);		  	              //ʹ��ͨ��1 
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);	           

	TIM_ARRPreloadConfig(TIM4, ENABLE);	


	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM4,ENABLE);
	
	TIM_CCxCmd(TIM4, TIM_Channel_1, TIM_CCx_Disable);
	TIM_ITConfig(TIM4,TIM_IT_CC1, DISABLE);  
}


//�������
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_CC1)!=RESET)
	{
		scraper_motor.pulse_cnt++;             //��������������
		if( SCRAPER_MOTOR_DIR == SCRAPER_BACKWARD )           //�ε�����
		{
			if(scraper_motor.pos > 0)
				scraper_motor.pos --;
			
			if(S_MOTOR_R)                      //������λʱֹͣ
			{
				motor_stop(SCRAPER_MOTOR);
				scraper_motor.pos = 0;        //���λ������
			}
		}
		else                                   //�ε�ǰ��
		{
			scraper_motor.pos ++;
			if(S_MOTOR_L)         //��������λֹͣ
			{
				motor_stop(SCRAPER_MOTOR);
			}
		}
		if( scraper_motor.pulse_cnt > scraper_motor.pulse_num )  //�����趨�г� ֹͣ���  
		{
			motor_stop(SCRAPER_MOTOR);
		}
		TIM_ClearITPendingBit(TIM4,TIM_IT_CC1);
	}
}



//ƽ̨�ϵĵ��
void platform_motor_init(void)
{		
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	//�������˿�
	GPIO_Set(GPIOE,PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource5, GPIO_AF_TIM9);//Z PE5

	//�������˿�
	GPIO_Set(GPIOE,PIN9,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);   //����
	//���ʹ�ܶ˿�
	GPIO_Set(GPIOG,PIN0,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);		//TIM9_CH1
	TIM_TimeBaseInitStructure.TIM_Period = 600-1; 
	TIM_TimeBaseInitStructure.TIM_Prescaler = 16800-1;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_Pulse = 300;  
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OC1Init(TIM9,&TIM_OCInitStructure);		  	              //ʹ��ͨ��1 
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);	           

	TIM_ARRPreloadConfig(TIM9, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM9,ENABLE);	
	TIM_CtrlPWMOutputs(TIM9,ENABLE);	
	
	TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Disable);      //TIM9ͨ��1�ر�
	TIM_ITConfig(TIM9,TIM_IT_CC1, DISABLE);                //TIM9ͨ��1�ж�ʧ��
}



void TIM1_BRK_TIM9_IRQHandler(void)
{
	static u32 dec_step = 0;
	if(TIM_GetITStatus(TIM9,TIM_IT_CC1)!=RESET)
    {
		//���߼Ӽ���ʵ��
		if(platform_motor.pulse_num > (g_calc_t.decel_step+g_calc_t.accel_step))
		{
			if(platform_motor.pulse_cnt < g_calc_t.accel_step - 1 )                                      //���ٶ�
			{
				TIM9->PSC = 280000/(velocity_tab[platform_motor.pulse_cnt]);
				dec_step = 0;
			}
			else if( platform_motor.pulse_cnt >= (platform_motor.pulse_num - g_calc_t.decel_step) )   //���ٶ�
			{
				TIM9->PSC = 280000/(dec_velocity_tab[dec_step]);
				if(dec_step < g_calc_t.decel_step - 1) dec_step++;
			}
			else                                                                                       //���ٶ�
			{
//				TIM9->PSC = 280000/(velocity_tab[g_calc_t.accel_step-1]*2);
				dec_step = 0;
			}
		}
//		TIM9->PSC = 280000/(m->max_v/PLATFORM_STEP_LEN); 
		platform_motor.pulse_cnt++;             //���������ۼ�
		if( MAIN_MOTOR_DIR == MAIN_BACKWARD )               //����
		{
			if(platform_motor.pos > 0)
				platform_motor.pos --;
			
			if(M_MOTOR_R == 0)                      //������λʱֹͣ
			{
				motor_stop(PLATFORM_MOTOR);
				platform_motor.pos = 0;         //���λ������
			}
		}
		else                                   //�ε�ǰ��
		{
			platform_motor.pos ++;
			if(M_MOTOR_L == 0)                 //ǰ��ʱ������λ
			{
				motor_stop(PLATFORM_MOTOR);
			}
		}
		if( platform_motor.pulse_cnt > platform_motor.pulse_num )  //�����趨�г� ֹͣ���
		{
			motor_stop(PLATFORM_MOTOR);
		}
			
		TIM_ClearITPendingBit(TIM9,TIM_IT_CC1);
	}
}




//////////////////////////////////////////////////////////////////////////////////////////////////
//T=��arr+1��(PSC+1)/Tck ����TCKΪʱ��Ƶ��
//���õ���ٶ�   �� um/s
void set_motor_speed(u8 motor_sn,float fre_v)
{
	if(fre_v <= 0)
		return; 
	switch(motor_sn)
	{
		case PLATFORM_MOTOR:
			TIM9->PSC = 280000/(fre_v/PLATFORM_STEP_LEN);
			break;
		case SCRAPER_MOTOR:
			TIM4->PSC = 280000/(fre_v/SCRAPER_STEP_LEN);
			break;
	}
}


//����ƶ� 

u8 motor_move(motor_parameter *m)
{
	if(m->max_v > 0 && m->dis > 0 && m->dis <= m->max_len) 
	{
		if(m->motor_sn == PLATFORM_MOTOR)
		{
			if(calc_speed(PLATFORM_MOTOR,m->start_v,m->max_v,m->acc_tm) == FALSE) // ���� 
				return 1;

			if(calc_speed(PLATFORM_MOTOR,m->max_v,m->end_v,m->dec_tm) == FALSE) // ���� 
				return 1;

			m->pulse_num = m->dis/PLATFORM_STEP_LEN;              //���ݾ���ó�����
			TIM9->PSC = 280000/(m->max_v/PLATFORM_STEP_LEN);  //�����ٶȵó�PSC
			MAIN_MOTOR_DIR = m->dir;                              //�趨�������
			vTaskDelay(10);
			TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Enable);      
			TIM_ITConfig(TIM9,TIM_IT_CC1, ENABLE); 			
		}
		else if(m->motor_sn == SCRAPER_MOTOR)
		{
			m->pulse_num = m->dis/SCRAPER_STEP_LEN;
			TIM4->PSC = 280000/(m->max_v/SCRAPER_STEP_LEN);
			SCRAPER_MOTOR_DIR = m->dir;  
			vTaskDelay(10);
			TIM_CCxCmd(TIM4, TIM_Channel_1, TIM_CCx_Enable);
			TIM_ITConfig(TIM4,TIM_IT_CC1, ENABLE);	
		}
		m->run = 1;	
		return 0;
	}
	return 1;
}


void motor_stop(u8 motor_sn)
{
	if( motor_sn == PLATFORM_MOTOR )      //����ƽ̨
	{
		TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Disable);      //TIM9ͨ��1�ر�
		TIM_ITConfig(TIM9,TIM_IT_CC1, DISABLE);  	
		platform_motor.run = 0;
		platform_motor.pulse_cnt = 0;

	}
	else if( motor_sn == SCRAPER_MOTOR )  //�ε�ƽ̨
	{
		TIM_CCxCmd(TIM4, TIM_Channel_1, TIM_CCx_Disable);
		TIM_ITConfig(TIM4,TIM_IT_CC1, DISABLE); 
		scraper_motor.run = 0;
		scraper_motor.pulse_cnt = 0;
	}
}



//�϶� ����   0:��ֹ  1: ��λ   2: ���� 
u8 hopper_dump(u8 sta)
{
	int i = 0;
	if(sta == 0)
	{
		HOPPER_STATIC;
		return 0;
	}
	else if(sta == 1)
	{
		while(i++ < 3000)
		{
			if(HOPPER_L == 1)   //����λ������Ϊ��λ״̬  �͵�ƽ����
			{
				HOPPER_RESET;
			}
			else
			{
				HOPPER_STATIC;
				return 0;
			}
			vTaskDelay(1);
		}
	}
	else if(sta == 2)
	{
		while(i++ < 3000)
		{
			if(HOPPER_R == 1)   //���Ǹ���λû�д��� ����
			{
				HOPPER_DUMP;
			}
			else
			{
				HOPPER_STATIC;
				return 0;
			}
			vTaskDelay(1);
		}
	}
	return 1;	
}


/**
 * @brief       �ٶȱ���㺯��
 * @param       vo,���ٶ�;vt,ĩ�ٶ�;time,����ʱ��
 * @retval      TRUE���ɹ���FALSE��ʧ��
 */
uint8_t calc_speed(u8 motor_sn, int32_t vo, int32_t vt, float time)
{
    uint8_t is_dec = FALSE;
    int32_t i = 0;
    int32_t vm =0;                              /* �м���ٶ� */
    int32_t inc_acc_stp = 0;                    /* �Ӽ�������Ĳ��� */
    int32_t dec_acc_stp = 0;                    /* ����������Ĳ��� */
    int32_t accel_step = 0;                     /* ���ٻ������Ҫ�Ĳ��� */
    float jerk = 0;                             /* �Ӽ��ٶ� */
    float ti = 0;                               /* ʱ���� dt */
    float sum_t = 0;                            /* ʱ���ۼ��� */
    float delta_v = 0;                          /* �ٶȵ�����dv */
    float ti_cube = 0;                          /* ʱ���������� */

	memset(velocity_tab,0x00,sizeof(velocity_tab));
	memset(dec_velocity_tab,0x00,sizeof(dec_velocity_tab));
	
    if(vo > vt )                                /* ���ٶȱ�ĩ�ٶȴ�,�������˶�,��ֵ�仯�������˶���ͬ */
    {                                           /* ֻ�ǽ����ʱ��ע�⽫�ٶȵ��� */
        is_dec = TRUE;                          /* ���ٶ� */
		if(motor_sn == PLATFORM_MOTOR)
		{
			g_calc_t.vo = PLATFORM_MOTOR_STEPPS(vt);     /* ת����λ ����:step/s */
			g_calc_t.vt = PLATFORM_MOTOR_STEPPS(vo);     /* ת����λ ĩ��:step/s */			
		}
		else if(motor_sn == SCRAPER_MOTOR)
		{
			g_calc_t.vo = SCRAPER_MOTOR_STEPPS(vt);      /* ת����λ ����:step/s */
			g_calc_t.vt = SCRAPER_MOTOR_STEPPS(vo);      /* ת����λ ĩ��:step/s */					
		}
    }
    else
    {
        is_dec = FALSE;                         /* ���ٶ� */
		if(motor_sn == PLATFORM_MOTOR)
		{
			g_calc_t.vo = PLATFORM_MOTOR_STEPPS(vo);     /* ת����λ ����:step/s */
			g_calc_t.vt = PLATFORM_MOTOR_STEPPS(vt);     /* ת����λ ĩ��:step/s */			
		}
		else if(motor_sn == SCRAPER_MOTOR)
		{
			g_calc_t.vo = SCRAPER_MOTOR_STEPPS(vo);     /* ת����λ ����:step/s */
			g_calc_t.vt = SCRAPER_MOTOR_STEPPS(vt);     /* ת����λ ĩ��:step/s */					
		}
    }

    time = ACCEL_TIME(time);                                                    /* �õ��Ӽ��ٶε�ʱ�� */
//    printf("time=%f\r\n",time);
    vm =  (g_calc_t.vo + g_calc_t.vt) / 2 ;                                     /* �����е��ٶ� */
    
    jerk = fabs(2.0f * (vm - g_calc_t.vo) /  (time * time));                    /* �����е��ٶȼ���Ӽ��ٶ� */

    inc_acc_stp = (int32_t)(g_calc_t.vo * time + INCACCELSTEP(jerk,time));      /* �Ӽ�����Ҫ�Ĳ��� */

    dec_acc_stp = (int32_t)((g_calc_t.vt + g_calc_t.vo) * time - inc_acc_stp);  /* ��������Ҫ�Ĳ��� S = vt * time - S1 */

    /* �����ڴ�ռ����ٶȱ� */
    accel_step = dec_acc_stp + inc_acc_stp;                                     /* ������Ҫ�Ĳ��� */
    if( accel_step  % 2 != 0)                                                   /* ���ڸ���������ת�����������ݴ��������,���������1 */
        accel_step  += 1;
	if(accel_step + 1 > 5000)      //�Ų���
		return FALSE;
//    /* mallo�����ڴ�ռ�,�ǵ��ͷ� */
//    velocity_tab = (float*)(mymalloc(SRAMIN,((accel_step + 1) * sizeof(float))));
//    if(velocity_tab == NULL)
//    {
//        printf("�ڴ治��!���޸Ĳ���\r\n");
//        return FALSE;
//    }
/*
 * Ŀ���S���ٶ������Ƕ�ʱ��ķ���,�����ڿ��Ƶ����ʱ�������Բ����ķ�ʽ����,���������V-t������ת��
 * �õ�V-S����,����õ����ٶȱ��ǹ��ڲ������ٶ�ֵ.ʹ�ò������ÿһ�����ڿ��Ƶ���
 */
/* �����һ���ٶ�,���ݵ�һ�����ٶ�ֵ�ﵽ��һ����ʱ�� */
    ti_cube  = 6.0f * 1.0f / jerk;                  /* ����λ�ƺ�ʱ��Ĺ�ʽS = 1/6 * J * ti^3 ��1����ʱ��:ti^3 = 6 * 1 / jerk */
    ti = pow(ti_cube,(1 / 3.0f));                   /* ti */
    sum_t = ti;
    delta_v = 0.5f * jerk * pow(sum_t,2);           /* ��һ�����ٶ� */
    velocity_tab[0] = g_calc_t.vo + delta_v;

/*****************************************************/
    if( velocity_tab[0] <= SPEED_MIN )              /* �Ե�ǰ��ʱ��Ƶ�����ܴﵽ������ٶ� */
        velocity_tab[0] = SPEED_MIN;
    
/*****************************************************/
    
    for(i = 1; i < accel_step; i++)
    {
        /* ����������ٶȾ��Ƕ�ʱ���������Ƶ��,���Լ����ÿһ����ʱ�� */
        /* �õ���i-1����ʱ�� */
        ti = 1.0f / velocity_tab[i-1];              /* ���ÿ��һ����ʱ�� ti = 1 / Vn-1 */
        /* �Ӽ��ٶ��ٶȼ��� */
        if( i < inc_acc_stp)
        {
            sum_t += ti;                            /* ��0��ʼ��i��ʱ���ۻ� */
            delta_v = 0.5f * jerk * pow(sum_t,2);   /* �ٶȵı仯��: dV = 1/2 * jerk * ti^2 */
            velocity_tab[i] = g_calc_t.vo + delta_v;/* �õ��Ӽ��ٶ�ÿһ����Ӧ���ٶ� */
            /* �����һ����ʱ��,ʱ�䲢���ϸ����time,��������Ҫ��������,��Ϊ�����ٶε�ʱ�� */
            if(i == inc_acc_stp - 1)
                sum_t  = fabs(sum_t - time );
        }
        /* �����ٶ��ٶȼ��� */
        else
        {
            sum_t += ti;                                        /* ʱ���ۼ� */
            delta_v = 0.5f * jerk * pow(fabs( time - sum_t),2); /* dV = 1/2 * jerk *(T-t)^2 ��������򿴼����ٵ�ͼ */
            velocity_tab[i] = g_calc_t.vt - delta_v;            /* V = vt - delta_v */
            if(velocity_tab[i] >= g_calc_t.vt)
            {
                accel_step = i;
                break;
            }
        }
//		printf("step/s = %f\r\n",velocity_tab[i]);
    }
    if(is_dec == TRUE)                                          /* ���� */
    {
        float tmp_Speed = 0;
        /* �������� */
		memcpy(dec_velocity_tab,velocity_tab,sizeof(velocity_tab));
        for(i = 0; i< (accel_step / 2); i++)
        {
            tmp_Speed = dec_velocity_tab[i];
            dec_velocity_tab[i] = dec_velocity_tab[accel_step-1 - i];   /* ͷβ�ٶȶԻ� */
            dec_velocity_tab[accel_step-1 - i] = tmp_Speed;
        }

        g_calc_t.decel_tab = dec_velocity_tab;                      /* ���ٶ��ٶȱ� */
        g_calc_t.decel_step = accel_step;                       /* ���ٶε��ܲ��� */
//		for(int i = 0;i < g_calc_t.decel_step;i++)
//			printf("step/s = %f\r\n",dec_velocity_tab[i]);
    }
    else                                                        /* ���� */
    {
        g_calc_t.accel_tab = velocity_tab;                      /* ���ٶ��ٶȱ� */
        g_calc_t.accel_step = accel_step;                       /* ���ٶε��ܲ��� */
    }
    return TRUE;
}


/*
1�����ɼӼ����ٶȱ�
2�����ݱ�ó�pscֵ
3����д�����ٶ�ֵ�����pscֵ��֮�Ա���֤
*/


