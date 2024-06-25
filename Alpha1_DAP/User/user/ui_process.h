#ifndef UI_PROCESS_H
#define UI_PROCESS_H

#include "app.h"

//frame header  
#define  FHEAD               0xEE 
#define  FEND                0xFFFCFFFF


//��¼����
#pragma pack(1)
typedef struct
{
	u16  pre_picture;        //ǰһҳͼ
	u16  cur_picture;        //��ǰͼ    
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
extern u16 scene_mode;


void ui_updata_tx(u8 *p,int n);
u8 touch_cmd_pro(u8 *b ,int len);
void pop_up_main(u16 touch_id);
void pop_tips(u16 screen_id,u8* t,u8* str);


/******************************************************************************/
#define WARNING         "Warning"
#define TIPS            "Tips"
#define DELETE          "Delate"
#define WIFI_ERROR      "Error"
#define CHECK_UPDATE    "Check for Update"
#define	NO_NETWORK		"No network"
#define INITIALIZE      "Initialize"
#define HMI_SUCCEEDED   "Succeeded"
#define HMI_FAILED      "Failed"

#define WIFI_INIT       "WiFi module initialization..."
#define ETHERNET_INIT   "Ethernet module initialization..."

#define PLEASE_WAIT     "Please wait..."                        //��ȴ�
#define SAVE_OK         "Save successfully!"                    //����ɹ�
#define CLOSE_DOOR      "Please close the door before curing!"  //������ʾ
#define SELECT_FILE     "Please select a file."                 //��ѡ��һ���ļ�
#define EN_PARAMETER    "Please enter the parameter"            //����ѡ�в���
#define EN_NAME         "Please enter resin name"               //�������������
#define CURING_TIME     "The curing time should be less than 480 min."  //�̻���ʱ�䲻�ܳ���480����
#define MANY_FILES      "Too many files!"                       //�ļ�����
  

/*******************************************************************************/
#define NO_OTA_UPDATES              "No network connection, cannot perform OTA updates!" //���������ӣ�����ִ��OTA���£�
#define EN_WIFI_PASSWORD            "Please enter the WiFi password."             //������WiFi���롣
#define WIFI_CONNECTION_WAIT	    "Please wait for Wi-Fi connection..."         //��ȴ�wifi����
#define WIFI_CONNECTION_TIMEOUT     "Wi-Fi connection timed out..."               //wifi���ӳ�ʱ
#define WIFI_INIT_ERROR             "Wi-Fi module initialization failed!"         //WiFiģ���ʼ��ʧ��
#define NO_WIFI_                    "No wifi was found."                          //û���ҵ������ӵ�WiFi
#define SELECT_WIFI                 "Please select a Wi-Fi first."                //����ѡ��һ��wifi
#define WIFI_DISCONNECTED           "The Wi-Fi connection has been disconnected." //wifi�����ѶϿ�
#define WIFI_GET_IP_ERR             "Failed to obtain IP address!"                //��ȡIP��ַʧ��
#define RTC_INIT_ERROR              "If the RTC peripheral fails to be initialized, the time of data upload will be affected." //RTC�����ʼ��ʧ�ܣ���Ӱ�������ϴ���ʱ���¼��
#define CONFIGURE_NTP_FAILED        "Failed to configure the NTP server!"         //����NTP������ʧ��
#define WIFI_FILES_ERROR            "WiFi file reading error!"                    //WiFi�ļ���ȡ����
#define WIFI_AUTO_CONNECT_FAILED    "WiFi automatic connection failed, please manually select the available WiFi connection!" //WiFi�Զ�����ʧ�ܣ������ֶ�ѡ����õ�WiFi�������ӣ�
#define NET_TM_CAL_ERROR            "Network time calibration failed!"            //����ʱ��У׼ʧ�ܣ�
#define F_UPDATE_REQUEST_FAIL		"The file update request failed."             //�����ļ�����ʧ�ܡ�
/*********************************************************************************/
#define ETHERNET_INIT_FAIL          "Ethernet peripheral initialization failed, please check the Ethernet module hardware connection!"    //��̫�������ʼ��ʧ�ܣ�������̫��ģ���Ӳ�����ӣ�
#define ETHERNET_UPDATE_FAIL        "Data request failed, exit update!"           //��������ʧ�ܣ��˳����£�
#define REQUEST_TIMED_OUT           "Request timed out, unable to update!"        //����ʱ���޷�����

/*******************************************************************************/
//u disk
#define USB_DISK_CONNECTION      "USB flash drive insertion detected."   //USB�豸����
#define USB_DISK_DISCONNECT      "USB device unplugged!"                 //USB�豸�Ͽ�����
#define USB_DISK_UNKNOWN		 "Unrecognized USB device."              //�޷�ʶ���USB�豸
#define NO_USB_DISK              "No flash drive detected."              //û�м�⵽USB�豸


//�ļ�����
#define FILE_IM_SUCCEEDED        "Parameter file imported successfully!"               //�����ļ�����ɹ�
#define FILE_IM_FAIL             "Parameter file import failed!"                       //�����ļ�����ʧ��
#define NO_FILE                  "No file!"                             //û���ļ�
#define MEMORY_ERROR             "memory error!"                        //�ڴ����
#define MEMORY_MOUNTFAIL         "Failed to mount memory!"              //�����ڴ�ʧ��
#define FORMAT_FAILED            "Memory formatting failed!"            //�ڴ��ʽ��ʧ��
#define USB_FILE_OPEN_FAIL       "Failed to open the USB flash drive file!"  //u���ļ���ʧ��
#define USB_FILE_READ_FAIL 		 "Failed to read files from the USB flash drive!"   //u���ļ���ȡʧ��
#define FILE_LARGE               "the file is too large!"               //�ļ�����
#define OTA_DOWNLOAD_MEM_ERROR   "The memory read/write error prevented OTA data download."     //�ڴ��д�����޷�����OTA�������ء�
#define DATA_LOAD_FAILED         "Data loading failed and the OTA was exited."  //���ݼ���ʧ�ܣ��˳�OTA��
#define MEM_SPACE_INSUFFF        "The memory space is insufficient."     //�ڴ�ռ䲻��
 
#define SET_OK                   "OK!" 
#define SET_ERROR                "ERROR!" 
#define INPUT_PASSWORD           "Please enter password"           
#define PASSWORD_ERROR           "Enter the wrong password!"
#define ENTER_SN                 "Please enter a new SN"


#define SET_PTC_110V             "The operating voltage of the PTC is set to 110V." //PTCʹ�õ�ѹ������Ϊ110V.
#define SET_PTC_220V             "The operating voltage of the PTC is set to 220V." //PTCʹ�õ�ѹ������Ϊ110V.

//����ѯ��
#define IS_CLEAR_CURE_TIME       "Is the total cure time cleared?"   //�Ƿ�����ܹ̻�ʱ����
#define USER_PARA_DELETE         "Are you sure to delete?"           //�Ƿ�ɾ����
#define CLEAR_TOTAL_TIME         "Total curing time record cleared!" //�̻���ʱ���¼����
#define USER_PARA_CLEARED        "User parameters cleared!"          //�û��������
#define LOCAL_PARA_CLEARED       "Local parameters cleared!"         //���ز������

//��������ʾ
#define	FILE_DELETE_ERR	         "Error: File deletion exception!"   //�ļ�ɾ���쳣
#define	FILE_RW_ERR				 "Error: File read/write exception!" //������ʾ �ļ���д�쳣
#define WRITE_SN_ERR             "Exception: Writing SN failed!"     //�쳣��д��SN����
#define READ_SN_ERR              "Exception: SN reading error!"      //�쳣����ȡSN����
#define MEMORY_UPGRADE_ERR       "Failed to upgrade due to memory error."  //�ڴ���ش����޷�����
#define UPGRADE_PATH_ERR         "Update file not detected in valid path!" //û������Ч·���¼�⵽�����ļ�!
#define MEM_WRITE_UPGRADE_ERROR  "Memory writing error, can not be updated properly!"     //�ڴ�д����󣬲����������£�
#define NO_LOCAL_PARA            "Local parameter does not exist!"   //���ز���������
#define NO_REQYEST_FIRMWARE      "No request to firmware program."   //û�����󵽹̼�
#define EQYEST_UPDATE_FIRMWARE   "Request to update file, please wait..."    //������������ļ�����Ⱥ�...




#endif




