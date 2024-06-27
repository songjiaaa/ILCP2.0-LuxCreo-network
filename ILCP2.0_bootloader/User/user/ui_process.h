#ifndef UI_PROCESS_H
#define UI_PROCESS_H

#include "app.h"

//frame header  
#define  FHEAD                 0xEE 
#define  FEND                  0xFFFCFFFF


//记录画面
#pragma pack(1)
typedef struct
{
	u16  pre_picture;       //前一页图
	u16  cur_picture;       //当前图    
}save_ui_id;
#pragma pack()

//读取画面
#pragma pack(1)
typedef struct
{
	u8   head;     
	u16  cmd;                //指令类型
	u16  pic_id;             //图片ID
	u32  end;                //帧尾
}read_pic_id;
#pragma pack()

//按钮控件ID上传
#pragma pack(1)
typedef struct
{
	u8   head;     
	u16  cmd;                //指令类型
	u16  pic_id;             //图片ID
	u16  touch_id;           //触控ID
	u8   button_type;        //按钮类型
   	u8   button_attribute;   //按钮属性
	u8   buttin_state;       //按钮状态
	u32  end;                //帧尾
}button_control;
#pragma pack()

extern COMM_SYNC button_ctrl_pack;
extern save_ui_id ui_id;

u8 touch_cmd_pro(u8 *b ,int len);
void pop_up_main(u16 touch_id);
void pop_tips(u16 screen_id,u8* t,u8* str);


/******************************************************************************/
#define WARNING         "Warning"
#define TIPS            "Tips"

#define SAVE_OK         "Save successfully!"
#define CLOSE_DOOR      "Please close the door before curing!"
#define SELECT_FILE     "Please select a file."
#define EN_PARAMETER    "Please enter the parameter"
#define EN_NAME         "Please enter resin name"
#define CURING_TIME     "The curing time should be less than 480 min."
#define MANY_FILES      "Too many files!"



/*******************************************************************************/
//u disk
#define USB_DISK_CONNECTION      "USB flash drive insertion detected."
#define USB_DISK_DISCONNECT      "USB device unplugged!"
#define USB_DISK_UNKNOWN		 "Unrecognized USB device."
#define NO_USB_DISK              "No flash drive detected."


#endif




