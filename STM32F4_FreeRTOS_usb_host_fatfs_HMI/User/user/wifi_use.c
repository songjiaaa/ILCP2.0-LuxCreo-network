#include "wifi_use.h"

network_flow_type wifi_flow = {0};

ap_info cur_connected_ap = {0};  //��ǰ���ӵ�AP
ap_info record_ap[SHOW_MAX_WIFI_LIST] = {0};
wifi_page ap_page = {0};

//WiFi��ʼ��
u8 wifi_init_process(void)
{
	u8 ret = 0;
	wifi_flow.execution_step = 0;
	esp8266_hw_reset();             //Ӳ����λģ��
	ret  = esp8266_restore();       //�ָ���������
	ret += esp8266_sw_reset();      //�����λ
	vTaskDelay(500);
	ret += esp8266_at_test();       //ATָ�����		
	ret += esp8266_ate_config(0);   //�رջ��Թ���
	ret += esp8266_set_mode(1);     //stationģʽ
	ret += esp8266_set_auto_connected(0);  //�����ϵ粻�Զ�����	
	return ret;
}

//WiFiģ�����
void wifi_module_run(void)
{
	if(wifi_flow.execution_step == 1)       //WiFi�б�ˢ��
	{
		if(0 == wifi_list_refresh())        //ˢ�³ɹ�
		{
			if(wifi_flow.connect_state == 0)   
				wifi_flow.execution_step = 2;
			else
			{
				wifi_flow.execution_step = 4; 
				wifi_flow.sntp_cal = 1;    //ִ��һ��sntp ʱ��У׼
			}
		}
		else                                //û��ˢ�³ɹ�
		{
			wifi_flow.execution_step = 6;   //WiFiû��ˢ�µ� ����
		}
	}
	else if(wifi_flow.execution_step == 2)
	{
		if( 0 != strlen(record_ap[0].ssid) && 0 != strlen(record_ap[0].password) )  //������λWiFi�����붼������ ִ��һ��WiFi�Զ�����
		{
			wifi_flow.connect_flag = 1;
			ap_page.select_num = 0;
			wifi_flow.execution_step = 3;
			wifi_flow.auto_connect = 1;     //ִ��һ���Զ�����
		}
		else
			wifi_flow.execution_step = 6;		
	}
	else if(wifi_flow.execution_step == 3) 
	{	
		if(wifi_flow.connect_flag == 1) 
		{
			wifi_flow.connect_flag = 0;
			if( 0 == wifi_link_ctrl() )
			{
				if(ui_id.cur_picture == 22)        //����ǰҳ�洦����ʾWiFi�б�ҳ��
				{
					wifi_flow.execution_step = 1;  //ִ��һ���б�ˢ�£������ӵ�WiFi��ʾ������
				}
				else
				{
					wifi_flow.sntp_cal = 1;        //ִ��һ��sntp ʱ��У׼
					wifi_flow.execution_step = 4;
				}
			}
			else
			{
				wifi_flow.execution_step = 6;
			}
		}
	}
	else if(wifi_flow.execution_step == 4)
	{
		rtc_time_calibration();                    //ִ��ʱ��У׼�����͸��ģʽ�������ϴ�����
	}
	else if(wifi_flow.execution_step == 5)         
	{
	}
	else if(wifi_flow.execution_step == 6)         //����
	{
		
	}
}
//wifi�б�ˢ�� ���� 1 û��ˢ�µ�AP
u8 wifi_list_refresh(void)
{
	ap_page.select_num = 0xFF;
	AnimationStart(22,31);       		//�������ˢ��ͼ��

	if(0 == esp8266_query_ap())         //��ѯ����wifi��Ϣ
	{
		extern u8 wifi_pack_buf[];
		parse_ap_name(wifi_pack_buf);
		show_wifi_list(0);
	}
	else
	{
		//û��������wifi
		pop_tips(30,(u8*)TIPS,(u8*)NO_WIFI_);
		memset(record_ap,0x00,sizeof(record_ap));  //WiFi�б����
		wifi_flow.connect_state = 0;   //û��������WiFi�������ӱض�û��
		return 1;
	}
	AnimationPause(22,31);             //��ͣͼ��ˢ��
	return 0;
}

u8 wifi_link_ctrl(void)
{
	if(wifi_flow.connect_state == 0)     //��û������ ���������
	{
		if(ap_page.select_num != 0xFF)       //�Ƿ���ѡ��wifi
		{
			if(wifi_flow.auto_connect == 1)  //�����״��Զ�����
			{
				wifi_flow.auto_connect = 0;

				if( 0 == esp8266_join_ap(record_ap[0].ssid,record_ap[0].password) )
				{
					if(0 == esp8266_get_ip(record_ap[0].ip))
					{		
						wifi_flow.connect_state = 1;                                 //����״̬��λ
						SetTextValue(22,9,(u8*)record_ap[0].ssid);                   //��WiFi�б�ҳ����ʾ����AP��
						SetTextValue(22,20,(u8*)"******");                           //��ʾ���ӵ�����
						SetTextValue(22,21,(u8*)record_ap[0].ip);                    //��ʾIP
						switch_icon(22,30,wifi_flow.connect_state);                  //���½ǰ�ť��ʾ�Ͽ�����

						switch_icon(22,15,1);                                        //��ʾ����ͼ��
						SetTextValue(20,11,(u8*)record_ap[0].ssid);                  //�����ý�����ʾ�����ӵ�AP��
//						switch_icon(0,5,1);                                          //������ͼ�����
						ap_page.select_num = 0xFF;
						return 0;						
					}
					else
					{
						pop_tips(30,(u8*)WIFI_ERROR,(u8*)WIFI_GET_IP_ERR);
					}
				}
				else
				{
					pop_tips(30,(u8*)WIFI_ERROR,(u8*)WIFI_AUTO_CONNECT_FAILED);
				}
				return 1;
			}
			else
			{
				if( 0 == strlen(record_ap[ap_page.select_num].password) )
				{
					set_screen(22);
					pop_tips(30,(u8*)TIPS,(u8*)EN_WIFI_PASSWORD);    //��ʾ����WiFi����
					wifi_flow.connect_flag = 0;
					return 1;
				}
				pop_tips(28,(u8*)TIPS,(u8*)WIFI_CONNECTION_WAIT);                    //����������ʾ
				if( 0 == esp8266_join_ap(record_ap[ap_page.select_num].ssid,record_ap[ap_page.select_num].password) )
				{
					if(0 == esp8266_get_ip(record_ap[ap_page.select_num].ip))
					{		
						cur_connected_ap = record_ap[ap_page.select_num];
						record_ap[ap_page.select_num] = record_ap[0];
						record_ap[0] = cur_connected_ap;
						
						show_wifi_list(0);                                          //��ʾ��0ҳwifi�б�
						wifi_flow.connect_state = 1;                                //����״̬��λ
						SetTextValue(22,21,(u8*)record_ap[0].ip);                   //���ӳɹ�����ʾIP
						switch_icon(22,30,wifi_flow.connect_state);                 //���½ǰ�ť��ʾ�Ͽ�����
						if(ap_page.select_num == 0 && ap_page.cur_page == 0)        //����ǰѡ�к�����Ϊ��0λ
							switch_icon(22,15,2);                                   //��ʾѡ������ͼ��
						else
							switch_icon(22,15,1);                                   //��ʾ����ͼ��
						SetTextValue(20,11,(u8*)record_ap[0].ssid);                 //�����ý�����ʾ�����ӵ�AP��
						
						ap_page.select_num = 0xFF;     //���ѡ��
						
						if(FR_OK != write_user_parameter(WIFI_LINK_SAVE,(const void*)&record_ap[0],sizeof(ap_info)))  //WiFi����ap�����뱣��
						{
							//���� д��ʧ��
							pop_tips(30,(u8*)WARNING,(u8*)FILE_RW_ERR); 
						}
						
						set_screen(22); 
//						switch_icon(0,5,1);                                          //������ͼ�����
						return 0;
					}
					else
					{
						set_screen(22);
						pop_tips(30,(u8*)WIFI_ERROR,(u8*)WIFI_GET_IP_ERR);
					}
				}
				else
				{
					//wifi ���ӳ�ʱ
					set_screen(22);
					pop_tips(30,(u8*)WIFI_ERROR,(u8*)WIFI_CONNECTION_TIMEOUT);
				}
				wifi_flow.connect_flag = 0;
			}
		}
		else
		{
			
			set_screen(22);
			pop_tips(30,(u8*)TIPS,(u8*)SELECT_WIFI);    //��ʾ��ѡ��wifi
			wifi_flow.connect_flag = 0;
		}
		return 1;
	}
	else 
	{
		//�Ͽ�����
		if( 0 != esp8266_at_test() )  //���Ǵ���͸��ģʽ
		{
			esp8266_exit_unvarnished();      //�˳�͸��
		}
		if(0 == esp8266_disconnect_ap())
		{
			memset(&cur_connected_ap,0x00,sizeof(cur_connected_ap));  //��յ�ǰ����wifi����Ϣ
			SetTextValue(22,9,(u8*)cur_connected_ap.ssid);             
			SetTextValue(22,20,(u8*)cur_connected_ap.password);         
			SetTextValue(22,21,(u8*)cur_connected_ap.ip); 
			SetTextValue(20,11,(u8*)cur_connected_ap.ssid);                 //�����ý�����ʾ
			wifi_flow.connect_state = 0;
			ap_page.select_num = 0xFF;
			switch_icon(22,10,0); 
			switch_icon(22,15,0);
			switch_icon(22,30,wifi_flow.connect_state);
			pop_tips(30,(u8*)TIPS,(u8*)WIFI_DISCONNECTED);
			return 2;
		}
	}
	return 1;
}

//����ap��Ϣ
void parse_ap_name(u8 *str)
{
	u32 offse = 0;
	char *ptr = NULL;
	
	memset(record_ap,0x00,sizeof(record_ap));

	for(int i = 1;i < SHOW_MAX_WIFI_LIST;i++)
	{
		ptr = strstr((const char*)(str+offse),"+CWLAP:");
		if(ptr == NULL) break;
		sscanf(ptr,"+CWLAP:(%d,\"%[^\"]\",%d,\"%[^\"]\",%d",&record_ap[i].enc, record_ap[i].ssid, &record_ap[i].rssi, record_ap[i].mac, &record_ap[i].channel);
		while(*ptr++ != '\n')
		{
			offse++;
		}
	}
	
	memset(&cur_connected_ap,0x00,sizeof(cur_connected_ap));
	if( 0 == esp8266_query_connected_ap(cur_connected_ap.ssid,cur_connected_ap.mac) ) //��ѯWiFiģ���Ƿ���������
	{
		if(0 == esp8266_get_ip(cur_connected_ap.ip))                    //��ȡ����wifi��IP
		{

			wifi_flow.connect_state = 1;                                //����״̬��λ
			SetTextValue(22,21,(u8*)cur_connected_ap.ip);               //���ӳɹ�����ʾIP
			switch_icon(22,30,wifi_flow.connect_state);                 //���½ǰ�ť��ʾ�Ͽ�����
			
			SetTextValue(22,9,(u8*)cur_connected_ap.ssid);              //��ʾ���ӵ�wifi��
			SetTextValue(22,20,(u8*)"******");                          //��ʾѡ��wifi������
			SetTextValue(20,11,(u8*)cur_connected_ap.ssid);             //�����ý���ҲҪ��ʾ���ӵ�ap
			switch_icon(22,15,2);                                       //WiFi���ƺ�������ͼ��
			
			//����Ѱ��WiFi�б�����ҵ���ǰ���ӵ�WiFi���������б��һλ
			for(int i = 1;i < SHOW_MAX_WIFI_LIST;i++)
			{
				if( 0 == memcmp(record_ap[i].ssid,cur_connected_ap.ssid,sizeof(cur_connected_ap.ssid)) &&\
					0 == memcmp(record_ap[i].mac,cur_connected_ap.mac,sizeof(cur_connected_ap.mac)) )
				{
					cur_connected_ap = record_ap[0];
					record_ap[0] = record_ap[i];
					record_ap[i] = cur_connected_ap;
					break;
				}				
			}
		}
	}
	else                                        //����WiFiδ����
	{
		if(FR_OK == read_user_parameter(WIFI_LINK_SAVE,(void*)&cur_connected_ap,sizeof(ap_info)))         //��ȡ�����WiFi
		{
			for(int i = 1;i < SHOW_MAX_WIFI_LIST;i++)                                                     //�����Ƿ��ڵ�ǰ��Ѱ��WiFi�б�
			{
				if( 0 == memcmp(record_ap[i].ssid,cur_connected_ap.ssid,sizeof(cur_connected_ap.ssid)) &&\
					0 == memcmp(record_ap[i].mac,cur_connected_ap.mac,sizeof(cur_connected_ap.mac)) )
				{
					//������Ѵ洢�������ӹ���WiFi �����б���λ
					record_ap[0] = cur_connected_ap;
//					cur_connected_ap = record_ap[0];
//					record_ap[0] = record_ap[i];
//					record_ap[i] = cur_connected_ap;
					break;
				}				
			}
		}	
	}
	//ɸѡ����
	remove_duplicates();
}

//ɸѡ��Wi-Fi�ظ���
ap_info wifi_temp_name[SHOW_MAX_WIFI_LIST] = {0};
void remove_duplicates(void)
{
	int inx = 0;
	for(int i = 0;i<SHOW_MAX_WIFI_LIST - 1;i++)
	{
		for(int k = i+1;k<SHOW_MAX_WIFI_LIST;k++)
		{
			if( 0 == strcmp((const char*)record_ap[i].ssid,(const char*)record_ap[k].ssid) )   //�����ظ�
			{
				memset(record_ap[k].ssid,0x00,WIFI_NAME_BUF_SIZE);
			}
		}
	}
	memset(wifi_temp_name,0x00,sizeof(wifi_temp_name));
	for(int x = 0;x<SHOW_MAX_WIFI_LIST;x++)
	{
		if(strlen(record_ap[x].ssid) != 0)
		{
			memcpy(&wifi_temp_name[inx],&record_ap[x],sizeof(ap_info));
			inx ++;
		}
	}
	memset(record_ap,0x00,sizeof(record_ap));
	for(int x = 0;x<SHOW_MAX_WIFI_LIST;x++)
	{
		if(strlen(wifi_temp_name[x].ssid) != 0)
		{
			memcpy(&record_ap[x],&wifi_temp_name[x],sizeof(ap_info));
		}
		else
		{
			ap_page.total_line = x;
			if(ap_page.total_line%SHOW_WIFI_LINE == 0)
				ap_page.total_page = ap_page.total_line/SHOW_WIFI_LINE;
			else
				ap_page.total_page = ap_page.total_line/SHOW_WIFI_LINE + 1;
			break;
		}
	}
}

//���뵱ǰҳ��ʾwifi�б�
void show_wifi_list(u8 cur_page)
{
	u8 show_buf[10] = {0};
	for(int i = 0;i < SHOW_WIFI_LINE;i++)
	{
		SetTextValue(22,4 + i,(u8*)record_ap[i+cur_page*SHOW_WIFI_LINE].ssid);
		switch_icon(22,10 + i,0); 
		switch_icon(22,15 + i,0);
	}
	//������wifi  ������ҳ��һ�� 
	if(wifi_flow.connect_state == 1 && cur_page == 0 )
	{
		switch_icon(22,15,1);                    //���������� 0���� 1������  2ѡ������ 3��û������ 4��ѡ��û������Ч��
	}
	//��ʾҳ��
	memset(show_buf,0x00,sizeof(show_buf));	
	sprintf((char*)show_buf,"%02d",cur_page+1);      
	show_buf[2] = 0x2F;                               
	sprintf((char*)&show_buf[3],"%02d",ap_page.total_page);  
	SetTextValue(22,27,show_buf); 
}


void wifi_list_ctrl(u16 touch_id)
{
	if(touch_id == 1)
	{
		set_screen(20);        //���ص����ý���
	}
	else if(touch_id == 2)     //������Ӱ�ť
	{
		wifi_flow.execution_step = 3;     //ִ��һ�����ӻ�Ͽ�����
		wifi_flow.connect_flag = 1;
	}
	else if(touch_id == 3)      //ˢ�°�ť
	{
		wifi_flow.execution_step = 1;     //����һ��WiFiˢ��
	}
	else if(22 <= touch_id && touch_id <= 26)  //ѡ��wifi��
	{
		ap_page.select_num = ap_page.cur_page*5 + touch_id -22;
		if(ap_page.select_num >= SHOW_MAX_WIFI_LIST) return;
		if( ap_page.select_num < ap_page.total_line )
		{
			SetTextValue(22,9,(u8*)record_ap[ap_page.select_num].ssid);       //��ʾѡ�е�wifi��
			if(0 != strlen(record_ap[ap_page.select_num].password))
				SetTextValue(22,20,(u8*)"******");
			else
				SetTextValue(22,20,(u8*)record_ap[ap_page.select_num].password);  //��ʾѡ��wifi������
			
			memcpy(&cur_connected_ap,&record_ap[ap_page.select_num],sizeof(ap_info));   //���µ�ǰWiFi
			wifi_check_box_icon(touch_id-22);     //��ѡ�е�wifi��ǰ��			
		}
	}
	else if(touch_id == 28)     //��ҳ
	{
		if(ap_page.cur_page > 0)
			ap_page.cur_page--;
		show_wifi_list(ap_page.cur_page);
	}
	else if(touch_id == 29)     //�ҷ�ҳ
	{
		if(ap_page.cur_page + 1 < ap_page.total_page)
			ap_page.cur_page++;
		show_wifi_list(ap_page.cur_page);
	}
}


//wifiҳ������
void wifi_input_pro(u16 touch_id,u8 *p,int len)
{
	u8 chars_len = len - 13;
	if( touch_id == 9 )                     //wifi������  
	{  
//		if(ap_page.select_num == 0xFF)
//		{
//			memset(cur_connected_ap.ssid,0x00,sizeof(cur_connected_ap.ssid));
//			memcpy(cur_connected_ap.ssid,p+8,chars_len);			
//		}
//		else
//		{
//			memset(cur_connected_ap.ssid,0x00,sizeof(cur_connected_ap.ssid));
//			memcpy(cur_connected_ap.ssid,p+8,chars_len);	
//			memset(record_ap[ap_page.select_num].ssid,0x00,sizeof(record_ap[ap_page.select_num].ssid));
//			memcpy(record_ap[ap_page.select_num].ssid,p+8,chars_len);
//		}
	}
	else if( touch_id == 20 )               //wifi��������
	{
		if(ap_page.select_num == 0xFF)      //��û��ѡ���κ�wifi
		{
			memset(cur_connected_ap.password,0x00,sizeof(cur_connected_ap.password));
			memcpy(cur_connected_ap.password,p+8,chars_len);	
		}
		else
		{
			memset(cur_connected_ap.password,0x00,sizeof(cur_connected_ap.password));
			memcpy(cur_connected_ap.password,p+8,chars_len);	
			memset(record_ap[ap_page.select_num].password,0x00,sizeof(record_ap[ap_page.select_num].password));
			memcpy(record_ap[ap_page.select_num].password,p+8,chars_len);
		}
	}
}

//���� ��ѡ�е�IDλ��
void wifi_check_box_icon(u16 id)
{
	for(int i = 0;i<SHOW_WIFI_LINE;i++)
	{
		if(i == id)   //��ǰѡ��ID
		{
			switch_icon(22,10 + i,1);       //��ѡ�е�wifi��ǰ��
			if( wifi_flow.connect_state == 1 && id == 0 && ap_page.cur_page == 0)  //wifi������
			{
				switch_icon(22,15 + i,2);   //��ʾѡ������
			}
			else
			{				
				switch_icon(22,15 + i,4);   //��ʾѡ��δ����
			}
		}
		else       
		{				
			switch_icon(22,10 + i,0); 
			switch_icon(22,15 + i,0);
			if(wifi_flow.connect_state == 1 && ap_page.cur_page == 0 && id != 0)    //����������״̬���һλ��Ҫ��ʾ����ͼ��
				switch_icon(22,15,1);
		}
	}
}


