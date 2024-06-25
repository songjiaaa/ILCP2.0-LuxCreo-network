#include "ui_process.h"


save_ui_id ui_id = {0};

void ui_updata_tx(u8 *p,int n)
{
	uart_send(p,n,&uart6);
}

static u8 hmi_pack_buf[100]={0};
//static u8 syn_buf[]={FHEAD};

COMM_SYNC button_ctrl_pack=
{
	hmi_pack_buf,//u8 *rec_buff;
	sizeof(hmi_pack_buf),//int buf_len;
	FEND,//u32 endc; 
	0,//int rec_p;
	touch_cmd_pro//u8 (*pro)(u8 * b,int len);
};


//#define B   (*(button_control*)b)

button_control b_ctrl = {0};        //按钮数据
read_pic_id r_pic_id = {0};         //读取当前图片
//const char frame_end[] = {0xFF,0xFF,0xFC,0xFF};
u8 touch_cmd_pro(u8 *b ,int len)    
{
//	uart_send(b,len,&uart1);        //串口1回显
	r_pic_id = *(read_pic_id*)b;
	
	return 1;
}


//弹窗提示
void pop_tips(u16 screen_id,u8* t,u8* str)
{
	set_screen(screen_id);
	SetTextValue(screen_id,2,t); 
	SetTextValue(screen_id,3,str); 
}














