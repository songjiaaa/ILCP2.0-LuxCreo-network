#ifndef _WIFI_USE_H_
#define _WIFI_USE_H_

#include "app.h"

#define SHOW_WIFI_LINE       5  
#define SHOW_MAX_WIFI_LIST   32
#define WIFI_NAME_BUF_SIZE   64




#pragma pack(1)
typedef struct 
{
	u8  execution_step;           //初始化步骤  0:开始初始化 1:WiFi列表刷新 2:检测是否需要自动连接 3:连接  4;进行NTP时间校准 5:执行OTA升级 6:空闲不执行操作
	u8  connect_state     :1,     //连网状态    0:未联网  1:联网
		connect_flag      :1,     //连接标志        0:不执行连接  1:执行一次连接
		auto_connect      :1,     //0 为首次连接  1;已经连接过
		sntp_cal 	      :1,     //1:执行NTP时间校准
		trans_mode	      :4;     //连接状态 0:正常连接  1:UDP透传  2:TCP连接
	u8  ota_run_step;             //OTA升级执行步骤
	u8  file_down_step;           //文件下载执行步
	u32 down_timeout;             //下载超时
}network_flow_type;
#pragma pack()


//wifi信息
#pragma pack(1)
typedef struct
{
	int enc;         //加密方式
	char ssid[WIFI_NAME_BUF_SIZE];     //ap 名
	int rssi;        //信号强度
	char mac[32];    //mac地址
	int channel;     //通道
	char password[WIFI_NAME_BUF_SIZE]; //密码
	char ip[16];     //IP
}ap_info;
#pragma pack()


//wifi列表翻页
#pragma pack(1)
typedef struct
{
	u8 cur_page;         //当前页
	u8 total_page;       //总页数
	u8 total_line;       //总行数
	u8 select_num;       //选中名
}wifi_page;
#pragma pack()


//extern wifi_user wifi_app_user;
extern network_flow_type wifi_flow;
extern ap_info cur_connected_ap;  //当前连接的AP
extern ap_info record_ap[SHOW_MAX_WIFI_LIST];

u8 wifi_init_process(void);
void wifi_module_run(void);
u8 wifi_list_refresh(void);
u8 wifi_link_ctrl(void);
void parse_ap_name(u8 *str);
void remove_duplicates(void);
void show_wifi_list(u8 cur_page);
void wifi_list_ctrl(u16 touch_id);
void wifi_input_pro(u16 touch_id,u8 *p,int len);

void wifi_check_box_icon(u16 touch_id);

#endif



