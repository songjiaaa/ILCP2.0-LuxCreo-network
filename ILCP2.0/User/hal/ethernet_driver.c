#include "ethernet_driver.h"


u8 etherne_pack_buf[2000]={0};

char eth_server_ip[30] = {0};

//以太网模块发送
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


//初始化 USR—K5 模块
u8 usr_k5_init(void)
{
	u8 ret = 0;
	ret = usr_enter_at_mode();      //进入AT指令模式
	ret += usr_restore_default();   //模块恢复默认参数
	vTaskDelay(2000);
	
	ret += usr_enter_at_mode();     //再次进入AT指令模式		
	ret += usr_set_wan_ip();        //设置模块为动态IP  DHCP
	vTaskDelay(500);
	if(save_config.set_server == 0)
		memcpy(eth_server_ip,OTA_DOMESTIC_IP,sizeof(OTA_DOMESTIC_IP));
	else
		memcpy(eth_server_ip,OTA_FOREIGN_IP,sizeof(OTA_FOREIGN_IP));

	if( 0 == usr_query_connect())   //查询连接状态
	{
		ret = usr_ping_web_server(eth_server_ip);  //pin 服务器
		if(ret == 0)
		{
			if( 0 == usr_query_wan_ip(enthernet_flow.ip))  //获取IP
			{
				SetTextValue(20,12,(u8*)enthernet_flow.ip); 
				ret = usr_set_tcp_server(eth_server_ip,PORT_NUMBER);  //设置 OTA 连接的服务器 TCP 连接
 	
				if(ret == 0)
				{						
					ret += usr_restart();	       //设备重启
					if(ret == 0)
						enthernet_flow.execution_step = 2;
					return ret;
				}
			}
		}
		else
			cfg_dft.network_selet = 0;
	}
	ret += usr_exit_at_mode();        //退出AT指令模式
	return ret;
}

//以太网模块运行
void ethernet_module_run(void)
{
	u8 ret = 0;
	static u32 e_run_tick = 0;
	e_run_tick++;

	if(enthernet_flow.execution_step == 1)         //查询是否联网
	{	
		if(e_run_tick % 400 == 0)
		{
			if(cfg_dft.network_selet == 0)
			{				
				ret = usr_enter_at_mode();      //再次进入AT指令模式	
				if(ret == 0)
				{
					ret = usr_ping_web_server(eth_server_ip);  //pin 服务器
					if(ret == 0)
					{
						if( 0 == usr_query_wan_ip(enthernet_flow.ip))  //获取IP
						{
							SetTextValue(20,12,(u8*)enthernet_flow.ip); 
							ret = usr_set_tcp_server(eth_server_ip,PORT_NUMBER);  //设置  连接的服务器
							if(ret == 0)
							{
								ret = usr_exit_at_mode();	   //退出AT指令模式
								ret += usr_enter_at_mode();	   //							
								ret += usr_restart();	       //设备重启
								if(ret == 0)
									enthernet_flow.execution_step = 2;
								return;
							}
						}
					}
					else
						cfg_dft.network_selet = 0;
				}
				ret = usr_exit_at_mode();        //退出AT指令模式
				if( ret != 0)
					enthernet_flow.execution_step = 1;
			}
		}
	}
	else if(enthernet_flow.execution_step == 2)    //校准网络时间 
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
			enthernet_flow.execution_step = 1;  //重新连接上传的服务器
			cfg_dft.network_selet = 0;          //以太网连接状态重置
		}	
	}
	else if(enthernet_flow.execution_step == 3)
	{
		ret = usr_enter_at_mode();      //再次进入AT指令模式	
		ret += usr_ping_web_server(cfg_dft.lux_domain_name);                        //pin 上传数据的服务器
//		ret += usr_set_tcp_server(cfg_dft.lux_domain_name,SERVER_TCP_PORT_NUMBER);  //设置为上传数据的服务器 TCP
		ret += usr_set_udp_server(cfg_dft.lux_domain_name,SERVER_UDP_PORT_NUMBER);  //设置为上传数据的服务器 UDP
		ret += usr_restart();	       //设备重启
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

//模块硬件重启
void ethernet_h_reset(void)
{
	EHT_RESET = 0;
	vTaskDelay(200);
	EHT_RESET = 1;
}	

//发送AT指令，等待响应
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

//以太网模块进入AT指令模式
u8 usr_enter_at_mode(void)
{
	u8 ret = 0;
	ret = usr_send_at_cmd("+++",strlen("+++"),"a",1000);
	ret += usr_send_at_cmd("a",strlen("a"),"+ok",1000);                    //进入AT指令模式
	if(ret) 
		return 1; 
	else 
		return 0;
}

//模块恢复默认参数
u8 usr_restore_default(void) 
{
	return usr_send_at_cmd("AT+RELD\r",strlen("AT+RELD\r"),"+OK=rebooting",1000);
}

//模块重启
u8 usr_restart(void)
{
	u8 ret = 0;
	ret = usr_send_at_cmd("AT+Z\r",strlen("AT+Z\r"),"+OK",1000); 
	vTaskDelay(500);
	return ret;
}

//设置模块的串口波特率
u8 usr_set_uart_baud(u32 baud)
{
	u8 ret = 0;
	char cmd[64] = {0};
	int n = 0;
	n = sprintf(cmd,"AT+UART=%d,8,1,NONE,NFC\r\n",baud);
	ret = usr_send_at_cmd(cmd,n,"+OK",1000); 
	if(ret == 0)
	{
		ret = usr_restart();   //模块重启
		if(ret == 0)
			uart_initial(&uart2,baud);        //重新初始化本地串口波特率
	}
	return ret;
}

//模块退出AT指令模式，进入透传模式
u8 usr_exit_at_mode(void)
{
	return usr_send_at_cmd("AT+ENTM\r",strlen("AT+ENTM\r"),"+OK",1000); 
}

//设置模块IP为DHCP
u8 usr_set_wan_ip(void)
{
	return usr_send_at_cmd("AT+WANN=DHCP\r",strlen("AT+WANN=DHCP\r"),"+OK",1000); 
}

//查询模块动态IP
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

//ping 网站判断是否联网
u8 usr_ping_web_server(char *server_ip)
{
	int n = 0;
    char cmd[64] = {0};
    
    n = sprintf(cmd, "AT+PING=%s\r", server_ip);
    return usr_send_at_cmd(cmd,n,"+OK=SUCCESS", 1000);
}

//设置连接的TCP服务器
u8 usr_set_tcp_server(char *server_ip, char *server_port)
{
	int n = 0;
    char cmd[64] = {0};
    
    n = sprintf(cmd, "AT+SOCK=TCPC,%s,%s\r", server_ip, server_port);
    return usr_send_at_cmd(cmd,n,"+OK", 1000);
}

//设置连接的TCP服务器
u8 usr_set_udp_server(char *server_ip, char *server_port)
{
	int n = 0;
    char cmd[64] = {0};
    
    n = sprintf(cmd, "AT+SOCK=UDPC,%s,%s\r", server_ip, server_port);
    return usr_send_at_cmd(cmd,n,"+OK", 1000);
}

//查询连接 0已连接  1未连接
u8 usr_query_connect(void)
{
	return usr_send_at_cmd("AT+SOCKLK\r",strlen("AT+SOCKLK\r"),"+OK=connect",1000); 
}


//char testaaa[] = "Date: Tue, 21 May 2024 09:43:40 GMT";
//tm usr_tm = {0}; 
//请求服务器信息，从服务器返回信息中得到时间来校准RTC时间
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

					RTC_Set_Time(usr_tm.tm_hour, usr_tm.tm_min, usr_tm.tm_sec, 0);    //设置时间

					usr_tm.tm_mon = getmonth(month);          //月份
					usr_tm.tm_weekday = getweekday(weekday);  //星期几
					year = usr_tm.tm_year - 2000;  
					RTC_Set_Date(year, usr_tm.tm_mon, usr_tm.tm_day, usr_tm.tm_weekday);  //设置日期					
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
 * @brief       依据http协议请求文件头部信息
 * @param       domain：服务器域名
 * @param       path：文件路径
 * @param       total_size：返回文件的总大小
 * @param       timeout：等待超时时间
 * @retval      0  : 请求下载成功
 *              1  : 请求下载失败
 */
//从服务器下载数据，domain： 域名 path：路径 
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
		ptr = strstr((const char *)etherne_pack_buf,"HTTP/1.1 200 OK");    //部分内容
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
			ptr = strstr((const char *)etherne_pack_buf,"404 Not Found");      //未找到 没有文件
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
u8 usr_server_download_request(char* domain,char* path,int* start,int* end,u8* buf,u32* total_size,u32 timeout)
{
	int n = 0,get_start = 0,get_end = 0;
	char cmd[256] = {0};
	char *start_p = NULL;
    char *ptr = NULL; 

    n = sprintf(cmd, "GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%d-%d\r\n\r\n", path,domain,*start,*end);
	memset(etherne_pack_buf,0x00,sizeof(etherne_pack_buf));
	ethernet_data_pack.rec_p = 0;                                                           //接收字节数
	usr_k5_tx((u8*)cmd,n);
	
	while(timeout > 0)
	{
		vTaskDelay(1);
		timeout--;
		start_p = strstr((const char *)etherne_pack_buf,"HTTP/1.1 206 Partial Content");    //部分内容
		if(start_p != NULL)                                        
		{
			ptr = strstr((const char*)etherne_pack_buf,"Content-Range: bytes ");
			if(ptr != NULL)
			{					
				sscanf((const char*)ptr,"Content-Range: bytes %d-%d/%d\r\n",&get_start,&get_end,total_size);  //提取文件数据位置和文件大小信息

				if(*start == get_start && *end == get_end)
				{
					ptr = strstr((const char *)ptr,"\r\n\r\n");                          //分割位置
					if(ptr != NULL)
					{
						if( uart2.que_rx.dlen == 0 )
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
			ptr = strstr((const char *)etherne_pack_buf,"Not Found");      //未找到 没有文件
			if(ptr != NULL)
			{				
				return 1;                                               //未找到文件直接返回
			}
		}
	}
	return 1;
}


