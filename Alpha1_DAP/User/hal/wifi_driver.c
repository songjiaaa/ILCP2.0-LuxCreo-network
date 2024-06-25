#include "wifi_driver.h"
#include "wifi_use.h"
//#include "aos_crc64.h"

u8 wifi_pack_buf[2000]={0};

//wifi模块发送
void esp8266_uart_tx(u8 *p,int n)
{
	uart_send(p,n,&uart3);
//	uart_send(p,n,&uart1);
}

void rec_copy_data(u8 b,COMM_SYNC* p)//同步方式接收函数
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
 * @brief       esp8266模块硬件复位
 * @param       无
 * @retval      无
 */
void esp8266_hw_reset(void)
{
	WIFI_RESET = 0;
    vTaskDelay(100);
	WIFI_RESET = 1;
    vTaskDelay(3000);
}

/**
 * @brief       esp8266发送AT指令
 * @param       cmd    : 待发送的AT指令
 *              ack    : 等待的响应
 *              timeout: 等待超时时间
 * @retval      0      : 函数执行成功
 *              1      : 等待期望应答超时，函数执行失败
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
 * @brief       esp8266恢复出厂设置
 * @param       无
 * @retval      0  : 恢复出场设置成功
 *              1  : 恢复出场设置失败
 */
uint8_t esp8266_restore(void)
{
	esp8266_uart_tx((u8*)"AT+RESTORE\r\n",strlen("AT+RESTORE\r\n"));
	vTaskDelay(2000);
    return 0;
}


/**
 * @brief       esp8266 AT指令测试
 * @param       无
 * @retval      0  : AT指令测试成功
 *              1  : AT指令测试失败
 */
uint8_t esp8266_at_test(void)
{
	return esp8266_send_at_cmd("AT\r\n",strlen("AT\r\n"),"OK", 500);
}

/**
 * @brief       esp8266 设置串口波特率256000
 * @param       无
 * @retval      0  : 成功
 *              1  : 失败
 */
uint8_t esp8266_set_baud(void)
{
	return esp8266_send_at_cmd("AT+UART_CUR=256000,8,1,0,0\r\n",strlen("T+UART_CUR=256000,8,1,0,0\r\n"),"OK", 500);
}

/**
 * @brief       设置ATK-MW8266D工作模式
 * @param       mode: 1，Station模式
 *                    2，AP模式
 *                    3，AP+Station模式
 * @retval      0   : 工作模式设置成功
 *              1   : 工作模式设置失败
 *              2   : mode参数错误，工作模式设置失败
 */
uint8_t esp8266_set_mode(uint8_t mode)
{
    switch (mode)
    {
        case 1:
        {
            return esp8266_send_at_cmd("AT+CWMODE=1\r\n",strlen("AT+CWMODE=1\r\n"),"OK", 500);    /* Station模式 */
        }
        case 2:
        {
            return esp8266_send_at_cmd("AT+CWMODE=2\r\n",strlen("AT+CWMODE=2\r\n"),"OK", 500);    /* AP模式 */
        }
        case 3:
        {
            return esp8266_send_at_cmd("AT+CWMODE=3\r\n",strlen("AT+CWMODE=3\r\n"),"OK", 500);    /* AP+Station模式 */
        }
        default:
        {
            return 2;
        }
    }
}

/**
 * @brief       esp8266软件复位
 * @param       无
 * @retval      0  : 软件复位成功
 *              1  : 软件复位失败
 */
uint8_t esp8266_sw_reset(void)
{
    return esp8266_send_at_cmd("AT+RST\r\n",strlen("AT+RST\r\n"),"OK", 2000);
}

/**
 * @brief       esp8266设置回显模式
 * @param       cfg: 0，关闭回显
 *                   1，打开回显
 * @retval      0  : 设置回显模式成功
 *              1  : 设置回显模式失败
 */
uint8_t esp8266_ate_config(uint8_t cfg)
{
    switch (cfg)
    {
        case 0:
        {
            return esp8266_send_at_cmd("ATE0\r\n",strlen("ATE0\r\n"),"OK", 500);   /* 关闭回显 */
        }
        case 1:
        {
            return esp8266_send_at_cmd("ATE1\r\n",strlen("ATE1\r\n"),"OK", 500);   /* 打开回显 */
        }
        default:
        {
            return 1;
        }
    }
}

/**
 * @brief       esp8266上电是否自动连接
 * @param       cfg: 0，不自动连接
 *                   1，自动连接
 * @retval      0  : 设置成功
 *              1  : 设置失败
 */
uint8_t esp8266_set_auto_connected(uint8_t cfg)
{
    switch (cfg)
    {
        case 0:
        {
            return esp8266_send_at_cmd("AT+CWAUTOCONN=0\r\n",strlen("AT+CWAUTOCONN=0\r\n"),"OK", 500);   /* 不自动连接 */
        }
        case 1:
        {
            return esp8266_send_at_cmd("AT+CWAUTOCONN=1\r\n",strlen("AT+CWAUTOCONN=1\r\n"),"OK", 500);   /* 自动连接 */
        }
        default:
        {
            return 1;
        }
    }
}
/**
 * @brief       esp8266查询可使用的ap
* @param        void 
 * @retval      0  : 成功
 *              1  : 失败
 */
uint8_t esp8266_query_ap(void)
{
    return esp8266_send_at_cmd("AT+CWLAP\r\n",strlen("AT+CWLAP\r\n"),"OK", 5000);
}

/**
 * @brief       esp8266连接WIFI
 * @param       ssid: WIFI名称
 *              pwd : WIFI密码
 * @retval      0  : WIFI连接成功
 *              1  : WIFI连接失败
 */
uint8_t esp8266_join_ap(char *ssid, char *pwd)
{
    int n = 0;
    char cmd[128];
    
    n = sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
    return esp8266_send_at_cmd(cmd,n,"WIFI GOT IP", 15000);
}

/**
 * @brief       esp8266域名解析  返回的是IPv4的地址
 * @param       domain: 域名地址
 * @param       ip_buf: 存储域名的IP地址
 * @retval      0  : 解析成功
 *              1  : 解析失败
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
 * @brief       esp8266查询已连接的wifi
 * @param       void
 * @retval      0  : 查询到已连接的ap
 *              1  : 没有连接
 *            注：使用结构体指针参数会有编译器报错问题待查找
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
 * @brief       esp8266断开与AP的连接
* @param        void
 * @retval      0  : 成功
 *              1  : 失败
 */
uint8_t esp8266_disconnect_ap(void)
{
    return esp8266_send_at_cmd("AT+CWQAP\r\n",strlen("AT+CWQAP\r\n"),"OK", 500);
}
/**
 * @brief       esp8266获取IP地址
 * @param       buf: IP地址，需要16字节内存空间
 * @retval      0  : 获取IP地址成功
 *              1  : 获取IP地址失败
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
 * @brief       esp8266设置NTP服务器
 * @param       void  :
 * @retval      0  : 成功
 *              1  : 失败
 */
uint8_t esp8266_set_ntp(void)
{
    char cmd[100] = "AT+CIPSNTPCFG=1,0,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\",\"us.pool.ntp.org\"\r\n";
    
    return esp8266_send_at_cmd(cmd,strlen(cmd),"OK", 1000);
}

/**
 * @brief       esp8266获取NTP服务器时间
 * @param       buf  : 存储时间的内存地址
 * @param       len  : 存储时间的内存长度
 * @retval      0  : 成功
 *              1  : 失败
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
 * @brief       esp8266连接TCP服务器
 * @param       server_ip  : TCP服务器IP地址
 *              server_port: TCP服务器端口号
 * @retval      0  : 连接TCP服务器成功
 *              1  : 连接TCP服务器失败
 */
uint8_t esp8266_connect_tcp_server(char *server_ip, char *server_port)
{
	int n = 0;
    char cmd[64] = {0};
    
    n = sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s\r\n", server_ip, server_port);
    return esp8266_send_at_cmd(cmd,n,"CONNECT", 5000);
}



/**
 * @brief       esp8266连接服务器UDP传输
 * @param       server_ip  : 服务器域名或IP
 *              server_port: 服务器端口号
 * @retval      0  : 连接TCP服务器成功
 *              1  : 连接TCP服务器失败
 */
uint8_t esp8266_connect_udp_server(char *server_ip, char *server_port)
{
	int n = 0;
    char cmd[64] = {0};
    
    n = sprintf(cmd, "AT+CIPSTART=\"UDP\",\"%s\",%s\r\n", server_ip, server_port);
	if( 1 == esp8266_send_at_cmd(cmd,n,"CONNECT", 5000) )
	{
		if( 0 == esp8266_send_at_cmd(cmd,n,"ALREADY CONNECTED", 5000) )  //若是已连接
		{
			return 0;
		}
		return 1;
	}
    return 0;
}

/**
 * @brief       esp8266进入透传
* @param        无
 * @retval      0  : 进入透传成功
 *              1  : 进入透传失败
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
 * @brief       esp8266退出透传
 * @param       无
 * @retval      无
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
 * @brief       esp8266断开与服务器的连接
 * @param        void
 * @retval      0  : 成功
 *              1  : 失败
 */
uint8_t esp8266_disconnect_server(void)
{
    return esp8266_send_at_cmd("AT+CIPCLOSE\r\n",strlen("AT+CIPCLOSE\r\n"),"OK", 1000);
}


/**
 * @brief       依据http协议请求文件头部信息
 * @param       domain：服务器域名
 * @param       path：文件路径
 * @param       total_size：返回文件的总大小
 * @param       timeout：等待超时时间
 * @retval      0  : 请求下载成功
 *              1  : 请求下载失败
 */
//从服务器下载数据，domain： 域名 path：路径 
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
		ptr = strstr((const char *)wifi_pack_buf,"HTTP/1.1 200 OK");    //部分内容
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
			ptr = strstr((const char *)wifi_pack_buf,"404 Not Found");      //未找到 没有文件
			if(ptr != NULL)                        
				return 1;                                                   //未找到文件直接返回
		}
		vTaskDelay(1);
		timeout--;
	}
	return 1;
}

/**
 * @brief       依据http协议分包下载数据
 * @param       domain：服务器域名
 * @param       path：文件路径
 * @param       start：起始字节 
 * @param       end：结束字节
 * @param       buf：存储buf
 * @param       total_size：返回文件的总大小
 * @param       timeout：等待超时时间
 * @retval      0  : 请求下载成功
 *              1  : 请求下载失败
 */
u8 server_download_request(char* domain,char* path,int* start,int* end,u8* buf,u32* total_size,u32 timeout)
{
	int n = 0,get_start = 0,get_end = 0;
	char cmd[256] = {0};
	char *start_p = NULL;
    char *ptr = NULL; 

    n = sprintf(cmd, "GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%d-%d\r\n\r\n", path,domain,*start,*end);
	memset(wifi_pack_buf,0x00,sizeof(wifi_pack_buf));
	wifi_data_pack.rec_p = 0;                                                           //接收字节数
	esp8266_uart_tx((u8*)cmd,n);
	
	while(timeout > 0)
	{
		vTaskDelay(1);
		timeout--;
		start_p = strstr((const char *)wifi_pack_buf,"HTTP/1.1 206 Partial Content");    //部分内容
		if(start_p != NULL)                                        
		{
			ptr = strstr((const char*)wifi_pack_buf,"Content-Range: bytes ");
			if(ptr != NULL)
			{					
				sscanf((const char*)ptr,"Content-Range: bytes %d-%d/%d\r\n",&get_start,&get_end,total_size);  //提取文件数据位置和文件大小信息

				if(*start == get_start && *end == get_end)
				{
					ptr = strstr((const char *)ptr,"\r\n\r\n");                          //分割位置
					if(ptr != NULL)
					{
						if( uart3.que_rx.dlen == 0 )
						{
							n = get_end - get_start + 1;
							if(n <= DOWNLOAD_DATA_LEN)            //防止数组拷贝越界
							{
								memcpy(buf,ptr + 4,n);            //把请求的内容拷贝到buf
								return 0;
							}
						}
						else
							vTaskDelay(100);       //延迟100ms等接收完  
					}
					else	
						vTaskDelay(100);       //延迟100ms等接收完  						
				}
			}
		}
		else
		{
			ptr = strstr((const char *)wifi_pack_buf,"Not Found");      //未找到 没有文件
			if(ptr != NULL)
			{				
				return 1;                                               //未找到文件直接返回
			}
		}
	}
	return 1;
}

