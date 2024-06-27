#ifndef _CUSTOM_H__
#define _CUSTOM_H__


#include "app.h"



#define USER_FILE  "2:User.txt"


#define CUSTOM_MAX_STEP   4
#define CURE_MAX_TIME     480       



void custom_select(u16 id);

void show_cur_custom_page(u16 cur_page);
void read_custom_info(void);
void add_custom_info(void);
void custom_input_pro(u16 touch_id,u8 *p,int len);
void delete_para(void);
void save_para(void);
u32 input_curing_time(void);


DSTATUS write_user_parameter(const TCHAR* path,const void *buff,u32 len);
DSTATUS read_user_parameter(const TCHAR* path,void *buff,u32 len);

#endif


