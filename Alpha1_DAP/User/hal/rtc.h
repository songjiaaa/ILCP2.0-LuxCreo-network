#ifndef _RTC_H_
#define _RTC_H_

#include "app.h"


typedef struct 
{
	int tm_sec;         /* �룬��Χ�� 0 �� 59        */
	int tm_min;         /* �֣���Χ�� 0 �� 59        */
	int tm_hour;        /* Сʱ����Χ�� 0 �� 23        */
	int tm_day;         /* һ���еĵڼ��죬��Χ�� 1 �� 31    */
	int tm_mon;         /* �£���Χ�� 1 �� 12    */
	int tm_year;        /* ����   */
	int tm_weekday;     /* һ���еĵڼ��죬��Χ�� 0 �� 6������ע�⣺0���������� */
}tm;


ErrorStatus RTC_Set_Time(u8 hour, u8 min, u8 sec, u8 ampm);
ErrorStatus RTC_Set_Date(u8 year, u8 month, u8 date, u8 week);
u8 my_rtc_init(void);

void rtc_time_calibration(void);
int getmonth(const char * m);
int getweekday(const char * m);

#endif





