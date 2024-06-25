#ifndef _RTC_H_
#define _RTC_H_

#include "app.h"


typedef struct 
{
	int tm_sec;         /* 秒，范围从 0 到 59        */
	int tm_min;         /* 分，范围从 0 到 59        */
	int tm_hour;        /* 小时，范围从 0 到 23        */
	int tm_day;         /* 一月中的第几天，范围从 1 到 31    */
	int tm_mon;         /* 月，范围从 1 到 12    */
	int tm_year;        /* 年数   */
	int tm_weekday;     /* 一周中的第几天，范围从 0 到 6，这里注意：0代表星期天 */
}tm;


ErrorStatus RTC_Set_Time(u8 hour, u8 min, u8 sec, u8 ampm);
ErrorStatus RTC_Set_Date(u8 year, u8 month, u8 date, u8 week);
u8 my_rtc_init(void);

void rtc_time_calibration(void);
int getmonth(const char * m);
int getweekday(const char * m);

#endif





