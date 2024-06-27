#ifndef __WS2812B_H
#define	__WS2812B_H

#include "app.h"
#include "ws2812b_data.h"

//#define TIMING_ONE  67        
//#define TIMING_ZERO 34   

#define TIMING_ONE  130        
#define TIMING_ZERO 60 

#define RGB_LED_NUM  32           //µÆÖéÊý

extern u8 ws12b_on; 

extern u8 red[];
extern u8 orange[];
extern u8 yellow[];
extern u8 green[];
extern u8 cyan[];
extern u8 blue[];
extern u8 purple[];

extern u8 black[3];
extern u8 white[3];

void ws2812_init(void);
void led_single_show(u8 color[], u16 len);
void led_alarm_information(u8 color[]);
void green_breathing_light(void);
void rgb_ctrl_show(u8 *color[], u16 len);
void progress_light(u8 color[],u8 rgb_num);


#endif /* __LED_H */



