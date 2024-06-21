#include "ethernet_driver.h"


u8 etherne_pack_buf[2000]={0};

char eth_server_ip[30] = {0};

//��̫��ģ�鷢��
void usr_k5_tx(u8 *p,int n)
{
	uart_send(p,n,&uart2);
	uart_send(p,n,&uart1);
}

COMM_SYNC ethernet_data_pack=
{
	etherne_pack_buf,//u8 *rec_buff;
	sizeof(etherne_pack_buf),//int buf_len;
	ACK_OK_END,//u32 endc; 
	0,//int rec_p;
	entherne_data_pro//u8 (*pro)(u8 * b,int len);
};


u8 entherne_data_pro(u8 *b ,int len) 
{
	return 0;
}


//��ʼ�� USR��K5 ģ��
u8 usr_k5_init(void)
{
	u8 ret = 0;
	ret = usr_enter_at_mode();      //����ATָ��ģʽ
	ret += usr_restore_default();   //ģ��ָ�Ĭ�ϲ���
	vTaskDelay(2000);
	
	ret += usr_enter_at_mode();     //�ٴν���ATָ��ģʽ		
	ret += usr_set_wan_ip();        //����ģ��Ϊ��̬IP  DHCP
	vTaskDelay(500);
	if(save_config.set_server == 0)
		memcpy(eth_server_ip,OTA_DOMESTIC_IP,sizeof(OTA_DOMESTIC_IP));
	else
		memcpy(eth_server_ip,OTA_FOREIGN_IP,sizeof(OTA_FOREIGN_IP));

	if( 0 == usr_query_connect())   //��ѯ����״̬
	{
		ret = usr_ping_web_server(eth_server_ip);  //pin ������
		if(ret == 0)
		{
			if( 0 == usr_query_wan_ip(enthernet_flow.ip))  //��ȡIP
			{
				SetTextValue(20,12,(u8*)enthernet_flow.ip); 
				ret = usr_set_tcp_server(eth_server_ip,PORT_NUMBER);  //���� OTA ���ӵķ����� TCP ����
 	
				if(ret == 0)
				{						
					ret += usr_restart();	       //�豸����
					if(ret == 0)
						enthernet_flow.execution_step = 2;
					return ret;
				}
			}
		}
		else
			cfg_dft.network_selet = 0;
	}
	ret += usr_exit_at_mode();        //�˳�ATָ��ģʽ
	return ret;
}

//��̫��ģ������
void ethernet_module_run(void)
{
	u8 ret = 0;
	static u32 e_run_tick = 0;
	e_run_tick++;

	if(enthernet_flow.execution_step == 1)         //��ѯ�Ƿ�����
	{	
		if(e_run_tick % 400 == 0)
		{
			if(cfg_dft.network_selet == 0)
			{				
				ret = usr_enter_at_mode();      //�ٴν���ATָ��ģʽ	
				if(ret == 0)
				{
					ret = usr_ping_web_server(eth_server_ip);  //pin ������
					if(ret == 0)
					{
						if( 0 == usr_query_wan_ip(enthernet_flow.ip))  //��ȡIP
						{
							SetTextValue(20,12,(u8*)enthernet_flow.ip); 
							ret = usr_set_tcp_server(eth_server_ip,PORT_NUMBER);  //����  ���ӵķ�����
							if(ret == 0)
							{
								ret = usr_exit_at_mode();	   //�˳�ATָ��ģʽ
								ret += usr_enter_at_mode();	   //							
								ret += usr_restart();	       //�豸����
								if(ret == 0)
									enthernet_flow.execution_step = 2;
								return;
							}
						}
					}
					else
						cfg_dft.network_selet = 0;
				}
				ret = usr_exit_at_mode();        //�˳�ATָ��ģʽ
				if( ret != 0)
					enthernet_flow.execution_step = 1;
			}
		}
	}
	else if(enthernet_flow.execution_step == 2)    //У׼����ʱ�� 
	{
		int i= 0;
		while(i++ < 10)
		{
			ret = usr_cal_rtc_time(cfg_dft.ali_domain_name,MCU_OTA_FILE_PATH,2000);
			if(ret == 0)
				break;
		}
		if(ret == 0)
		{
			enthernet_flow.execution_step = 3;
		}
		else
		{
			enthernet_flow.execution_step = 1;  //���������ϴ��ķ�����
			cfg_dft.network_selet = 0;          //��̫������״̬����
		}	
	}
	else if(enthernet_flow.execution_step == 3)
	{
		ret = usr_enter_at_mode();      //�ٴν���ATָ��ģʽ	
		ret += usr_ping_web_server(cfg_dft.lux_domain_name);                        //pin �ϴ����ݵķ�����
//		ret += usr_set_tcp_server(cfg_dft.lux_domain_name,SERVER_TCP_PORT_NUMBER);  //����Ϊ�ϴ����ݵķ����� TCP
		ret += usr_set_udp_server(cfg_dft.lux_domain_name,SERVER_UDP_PORT_NUMBER);  //����Ϊ�ϴ����ݵķ����� UDP
		ret += usr_restart();	       //�豸����
		if(ret == 0)
		{
			vTaskDelay(5000);
			cfg_dft.network_selet = 1; 
			enthernet_flow.execution_step = 4;
		}
	}
	else if(enthernet_flow.execution_step == 4)
	{
		
	}
}

//ģ��Ӳ������
void ethernet_h_reset(void)
{
	EHT_RESET = 0;
	vTaskDelay(200);
	EHT_RESET = 1;
}	

//����ATָ��ȴ���Ӧ
u8 usr_send_at_cmd(char *cmd, u32 len, char *ack, uint32_t timeout)
{
	memset(etherne_pack_buf,0x00,sizeof(etherne_pack_buf));
	ethernet_data_pack.rec_p = 0;
	usr_k5_tx((u8*)cmd,len);
	if ((ack == NULL) || (timeout == 0))
    {
        return 0;
    }
	else
	{
		while(timeout > 0)
		{
			if(strstr((const char *)etherne_pack_buf,ack) != NULL)
			{
				return 0;
			}
			vTaskDelay(1);
			timeout--;
		}
		return 1;
	}
}

//��̫��ģ�����ATָ��ģʽ
u8 usr_enter_at_mode(void)
{
	u8 ret = 0;
	ret = usr_send_at_cmd("+++",strlen("+++"),"a",1000);
	ret += usr_send_at_cmd("a",strlen("a"),"+ok",1000);                    //����ATָ��ģʽ
	if(ret) 
		return 1; 
	else 
		return 0;
}

//ģ��ָ�Ĭ�ϲ���
u8 usr_restore_default(void) 
{
	return usr_send_at_cmd("AT+RELD\r",strlen("AT+RELD\r"),"+OK=rebooting",1000);
}

//ģ������
u8 usr_restart(void)
{
	u8 ret = 0;
	ret = usr_send_at_cmd("AT+Z\r",strlen("AT+Z\r"),"+OK",1000); 
	vTaskDelay(500);
	return ret;
}

//����ģ��Ĵ��ڲ�����
u8 usr_set_uart_baud(u32 baud)
{
	u8 ret = 0;
	char cmd[64] = {0};
	int n = 0;
	n = sprintf(cmd,"AT+UART=%d,8,1,NONE,NFC\r\n",baud);
	ret = usr_send_at_cmd(cmd,n,"+OK",1000); 
	if(ret == 0)
	{
		ret = usr_restart();   //ģ������
		if(ret == 0)
			uart_initial(&uart2,baud);        //���³�ʼ�����ش��ڲ�����
	}
	return ret;
}

//ģ���˳�ATָ��ģʽ������͸��ģʽ
u8 usr_exit_at_mode(void)
{
	return usr_send_at_cmd("AT+ENTM\r",strlen("AT+ENTM\r"),"+OK",1000); 
}

//����ģ��IPΪDHCP
u8 usr_set_wan_ip(void)
{
	return usr_send_at_cmd("AT+WANN=DHCP\r",strlen("AT+WANN=DHCP\r"),"+OK",1000); 
}

//��ѯģ�鶯̬IP
u8 usr_query_wan_ip(char *buf)
{
	u8 ret = 0;
	char *ptr = NULL;	
	ret = usr_send_at_cmd("AT+WANN\r",strlen("AT+WANN\r"),"+OK",1000); 
	if(ret == 0)
	{
		ptr = strstr((const char*)etherne_pack_buf,"+OK=DHCP,");
		if(ptr != NULL)
		{
			sscanf((const char*)ptr,"+OK=DHCP,%[^,]",buf);
			return 0;
		}
		else
			return 1;
	}
	return ret;
}

//ping ��վ�ж��Ƿ�����
u8 usr_ping_web_server(char *server_ip)
{
	int n = 0;
    char cmd[64] = {0};
    
    n = sprintf(cmd, "AT+PING=%s\r", server_ip);
    return usr_send_at_cmd(cmd,n,"+OK=SUCCESS", 1000);
}

//�������ӵ�TCP������
u8 usr_set_tcp_server(char *server_ip, char *server_port)
{
	int n = 0;
    char cmd[64] = {0};
    
    n = sprintf(cmd, "AT+SOCK=TCPC,%s,%s\r", server_ip, server_port);
    return usr_send_at_cmd(cmd,n,"+OK", 1000);
}

//�������ӵ�TCP������
u8 usr_set_udp_server(char *server_ip, char *server_port)
{
	int n = 0;
    char cmd[64] = {0};
    
    n = sprintf(cmd, "AT+SOCK=UDPC,%s,%s\r", server_ip, server_port);
    return usr_send_at_cmd(cmd,n,"+OK", 1000);
}

//��ѯ���� 0������  1δ����
u8 usr_query_connect(void)
{
	return usr_send_at_cmd("AT+SOCKLK\r",strlen("AT+SOCKLK\r"),"+OK=connect",1000); 
}


//char testaaa[] = "Date: Tue, 21 May 2024 09:43:40 GMT";
//tm usr_tm = {0}; 
//�����������Ϣ���ӷ�����������Ϣ�еõ�ʱ����У׼RTCʱ��
u8 usr_cal_rtc_time(char* domain, char* path,u32 timeout)
{
	tm usr_tm = {0}; 
	u8 year = 0;
	int n = 0;
	char cmd[256] = {0};
    char *ptr = NULL; 
	static char weekday[10] = {0}, month[10] = {0};
    n = sprintf(cmd, "HEAD %s HTTP/1.1\r\nHost: %s\r\n\r\n", path,domain);

	memset(etherne_pack_buf,0x00,sizeof(etherne_pack_buf));
	ethernet_data_pack.rec_p = 0;
	usr_k5_tx((u8*)cmd,n);
	
	while(timeout > 0)
	{
		ptr = strstr((const char *)etherne_pack_buf,"HTTP/1.1");    
		if(ptr != NULL)                                        
		{
			ptr = strstr((const char*)etherne_pack_buf,"Date:");
			if(ptr != NULL)
			{
				if(uart2.que_rx.dlen == 0)
				{								
					sscanf((const char*)ptr,"Date: %3s, %d %s %d %d:%d:%d GMT",weekday, &usr_tm.tm_day, month, &usr_tm.tm_year, &usr_tm.tm_hour, &usr_tm.tm_min, &usr_tm.tm_sec);

					RTC_Set_Time(usr_tm.tm_hour, usr_tm.tm_min, usr_tm.tm_sec, 0);    //����ʱ��

					usr_tm.tm_mon = getmonth(month);          //�·�
					usr_tm.tm_weekday = getweekday(weekday);  //���ڼ�
					year = usr_tm.tm_year - 2000;  
					RTC_Set_Date(year, usr_tm.tm_mon, usr_tm.tm_day, usr_tm.tm_weekday);  //��������					
					return 0;
				}
			}		
		}
		vTaskDelay(1);
		timeout--;
	}
	return 1;
}


/**
 * @brief       ����httpЭ�������ļ�ͷ����Ϣ
 * @param       domain������������
 * @param       path���ļ�·��
 * @param       total_size�������ļ����ܴ�С
 * @param       timeout���ȴ���ʱʱ��
 * @retval      0  : �������سɹ�
 *              1  : ��������ʧ��
 */
//�ӷ������������ݣ�domain�� ���� path��·�� 
u8 usr_file_header_request(char* domain, char* path,u32* total_size,u32 timeout)
{
	int n = 0;
	char cmd[256] = {0};
    char *ptr = NULL; 
    n = sprintf(cmd, "HEAD %s HTTP/1.1\r\nHost: %s\r\n\r\n", path,domain);

	memset(etherne_pack_buf,0x00,sizeof(etherne_pack_buf));
	ethernet_data_pack.rec_p = 0;
	usr_k5_tx((u8*)cmd,n);
	
	while(timeout > 0)
	{
		ptr = strstr((const char *)etherne_pack_buf,"HTTP/1.1 200 OK");    //��������
		if(ptr != NULL)                                        
		{
			ptr = strstr((const char*)etherne_pack_buf,"Content-Length:");
			if(ptr != NULL)
			{
				if(uart2.que_rx.dlen == 0)
				{					
					sscanf((const char*)ptr,"Content-Length:%d\r\nConnection",total_size);	
					return 0;
				}
			}		
		}
		else
		{
			ptr = strstr((const char *)etherne_pack_buf,"404 Not Found");      //δ�ҵ� û���ļ�
			if(ptr != NULL)                        
				return 1;                                                   //δ�ҵ��ļ�ֱ�ӷ���
		}
		vTaskDelay(1);
		timeout--;
	}
	return 1;
}

/**
 * @brief       ����httpЭ��ְ���������
 * @param       domain������������
 * @param       path���ļ�·��
 * @param       start����ʼ�ֽ� 
 * @param       end�������ֽ�
 * @param       buf���洢buf
 * @param       total_size�������ļ����ܴ�С
 * @param       timeout���ȴ���ʱʱ��
 * @retval      0  : �������سɹ�
 *              1  : ��������ʧ��
 */
u8 usr_server_download_request(char* domain,char* path,int* start,int* end,u8* buf,u32* total_size,u32 timeout)
{
	int n = 0,get_start = 0,get_end = 0;
	char cmd[256] = {0};
	char *start_p = NULL;
    char *ptr = NULL; 

    n = sprintf(cmd, "GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%d-%d\r\n\r\n", path,domain,*start,*end);
	memset(etherne_pack_buf,0x00,sizeof(etherne_pack_buf));
	ethernet_data_pack.rec_p = 0;                                                           //�����ֽ���
	usr_k5_tx((u8*)cmd,n);
	
	while(timeout > 0)
	{
		vTaskDelay(1);
		timeout--;
		start_p = strstr((const char *)etherne_pack_buf,"HTTP/1.1 206 Partial Content");    //��������
		if(start_p != NULL)                                        
		{
			ptr = strstr((const char*)etherne_pack_buf,"Content-Range: bytes ");
			if(ptr != NULL)
			{					
				sscanf((const char*)ptr,"Content-Range: bytes %d-%d/%d\r\n",&get_start,&get_end,total_size);  //��ȡ�ļ�����λ�ú��ļ���С��Ϣ

				if(*start == get_start && *end == get_end)
				{
					ptr = strstr((const char *)ptr,"\r\n\r\n");                          //�ָ�λ��
					if(ptr != NULL)
					{
						if( uart2.que_rx.dlen == 0 )
						{
							n = get_end - get_start + 1;
							if(n <= DOWNLOAD_DATA_LEN)            //��ֹ���鿽��Խ��
							{
								memcpy(buf,ptr + 4,n);            //����������ݿ�����buf
								return 0;
							}
						}
						else
							vTaskDelay(100);       //�ӳ�100ms�Ƚ�����  
					}
					else	
						vTaskDelay(100);       //�ӳ�100ms�Ƚ�����  						
				}
			}
		}
		else
		{
			ptr = strstr((const char *)etherne_pack_buf,"Not Found");      //δ�ҵ� û���ļ�
			if(ptr != NULL)
			{				
				return 1;                                               //δ�ҵ��ļ�ֱ�ӷ���
			}
		}
	}
	return 1;
}


