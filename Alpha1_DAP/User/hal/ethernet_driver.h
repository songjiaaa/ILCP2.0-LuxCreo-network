#ifndef ETHERNET_DRIVER_H_
#define ETHERNET_DRIVER_H_

#include "app.h"

#define USR_K5_UART_BAUD     230400

extern char eth_server_ip[30];
extern COMM_SYNC ethernet_data_pack;

void usr_k5_tx(u8 *p,int n);
u8 entherne_data_pro(u8 *b ,int len); 

u8 usr_k5_init(void);
void ethernet_module_run(void);
void ethernet_h_reset(void);
u8 usr_send_at_cmd(char *cmd, u32 len, char *ack, uint32_t timeout);
u8 usr_enter_at_mode(void);
u8 usr_restore_default(void);
u8 usr_restart(void);
u8 usr_set_uart_baud(u32 baud);
u8 usr_exit_at_mode(void);
u8 usr_set_wan_ip(void);
u8 usr_query_wan_ip(char *buf);
u8 usr_ping_web_server(char *server_ip);
u8 usr_set_tcp_server(char *server_ip, char *server_port);
u8 usr_set_udp_server(char *server_ip, char *server_port);
u8 usr_query_connect(void);
u8 usr_cal_rtc_time(char* domain, char* path,u32 timeout);

u8 usr_file_header_request(char* domain, char* path,u32* total_size,u32 timeout);
u8 usr_server_download_request(char* domain,char* path,int* start,int* end,u8* buf,u32* total_size,u32 timeout);

#endif












