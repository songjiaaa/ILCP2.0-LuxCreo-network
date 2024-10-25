#ifndef __WS2812B_H
#define	__WS2812B_H

#include "app.h"
#include "ws2812b_data.h"

//#define TIMING_ONE  67        
//#define TIMING_ZERO 34   

#define TIMING_ONE  50        
#define TIMING_ZERO 25 

//#define TIMING_ONE  130        
//#define TIMING_ZERO 60 

#define RGB_LED_NUM  32          

extern u8 rgb_light_state;
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
void steady_green_light(void);
void led_alarm_information(u8 color[]);
void green_breathing_light(void);
void rgb_ctrl_show(u8 *color[], u16 len);
void progress_light(u8 color[],u8 rgb_num);
void rgb_light_stat(u8 stat);

#endif /* __LED_H */



