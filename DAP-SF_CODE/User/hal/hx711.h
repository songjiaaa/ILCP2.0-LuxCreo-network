#ifndef _HX711_H_
#define _HX711_H_

#include "stm32f4_sys.h"
#include "stm32f4xx.h" 
#include "main.h"

#include "dwt_delay.h"


#define FILTER_BUFF_SIZE   10

#pragma pack(1)
typedef struct 
{
	float filter_data[FILTER_BUFF_SIZE];
	float sum;
	float average;
	unsigned int id;
}ad_filter;           
#pragma pack()


#define HX711_SDA   PDin(13)
#define HX711_SCK   PDout(14)

#define V_WEIGHT_CON   (1.65f/10000)     //1g带来的电压变化

void hx711_io_init(void);
int hx711_recv(void);
float get_weight(void);


#endif



