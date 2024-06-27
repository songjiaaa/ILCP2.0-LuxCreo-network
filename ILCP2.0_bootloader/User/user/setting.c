#include "setting.h"


void setting_show(void)
{
	version_release();      //显示版本号
	//显示SN码
	//显示总时间
	uvled_total_time_show();
	//显示图标
	switch_icon(SETTTING_PAGE_ID1,6,cfg_dft.light_strip);
	switch_icon(SETTTING_PAGE_ID1,7,cfg_dft.head_lamp);
}


//设置界面输入控制
void setting_ctrl(u16 touch_id)
{
	if(touch_id == 1)          //back
	{
	
	}
	else if(touch_id == 2)     //left
	{
		
	}
	else if(touch_id == 3)     //right
	{
		
	}
	else if(touch_id == 4)     //版本位置处
	{
	
	}
	else if(touch_id == 5)     //校准界面处
	{
	
	}
	else if(touch_id == 9)
	{
		cfg_dft.light_strip ^= 1;
		switch_icon(SETTTING_PAGE_ID1,6,cfg_dft.light_strip);
	}
	else if(touch_id == 10)
	{
		cfg_dft.head_lamp ^= 1;
		switch_icon(SETTTING_PAGE_ID1,7,cfg_dft.head_lamp);
	}
	else if(touch_id == 16)    //wifi  input
	{
	
	}
	else if(touch_id == 17)    //upgrade
	{
		set_screen(32);
	}		
}


//void uvled_total_time_show(void)
//{
//	char hour[30] = {0};
//	int n = 0;
//	u32 hours = 0,minute = 0;
//	hours = (save_set.uv_total_time / 60.0f) / 60.0f;
//	minute = (save_set.uv_total_time / 60) % 60;  
//	
//	memset(hour,0x00,sizeof(hour));

//	n = sprintf(hour,"%dh",hours);
//	
//	n = sprintf(&hour[n]," %dmin",minute);
//	SetTextValue(SETTTING_PAGE_ID2,2,(u8*)hour);  
//}












