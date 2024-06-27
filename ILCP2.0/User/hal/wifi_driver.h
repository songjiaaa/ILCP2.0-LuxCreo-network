#ifndef WIFI_DRIVER_H_
#define WIFI_DRIVER_H_

#include "app.h"
#include "wifi_use.h"
#define ACK_OK_END    0x0D0A         //字符 OK\r\n   

//全球通用NTP服务器
#define NTP_SERVER_URL1       "time.windows.com"     //微软
#define NTP_SERVER_URL2       "time.asia.apple.com"  //苹果


//公司服务器 
#define LUX_SERVER_URL_CN     "service-link.luxcreo.cn"    //国内
#define LUX_SERVER_URL_COM    "service-link.luxcreo.com"   //国外
 
#define SERVER_UDP_PORT_NUMBER    "22098"                  //UDP连接端口
#define SERVER_TCP_PORT_NUMBER    "22099"                  //TCP连接端口


extern COMM_SYNC wifi_data_pack;

void esp8266_uart_tx(u8 *p,int n);
u8 wifi_data_pro(u8 *b,int len);

void esp8266_hw_reset(void);
u8 esp8266_send_at_cmd(char *cmd, u32 len, char *ack, uint32_t timeout);
uint8_t esp8266_restore(void);
uint8_t esp8266_at_test(void);
uint8_t esp8266_set_baud(void);
uint8_t esp8266_set_mode(uint8_t mode);
uint8_t esp8266_sw_reset(void);
uint8_t esp8266_ate_config(uint8_t cfg);
uint8_t esp8266_set_auto_connected(uint8_t cfg);
uint8_t esp8266_query_ap(void);
uint8_t esp8266_join_ap(char *ssid, char *pwd);
uint8_t esp8266_DNS(char *domain,char *ip_buf);
uint8_t esp8266_query_connected_ap(char *ssid,char* mac);
uint8_t esp8266_disconnect_ap(void);
uint8_t esp8266_get_ip(char *buf);
uint8_t esp8266_set_ntp(void);
uint8_t esp8266_get_ntp_time(char *buf);
uint8_t esp8266_connect_tcp_server(char *server_ip, char *server_port);
uint8_t esp8266_connect_udp_server(char *server_ip, char *server_port);
uint8_t esp8266_enter_unvarnished(void);
u8 esp8266_exit_unvarnished(void);
uint8_t esp8266_disconnect_server(void);
u8 query_IPV6_EN(void);
u8 set_IPV6_EN(u8 en);
u8 file_header_request(char* domain, char* path,u32* total_size,u32 timeout);
u8 server_download_request(char* domain, char* path,int* start,int* end,u8* buf,u32* total_size,u32 timeout);

#endif

