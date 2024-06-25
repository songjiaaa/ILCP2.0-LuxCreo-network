#include "wifi_driver.h"
#include "wifi_use.h"
//#include "aos_crc64.h"

u8 wifi_pack_buf[2000]={0};

//wifiģ�鷢��
void esp8266_uart_tx(u8 *p,int n)
{
	uart_send(p,n,&uart3);
//	uart_send(p,n,&uart1);
}

void rec_copy_data(u8 b,COMM_SYNC* p)//ͬ����ʽ���պ���
{


}

COMM_SYNC wifi_data_pack=
{
	wifi_pack_buf,//u8 *rec_buff;
	sizeof(wifi_pack_buf),//int buf_len;
	ACK_OK_END,//u32 endc; 
	0,//int rec_p;
	wifi_data_pro//u8 (*pro)(u8 * b,int len);
};


u8 wifi_data_pro(u8 *b ,int len) 
{
	return 0;
}


/**
 * @brief       esp8266ģ��Ӳ����λ
 * @param       ��
 * @retval      ��
 */
void esp8266_hw_reset(void)
{
	WIFI_RESET = 0;
    vTaskDelay(100);
	WIFI_RESET = 1;
    vTaskDelay(3000);
}

/**
 * @brief       esp8266����ATָ��
 * @param       cmd    : �����͵�ATָ��
 *              ack    : �ȴ�����Ӧ
 *              timeout: �ȴ���ʱʱ��
 * @retval      0      : ����ִ�гɹ�
 *              1      : �ȴ�����Ӧ��ʱ������ִ��ʧ��
 */
u8 esp8266_send_at_cmd(char *cmd, u32 len, char *ack, uint32_t timeout)
{
	memset(wifi_pack_buf,0x00,sizeof(wifi_pack_buf));
	wifi_data_pack.rec_p = 0;
	esp8266_uart_tx((u8*)cmd,len);
	if ((ack == NULL) || (timeout == 0))
    {
        return 0;
    }
	else
	{
		while(timeout > 0)
		{
			if(strstr((const char *)wifi_pack_buf,ack) != NULL)
			{
				return 0;
			}
			vTaskDelay(1);
			timeout--;
		}
		return 1;
	}
}


/**
 * @brief       esp8266�ָ���������
 * @param       ��
 * @retval      0  : �ָ��������óɹ�
 *              1  : �ָ���������ʧ��
 */
uint8_t esp8266_restore(void)
{
	esp8266_uart_tx((u8*)"AT+RESTORE\r\n",strlen("AT+RESTORE\r\n"));
	vTaskDelay(2000);
    return 0;
}


/**
 * @brief       esp8266 ATָ�����
 * @param       ��
 * @retval      0  : ATָ����Գɹ�
 *              1  : ATָ�����ʧ��
 */
uint8_t esp8266_at_test(void)
{
	return esp8266_send_at_cmd("AT\r\n",strlen("AT\r\n"),"OK", 500);
}

/**
 * @brief       esp8266 ���ô��ڲ�����256000
 * @param       ��
 * @retval      0  : �ɹ�
 *              1  : ʧ��
 */
uint8_t esp8266_set_baud(void)
{
	return esp8266_send_at_cmd("AT+UART_CUR=256000,8,1,0,0\r\n",strlen("T+UART_CUR=256000,8,1,0,0\r\n"),"OK", 500);
}

/**
 * @brief       ����ATK-MW8266D����ģʽ
 * @param       mode: 1��Stationģʽ
 *                    2��APģʽ
 *                    3��AP+Stationģʽ
 * @retval      0   : ����ģʽ���óɹ�
 *              1   : ����ģʽ����ʧ��
 *              2   : mode�������󣬹���ģʽ����ʧ��
 */
uint8_t esp8266_set_mode(uint8_t mode)
{
    switch (mode)
    {
        case 1:
        {
            return esp8266_send_at_cmd("AT+CWMODE=1\r\n",strlen("AT+CWMODE=1\r\n"),"OK", 500);    /* Stationģʽ */
        }
        case 2:
        {
            return esp8266_send_at_cmd("AT+CWMODE=2\r\n",strlen("AT+CWMODE=2\r\n"),"OK", 500);    /* APģʽ */
        }
        case 3:
        {
            return esp8266_send_at_cmd("AT+CWMODE=3\r\n",strlen("AT+CWMODE=3\r\n"),"OK", 500);    /* AP+Stationģʽ */
        }
        default:
        {
            return 2;
        }
    }
}

/**
 * @brief       esp8266�����λ
 * @param       ��
 * @retval      0  : �����λ�ɹ�
 *              1  : �����λʧ��
 */
uint8_t esp8266_sw_reset(void)
{
    return esp8266_send_at_cmd("AT+RST\r\n",strlen("AT+RST\r\n"),"OK", 2000);
}

/**
 * @brief       esp8266���û���ģʽ
 * @param       cfg: 0���رջ���
 *                   1���򿪻���
 * @retval      0  : ���û���ģʽ�ɹ�
 *              1  : ���û���ģʽʧ��
 */
uint8_t esp8266_ate_config(uint8_t cfg)
{
    switch (cfg)
    {
        case 0:
        {
            return esp8266_send_at_cmd("ATE0\r\n",strlen("ATE0\r\n"),"OK", 500);   /* �رջ��� */
        }
        case 1:
        {
            return esp8266_send_at_cmd("ATE1\r\n",strlen("ATE1\r\n"),"OK", 500);   /* �򿪻��� */
        }
        default:
        {
            return 1;
        }
    }
}

/**
 * @brief       esp8266�ϵ��Ƿ��Զ�����
 * @param       cfg: 0�����Զ�����
 *                   1���Զ�����
 * @retval      0  : ���óɹ�
 *              1  : ����ʧ��
 */
uint8_t esp8266_set_auto_connected(uint8_t cfg)
{
    switch (cfg)
    {
        case 0:
        {
            return esp8266_send_at_cmd("AT+CWAUTOCONN=0\r\n",strlen("AT+CWAUTOCONN=0\r\n"),"OK", 500);   /* ���Զ����� */
        }
        case 1:
        {
            return esp8266_send_at_cmd("AT+CWAUTOCONN=1\r\n",strlen("AT+CWAUTOCONN=1\r\n"),"OK", 500);   /* �Զ����� */
        }
        default:
        {
            return 1;
        }
    }
}
/**
 * @brief       esp8266��ѯ��ʹ�õ�ap
* @param        void 
 * @retval      0  : �ɹ�
 *              1  : ʧ��
 */
uint8_t esp8266_query_ap(void)
{
    return esp8266_send_at_cmd("AT+CWLAP\r\n",strlen("AT+CWLAP\r\n"),"OK", 5000);
}

/**
 * @brief       esp8266����WIFI
 * @param       ssid: WIFI����
 *              pwd : WIFI����
 * @retval      0  : WIFI���ӳɹ�
 *              1  : WIFI����ʧ��
 */
uint8_t esp8266_join_ap(char *ssid, char *pwd)
{
    int n = 0;
    char cmd[128];
    
    n = sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
    return esp8266_send_at_cmd(cmd,n,"WIFI GOT IP", 15000);
}

/**
 * @brief       esp8266��������  ���ص���IPv4�ĵ�ַ
 * @param       domain: ������ַ
 * @param       ip_buf: �洢������IP��ַ
 * @retval      0  : �����ɹ�
 *              1  : ����ʧ��
 */
uint8_t esp8266_DNS(char *domain,char *ip_buf)
{
	int n = 0;
    char cmd[128];
	char *ptr = NULL;
	n = sprintf(cmd, "AT+CIPDOMAIN=\"%s\"\r\n", domain);
	if(0 == esp8266_send_at_cmd(cmd,n,"OK", 1000))
	{
		ptr = strstr((const char*)wifi_pack_buf,"+CIPDOMAIN:");
		if(ptr != NULL) 
		{
			sscanf(ptr,"+CIPDOMAIN:\"%[^\"]\"",ip_buf);
			return 0;
		}
	}
	return 1;
}

/**
 * @brief       esp8266��ѯ�����ӵ�wifi
 * @param       void
 * @retval      0  : ��ѯ�������ӵ�ap
 *              1  : û������
 *            ע��ʹ�ýṹ��ָ��������б������������������
 */
uint8_t esp8266_query_connected_ap(char *ssid,char* mac)
{
	char *ptr = NULL;
	if(0 == esp8266_send_at_cmd("AT+CWJAP?\r\n",strlen("AT+CWJAP?\r\n"),"OK", 1000))
	{
		ptr = strstr((const char*)wifi_pack_buf,"+CWJAP:");
		if(ptr != NULL) 
		{
			sscanf(ptr,"+CWJAP:\"%[^\"]\",\"%[^\"]\"",ssid,mac);
			return 0;
		}
	}
	return 1;
}
/**
 * @brief       esp8266�Ͽ���AP������
* @param        void
 * @retval      0  : �ɹ�
 *              1  : ʧ��
 */
uint8_t esp8266_disconnect_ap(void)
{
    return esp8266_send_at_cmd("AT+CWQAP\r\n",strlen("AT+CWQAP\r\n"),"OK", 500);
}
/**
 * @brief       esp8266��ȡIP��ַ
 * @param       buf: IP��ַ����Ҫ16�ֽ��ڴ�ռ�
 * @retval      0  : ��ȡIP��ַ�ɹ�
 *              1  : ��ȡIP��ַʧ��
 */
uint8_t esp8266_get_ip(char *buf)
{
    uint8_t ret; 
	char *ptr = NULL;	
	u8 count = 0;
	while(1)
	{
		ret = esp8266_send_at_cmd("AT+CIFSR\r\n",strlen("AT+CIFSR\r\n"),"OK", 1000);
		if(ret == 0)
		{
			ptr = strstr((const char*)wifi_pack_buf,"busy p...");
			if(ptr == NULL)
				break;
			else
				vTaskDelay(500);
		}
		if(count++ > 5) 
			break;
	}
	
	ptr = strstr((const char*)wifi_pack_buf,"+CIFSR:STAIP,");
	if(ptr == NULL)
	{			
		return 1;
	}		
  	sscanf((const char*)ptr,"+CIFSR:STAIP,\"%[^\"]\"",buf);
    return 0;
}

/**
 * @brief       esp8266����NTP������
 * @param       void  :
 * @retval      0  : �ɹ�
 *              1  : ʧ��
 */
uint8_t esp8266_set_ntp(void)
{
    char cmd[100] = "AT+CIPSNTPCFG=1,0,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\",\"us.pool.ntp.org\"\r\n";
    
    return esp8266_send_at_cmd(cmd,strlen(cmd),"OK", 1000);
}

/**
 * @brief       esp8266��ȡNTP������ʱ��
 * @param       buf  : �洢ʱ����ڴ��ַ
 * @param       len  : �洢ʱ����ڴ泤��
 * @retval      0  : �ɹ�
 *              1  : ʧ��
 */
uint8_t esp8266_get_ntp_time(char *buf)
{
	uint8_t ret; 
	char *ptr = NULL;  
    ret = esp8266_send_at_cmd("AT+CIPSNTPTIME?\r\n",strlen("AT+CIPSNTPTIME?\r\n"),"OK", 1000);
	if(ret == 0)
	{
		ptr = strstr((const char*)wifi_pack_buf,"+CIPSNTPTIME:");
		if(ptr != NULL)
		{
			sscanf((const char*)ptr,"+CIPSNTPTIME:%[^\r]",buf);
			return 0;
		}
		return 1;
	}
	return 1;
}

/**
 * @brief       esp8266����TCP������
 * @param       server_ip  : TCP������IP��ַ
 *              server_port: TCP�������˿ں�
 * @retval      0  : ����TCP�������ɹ�
 *              1  : ����TCP������ʧ��
 */
uint8_t esp8266_connect_tcp_server(char *server_ip, char *server_port)
{
	int n = 0;
    char cmd[64] = {0};
    
    n = sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s\r\n", server_ip, server_port);
    return esp8266_send_at_cmd(cmd,n,"CONNECT", 5000);
}



/**
 * @brief       esp8266���ӷ�����UDP����
 * @param       server_ip  : ������������IP
 *              server_port: �������˿ں�
 * @retval      0  : ����TCP�������ɹ�
 *              1  : ����TCP������ʧ��
 */
uint8_t esp8266_connect_udp_server(char *server_ip, char *server_port)
{
	int n = 0;
    char cmd[64] = {0};
    
    n = sprintf(cmd, "AT+CIPSTART=\"UDP\",\"%s\",%s\r\n", server_ip, server_port);
	if( 1 == esp8266_send_at_cmd(cmd,n,"CONNECT", 5000) )
	{
		if( 0 == esp8266_send_at_cmd(cmd,n,"ALREADY CONNECTED", 5000) )  //����������
		{
			return 0;
		}
		return 1;
	}
    return 0;
}

/**
 * @brief       esp8266����͸��
* @param        ��
 * @retval      0  : ����͸���ɹ�
 *              1  : ����͸��ʧ��
 */
uint8_t esp8266_enter_unvarnished(void)
{
    uint8_t ret;
    
    ret  = esp8266_send_at_cmd("AT+CIPMODE=1\r\n",strlen("AT+CIPMODE=1\r\n"),"OK", 500);
    ret += esp8266_send_at_cmd("AT+CIPSEND\r\n",strlen("AT+CIPSEND\r\n"),">", 500);
    if (ret == 0)
        return 0;
    else
        return 1;
}


/**
 * @brief       esp8266�˳�͸��
 * @param       ��
 * @retval      ��
 */
u8 esp8266_exit_unvarnished(void)
{
	u8 ret = 1;
	int i = 0;
	while(ret)
	{
		esp8266_uart_tx((u8*)("+++"),3);
		vTaskDelay(200);
		ret = esp8266_at_test();
		vTaskDelay(200);
		if(i++ > 20)
			break;
	}
	return ret;
}

/**
 * @brief       esp8266�Ͽ��������������
 * @param        void
 * @retval      0  : �ɹ�
 *              1  : ʧ��
 */
uint8_t esp8266_disconnect_server(void)
{
    return esp8266_send_at_cmd("AT+CIPCLOSE\r\n",strlen("AT+CIPCLOSE\r\n"),"OK", 1000);
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
u8 file_header_request(char* domain, char* path,u32* total_size,u32 timeout)
{
	int n = 0;
	char cmd[256] = {0};
    char *ptr = NULL; 
    n = sprintf(cmd, "HEAD %s HTTP/1.1\r\nHost: %s\r\n\r\n", path,domain);

	memset(wifi_pack_buf,0x00,sizeof(wifi_pack_buf));
	wifi_data_pack.rec_p = 0;
	esp8266_uart_tx((u8*)cmd,n);
	
	while(timeout > 0)
	{
		ptr = strstr((const char *)wifi_pack_buf,"HTTP/1.1 200 OK");    //��������
		if(ptr != NULL)                                        
		{
			ptr = strstr((const char*)wifi_pack_buf,"Content-Length:");
			if(ptr != NULL)
			{
				if(uart3.que_rx.dlen == 0)
				{					
					sscanf((const char*)ptr,"Content-Length:%d\r\nConnection",total_size);	
					return 0;
				}
			}		
		}
		else
		{
			ptr = strstr((const char *)wifi_pack_buf,"404 Not Found");      //δ�ҵ� û���ļ�
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
u8 server_download_request(char* domain,char* path,int* start,int* end,u8* buf,u32* total_size,u32 timeout)
{
	int n = 0,get_start = 0,get_end = 0;
	char cmd[256] = {0};
	char *start_p = NULL;
    char *ptr = NULL; 

    n = sprintf(cmd, "GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%d-%d\r\n\r\n", path,domain,*start,*end);
	memset(wifi_pack_buf,0x00,sizeof(wifi_pack_buf));
	wifi_data_pack.rec_p = 0;                                                           //�����ֽ���
	esp8266_uart_tx((u8*)cmd,n);
	
	while(timeout > 0)
	{
		vTaskDelay(1);
		timeout--;
		start_p = strstr((const char *)wifi_pack_buf,"HTTP/1.1 206 Partial Content");    //��������
		if(start_p != NULL)                                        
		{
			ptr = strstr((const char*)wifi_pack_buf,"Content-Range: bytes ");
			if(ptr != NULL)
			{					
				sscanf((const char*)ptr,"Content-Range: bytes %d-%d/%d\r\n",&get_start,&get_end,total_size);  //��ȡ�ļ�����λ�ú��ļ���С��Ϣ

				if(*start == get_start && *end == get_end)
				{
					ptr = strstr((const char *)ptr,"\r\n\r\n");                          //�ָ�λ��
					if(ptr != NULL)
					{
						if( uart3.que_rx.dlen == 0 )
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
			ptr = strstr((const char *)wifi_pack_buf,"Not Found");      //δ�ҵ� û���ļ�
			if(ptr != NULL)
			{				
				return 1;                                               //δ�ҵ��ļ�ֱ�ӷ���
			}
		}
	}
	return 1;
}

