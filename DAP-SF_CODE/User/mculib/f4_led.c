
#include "f4_led.h"

//S_LED_CTRL led_list[]=
//{ //PORT    PIN  亮电平  亮时间,周期(若小于duty，则为单次触发)  tick
	//GPIOE,13,  1,      0,0,   0, //
	//GPIOE,14,  1,      5,20,  0, //
	//GPIOE,15,  1,      4,20,  0 //
//};
S_LED_CTRL *pled_list=0;
u8 led_len=0; //LED列表长度
void led_ini(S_LED_CTRL *pled,u8 n)
{
	int i;
	pled_list=pled;
	led_len=n;
	for(i = 0; i < led_len; i++)
	{
		GPIO_Set(pled_list[i].port,(1<<(pled_list[i].pin)),GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
		if(pled_list[i].light_lev==0)//初始化为不亮
			pled_list[i].port->BSRRL=1<<pled_list[i].pin;
		else
			pled_list[i].port->BSRRH=1<<pled_list[i].pin;
	}
}
void led_toggle(int offset) //对于非周期的，触发led	
{
	if(offset<led_len)
	{
		pled_list[offset].tick=0;
	}
}
void led_period(int offset,int t) //设置led周期，单位tick，若为0则为关闭
{
	if(offset<led_len)
	{
		pled_list[offset].T=t;
		if(t==0)
		{
			pled_list[offset].tick=pled_list[offset].duty;
		}
	}
}
void led_tick(void)
{
	int i;
	for(i = 0; i < led_len; i++)
	{
		if(pled_list[i].tick<pled_list[i].duty)//若需要亮
		{
			if(pled_list[i].light_lev==0)
				pled_list[i].port->BSRRH=1<<pled_list[i].pin;
			else
				pled_list[i].port->BSRRL=1<<pled_list[i].pin;
			pled_list[i].tick++;
		}
		else //若需要灭
		{
			//是否归零？
			if(pled_list[i].T>=pled_list[i].duty) //若不是触发型
			{
				if(pled_list[i].tick<pled_list[i].T)
				{
					pled_list[i].tick++;
				}
				else
				{
					pled_list[i].tick=0;
				}
			}
			if(pled_list[i].light_lev==0)
				pled_list[i].port->BSRRL=1<<pled_list[i].pin;
			else
				pled_list[i].port->BSRRH=1<<pled_list[i].pin;
		}
	}
}

