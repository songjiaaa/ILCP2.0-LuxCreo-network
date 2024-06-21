#include "wifi_use.h"

network_flow_type wifi_flow = {0};

ap_info cur_connected_ap = {0};  //当前连接的AP
ap_info record_ap[SHOW_MAX_WIFI_LIST] = {0};
wifi_page ap_page = {0};

//WiFi初始化
u8 wifi_init_process(void)
{
	u8 ret = 0;
	wifi_flow.execution_step = 0;
	esp8266_hw_reset();             //硬件复位模块
	ret  = esp8266_restore();       //恢复出厂设置
	ret += esp8266_sw_reset();      //软件复位
	vTaskDelay(500);
	ret += esp8266_at_test();       //AT指令测试		
	ret += esp8266_ate_config(0);   //关闭回显功能
	ret += esp8266_set_mode(1);     //station模式
	ret += esp8266_set_auto_connected(0);  //设置上电不自动连接	
	return ret;
}

//WiFi模块控制
void wifi_module_run(void)
{
	if(wifi_flow.execution_step == 1)       //WiFi列表刷新
	{
		if(0 == wifi_list_refresh())        //刷新成功
		{
			if(wifi_flow.connect_state == 0)   
				wifi_flow.execution_step = 2;
			else
			{
				wifi_flow.execution_step = 4; 
				wifi_flow.sntp_cal = 1;    //执行一次sntp 时间校准
			}
		}
		else                                //没有刷新成功
		{
			wifi_flow.execution_step = 6;   //WiFi没有刷新到 空闲
		}
	}
	else if(wifi_flow.execution_step == 2)
	{
		if( 0 != strlen(record_ap[0].ssid) && 0 != strlen(record_ap[0].password) )  //若是首位WiFi和密码都存在则 执行一次WiFi自动连接
		{
			wifi_flow.connect_flag = 1;
			ap_page.select_num = 0;
			wifi_flow.execution_step = 3;
			wifi_flow.auto_connect = 1;     //执行一次自动连接
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
				if(ui_id.cur_picture == 22)        //若当前页面处于显示WiFi列表页面
				{
					wifi_flow.execution_step = 1;  //执行一次列表刷新，把连接的WiFi显示在首行
				}
				else
				{
					wifi_flow.sntp_cal = 1;        //执行一次sntp 时间校准
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
		rtc_time_calibration();                    //执行时间校准后进入透传模式并周期上传数据
	}
	else if(wifi_flow.execution_step == 5)         
	{
	}
	else if(wifi_flow.execution_step == 6)         //空闲
	{
		
	}
}
//wifi列表刷新 返回 1 没有刷新到AP
u8 wifi_list_refresh(void)
{
	ap_page.select_num = 0xFF;
	AnimationStart(22,31);       		//网络界面刷新图标

	if(0 == esp8266_query_ap())         //查询可用wifi信息
	{
		extern u8 wifi_pack_buf[];
		parse_ap_name(wifi_pack_buf);
		show_wifi_list(0);
	}
	else
	{
		//没有搜索到wifi
		pop_tips(30,(u8*)TIPS,(u8*)NO_WIFI_);
		memset(record_ap,0x00,sizeof(record_ap));  //WiFi列表清空
		wifi_flow.connect_state = 0;   //没有搜索到WiFi，则连接必定没有
		return 1;
	}
	AnimationPause(22,31);             //暂停图标刷新
	return 0;
}

u8 wifi_link_ctrl(void)
{
	if(wifi_flow.connect_state == 0)     //若没有连接 则进入连接
	{
		if(ap_page.select_num != 0xFF)       //是否有选中wifi
		{
			if(wifi_flow.auto_connect == 1)  //若是首次自动连接
			{
				wifi_flow.auto_connect = 0;

				if( 0 == esp8266_join_ap(record_ap[0].ssid,record_ap[0].password) )
				{
					if(0 == esp8266_get_ip(record_ap[0].ip))
					{		
						wifi_flow.connect_state = 1;                                 //连接状态置位
						SetTextValue(22,9,(u8*)record_ap[0].ssid);                   //在WiFi列表页面显示连接AP名
						SetTextValue(22,20,(u8*)"******");                           //显示连接的密码
						SetTextValue(22,21,(u8*)record_ap[0].ip);                    //显示IP
						switch_icon(22,30,wifi_flow.connect_state);                  //右下角按钮显示断开连接

						switch_icon(22,15,1);                                        //显示连接图标
						SetTextValue(20,11,(u8*)record_ap[0].ssid);                  //在设置界面显示已连接的AP名
//						switch_icon(0,5,1);                                          //主界面图标更新
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
					pop_tips(30,(u8*)TIPS,(u8*)EN_WIFI_PASSWORD);    //提示输入WiFi密码
					wifi_flow.connect_flag = 0;
					return 1;
				}
				pop_tips(28,(u8*)TIPS,(u8*)WIFI_CONNECTION_WAIT);                    //正在连接提示
				if( 0 == esp8266_join_ap(record_ap[ap_page.select_num].ssid,record_ap[ap_page.select_num].password) )
				{
					if(0 == esp8266_get_ip(record_ap[ap_page.select_num].ip))
					{		
						cur_connected_ap = record_ap[ap_page.select_num];
						record_ap[ap_page.select_num] = record_ap[0];
						record_ap[0] = cur_connected_ap;
						
						show_wifi_list(0);                                          //显示第0页wifi列表
						wifi_flow.connect_state = 1;                                //连接状态置位
						SetTextValue(22,21,(u8*)record_ap[0].ip);                   //连接成功，显示IP
						switch_icon(22,30,wifi_flow.connect_state);                 //右下角按钮显示断开连接
						if(ap_page.select_num == 0 && ap_page.cur_page == 0)        //若当前选中号正好为第0位
							switch_icon(22,15,2);                                   //显示选中连接图标
						else
							switch_icon(22,15,1);                                   //显示连接图标
						SetTextValue(20,11,(u8*)record_ap[0].ssid);                 //在设置界面显示已连接的AP名
						
						ap_page.select_num = 0xFF;     //清除选中
						
						if(FR_OK != write_user_parameter(WIFI_LINK_SAVE,(const void*)&record_ap[0],sizeof(ap_info)))  //WiFi连接ap和密码保存
						{
							//弹框 写入失败
							pop_tips(30,(u8*)WARNING,(u8*)FILE_RW_ERR); 
						}
						
						set_screen(22); 
//						switch_icon(0,5,1);                                          //主界面图标更新
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
					//wifi 连接超时
					set_screen(22);
					pop_tips(30,(u8*)WIFI_ERROR,(u8*)WIFI_CONNECTION_TIMEOUT);
				}
				wifi_flow.connect_flag = 0;
			}
		}
		else
		{
			
			set_screen(22);
			pop_tips(30,(u8*)TIPS,(u8*)SELECT_WIFI);    //提示先选中wifi
			wifi_flow.connect_flag = 0;
		}
		return 1;
	}
	else 
	{
		//断开连接
		if( 0 != esp8266_at_test() )  //若是处于透传模式
		{
			esp8266_exit_unvarnished();      //退出透传
		}
		if(0 == esp8266_disconnect_ap())
		{
			memset(&cur_connected_ap,0x00,sizeof(cur_connected_ap));  //清空当前连接wifi的信息
			SetTextValue(22,9,(u8*)cur_connected_ap.ssid);             
			SetTextValue(22,20,(u8*)cur_connected_ap.password);         
			SetTextValue(22,21,(u8*)cur_connected_ap.ip); 
			SetTextValue(20,11,(u8*)cur_connected_ap.ssid);                 //在设置界面显示
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

//解析ap信息
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
	if( 0 == esp8266_query_connected_ap(cur_connected_ap.ssid,cur_connected_ap.mac) ) //查询WiFi模块是否已有连接
	{
		if(0 == esp8266_get_ip(cur_connected_ap.ip))                    //获取连接wifi的IP
		{

			wifi_flow.connect_state = 1;                                //连接状态置位
			SetTextValue(22,21,(u8*)cur_connected_ap.ip);               //连接成功，显示IP
			switch_icon(22,30,wifi_flow.connect_state);                 //右下角按钮显示断开连接
			
			SetTextValue(22,9,(u8*)cur_connected_ap.ssid);              //显示连接的wifi名
			SetTextValue(22,20,(u8*)"******");                          //显示选中wifi的密码
			SetTextValue(20,11,(u8*)cur_connected_ap.ssid);             //在设置界面也要显示连接的ap
			switch_icon(22,15,2);                                       //WiFi名称后面连接图标
			
			//在搜寻的WiFi列表里查找到当前连接的WiFi，并放在列表第一位
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
	else                                        //若是WiFi未连接
	{
		if(FR_OK == read_user_parameter(WIFI_LINK_SAVE,(void*)&cur_connected_ap,sizeof(ap_info)))         //读取保存的WiFi
		{
			for(int i = 1;i < SHOW_MAX_WIFI_LIST;i++)                                                     //查找是否在当前搜寻的WiFi列表
			{
				if( 0 == memcmp(record_ap[i].ssid,cur_connected_ap.ssid,sizeof(cur_connected_ap.ssid)) &&\
					0 == memcmp(record_ap[i].mac,cur_connected_ap.mac,sizeof(cur_connected_ap.mac)) )
				{
					//若是则把存储的已连接过的WiFi 放在列表首位
					record_ap[0] = cur_connected_ap;
//					cur_connected_ap = record_ap[0];
//					record_ap[0] = record_ap[i];
//					record_ap[i] = cur_connected_ap;
					break;
				}				
			}
		}	
	}
	//筛选重名
	remove_duplicates();
}

//筛选掉Wi-Fi重复名
ap_info wifi_temp_name[SHOW_MAX_WIFI_LIST] = {0};
void remove_duplicates(void)
{
	int inx = 0;
	for(int i = 0;i<SHOW_MAX_WIFI_LIST - 1;i++)
	{
		for(int k = i+1;k<SHOW_MAX_WIFI_LIST;k++)
		{
			if( 0 == strcmp((const char*)record_ap[i].ssid,(const char*)record_ap[k].ssid) )   //若是重复
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

//传入当前页显示wifi列表
void show_wifi_list(u8 cur_page)
{
	u8 show_buf[10] = {0};
	for(int i = 0;i < SHOW_WIFI_LINE;i++)
	{
		SetTextValue(22,4 + i,(u8*)record_ap[i+cur_page*SHOW_WIFI_LINE].ssid);
		switch_icon(22,10 + i,0); 
		switch_icon(22,15 + i,0);
	}
	//已连接wifi  必在首页第一行 
	if(wifi_flow.connect_state == 1 && cur_page == 0 )
	{
		switch_icon(22,15,1);                    //第三个参数 0：空 1：连接  2选中连接 3：没有连接 4：选中没有连接效果
	}
	//显示页脚
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
		set_screen(20);        //返回到设置界面
	}
	else if(touch_id == 2)     //点击连接按钮
	{
		wifi_flow.execution_step = 3;     //执行一次连接或断开流程
		wifi_flow.connect_flag = 1;
	}
	else if(touch_id == 3)      //刷新按钮
	{
		wifi_flow.execution_step = 1;     //进行一次WiFi刷新
	}
	else if(22 <= touch_id && touch_id <= 26)  //选中wifi名
	{
		ap_page.select_num = ap_page.cur_page*5 + touch_id -22;
		if(ap_page.select_num >= SHOW_MAX_WIFI_LIST) return;
		if( ap_page.select_num < ap_page.total_line )
		{
			SetTextValue(22,9,(u8*)record_ap[ap_page.select_num].ssid);       //显示选中的wifi名
			if(0 != strlen(record_ap[ap_page.select_num].password))
				SetTextValue(22,20,(u8*)"******");
			else
				SetTextValue(22,20,(u8*)record_ap[ap_page.select_num].password);  //显示选中wifi的密码
			
			memcpy(&cur_connected_ap,&record_ap[ap_page.select_num],sizeof(ap_info));   //更新当前WiFi
			wifi_check_box_icon(touch_id-22);     //在选中的wifi名前打钩			
		}
	}
	else if(touch_id == 28)     //左翻页
	{
		if(ap_page.cur_page > 0)
			ap_page.cur_page--;
		show_wifi_list(ap_page.cur_page);
	}
	else if(touch_id == 29)     //右翻页
	{
		if(ap_page.cur_page + 1 < ap_page.total_page)
			ap_page.cur_page++;
		show_wifi_list(ap_page.cur_page);
	}
}


//wifi页面输入
void wifi_input_pro(u16 touch_id,u8 *p,int len)
{
	u8 chars_len = len - 13;
	if( touch_id == 9 )                     //wifi名输入  
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
	else if( touch_id == 20 )               //wifi密码输入
	{
		if(ap_page.select_num == 0xFF)      //若没有选中任何wifi
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

//参数 ：选中的ID位置
void wifi_check_box_icon(u16 id)
{
	for(int i = 0;i<SHOW_WIFI_LINE;i++)
	{
		if(i == id)   //当前选中ID
		{
			switch_icon(22,10 + i,1);       //在选中的wifi名前打钩
			if( wifi_flow.connect_state == 1 && id == 0 && ap_page.cur_page == 0)  //wifi已连接
			{
				switch_icon(22,15 + i,2);   //显示选中连接
			}
			else
			{				
				switch_icon(22,15 + i,4);   //显示选中未连接
			}
		}
		else       
		{				
			switch_icon(22,10 + i,0); 
			switch_icon(22,15 + i,0);
			if(wifi_flow.connect_state == 1 && ap_page.cur_page == 0 && id != 0)    //若是已连接状态则第一位需要显示连接图标
				switch_icon(22,15,1);
		}
	}
}


