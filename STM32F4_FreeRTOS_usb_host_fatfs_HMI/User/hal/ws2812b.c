#include "ws2812b.h"

u16 led_data_buf[1000];  //µÆÖéÑÕÉ«»º´æ

//ºì³È»ÆÂÌÇàÀ¶×Ï °×ºÚ
u8 red[] = {100,0,0};        //ºì
u8 orange[] = {150,60,0};
u8 yellow[] = {60,60,0};    //»Æ 255 255
u8 green[] = {0,30,0};       //ÂÌ
u8 cyan[] = {0,255,255};     //Çà
u8 blue[] = {0,0,255};       //À¶
u8 purple[] = {128,0,128};   //×Ï

u8 black[3] = {0};
u8 white[3] = {255,255,255};

void ws2812_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;          
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      
	GPIO_Init(GPIOC,&GPIO_InitStructure);              
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM8);

	TIM_TimeBaseStructure.TIM_Period = 210-1; // 800kHz      168M/210 = 800k
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;    
	  
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCNIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

	
	TIM_OC3Init(TIM8,&TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM8,TIM_OCPreload_Enable);
	TIM_Cmd(TIM8, ENABLE);
    TIM_CtrlPWMOutputs(TIM8,ENABLE);
	
	TIM_ARRPreloadConfig(TIM8,ENABLE);


    DMA_DeInit(DMA2_Stream4);
    DMA_InitStructure.DMA_Channel = DMA_Channel_7;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(TIM8->CCR3);
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)led_data_buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = 42;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    DMA_Init(DMA2_Stream4,&DMA_InitStructure);

    TIM_DMACmd(TIM8,TIM_DMA_CC3, ENABLE);
}

//ÑÕÉ«£¬µÆÖé¸öÊý£¬µÆÖé¸öÊýËæÊµ¼ÊÊ¹ÓÃ¶ø¶¨¡£
void led_single_show(u8 color[], u16 len)
{
    u8 i = 0;
    u16 memaddr = 0;
    u16 buffersize = 0;

    buffersize = (len * 24) + 1;       // number of bytes needed is #LEDs * 24 bytes + 42 trailing bytes

    while(len)
    {
        for(i = 0; i < 8; i++)          //ÂÌ
        {
            led_data_buf[memaddr] = ((color[1] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        for(i = 0; i < 8; i++)          //ºì
        {   
            led_data_buf[memaddr] = ((color[0] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        for(i = 0; i < 8; i++)          //À¶
        {
            led_data_buf[memaddr] = ((color[2] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        len--;
    }
//	OS_CLOSE_INT;  //¹ØÖÐ¶Ï
    DMA_SetCurrDataCounter(DMA2_Stream4, buffersize);
    TIM_DMACmd(TIM8, TIM_DMA_CC3, ENABLE);
    DMA_Cmd(DMA2_Stream4, ENABLE);
	TIM_Cmd(TIM8, ENABLE);
	
    while(!DMA_GetFlagStatus(DMA2_Stream4, DMA_FLAG_TCIF4))
	{
//		vTaskDelay(1);
	}
	
    DMA_Cmd(DMA2_Stream4, DISABLE);
    DMA_ClearFlag(DMA2_Stream4, DMA_FLAG_TCIF4);
    TIM_Cmd(TIM8, DISABLE);
//	OS_OPEN_INT;  //¿ªÖÐ¶Ï
}






//ÑÕÉ«£¬µÆÖé¸öÊý£¬µÆÖé¸öÊýËæÊµ¼ÊÊ¹ÓÃ¶ø¶¨¡£
u8 **color_p = NULL;
void rgb_ctrl_show(u8 *color[], u16 len)
{
    u8 i = 0;
    u16 memaddr = 0;
    u16 buffersize = 0;

	color_p = color;
	
    buffersize = (len * 24) + 1;        

    for(int x=0; x<len; x++)
    {
        for(i = 0; i < 8; i++)          //ÂÌ
        {
            led_data_buf[memaddr] = ((((u8*)(color_p[x]))[1] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        for(i = 0; i < 8; i++)          //ºì
        {   
            led_data_buf[memaddr] = ((((u8*)(color_p[x]))[0] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        for(i = 0; i < 8; i++)          //À¶
        {
            led_data_buf[memaddr] = ((((u8*)(color_p[x]))[2] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
    }

	DMA_SetCurrDataCounter(DMA2_Stream4, buffersize);

    TIM_DMACmd(TIM8, TIM_DMA_CC3, ENABLE);
    DMA_Cmd(DMA2_Stream4, ENABLE);
	TIM_Cmd(TIM8, ENABLE);
	
    while(!DMA_GetFlagStatus(DMA2_Stream4, DMA_FLAG_TCIF4))
	{
//		vTaskDelay(1);
	}
	
    DMA_Cmd(DMA2_Stream4, DISABLE);
    DMA_ClearFlag(DMA2_Stream4, DMA_FLAG_TCIF4);
    TIM_Cmd(TIM8, DISABLE);
}




u8 *rgb_color[RGB_LED_NUM] = {NULL};
void progress_light(u8 color[],u8 rgb_num)
{
//	for(int i = 0;i<rgb_num;i++)
//	{
//		rgb_color[RGB_LED_NUM - i -1] = color;		
//	}
//	for(int i = rgb_num;i<RGB_LED_NUM;i++)
//	{
//		rgb_color[RGB_LED_NUM - i -1] = black;	
//	}
//	
	for(int i = 0;i<RGB_LED_NUM;i++)
	{
		if(i<rgb_num)
			rgb_color[RGB_LED_NUM - i -1] = color;	
		else
			rgb_color[RGB_LED_NUM - i -1] = black;	
	}
	rgb_ctrl_show(rgb_color,RGB_LED_NUM);
}


//ÌáÊ¾¾¯¸æÐÅÏ¢ µÆÉÁËõÈý´Î»Ö¸´´ý»ú×´Ì¬
void rgb_warning_show(u8 color[])
{
	static u32 confirm_tick = 0,confirm_num = 0;

	confirm_tick ++;
	if(confirm_tick == 1)
	{
		led_single_show(color,RGB_LED_NUM); 		
	}
	else if(confirm_tick == 20)       
	{
		led_single_show(black,RGB_LED_NUM);   //Ãð
	}
	else if(confirm_tick == 40)       //10*x ms
	{
		confirm_tick = 0;
		confirm_num++;
	}
	if(confirm_num == 3)
	{
		confirm_num = 0;
		rgb_light_state = 0;        //»Ö¸´´ý»úÂÌµÆ³£ÁÁ×´Ì¬
	}
}

//ÂÌµÆ³£ÁÁ
void steady_green_light(void)
{
	green[0] = 0; green[1] = 30; green[2] = 0; 
	led_single_show(green,RGB_LED_NUM);
}


//¸æ¾¯ÏÔÊ¾
void led_alarm_information(u8 color[])
{
    led_single_show(color,RGB_LED_NUM);
}

//ÂÌµÆºôÎü
void green_breathing_light(void)
{
	static u16 cyc_green = 0;
	green[1] = index_wave[ cyc_green % ARRAY_SIZE(index_wave) ];
	led_single_show(green,RGB_LED_NUM);
	if(++cyc_green == ARRAY_SIZE(index_wave)) cyc_green = 0;
}


//ºìµÆÉÁË¸  100hzµ÷ÓÃ
void blinking_red_lamp(void)
{
	static u32 led_tick = 0;

	led_tick ++;
	if(led_tick == 1)
	{
		led_single_show(red,RGB_LED_NUM);	 	
	}
	else if(led_tick == 100)       //10*100 = 1000ms
	{
		led_single_show(black,RGB_LED_NUM);   //Ãð
	}
	else if(led_tick == 180)       //10*180 = 1800ms
	{
		led_tick = 0;
	}	
}

//³ÈµÆÉÁË¸  100hzµ÷ÓÃ
void blinking_orange_lamp(void)
{
	static u32 led_tick = 0;

	led_tick ++;
	if(led_tick == 1)
	{
		led_single_show(orange,RGB_LED_NUM);	 	
	}
	else if(led_tick == 100)       //10*100 = 1000ms
	{
		led_single_show(black,RGB_LED_NUM);   //Ãð
	}
	else if(led_tick == 180)       //10*180 = 1800ms
	{
		led_tick = 0;
	}	
}

u8 rgb_light_state =  0;
void rgb_light_stat(u8 stat)
{
	switch(stat)
	{
		case 0:
			steady_green_light();             //ÂÌµÆ³£ÁÁ
			break;
		case 1:
			green_breathing_light();          //ÂÌµÆºôÎü
			break;
		case 2:
			blinking_orange_lamp();           //³ÈµÆÉÁË¸
			break;
		case 3:
			blinking_red_lamp();              //ºìµÆÉÁË¸
			break;
		case 4:
			rgb_warning_show(orange);         //³ÈµÆÉÁË¸Èý´Î»Ö¸´´ý»ú×´Ì¬
			break;
		default:
			break;
	}
}





