#include "custom.h"

cure_para user_custom[MAX_PARA_NUM] = {0};
page_record custom_page = {0,0,0,-1,-1};
void show_para(u16 screen_id,u16 control_id,cure_para *para);


void custom_select(u16 id)
{
	if(id == 0x0001)
	{
		set_screen(0x0000);    
	}
	else if(id == 0x0002)      
	{
		if(custom_page.cur_page > 0)
		{
			custom_page.cur_page --;
			show_cur_custom_page(custom_page.cur_page);
		}
	}
	else if(id == 0x0003)      
	{
		if(custom_page.cur_page + 1 < custom_page.total_page)
		{			
			custom_page.cur_page++;
			show_cur_custom_page(custom_page.cur_page);
		}
	}
	else if(id == 0x0004)  
	{
		if(custom_page.select_num != -1)        //有选中  
		{
			cure_start();
		}
		else
		{
			//弹框提示需要选中
			pop_tips(30,(u8*)TIPS,(u8*)SELECT_FILE);
		}
	}
	else if(id >= 0x0005  && id <= 0x0008)
	{
		custom_page.select_line = id - 4;
		custom_page.select_num = custom_page.cur_page * TOTAL_NUM_LINE_PAGE +  custom_page.select_line;
		
		if(custom_page.select_num <= custom_page.total_line)
		{
			exe_parameter = user_custom[custom_page.select_num - 1];
			switch_icon(0x06,13,1);  
			switch_icon(0x06,14,1);  
			switch_icon(0x06,15,1);  
			if(id == 0x0005)
				clear_button_value(0x0006,0x0006,0x0007,0x0008);
			else if(id == 0x0006)
				clear_button_value(0x0006,0x0005,0x0007,0x0008);	
			else if(id == 0x0007)
				clear_button_value(0x0006,0x0005,0x0006,0x0008);	
			else if(id == 0x0008)
				clear_button_value(0x0006,0x0005,0x0006,0x0007);
		}
		else if(custom_page.select_num == custom_page.total_line + 1)
		{
			add_custom_info();
		}
		else
		{
			switch_icon(0x06,13,0);  
			switch_icon(0x06,14,0);  
			switch_icon(0x06,15,0);  
			clear_button_all(0x0006,0x0005,0x0006,0x0007,0x0008);
			custom_page.select_num = -1;
		}
	}
	else if(id == 29)  
	{
		if(custom_page.select_num != -1 )    
		{
			show_para(7,4,&exe_parameter);   
			SetTextValue(7,3,exe_parameter.resin);  
			set_screen(7);
		}
	}
	else if(id == 30)  
	{
		if(custom_page.select_num != -1 )    
		{
			show_para(15,1,&exe_parameter);   
			set_screen(15);
		}		
	}
	else if(id == 31)    //删除
	{
		if(custom_page.select_num != -1 )   
		{
			set_screen(19);
			rgb_light_state = 4;    //橙灯闪烁告警			
		}
	}
}

//查看参数
void show_para(u16 screen_id,u16 control_id,cure_para *para)
{
	u8 show_buf[10];
	for(int i = 0;i < 4;i++)
	{
		if( para->time[i] != 0 )
		{
			memset(show_buf,0x00,sizeof(show_buf));
			sprintf((char*)show_buf,"%d",para->up_uv[i]); 
			SetTextValue(screen_id,control_id + i,show_buf);       
	
			memset(show_buf,0x00,sizeof(show_buf));
			sprintf((char*)show_buf,"%d",para->down_uv[i]); 
			SetTextValue(screen_id,control_id + i + 4,show_buf);   		
 
			memset(show_buf,0x00,sizeof(show_buf));
			sprintf((char*)show_buf,"%d",para->heat[i]); 
			SetTextValue(screen_id,control_id + i + 8,show_buf);   
	
			memset(show_buf,0x00,sizeof(show_buf));
			sprintf((char*)show_buf,"%d",para->time[i]); 
			SetTextValue(screen_id,control_id + i + 12,show_buf); 
			
			sprintf((char*)show_buf,"%d",i+1); 
			SetTextValue(screen_id,20+i,show_buf); 			
		}
		else
		{
			clear_text_value(screen_id,control_id + i);
			clear_text_value(screen_id,control_id + i + 4);
			clear_text_value(screen_id,control_id + i + 8);
			clear_text_value(screen_id,control_id + i + 12);
			clear_text_value(screen_id,20+i);
		}
	}
}

void delete_para(void)
{
	for(int i = custom_page.select_num - 1;i < MAX_PARA_NUM;i++)
	{
		user_custom[i] = user_custom[i+1];
	}

	write_user_parameter(USER_FILE,(const void*)user_custom,sizeof(user_custom));	

	read_custom_info();                               
	custom_page.select_num = -1; 
	if(custom_page.cur_page + 1 > custom_page.total_page) 
	{
		if(custom_page.cur_page != 0) custom_page.cur_page -= 1;
	}
	show_cur_custom_page(custom_page.cur_page);  
}

void save_para(void)
{
	if(strlen((const char*)user_custom[custom_page.select_num - 1].resin) == 0)  
	{
		pop_tips(30,(u8*)WARNING,(u8*)EN_NAME);
		return;
	}
	else if(user_custom[custom_page.select_num - 1].time[0] == 0)    
	{
		pop_tips(30,(u8*)WARNING,(u8*)EN_PARAMETER);
		return;
	}
	else if(input_curing_time() > CURE_MAX_TIME)
	{
		pop_tips(30,(u8*)WARNING,(u8*)CURING_TIME);
		return;
	}
	write_user_parameter(USER_FILE,user_custom,sizeof(user_custom)); 
	read_custom_info();
	custom_page.select_num = -1; 
	if(custom_page.total_page > 0) 
		custom_page.cur_page = custom_page.total_page - 1;   
	show_cur_custom_page(custom_page.cur_page);
}

u32 input_curing_time(void)
{
	u32 curing_time = 0;
	for(int i = 0; i < CUSTOM_MAX_STEP;i++)
	{
		if(user_custom[custom_page.select_num-1].up_uv[i] != 0 || user_custom[custom_page.select_num-1].down_uv[i] != 0)
		{
			curing_time += user_custom[custom_page.select_num-1].time[i];
		}
	}
	return curing_time;
}

void show_cur_custom_page(u16 cur_page)
{
	u32 total_timer = 0;
	u8 show_buf[10] = {0};

	switch_icon(0x06,13,0);  
	switch_icon(0x06,14,0);  
	switch_icon(0x06,15,0);  
	switch_icon(0x06,16,0);	switch_icon(0x06,17,0);	switch_icon(0x06,18,0);	switch_icon(0x06,19,0);   
	switch_icon(0x06,25,0);	switch_icon(0x06,26,0);	switch_icon(0x06,27,0);	switch_icon(0x06,28,0);   
	clear_text_value(6,9);
	clear_text_value(6,20);
	
	clear_text_value(6,10);
	clear_text_value(6,21);
	
	clear_text_value(6,11);
	clear_text_value(6,22);
	
	clear_text_value(6,12);
	clear_text_value(6,23);
	custom_page.select_num = -1;                    
	custom_page.cur_page = cur_page;
	clear_button_all(0x0006,0x0005,0x0006,0x0007,0x0008);
	for(int i = 0;i < TOTAL_NUM_LINE_PAGE;i++)
	{
		if( 0 != strlen((const char*)user_custom[i + (cur_page*TOTAL_NUM_LINE_PAGE)].resin) )
		{
//			set_text_value(6,9 + i,user_custom[i + (cur_page*TOTAL_NUM_LINE_PAGE)].resin,strlen((const char*)user_custom[i + (cur_page*TOTAL_NUM_LINE_PAGE)].resin));
			SetTextValue(6,9 + i,user_custom[i + (cur_page*TOTAL_NUM_LINE_PAGE)].resin);		
			total_timer = user_custom[i + (cur_page*TOTAL_NUM_LINE_PAGE)].time[0] + user_custom[i + (cur_page*TOTAL_NUM_LINE_PAGE)].time[1] +  user_custom[i + (cur_page*TOTAL_NUM_LINE_PAGE)].time[2] +  \
			user_custom[i + (cur_page*TOTAL_NUM_LINE_PAGE)].time[3];	
			sprintf((char*)show_buf,"Total Time:%dmin",total_timer);  //��ʱ��
			SetTextValue(6,20 + i,show_buf); 
			if((user_custom[i + (cur_page*TOTAL_NUM_LINE_PAGE)].heat[0] > 0 && user_custom[i + (cur_page*TOTAL_NUM_LINE_PAGE)].time[0] >0) || \
			(user_custom[i + (cur_page*TOTAL_NUM_LINE_PAGE)].heat[1] > 0 && user_custom[i + (cur_page*TOTAL_NUM_LINE_PAGE)].time[1] >0) || \
			(user_custom[i + (cur_page*TOTAL_NUM_LINE_PAGE)].heat[2] > 0 && user_custom[i + (cur_page*TOTAL_NUM_LINE_PAGE)].time[2] >0) || \
			(user_custom[i + (cur_page*TOTAL_NUM_LINE_PAGE)].heat[3] > 0 && user_custom[i + (cur_page*TOTAL_NUM_LINE_PAGE)].time[3] >0) )
			{
				switch_icon(0x06,25+i,1);
			}
			else
			{
				switch_icon(0x06,25+i,0xff);
			}
		}
		else
		{
			switch_icon(0x06,16+i,1);
			break;
		}
	}
	memset(show_buf,0x00,sizeof(show_buf));	
	sprintf((char*)show_buf,"%02d",cur_page+1);       
	show_buf[2] = 0x2F;                               
	sprintf((char*)&show_buf[3],"%02d",custom_page.total_page);  
	SetTextValue(6,24,show_buf);         
	set_screen(6);                       
}

void read_custom_info(void)
{
	if( FR_OK == read_user_parameter(USER_FILE,user_custom,sizeof(user_custom)))     
	{
		for(int i = 0;i < MAX_PARA_NUM;i++)
		{
			if(strlen((const char*)user_custom[i].resin) == 0)
			{
				custom_page.total_line = i ;
				break;
			}
			else if(i == MAX_PARA_NUM -1)
			{
				custom_page.total_line = MAX_PARA_NUM;
			}
		}
		custom_page.total_page = custom_page.total_line/TOTAL_NUM_LINE_PAGE + 1;
	}
}



void add_custom_info(void)
{
	read_custom_info();      
	for(int i = 0;i < MAX_PARA_NUM; i++)
	{
		if( strlen((const char*)user_custom[i].resin) == 0 ) 
		{
			custom_page.select_num  = i + 1;       
			break;
		}
		else if(i == MAX_PARA_NUM - 1)
		{
			pop_tips(30,(u8*)TIPS,(u8*)MANY_FILES);
			return;
		}
	}
	for(int i = 3;i < 20;i++)  
		clear_text_value(7,i);	

	set_screen(7);       
}


void custom_input_pro(u16 touch_id,u8 *p,int len)
{
	if(custom_page.select_num < 0 || custom_page.select_num > MAX_PARA_NUM)
		return;
	u8 chars_len = len - 13;
	if( touch_id == 3 )   
	{
		memset(user_custom[custom_page.select_num - 1].resin,0x00,sizeof(user_custom[custom_page.select_num - 1].resin));
		memcpy(user_custom[custom_page.select_num - 1].resin,p+8,chars_len);  
	}
	else if( touch_id >= 4 && touch_id <= 7 )  
	{
		user_custom[custom_page.select_num - 1].up_uv[touch_id - 4] = get_str_num((char*)(p+8),chars_len);
	}
	else if( touch_id >= 8 && touch_id <= 11 ) 
	{
		user_custom[custom_page.select_num - 1].down_uv[touch_id - 8] = get_str_num((char*)(p+8),chars_len);
	}
	else if( touch_id >= 12 && touch_id <= 15 )  
	{
		user_custom[custom_page.select_num - 1].heat[touch_id - 12] = get_str_num((char*)(p+8),chars_len);
	}
	else if( touch_id >= 16 && touch_id <= 19 )  
	{
		user_custom[custom_page.select_num - 1].time[touch_id - 16] = get_str_num((char*)(p+8),chars_len);
	}
}




DSTATUS write_user_parameter(const TCHAR* path,const void *buff,u32 len)
{
	DSTATUS f_sta = 0;

	f_sta = f_open(&f_user, path, FA_CREATE_ALWAYS | FA_WRITE);  
	if(f_sta == FR_OK)
	{
		f_sta = f_write(&f_user,buff,len,&b_write);
	}
	f_close(&f_user);

	return f_sta;
}



DSTATUS read_user_parameter(const TCHAR* path,void *buff,u32 len)
{
	DSTATUS f_stat = 0;

	f_stat = f_open(&f_user,path, FA_READ); 
	if(f_stat == FR_OK)
	{
		f_stat = f_read(&f_user,buff,len,&b_read);
	}
	f_close(&f_user);

	return f_stat;
}


