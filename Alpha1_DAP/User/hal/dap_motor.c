#include "dap_motor.h"

speed_calc_t g_calc_t = {0};
float velocity_tab[5000] = {0} ;                 /* 速度表格指针 */
float dec_velocity_tab[5000] = {0} ;             //减速

//float main_acc_velocity_tab[5000] = {0};
//float main_dec_velocity_tab[5000] = {0};
//float scraper_acc_velocity_tab[5000] = {0}; 
//float scraper_dec_velocity_tab[5000] = {0}; 
//刮刀电机参数 速度um/s  移动距离um
motor_parameter scraper_motor = 
{
	.motor_sn = SCRAPER_MOTOR,     //刮刀电机
	.max_len = 300 * 1000,          //um
};

//平台电机   303000 um  2500
motor_parameter platform_motor =
{
	.motor_sn = PLATFORM_MOTOR,    //平台电机
	.max_len = 350 * 1000,         //最大行程 350mm    
};
	
//触发信号传感器初始化
void limit_sensor_init(void)
{
	GPIO_Set(GPIOD,PIN0,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //主体正限位，触发为0
	GPIO_Set(GPIOD,PIN1,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //主体负限位，触发为0
	GPIO_Set(GPIOD,PIN2,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //铲刀正限位，靠近打印机这端  触发为高
	GPIO_Set(GPIOD,PIN3,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //铲刀负限位，远离打印机     触发为高
	GPIO_Set(GPIOD,PIN4,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //料斗正限位 触发 低电平
	GPIO_Set(GPIOD,PIN5,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //料斗负限位 触发 低电平
	GPIO_Set(GPIOD,PIN6,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //急停 松开是低电平  按下为高
	GPIO_Set(GPIOD,PIN7,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //料斗检测牙膜高度 触发低电平
	GPIO_Set(GPIOD,PIN8,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //料桶安装到位检测 触发为底电平
	GPIO_Set(GPIOD,PIN9,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //打印机旋转到位检测 触发低电平
	GPIO_Set(GPIOD,PIN10,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //加料中检测
	GPIO_Set(GPIOD,PIN11,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //铲刀右边传感器 触发为底  目前无法触发  目前结构原因无法触发
	GPIO_Set(GPIOD,PIN12,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //铲刀左边传感器 触发为底  目前无法触发  目前结构原因无法触发
	GPIO_Set(GPIOD,PIN13,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //外框门到位检测  触发低电平
	GPIO_Set(GPIOD,PIN14,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //刮刀电机报警
	GPIO_Set(GPIOD,PIN15,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //主体电机报警
	
//	GPIO_Set(GPIOB,PIN6,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //铲刀电机脉冲接口
//	GPIO_Set(GPIOE,PIN5,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //主体电机脉冲接口
	
	GPIO_Set(GPIOG,PIN1,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //料斗
	GPIO_Set(GPIOG,PIN2,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //料斗
	
	//输出
	GPIO_Set(GPIOG,PIN4,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);  //允许加料
	IN_MATERIALS_SW = 1;  //加料不允许
	
	GPIO_Set(GPIOG,PIN13,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);   //R
	GPIO_Set(GPIOG,PIN14,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);   //G
	GPIO_Set(GPIOG,PIN15,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);   //Y
}




//TIM1、TIM8~TIM11的时钟为APB2时钟的两倍即168M，
//TIM2~TIM7、TIM12~TIM14的时钟为APB1的时钟的两倍即84M。
//铲刀电机 
void scraper_motor_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//电机脉冲端口
	GPIO_Set(GPIOB,PIN6,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6, GPIO_AF_TIM4);//PB6

	//电机方向端口
	GPIO_Set(GPIOE,PIN8,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
	//电机刹车端口
	GPIO_Set(GPIOG,PIN0,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
	
	//Tout =(ARR+1)*(PSC+1)/Tclk 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	
	TIM_TimeBaseInitStructure.TIM_Period = 300-1; 
	TIM_TimeBaseInitStructure.TIM_Prescaler = 8020-1;      //psc
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //TIMx_CNT<TIMx_CCRn时 也就是默认情况下 
	TIM_OCInitStructure.TIM_Pulse = 150;  
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      //有效电平为低电平  也就是默认情况下脉冲输出低电平

	TIM_OC1Init(TIM4,&TIM_OCInitStructure);		  	              //使能通道1 
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


//铲刀电机
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_CC1)!=RESET)
	{
		scraper_motor.pulse_cnt++;             //发送脉冲数清零
		if( SCRAPER_MOTOR_DIR == SCRAPER_BACKWARD )           //刮刀后退
		{
			if(scraper_motor.pos > 0)
				scraper_motor.pos --;
			
			if(S_MOTOR_R)                      //触发限位时停止
			{
				motor_stop(SCRAPER_MOTOR);
				scraper_motor.pos = 0;        //电机位置清零
			}
		}
		else                                   //刮刀前进
		{
			scraper_motor.pos ++;
			if(S_MOTOR_L)         //触发正限位停止
			{
				motor_stop(SCRAPER_MOTOR);
			}
		}
		if( scraper_motor.pulse_cnt > scraper_motor.pulse_num )  //超过设定行程 停止电机  
		{
			motor_stop(SCRAPER_MOTOR);
		}
		TIM_ClearITPendingBit(TIM4,TIM_IT_CC1);
	}
}



//平台上的电机
void platform_motor_init(void)
{		
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	//电机脉冲端口
	GPIO_Set(GPIOE,PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource5, GPIO_AF_TIM9);//Z PE5

	//电机方向端口
	GPIO_Set(GPIOE,PIN9,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);   //方向
	//电机使能端口
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

	TIM_OC1Init(TIM9,&TIM_OCInitStructure);		  	              //使能通道1 
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);	           

	TIM_ARRPreloadConfig(TIM9, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM9,ENABLE);	
	TIM_CtrlPWMOutputs(TIM9,ENABLE);	
	
	TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Disable);      //TIM9通道1关闭
	TIM_ITConfig(TIM9,TIM_IT_CC1, DISABLE);                //TIM9通道1中断失能
}



void TIM1_BRK_TIM9_IRQHandler(void)
{
	static u32 dec_step = 0;
	if(TIM_GetITStatus(TIM9,TIM_IT_CC1)!=RESET)
    {
		//曲线加减速实现
		if(platform_motor.pulse_num > (g_calc_t.decel_step+g_calc_t.accel_step))
		{
			if(platform_motor.pulse_cnt < g_calc_t.accel_step - 1 )                                      //加速段
			{
				TIM9->PSC = 280000/(velocity_tab[platform_motor.pulse_cnt]);
				dec_step = 0;
			}
			else if( platform_motor.pulse_cnt >= (platform_motor.pulse_num - g_calc_t.decel_step) )   //减速段
			{
				TIM9->PSC = 280000/(dec_velocity_tab[dec_step]);
				if(dec_step < g_calc_t.decel_step - 1) dec_step++;
			}
			else                                                                                       //匀速段
			{
//				TIM9->PSC = 280000/(velocity_tab[g_calc_t.accel_step-1]*2);
				dec_step = 0;
			}
		}
//		TIM9->PSC = 280000/(m->max_v/PLATFORM_STEP_LEN); 
		platform_motor.pulse_cnt++;             //发送脉冲累计
		if( MAIN_MOTOR_DIR == MAIN_BACKWARD )               //后退
		{
			if(platform_motor.pos > 0)
				platform_motor.pos --;
			
			if(M_MOTOR_R == 0)                      //触发限位时停止
			{
				motor_stop(PLATFORM_MOTOR);
				platform_motor.pos = 0;         //电机位置清零
			}
		}
		else                                   //刮刀前进
		{
			platform_motor.pos ++;
			if(M_MOTOR_L == 0)                 //前进时触发限位
			{
				motor_stop(PLATFORM_MOTOR);
			}
		}
		if( platform_motor.pulse_cnt > platform_motor.pulse_num )  //超过设定行程 停止电机
		{
			motor_stop(PLATFORM_MOTOR);
		}
			
		TIM_ClearITPendingBit(TIM9,TIM_IT_CC1);
	}
}




//////////////////////////////////////////////////////////////////////////////////////////////////
//T=（arr+1）(PSC+1)/Tck 其中TCK为时钟频率
//设置电机速度   按 um/s
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


//电机移动 

u8 motor_move(motor_parameter *m)
{
	if(m->max_v > 0 && m->dis > 0 && m->dis <= m->max_len) 
	{
		if(m->motor_sn == PLATFORM_MOTOR)
		{
			if(calc_speed(PLATFORM_MOTOR,m->start_v,m->max_v,m->acc_tm) == FALSE) // 加速 
				return 1;

			if(calc_speed(PLATFORM_MOTOR,m->max_v,m->end_v,m->dec_tm) == FALSE) // 减速 
				return 1;

			m->pulse_num = m->dis/PLATFORM_STEP_LEN;              //根据距离得出步数
			TIM9->PSC = 280000/(m->max_v/PLATFORM_STEP_LEN);  //根据速度得出PSC
			MAIN_MOTOR_DIR = m->dir;                              //设定点击方向
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
	if( motor_sn == PLATFORM_MOTOR )      //主体平台
	{
		TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Disable);      //TIM9通道1关闭
		TIM_ITConfig(TIM9,TIM_IT_CC1, DISABLE);  	
		platform_motor.run = 0;
		platform_motor.pulse_cnt = 0;

	}
	else if( motor_sn == SCRAPER_MOTOR )  //刮刀平台
	{
		TIM_CCxCmd(TIM4, TIM_Channel_1, TIM_CCx_Disable);
		TIM_ITConfig(TIM4,TIM_IT_CC1, DISABLE); 
		scraper_motor.run = 0;
		scraper_motor.pulse_cnt = 0;
	}
}



//料斗 倒料   0:静止  1: 回位   2: 倒料 
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
			if(HOPPER_L == 1)   //正限位触发则为回位状态  低电平触发
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
			if(HOPPER_R == 1)   //若是负限位没有触发 则倒料
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
 * @brief       速度表计算函数
 * @param       vo,初速度;vt,末速度;time,加速时间
 * @retval      TRUE：成功；FALSE：失败
 */
uint8_t calc_speed(u8 motor_sn, int32_t vo, int32_t vt, float time)
{
    uint8_t is_dec = FALSE;
    int32_t i = 0;
    int32_t vm =0;                              /* 中间点速度 */
    int32_t inc_acc_stp = 0;                    /* 加加速所需的步数 */
    int32_t dec_acc_stp = 0;                    /* 减加速所需的步数 */
    int32_t accel_step = 0;                     /* 加速或减速需要的步数 */
    float jerk = 0;                             /* 加加速度 */
    float ti = 0;                               /* 时间间隔 dt */
    float sum_t = 0;                            /* 时间累加量 */
    float delta_v = 0;                          /* 速度的增量dv */
    float ti_cube = 0;                          /* 时间间隔的立方 */

	memset(velocity_tab,0x00,sizeof(velocity_tab));
	memset(dec_velocity_tab,0x00,sizeof(dec_velocity_tab));
	
    if(vo > vt )                                /* 初速度比末速度大,做减速运动,数值变化跟加速运动相同 */
    {                                           /* 只是建表的时候注意将速度倒序 */
        is_dec = TRUE;                          /* 减速段 */
		if(motor_sn == PLATFORM_MOTOR)
		{
			g_calc_t.vo = PLATFORM_MOTOR_STEPPS(vt);     /* 转换单位 起速:step/s */
			g_calc_t.vt = PLATFORM_MOTOR_STEPPS(vo);     /* 转换单位 末速:step/s */			
		}
		else if(motor_sn == SCRAPER_MOTOR)
		{
			g_calc_t.vo = SCRAPER_MOTOR_STEPPS(vt);      /* 转换单位 起速:step/s */
			g_calc_t.vt = SCRAPER_MOTOR_STEPPS(vo);      /* 转换单位 末速:step/s */					
		}
    }
    else
    {
        is_dec = FALSE;                         /* 加速段 */
		if(motor_sn == PLATFORM_MOTOR)
		{
			g_calc_t.vo = PLATFORM_MOTOR_STEPPS(vo);     /* 转换单位 起速:step/s */
			g_calc_t.vt = PLATFORM_MOTOR_STEPPS(vt);     /* 转换单位 末速:step/s */			
		}
		else if(motor_sn == SCRAPER_MOTOR)
		{
			g_calc_t.vo = SCRAPER_MOTOR_STEPPS(vo);     /* 转换单位 起速:step/s */
			g_calc_t.vt = SCRAPER_MOTOR_STEPPS(vt);     /* 转换单位 末速:step/s */					
		}
    }

    time = ACCEL_TIME(time);                                                    /* 得到加加速段的时间 */
//    printf("time=%f\r\n",time);
    vm =  (g_calc_t.vo + g_calc_t.vt) / 2 ;                                     /* 计算中点速度 */
    
    jerk = fabs(2.0f * (vm - g_calc_t.vo) /  (time * time));                    /* 根据中点速度计算加加速度 */

    inc_acc_stp = (int32_t)(g_calc_t.vo * time + INCACCELSTEP(jerk,time));      /* 加加速需要的步数 */

    dec_acc_stp = (int32_t)((g_calc_t.vt + g_calc_t.vo) * time - inc_acc_stp);  /* 减加速需要的步数 S = vt * time - S1 */

    /* 申请内存空间存放速度表 */
    accel_step = dec_acc_stp + inc_acc_stp;                                     /* 加速需要的步数 */
    if( accel_step  % 2 != 0)                                                   /* 由于浮点型数据转换成整形数据带来了误差,所以这里加1 */
        accel_step  += 1;
	if(accel_step + 1 > 5000)      //放不下
		return FALSE;
//    /* mallo申请内存空间,记得释放 */
//    velocity_tab = (float*)(mymalloc(SRAMIN,((accel_step + 1) * sizeof(float))));
//    if(velocity_tab == NULL)
//    {
//        printf("内存不足!请修改参数\r\n");
//        return FALSE;
//    }
/*
 * 目标的S型速度曲线是对时间的方程,但是在控制电机的时候则是以步进的方式控制,所以这里对V-t曲线做转换
 * 得到V-S曲线,计算得到的速度表是关于步数的速度值.使得步进电机每一步都在控制当中
 */
/* 计算第一步速度,根据第一步的速度值达到下一步的时间 */
    ti_cube  = 6.0f * 1.0f / jerk;                  /* 根据位移和时间的公式S = 1/6 * J * ti^3 第1步的时间:ti^3 = 6 * 1 / jerk */
    ti = pow(ti_cube,(1 / 3.0f));                   /* ti */
    sum_t = ti;
    delta_v = 0.5f * jerk * pow(sum_t,2);           /* 第一步的速度 */
    velocity_tab[0] = g_calc_t.vo + delta_v;

/*****************************************************/
    if( velocity_tab[0] <= SPEED_MIN )              /* 以当前定时器频率所能达到的最低速度 */
        velocity_tab[0] = SPEED_MIN;
    
/*****************************************************/
    
    for(i = 1; i < accel_step; i++)
    {
        /* 步进电机的速度就是定时器脉冲输出频率,可以计算出每一步的时间 */
        /* 得到第i-1步的时间 */
        ti = 1.0f / velocity_tab[i-1];              /* 电机每走一步的时间 ti = 1 / Vn-1 */
        /* 加加速段速度计算 */
        if( i < inc_acc_stp)
        {
            sum_t += ti;                            /* 从0开始到i的时间累积 */
            delta_v = 0.5f * jerk * pow(sum_t,2);   /* 速度的变化量: dV = 1/2 * jerk * ti^2 */
            velocity_tab[i] = g_calc_t.vo + delta_v;/* 得到加加速段每一步对应的速度 */
            /* 当最后一步的时候,时间并不严格等于time,所以这里要稍作处理,作为减加速段的时间 */
            if(i == inc_acc_stp - 1)
                sum_t  = fabs(sum_t - time );
        }
        /* 减加速段速度计算 */
        else
        {
            sum_t += ti;                                        /* 时间累计 */
            delta_v = 0.5f * jerk * pow(fabs( time - sum_t),2); /* dV = 1/2 * jerk *(T-t)^2 看这个逆向看减加速的图 */
            velocity_tab[i] = g_calc_t.vt - delta_v;            /* V = vt - delta_v */
            if(velocity_tab[i] >= g_calc_t.vt)
            {
                accel_step = i;
                break;
            }
        }
//		printf("step/s = %f\r\n",velocity_tab[i]);
    }
    if(is_dec == TRUE)                                          /* 减速 */
    {
        float tmp_Speed = 0;
        /* 倒序排序 */
		memcpy(dec_velocity_tab,velocity_tab,sizeof(velocity_tab));
        for(i = 0; i< (accel_step / 2); i++)
        {
            tmp_Speed = dec_velocity_tab[i];
            dec_velocity_tab[i] = dec_velocity_tab[accel_step-1 - i];   /* 头尾速度对换 */
            dec_velocity_tab[accel_step-1 - i] = tmp_Speed;
        }

        g_calc_t.decel_tab = dec_velocity_tab;                      /* 减速段速度表 */
        g_calc_t.decel_step = accel_step;                       /* 减速段的总步数 */
//		for(int i = 0;i < g_calc_t.decel_step;i++)
//			printf("step/s = %f\r\n",dec_velocity_tab[i]);
    }
    else                                                        /* 加速 */
    {
        g_calc_t.accel_tab = velocity_tab;                      /* 加速段速度表 */
        g_calc_t.accel_step = accel_step;                       /* 加速段的总步数 */
    }
    return TRUE;
}


/*
1、生成加减速速度表
2、根据表得出psc值
3、填写几组速度值计算出psc值与之对比验证
*/


