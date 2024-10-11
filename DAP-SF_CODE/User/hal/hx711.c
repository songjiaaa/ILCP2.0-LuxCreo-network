#include "hx711.h"


void hx711_io_init(void)
{
	GPIO_Set(GPIOD,PIN13,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_NONE);   //Dout
	GPIO_Set(GPIOD,PIN14,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);  //SCK
}


int hx711_recv(void)
{
	u32 data = 0;
	HX711_SCK = 0;
//	bsp_DelayUS(1);
	while(HX711_SDA);  
	for(int i = 0;i < 24;i++)
	{
		HX711_SCK = 1;
		data <<= 1;
		HX711_SCK = 0;
//		bsp_DelayUS(1);
		if(HX711_SDA) data++;
	}
	HX711_SCK = 1;
	data = data ^ 0x800000;
	HX711_SCK = 0;
	return (int)data;
}



