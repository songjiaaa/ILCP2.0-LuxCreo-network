#include "version.h"

//__DATE__中获取月份
u8 get_month(void)
{
	u8 month_val;
	switch(__DATE__[2])
	{
		case 'n':
			if(__DATE__[1] == 'a')
			month_val = 1;
			else
			month_val = 6;
		break;
		case 'b':
			month_val = 2;
		break;
		case 'r':
			if(__DATE__[1] == 'a')
			month_val = 3;
			else
			month_val = 4;
		break;
		case 'y':
			month_val = 5;
		break;
		case 'l':
			month_val = 7;
		break;
		case 'g':
			month_val = 8;
		break;
		case 'p':
			month_val = 9;
		break;
		case 't':
			month_val = 10;
		break;
		case 'v':
			month_val = 11;
		break; 
		case 'c':
			month_val = 12;
		break; 
		default:
			month_val = 0;
			break;
	}
	return month_val;
}

void version_release(void)
{
	volatile u8  DataStr[]=__DATE__;
	u8  firmwave_ver[30] = {0}; 

	memcpy(firmwave_ver,VERSION,sizeof(VERSION));
	firmwave_ver[sizeof(VERSION)-1] = '.';               //减去位置\0
	
	firmwave_ver[sizeof(VERSION)] = __DATE__[9];
	firmwave_ver[sizeof(VERSION)+1] = __DATE__[10];
	
	firmwave_ver[sizeof(VERSION)+3] = get_month();      
	sprintf((char*)&firmwave_ver[sizeof(VERSION)+2],"%02d",firmwave_ver[sizeof(VERSION)+3]);
	
	if(DataStr[4] == ' ') DataStr[4] = '0';
	memcpy(&firmwave_ver[sizeof(VERSION)+4],(char*)&DataStr[4],2);
	
//	firmwave_ver[sizeof(VERSION)+6] = __TIME__[0];
//	firmwave_ver[sizeof(VERSION)+7] = __TIME__[1];
	
	memcpy(cfg_dft.version,firmwave_ver,sizeof(cfg_dft.version));
	SetTextValue(SETTTING_PAGE_ID1,13,firmwave_ver);          
}



