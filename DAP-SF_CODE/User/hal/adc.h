#ifndef ADC_H_
#define ADC_H_
#include "app.h"

#define ADC_VERF  2.5f

extern volatile s16 ad1_data[];
extern volatile s16 ad3_data[];
void ADC1_Config(void);
void ADC3_Config(void);

float get_adc1_val(u8 channel);

float get_io_voltage(void);

#endif

