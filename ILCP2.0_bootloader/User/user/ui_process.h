#ifndef UI_PROCESS_H
#define UI_PROCESS_H

#include "app.h"

//frame header  
#define  FHEAD                 0xEE 
#define  FEND                  0xFFFCFFFF


//��¼����
#pragma pack(1)
typedef struct
{
	u16  pre_picture;       //ǰһҳͼ
	u16  cur_picture;       //��ǰͼ    
}save_ui_id;
#pragma pack()

//��ȡ����
#pragma pack(1)
typedef struct
{
	u8   head;     
	u16  cmd;                //ָ������
	u16  pic_id;             //ͼƬID
	u32  end;                //֡β
}read_pic_id;
#pragma pack()

//��ť�ؼ�ID�ϴ�
#pragma pack(1)
typedef struct
{
	u8   head;     
	u16  cmd;                //ָ������
	u16  pic_id;             //ͼƬID
	u16  touch_id;           //����ID
	u8   button_type;        //��ť����
   	u8   button_attribute;   //��ť����
	u8   buttin_state;       //��ť״̬
	u32  end;                //֡β
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




