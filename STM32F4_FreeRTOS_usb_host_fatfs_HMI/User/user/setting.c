#include "setting.h"

pop_type_flag p_flag = {0};

void setting_show(void)
{
	if(wifi_flow.connect_state == 1)
	{
		SetTextValue(20,11,(u8*)record_ap[0].ssid);
	}
	else
	{
		SetTextValue(20,11,(u8*)"Not Connected");
	}
	if(cfg_dft.network_selet == 1)
		SetTextValue(20,12,(u8*)enthernet_flow.ip); 
	else
		SetTextValue(20,12,(u8*)"Not Connected");	
	version_release();      //��ʾ�汾��
	//��ʾSN��
	SetTextValue(21,3,cfg_dft.sn);
	//��ʾ��ʱ��
	uvled_total_time_show();
	//��ʾ����ؿ���ͼ��
	switch_icon(SETTTING_PAGE_ID1,6,cfg_dft.light_strip);
	switch_icon(SETTTING_PAGE_ID1,7,cfg_dft.head_lamp);
}

u8 control_screen_click = 0;
u8 calibration_screen_click = 0;
u8 sn_screen_click = 0;
//���ý����������
void setting_ctrl1(u16 touch_id)
{
	if(touch_id == 1)          //back
	{
		control_screen_click = 0;
		calibration_screen_click = 0;
		sn_screen_click = 0;
		set_screen(0);
	}
	else if(touch_id == 2)     //left
	{
	}
	else if(touch_id == 3)     //right
	{	
		set_screen(21);           //�л����ڶ�ҳ
		uvled_total_time_show();  //��ʾUVʹ��ʱ��
		SetTextValue(21,3,cfg_dft.sn); //��ʾSN
		control_screen_click = 0;
		calibration_screen_click = 0;
		sn_screen_click = 0;
	}
	else if(touch_id == 4)     //�汾λ�ô�
	{
		control_screen_click++;
		if(control_screen_click > 5)
		{
			control_screen_click = 0;
			set_screen(9); 
			vTaskDelay(20);
			SetTextValue(9,2,(u8*)"0");
			SetTextValue(9,3,(u8*)"0");
			SetTextValue(9,4,(u8*)"0");
		}
	}
	else if(touch_id == 5)     //У׼���洦
	{
		calibration_screen_click++;
		if(calibration_screen_click > 5)
		{
			calibration_screen_click = 0;
			pop_tips(29,(u8*)TIPS,(u8*)INPUT_PASSWORD);  // ��ʾ��������
			memset(&p_flag,0x00,sizeof(p_flag));
			p_flag.cal_set = 1;	
			rgb_light_state = 4;    //�ȵ���˸�澯			
		}	
	}
	else if(touch_id == 9)
	{
		cfg_dft.light_strip ^= 1;
		switch_icon(SETTTING_PAGE_ID1,6,cfg_dft.light_strip);
		RGBS_PWR_EN = cfg_dft.light_strip;
	}
	else if(touch_id == 10)
	{
		cfg_dft.head_lamp ^= 1;
		switch_icon(SETTTING_PAGE_ID1,7,cfg_dft.head_lamp);
		WLED_PWR_EN = cfg_dft.head_lamp;
	}
	else if(touch_id == 16)    //wifi  input
	{
		extern wifi_page ap_page;
		set_screen(22);        //��ת��wifi����
		
		ap_page.select_num = 0xFF;
		if( 0 != esp8266_at_test() )  //���Ѿ���͸��ģʽ��
		{
			esp8266_exit_unvarnished();    //�˳�͸��
		}
		wifi_flow.execution_step = 1;        //����һ��wifi�б�ˢ��
		AnimationStart(22,31); 
	}
	else if(touch_id == 17)    //upgrade
	{
		set_screen(32);
		rgb_light_state = 4;    //�ȵ���˸�澯
	}		
}


//���ý���2�������
void setting_ctrl2(u16 touch_id)
{
	if(touch_id == 1)          
	{
	}
	else if(touch_id == 4)     
	{
		sn_screen_click++;
		if(sn_screen_click > 5)
		{
			sn_screen_click = 0;
			set_screen(23); 
			SetTextValue(23,13,cfg_dft.sn);
		}
	}
	else if(touch_id == 5)     //����
	{	
		control_screen_click = 0;
		calibration_screen_click = 0;
		sn_screen_click = 0;
		set_screen(0);
	}
	else if(touch_id == 6)     
	{
		setting_show();
		control_screen_click = 0;
		calibration_screen_click = 0;
		sn_screen_click = 0;
	}
	else if(touch_id == 7)     
	{
		
	}
}

void control_input_pro(u16 touch_id,u8 *p,int len)
{
	int x = 0;
	u8 chars_len = len - 13;
	if( touch_id == 2 )        //�Ϲ�Դ
	{
		x = get_str_num((char*)(p+8),chars_len);
		if(x == 0)
		{
			UV_POWER_PER1 = 0;
			UV_FAN1_PWR_EN = 0;  			
		}
		else
		{
			UV_POWER_PER1 = save_config.uv_power_max * save_config.uv_cal_k * save_config.up_uv_k * x * 10 ;
			UV_FAN1_PWR_EN = 1;  			
		}
	}
	else if( touch_id == 3 )   //�¹�Դ
	{
		x = get_str_num((char*)(p+8),chars_len);
		if(x == 0)
		{
			UV_POWER_PER2 = 0;
			UV_FAN2_PWR_EN = 0; 			
		}
		else
		{
			UV_POWER_PER2 = save_config.uv_power_max * save_config.uv_cal_k * save_config.down_uv_k * x * 10;
			UV_FAN2_PWR_EN = 1; 
		}			
	}
	else if( touch_id == 4 )  //����
	{
		x = get_str_num((char*)(p+8),chars_len);
		if(x == 0)
		{
			cfg_dft.heat_sw = 0;   
			cfg_dft.heat_temp = -300;
		}
		else
		{
			cfg_dft.heat_sw = 1;   
			cfg_dft.heat_temp = x;
		}
	}
}


//������������
void eject_input_pro(u8 *p,int len)
{
	u8 key_buf[255] = {0};
	u8 chars_len = len - 7;
	memset(key_buf,0x00,sizeof(key_buf));
	memcpy(key_buf,p+3,chars_len);  
	if(p_flag.eject_key == 1)  //sn����
	{
		if( 0 == strcmp((const char*)"LuxCreo",(const char*)key_buf) )  //������ȷ
		{
			p_flag.eject_key = 2;
			pop_tips(29,(u8*)TIPS,(u8*)ENTER_SN);
			return;
		}
		else
		{
			rgb_light_state = 4;    //�ȵ���˸�澯
			pop_tips(30,(u8*)WARNING,(u8*)PASSWORD_ERROR); 
			return;
		}		
	}
	else if(p_flag.eject_key == 2)   //�µ�SN��
	{
		if(FR_OK != write_user_parameter(SN_FILE,(const void*)key_buf,30))
		{
			//���� д��SN����
			pop_tips(30,(u8*)WARNING,(u8*)WRITE_SN_ERR); 
			rgb_light_state = 4;    //�ȵ���˸�澯
			return;
		}
		else
		{
			if(FR_OK != read_user_parameter(SN_FILE,(void*)cfg_dft.sn,sizeof(cfg_dft.sn)))
			{
				//��ȡSN������ʾ
				pop_tips(30,(u8*)WARNING,(u8*)READ_SN_ERR); 
				rgb_light_state = 4;    //�ȵ���˸�澯
			}
			else
			{
				set_screen(23);
				SetTextValue(23,13,cfg_dft.sn);
				SetTextValue(21,3,cfg_dft.sn);
			}	
			return;
		}
	}

	if(p_flag.total_time == 1)
	{
		if( 0 == strcmp((const char*)"LuxCreo",(const char*)key_buf) )  //������ȷ
		{
			save_time.uv_total_time = 0;
			
			if(FR_OK == write_user_parameter(TT_FILE,(const void*)&save_time,sizeof(save_time)))
			{
				pop_tips(27,(u8*)TIPS,(u8*)CLEAR_TOTAL_TIME);  //����ɹ�
				uvled_total_time_show();
			}
			else
			{
				pop_tips(27,(u8*)WARNING,(u8*)FILE_RW_ERR);    //���� �ļ���д�쳣
				rgb_light_state = 4;    //�ȵ���˸�澯
			}
	
			return;
		}
		else
		{
			pop_tips(30,(u8*)WARNING,(u8*)PASSWORD_ERROR); 
			rgb_light_state = 4;    //�ȵ���˸�澯
			return;
		}
	}
	
	if(p_flag.decive_para == 1)
	{
		if( 0 == strcmp((const char*)"LuxCreo",(const char*)key_buf) )  //������ȷ
		{
			DSTATUS stat = 0;
			FILINFO fno = {0};
			memset(cure_parameter,0x00,sizeof(cure_parameter));
			stat = f_stat(DESTIN_CURE_FILE, &fno);
			if(FR_OK == stat)       //����ļ��Ƿ����
			{
				if(FR_OK == f_unlink(DESTIN_CURE_FILE)) 
				{
					pop_tips(27,(u8*)TIPS,(u8*)LOCAL_PARA_CLEARED);   // ɾ���ɹ�
				}	
				else
				{
					pop_tips(27,(u8*)WARNING,(u8*)FILE_DELETE_ERR);   //�ļ�ɾ���쳣
				}			
			}
			else
			{
				pop_tips(27,(u8*)TIPS,(u8*)NO_LOCAL_PARA);      //���ز���������
			}
			return;
		}
		else
		{
			pop_tips(30,(u8*)WARNING,(u8*)PASSWORD_ERROR);
			rgb_light_state = 4;    //�ȵ���˸�澯			
			return;
		}
	}
	if(p_flag.u_disk == 1)
	{
		if( 0 == strcmp((const char*)"LuxCreo",(const char*)key_buf) )  //������ȷ
		{
			FRESULT res = FR_OK;

			if(cfg_dft.u_disk_state == 99)             
			{
				res = my_copyfile(DESTIN_CURE_FILE,SOURCE_CURE_FILE);
				if(FR_OK == res)
					pop_tips(30,(u8*)HMI_SUCCEEDED,(u8*)FILE_IM_SUCCEEDED); 
				else

					pop_tips(30,(u8*)HMI_FAILED,(u8*)f_result[res]); 
			}
			else
			{
				pop_tips(30,(u8*)WARNING,(u8*)NO_USB_DISK);
				rgb_light_state = 4;    //�ȵ���˸�澯				
			}	
			return;			
		}
		else
		{
			pop_tips(30,(u8*)WARNING,(u8*)PASSWORD_ERROR); 
			rgb_light_state = 4;    //�ȵ���˸�澯
			return;
		}

	}
	if(p_flag.set_ac == 1)
	{
		if( 0 == strcmp((const char*)"LuxCreo",(const char*)key_buf) )  //������ȷ
		{
			save_config.set_ac ^= 1;
			switch_icon(24,6,save_config.set_ac);
			cfg_save();
			if(save_config.set_ac == 0)
				pop_tips(27,(u8*)TIPS,(u8*)SET_PTC_110V); 
			else
				pop_tips(27,(u8*)TIPS,(u8*)SET_PTC_220V); 
		}
		else
		{
			pop_tips(30,(u8*)WARNING,(u8*)PASSWORD_ERROR); 
			return;
		}
	}
	if(p_flag.cal_set == 1)
	{
		if( 0 == strcmp((const char*)"LuxCreo",(const char*)key_buf) )  //������ȷ
		{
		    char show_buf[10];
			set_screen(10);
			vTaskDelay(20);
			memset(show_buf,0x00,sizeof(show_buf));
			sprintf(show_buf,"%d",(int)(save_config.uv_cal_k*100));
			SetTextValue(10,5,(u8*)show_buf);
			
			memset(show_buf,0x00,sizeof(show_buf));
			sprintf(show_buf,"%d",(int)(save_config.up_uv_k*100));
			SetTextValue(10,6,(u8*)show_buf);
			
			memset(show_buf,0x00,sizeof(show_buf));
			sprintf(show_buf,"%d",(int)(save_config.down_uv_k*100));
			SetTextValue(10,7,(u8*)show_buf);

			memset(show_buf,0x00,sizeof(show_buf));
			sprintf(show_buf,"%d",(int)(save_config.heat_cal));
			SetTextValue(10,10,(u8*)show_buf);
			
			memset(show_buf,0x00,sizeof(show_buf));
			sprintf(show_buf,"%d",(int)(save_config.uv_heat_cal));
			SetTextValue(10,11,(u8*)show_buf);
		}
		else
		{
			pop_tips(30,(u8*)WARNING,(u8*)PASSWORD_ERROR); 
			return;
		}
	}
}





void cal_input_pro(u16 touch_id,u8 *p,int len)
{
	int x = 0;
	u8 chars_len = len - 13;
	if( touch_id == 5 ) 
	{
		x = get_str_num((char*)(p+8),chars_len);
		save_config.uv_cal_k = x/100.0f; 
	}
	else if( touch_id == 6 ) 
	{
		x = get_str_num((char*)(p+8),chars_len);
		save_config.up_uv_k = x/100.0f; 
	}
	else if( touch_id == 7 ) 
	{
		x = get_str_num((char*)(p+8),chars_len);
		save_config.down_uv_k = x/100.0f;
	}
	else if( touch_id == 10 )
	{
		x = get_str_num((char*)(p+8),chars_len);
		save_config.heat_cal = x/100.0f;
	}
	else if( touch_id == 11 )
	{
		x = get_str_num((char*)(p+8),chars_len);
		save_config.uv_heat_cal = x/100.0f;
	}
}


void uvled_total_time_show(void)
{
	char hour[30] = {0};
	int n = 0;
	u32 hours = 0,minute = 0;
	hours = (save_time.uv_total_time / 60.0f) / 60.0f;
	minute = (save_time.uv_total_time / 60) % 60;  
	
	memset(hour,0x00,sizeof(hour));

	n = sprintf(hour,"%dh",hours);
	
	n = sprintf(&hour[n]," %dmin",minute);
	SetTextValue(SETTTING_PAGE_ID2,2,(u8*)hour);  
}


//���ؽ��� 23
void hidden_function_screen(u16 touch_id)
{
	if( touch_id == 1 )         //���ص����ý��� 
	{
		set_screen(21);
		memset(&p_flag,0x00,sizeof(pop_type_flag));
	}	
	else if( touch_id == 7 )    //�ҷ�
	{
		set_screen(24);
		switch_icon(24,6,save_config.set_ac); //��ʾ��ѹ����
	}
	else if( touch_id == 9 )    //����SNҳ��
	{
		pop_tips(29,(u8*)TIPS,(u8*)INPUT_PASSWORD);  // ��ʾ��������
		memset(&p_flag,0x00,sizeof(p_flag));
		p_flag.eject_key = 1;
		rgb_light_state = 4;    //�ȵ���˸�澯
	}
	else if( touch_id == 10 )   //�̻���ʱ������
	{
		pop_tips(29,(u8*)TIPS,(u8*)INPUT_PASSWORD);  // ��ʾ��������
		memset(&p_flag,0x00,sizeof(p_flag));
		p_flag.total_time = 1;
		rgb_light_state = 4;    //�ȵ���˸�澯
	}
	else if( touch_id == 11 )   //�û��������
	{
		pop_tips(29,(u8*)DELETE,(u8*)USER_PARA_DELETE); 
		memset(&p_flag,0x00,sizeof(p_flag));
		p_flag.custom_para = 1; 
		rgb_light_state = 4;    //�ȵ���˸�澯
	}
	else if( touch_id == 12 )   //�����̻��������
	{
		pop_tips(29,(u8*)TIPS,(u8*)INPUT_PASSWORD);  // ��ʾ��������
		memset(&p_flag,0x00,sizeof(p_flag));
		p_flag.decive_para = 1;
		rgb_light_state = 4;    //�ȵ���˸�澯
	}
}


void hidden_function_screen_two(u16 touch_id)
{
	if( touch_id == 2 )        //���ص����ý��� 
	{
		set_screen(21);
		memset(&p_flag,0x00,sizeof(pop_type_flag));
	}
	else if( touch_id == 3 )   //��
	{
		set_screen(23);
		SetTextValue(23,13,cfg_dft.sn);
	}
	else if( touch_id == 5 )   //U�̵���̻�����
	{
		pop_tips(29,(u8*)TIPS,(u8*)INPUT_PASSWORD);  // ��ʾ��������
		memset(&p_flag,0x00,sizeof(p_flag));
		p_flag.u_disk = 1;
		rgb_light_state = 4;    //�ȵ���˸�澯		
	}
	else if( touch_id == 7 )         //����ʹ�õ�ѹ
	{
		pop_tips(29,(u8*)TIPS,(u8*)INPUT_PASSWORD);  // ��ʾ��������
		memset(&p_flag,0x00,sizeof(p_flag));
		p_flag.set_ac = 1;
		rgb_light_state = 4;    //�ȵ���˸�澯		
	}
}





