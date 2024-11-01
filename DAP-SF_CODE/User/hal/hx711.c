#include "hx711.h"

ad_filter weight = {0};

//滑动均值滤波
float sliding_mean_filter_calc(ad_filter *filter,float data)
{
	filter->sum -= filter->filter_data[filter->id];   
    filter->sum += data;
    filter->filter_data[filter->id] = data;
	filter->average = filter->sum / FILTER_BUFF_SIZE;
	if(++filter->id == FILTER_BUFF_SIZE) filter->id = 0;
	
	return filter->average;
}



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


//得到具体重量g
float get_weight(void)
{
	int weight_data = 0;
	float ad_val = 0,ad_mean = 0;
	weight_data = hx711_recv();    //24位AD转换芯片 
	ad_val = weight_data; 
	//滤波
	ad_mean = sliding_mean_filter_calc(&weight,ad_val);
	return (ad_mean - 8483000)/382.0f;
}



