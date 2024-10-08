#include "rtc.h"

tm sntp_tm = {0};
//RTC_GetDate           //获取日期
//RTC_GetTime           //获取时间

// RTC时间设置
// hour,min,sec:小时,分钟,秒钟
// ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM
// 返回值:SUCEE(1),成功
//        ERROR(0),进入初始化模式失败
ErrorStatus RTC_Set_Time(u8 hour, u8 min, u8 sec, u8 ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
 
	RTC_TimeTypeInitStructure.RTC_Hours = hour;
	RTC_TimeTypeInitStructure.RTC_Minutes = min;
	RTC_TimeTypeInitStructure.RTC_Seconds = sec;
	RTC_TimeTypeInitStructure.RTC_H12 = ampm;
 
	return RTC_SetTime(RTC_Format_BIN, &RTC_TimeTypeInitStructure);
}


// RTC日期设置
// year,month,date:年(0~99),月(1~12),日(0~31)
// week:星期(1~7,0,非法!)
// 返回值:SUCEE(1),成功
//        ERROR(0),进入初始化模式失败
ErrorStatus RTC_Set_Date(u8 year, u8 month, u8 date, u8 week)
{
 
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_DateTypeInitStructure.RTC_Date = date;
	RTC_DateTypeInitStructure.RTC_Month = month;
	RTC_DateTypeInitStructure.RTC_WeekDay = week;
	RTC_DateTypeInitStructure.RTC_Year = year;
	return RTC_SetDate(RTC_Format_BIN, &RTC_DateTypeInitStructure);
}
 

//RTC  初始化
u8 my_rtc_init(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	u16 retry=0X1FFF; 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR时钟
	PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问 

	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)		//是否第一次配置?
	{
		RCC_LSEConfig(RCC_LSE_ON);//LSE 开启    
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
		{
			retry++;
			bsp_DelayMS(10);
		}
		if(retry==0)return 1;		//LSE 开启失败. 
			
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟 

		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC异步分频系数(1~0X7F)
		RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTC同步分频系数(0~7FFF)
		RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC设置为,24小时格式
		RTC_Init(&RTC_InitStructure);

		RTC_Set_Time(00,00,00,RTC_H12_AM);	//设置时间
		RTC_Set_Date(70,1,1,1);		        //设置日期

		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//标记已经初始化过了
	} 

	return 0;
}

//RTC进行一次网络时间校准
//uint32_t ttCnt = 0;
//uint32_t ttStart = 0;
void rtc_time_calibration(void)
{
	static u8 rtc_cal_step = 0;
	static u32 cnt_tick = 0;
	static int second = 0;
	static char weekday[10] = {0}, month[10] = {0};
	static char ntp_time[30] = {0};
    u8 year = 0;
	
	if(wifi_flow.connect_state == 1 && wifi_flow.sntp_cal == 1)   //若网络已连接
	{
		if(rtc_cal_step == 0)
		{
			if( 0 != esp8266_at_test())     //若不在空闲连接下
			{
				esp8266_exit_unvarnished(); //退出透传模式
				if(0 == esp8266_at_test())
				{
					if(0 == esp8266_disconnect_server())   //断开服务连接
					{
						rtc_cal_step = 0; 
					}
				}
			}
			
			if( 0 ==esp8266_set_ntp() )
			{
				rtc_cal_step = 1;
			}
		}
		else if(rtc_cal_step == 1)
		{
			if(	cnt_tick ++ > 300)
			{
				cnt_tick = 0;
				rtc_cal_step = 2;
			}
		}
		else if(rtc_cal_step == 2)
		{
			memset(ntp_time,0x00,sizeof(ntp_time));
			memset(weekday,0x00,sizeof(weekday));
			memset(month,0x00,sizeof(month));		
			if(0 == esp8266_get_ntp_time(ntp_time))
			{
				sscanf(ntp_time, "%s %s %d %d:%d:%d %d",weekday, month, &sntp_tm.tm_day, &sntp_tm.tm_hour, &sntp_tm.tm_min, &sntp_tm.tm_sec, &sntp_tm.tm_year);
				second = sntp_tm.tm_sec;
				rtc_cal_step = 3;
			}
		}
		else if(rtc_cal_step == 3)
		{
			if(second == sntp_tm.tm_sec)  //若得到的秒数与上次相等则继续校准
			{
				memset(ntp_time,0x00,sizeof(ntp_time));
				if(0 == esp8266_get_ntp_time(ntp_time))
				{
					sscanf(ntp_time, "%s %s %d %d:%d:%d %d",weekday, month, &sntp_tm.tm_day, &sntp_tm.tm_hour, &sntp_tm.tm_min, &sntp_tm.tm_sec, &sntp_tm.tm_year);
				}
			}
			else
			{
				RTC_Set_Time(sntp_tm.tm_hour, sntp_tm.tm_min, sntp_tm.tm_sec, 0);    //设置时间

				sntp_tm.tm_mon = getmonth(month);          //月份
				sntp_tm.tm_weekday = getweekday(weekday);  //星期几
				year = sntp_tm.tm_year - 2000;  
				RTC_Set_Date(year, sntp_tm.tm_mon, sntp_tm.tm_day, sntp_tm.tm_weekday);  //设置日期
				rtc_cal_step = 4;
			}
		}
		else if(rtc_cal_step == 4)                             //执行一次服务器连接
		{
			if( 0 == esp8266_connect_udp_server(cfg_dft.lux_domain_name,SERVER_UDP_PORT_NUMBER) )     
			{
				if( 0 == esp8266_enter_unvarnished() )     //进入透传模式
					rtc_cal_step = 5;
			}
//			if( 0 == esp8266_connect_tcp_server(cfg_dft.lux_domain_name,SERVER_TCP_PORT_NUMBER) )    //连接TCP服务器 
//			{
//				if( 0 == esp8266_enter_unvarnished() )     //进入透传模式
//					rtc_cal_step = 5;
//			}
		}
		else if(rtc_cal_step == 5)                                //开始上传数据
		{
			rtc_cal_step = 0;
			wifi_flow.sntp_cal = 0; 
			wifi_flow.execution_step = 6;
		}
	}
	else
	{
		
	}
}

char *g_weekdays[]={ "Mon", "Tue", "Wed", "Thu", "Fri","Sat","Sun"};
int getweekday(const char * m)
{    
    int y=0;
    int x=0;
    for(y=0;y<7;y++){
        if(0==strncmp(g_weekdays[y],m,strlen(m)) ){
//            printf("date m=%d\n",++y);
            x=y;
            break;
        }
        if(0==strncasecmp(g_weekdays[y],m,strlen(m)) ){
//            printf("date ignore m=%d\n",++y);
            x=y;
            break;
        }
    }
    return x + 1;
}

char *g_months[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
int getmonth(const char * m)
{    
    int y=0;
    int x=0;
    for(y=0;y<12;y++){
        if(0==strncmp(g_months[y],m,strlen(m)) ){
//            printf("date m=%d\n",++y);
            x=y;
            break;
        }
        if(0==strncasecmp(g_months[y],m,strlen(m)) ){
//            printf("date ignore m=%d\n",++y);
            x=y;
            break;
        }
    }
    return x + 1;
}













