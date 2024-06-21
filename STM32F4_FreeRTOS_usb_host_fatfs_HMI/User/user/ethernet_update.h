#ifndef ETHERNET_UPDATE_H_
#define ETHERNET_UPDATE_H_

#include "app.h"


//��̫��
#pragma pack(1)
typedef struct
{
	u8  execution_step;           //��̫��ģ��ִ�в���
	u8  update_step;              //����ִ�в���
	u8  file_down_step;           //�ļ�����ִ�в�
	u32 down_timeout;             //��ʱ
	char dns_ip[16];              //������IP��ַ
	char ip[16];                  //IP�����ش�·���������IP��ַ
}ethernet;
#pragma pack()


extern ethernet enthernet_flow;


void eth_update_process(void);
void eth_download_check_file(char* s_fpath,char* mem_fpath);
void eth_download_update_file(char* s_fpath,char* mem_fpath,char* mem_check_fpath,u32 file_size);

#endif




