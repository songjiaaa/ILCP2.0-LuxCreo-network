#include "cure.h"

page_record cure_page = {0,0,0,-1,-1};
cure_para cure_parameter[MAX_PARA_NUM] = {0},exe_parameter = {0};
curing_pro cure = {0};
 
void cure_select(u16 id)
{
	if(id == 0x0001)       
	{
		set_screen(ui_id.pre_picture);
	}
	else if(id == 0x0002)  
	{
		if(cure_page.cur_page > 0)
		{	
			cure_page.cur_page--;
			show_cur_cure_page(cure_page.cur_page);
		}
	}
	else if(id == 0x0003)  
	{
		if(cure_page.cur_page + 1 < cure_page.total_page)
		{	
			cure_page.cur_page++;
			show_cur_cure_page(cure_page.cur_page);
		}
	}
	else if(id == 0x0004)    //启动固化
	{
		if(cure_page.select_num != -1)        //有选中  
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
		cure_page.select_line = id - 4;
		cure_page.select_num = cure_page.cur_page * TOTAL_NUM_LINE_PAGE +  cure_page.select_line;
		
		if(cure_page.select_num <= cure_page.total_line)
		{
			exe_parameter = cure_parameter[cure_page.select_num - 1];
			switch_icon(0x05,23,1);  
		
			if(id == 0x0005)
				clear_button_value(0x0005,0x0006,0x0007,0x0008);
			else if(id == 0x0006)
				clear_button_value(0x0005,0x0005,0x0007,0x0008);	
			else if(id == 0x0007)
				clear_button_value(0x0005,0x0006,0x0006,0x0008);	
			else if(id == 0x0008)
				clear_button_value(0x0005,0x0005,0x0006,0x0007);
		}
		else
		{
			switch_icon(0x05,23,0);  
			clear_button_all(0x0005,0x0005,0x0006,0x0007,0x0008);
		}
	}
	else if(id == 24)
	{
		if(cure_page.select_num != -1 )    
		{
			show_para(15,1,&exe_parameter);   
			set_screen(15);
		}
	}
}



void cure_page_info(u16 scene)
{
	get_cure_file(scene);    

	if( cure_page.total_line % TOTAL_NUM_LINE_PAGE == 0)
		cure_page.total_page = cure_page.total_line / TOTAL_NUM_LINE_PAGE;
	else
		cure_page.total_page = cure_page.total_line/TOTAL_NUM_LINE_PAGE + 1;
}


void show_cur_cure_page(u16 cur_page)
{
	u32 total_timer = 0;
	u8 show_buf[10] = {0};
	cure_page.select_num = -1;                    
	cure_page.cur_page = cur_page;
	clear_button_all(0x0005,0x0005,0x0006,0x0007,0x0008);  
	for(int i = 0;i < TOTAL_NUM_LINE_PAGE;i++)    
	{
		clear_text_value(5,10 + i);        
		clear_text_value(5,14 + i);
		switch_icon(0x05,19+i,0);
		if( 0 != strlen((const char*)cure_parameter[i + (cur_page*TOTAL_NUM_LINE_PAGE)].resin) ) 
		{
			set_text_value(5,10 + i,cure_parameter[i + (cur_page*TOTAL_NUM_LINE_PAGE)].resin,strlen((const char*)cure_parameter[i + (cur_page*TOTAL_NUM_LINE_PAGE)].resin));

			total_timer = cure_parameter[i + (cur_page*TOTAL_NUM_LINE_PAGE)].time[0] + cure_parameter[i + (cur_page*TOTAL_NUM_LINE_PAGE)].time[1] +  cure_parameter[i + (cur_page*TOTAL_NUM_LINE_PAGE)].time[2] +  \
			cure_parameter[i + (cur_page*TOTAL_NUM_LINE_PAGE)].time[3] + cure_parameter[i + (cur_page*TOTAL_NUM_LINE_PAGE)].time[4];	
			sprintf((char*)show_buf,"Total Time:%dmin",total_timer);  
			SetTextValue(5,14 + i,show_buf); 
			if((cure_parameter[i + (cur_page*TOTAL_NUM_LINE_PAGE)].heat[0] > 0 && cure_parameter[i + (cur_page*TOTAL_NUM_LINE_PAGE)].time[0] >0) || \
			   (cure_parameter[i + (cur_page*TOTAL_NUM_LINE_PAGE)].heat[1] > 0 && cure_parameter[i + (cur_page*TOTAL_NUM_LINE_PAGE)].time[1] >0) || \
			   (cure_parameter[i + (cur_page*TOTAL_NUM_LINE_PAGE)].heat[2] > 0 && cure_parameter[i + (cur_page*TOTAL_NUM_LINE_PAGE)].time[2] >0) || \
			   (cure_parameter[i + (cur_page*TOTAL_NUM_LINE_PAGE)].heat[3] > 0 && cure_parameter[i + (cur_page*TOTAL_NUM_LINE_PAGE)].time[3] >0) )
			{
				switch_icon(0x05,19+i,1);
			}
			else
			{
				switch_icon(0x05,19+i,0xff);
			}
		}
	}

	memset(show_buf,0x00,sizeof(show_buf));	
	sprintf((char*)show_buf,"%02d",cur_page+1);      
	show_buf[2] = 0x2F;                               
	sprintf((char*)&show_buf[3],"%02d",cure_page.total_page);  
	SetTextValue(5,18,show_buf);        
}


TCHAR read_buf[256] = {0};
int get_str_num(char *buf,unsigned char len)
{
	char num_buf[256] =  {0};
	memcpy(num_buf,buf,len);
	return atoi(num_buf);
}



void get_cure_file(u16 scene)
{
	char *scene_p = NULL;
	DSTATUS stat = 0;
	char scene_str_buf[10] = {0}; 

	stat = f_open(&fdst,CURE_FILE, FA_OPEN_EXISTING | FA_READ);
	if(FR_OK == stat)	
	{
		memset(cure_parameter,0x00,sizeof(cure_parameter));
		memset(read_buf,0x00,sizeof(read_buf));
		
		sprintf(scene_str_buf,"SCENE%d",scene);
		for(int i = 0; i < MAX_FILE_LINE; i++)              
		{
			if( NULL != f_gets(read_buf,sizeof(read_buf),&fdst) )
			{
				scene_p = strstr((char const *)read_buf, (char const *)scene_str_buf); 
				if(scene_p != NULL)
				{
					break;
				}
			}
		}
		analytical_curing_parameter();
	}		
}



void analytical_curing_parameter(void)
{
	char *resin = NULL, *step = NULL, *para = NULL;
	u32 parameter_number = 0,len = 0;

	for(int i = 0; i < MAX_FILE_LINE; i++)      
	{
		if( NULL != f_gets(read_buf,sizeof(read_buf),&fdst) )
		{
			if(read_buf[0] != 0x0D && read_buf[1] != 0x0A)
			{
				resin = strstr((char const *)read_buf, (char const *)RESIN);      
				if(resin != NULL)
				{
					if(parameter_number >= MAX_PARA_NUM) return;
					
					resin = resin+sizeof(RESIN)-1;
					
					while(*resin != '\0' && *resin == ' ')resin++;
					
					for(int i = 0;i<30;i++)
					{
						if( ' ' == *(resin+i) && ' ' == *(resin+i+1) )
						{
							len = i;
							break;
						}
						if(0x0D == *(resin+i) || 0x0A == *(resin+i))
						{
							len = i;
							break;
						}
					}
					MINMAX(len,1,30);
					memcpy(cure_parameter[parameter_number].resin,resin,len);    
					
					parameter_number++;
					cure_page.total_line = parameter_number;  
				}
				else
				{
					step = strstr((char const *)read_buf, (char const *)STEP);       
					para = strstr((char const *)read_buf, (char const *)UP_UV);
					if(step != NULL && para != NULL)
					{
						int step_num = 0;
						step_num = get_str_num(step+sizeof(STEP)-1,para - step - sizeof(STEP) + 1);
						if(step_num > 0) step_num -= 1;   
						
						para = strstr((char const *)read_buf, (char const *)UP_UV);
						if(para != NULL)
						{
							cure_parameter[parameter_number - 1].up_uv[step_num] = get_str_num(para+sizeof(UP_UV)-1,10);
						}
						else
						{						
						}					
						para = strstr((char const *)read_buf, (char const *)DOWN_UV);
						if(para != NULL)
						{
							cure_parameter[parameter_number - 1].down_uv[step_num] = get_str_num(para+sizeof(DOWN_UV)-1,7);
						}
						else
						{
						}
						para = strstr((char const *)read_buf, (char const *)HEAT);
						if(para != NULL)
						{
							cure_parameter[parameter_number - 1].heat[step_num] = get_str_num(para+sizeof(HEAT)-1,7);
						}
						else
						{							
						}						
						para = strstr((char const *)read_buf, (char const *)TIME);
						if(para != NULL)
						{
							cure_parameter[parameter_number - 1].time[step_num] = get_str_num(para+sizeof(TIME)-1,7);
						}
					}
					else
					{
						f_close(&fdst);              
						break;
					}
			    }
			}
		}
	}
}
	

int cure_start(void)
{
	if(DOOR_DET == 1)    
	{
		//门开告警
		pop_tips(30,(u8*)WARNING,(u8*)CLOSE_DOOR);
		return 1;
	}
	else
	{
		cure.mode = 1;               //固化模式
		cure.step = 0;               //步数清零
		cure.step_cnt_time = 0;	         //当前步骤时间
		cure.exp_temp = exe_parameter.heat[cure.step];     //期望温度

		cure.total_time = ( exe_parameter.time[0] + exe_parameter.time[1] + exe_parameter.time[2] + exe_parameter.time[3] + exe_parameter.time[4] ) * 60;	  //当前剩余时间，转换为秒
		cure.r_time = cure.total_time;    //总时间，转换为秒
		cfg_dft.heat_sw = 1;              //加热打开
		set_screen(12);                   //跳转固化中界面
		cure_show_text();
		return 0;
	}	
}

extern page_record custom_page;
int cure_stop(void)
{
	u8 show_text[20] = {0};
	curing_pause();
	cure_page.select_num = -1;
	custom_page.select_num = -1;
	memset(&cure,0x00,sizeof(cure));
	memset(&exe_parameter,0x00,sizeof(exe_parameter));
	set_screen(14);    //切换到固化结束页，显示固化进度
	show_hour_min_sec(14,2,cure.total_time-cure.r_time);  //显示使用时间
	
	sprintf((char*)show_text,"%d%%",(cure.total_time - cure.r_time)*100/cure.total_time); 
	SetTextValue(14,3,show_text); //显示进度
	//存储一次固化时间
	if(FR_OK != write_user_parameter(SET_FILE,(const void*)&save_set,sizeof(file_save_config)))
		return 1;
	return 0;
}

//
void curing_pause(void)
{
	UV_POWER_PER1 = 0;      //上灯板关闭
	UV_POWER_PER2 = 0;      //下灯板关闭
	UV_FAN1_PWR_EN = 0;         //上光源风扇关闭
	UV_FAN2_PWR_EN = 0;         //下光源风扇关闭
	motor_ctrl_on(0);       //电机关闭
	OFF_TIMER;
	cfg_dft.heat_sw = 0;    //加热关闭
}


void curing_continue(void)
{
	UV_POWER_PER1 = save_config.uv_power_max * save_config.uv_cal_k * save_config.up_uv_k * exe_parameter.up_uv[cure.step] * 10 ;
	UV_POWER_PER2 = save_config.uv_power_max * save_config.uv_cal_k * save_config.down_uv_k * exe_parameter.down_uv[cure.step] * 10;
	UV_FAN1_PWR_EN = 1;         //上光源风扇关闭
	UV_FAN2_PWR_EN = 1;         //下光源风扇关闭
	motor_ctrl_on(1);           //电机旋转
	ON_TIMER;                   //计数开始
	cfg_dft.heat_sw = 1;        //加热打开
}

//按100hz运行
void cure_state_process(void)
{
	static u32 tick = 0;
	tick ++;
	if(cure.mode == 0)       //视为空闲状态
	{
		temperature_control(cfg_dft.heat_temp,cfg_dft.heat_sw);    //空闲时单独开加热
	}
	else if(cure.mode == 1)
	{
		if(DOOR_DET == 1)    //开门
		{
			curing_pause();    //固化暂停
			//开门告警
			pop_tips(30,(u8*)WARNING,(u8*)CLOSE_DOOR);
			cure.mode = 2;
		}
		else
		{
			curing_continue();  //固化继续
			//灯条常亮
		}
		if(tick % 50 == 0)
		{
			//刷新剩余时间
			show_hour_min_sec(12,4,cure.r_time);
		}
		//加热控制
		temperature_control(cure.exp_temp,cfg_dft.heat_sw);
	}
	else if(cure.mode == 2)  //固化暂停中
	{
		if(DOOR_DET == 1)
		{
			pop_tips(30,(u8*)WARNING,(u8*)CLOSE_DOOR);
		}
		else
		{
		
		}
	}
}

//参数：页面id 控件id  秒数    功能:按小时分钟秒 显示
void show_hour_min_sec(u16 screen_id,u16 control_id,u32 seconds)
{
	int n = 0;
	u8 show[50] = {0}; 
	u32 h = 0,m = 0,s = 0,sec = seconds;
	h = sec/3600;
	sec %= 3600;
	m = sec/60;
	s = sec%60;

	n = sprintf((char*)show," %02d:",h);
	n += sprintf((char*)show+n,"%02d:",m);
	sprintf((char*)show+n,"%02d",s);
	SetTextValue(screen_id,control_id,show); 
}

void cure_show_text(void)
{
	int n = 0;
	u8 show[50] = {0}; 
	set_text_value(12,3,exe_parameter.resin,strlen((const char*)exe_parameter.resin));    //显示材料名
	sprintf((char*)show,"%02d",cure.step+1);     
	SetTextValue(12,5,show);                   //显示步骤
	sprintf((char*)show,"%d%%",exe_parameter.up_uv[cure.step]);
	SetTextValue(12,6,show);                   //显示上光强
	sprintf((char*)show,"%d%%",exe_parameter.down_uv[cure.step]);
	SetTextValue(12,7,show);	               //显示下光强
	if(exe_parameter.heat[cure.step] == 0)
		SetTextValue(12,8,(unsigned char*)"RT");	
	else
	{
		n = sprintf((char*)show,"%d",exe_parameter.heat[cure.step]); 
		show[n] = 0xA1;
		show[n+1] = 0xE6;	
		SetTextValue(12,8,show);
	}
}

//温度控制，参数: 期望温度  是否控制
int temperature_control(s16 exp_temp,u8 sta)
{
	static s16 heat_temp = 0,heat_per = 0;
	if(sta == 1 && DOOR_DET == 0)   //期望控温且门关闭
	{
		PTC_FAN1_EN = 1;                   //进入加热状态 加热风扇保持常开
		if(UV_POWER_PER1 == 0 && UV_POWER_PER2 == 0 && exp_temp > 40)
			heat_temp = heat_temp + save_config.heat_cal;
		else if(UV_POWER_PER1 > 300 && UV_POWER_PER2 > 300 && exp_temp > 40)
			heat_temp = heat_temp + save_config.uv_heat_cal;
	
		if( data_c.inside_t < exp_temp )
		{
			//若默认安装为110v PTC   
			if(data_c.ac_v > 0.5f && data_c.ac_v < 1.5f)      //输入交流电压为110v
				heat_per = 1000;
			else if(data_c.ac_v > 1.5f && data_c.ac_v < 2.5f) //输入交流电压为220v
			{
				heat_per = 0;
				return 1;
			}
			if(data_c.ptc_t > 150) HEAT_POWER_PER = 0;
			else HEAT_POWER_PER = heat_per;
			PTC_FAN2_EN = 0;               //加热时散热风扇保持关闭
		}
		else
		{
			if(data_c.inside_t >= exp_temp+1)
				PTC_FAN2_EN = 1;           //高于期望温度1摄氏度 散热打开 
		}
	}
	else
	{
		HEAT_POWER_PER = 0;                //加热保持关闭 
		PTC_FAN1_EN = 0;                   //加热风扇保持关闭
	}
	return 0;
}


