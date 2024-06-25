#ifndef ETHERNET_UPDATE_H_
#define ETHERNET_UPDATE_H_

#include "app.h"


//以太网
#pragma pack(1)
typedef struct
{
	u8  execution_step;           //以太网模块执行步骤
	u8  update_step;              //升级执行步骤
	u8  file_down_step;           //文件下载执行步
	u32 down_timeout;             //超时
	char dns_ip[16];              //服务器IP地址
	char ip[16];                  //IP，本地从路由器分配的IP地址
}ethernet;
#pragma pack()


extern ethernet enthernet_flow;


void eth_update_process(void);
void eth_download_check_file(char* s_fpath,char* mem_fpath);
void eth_download_update_file(char* s_fpath,char* mem_fpath,char* mem_check_fpath,u32 file_size);

#endif




