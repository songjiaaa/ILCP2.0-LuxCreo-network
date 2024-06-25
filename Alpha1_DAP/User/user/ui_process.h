#ifndef UI_PROCESS_H
#define UI_PROCESS_H

#include "app.h"

//frame header  
#define  FHEAD               0xEE 
#define  FEND                0xFFFCFFFF


//记录画面
#pragma pack(1)
typedef struct
{
	u16  pre_picture;        //前一页图
	u16  cur_picture;        //当前图    
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

#define PLEASE_WAIT     "Please wait..."                        //请等待
#define SAVE_OK         "Save successfully!"                    //保存成功
#define CLOSE_DOOR      "Please close the door before curing!"  //关门提示
#define SELECT_FILE     "Please select a file."                 //请选中一个文件
#define EN_PARAMETER    "Please enter the parameter"            //请先选中参数
#define EN_NAME         "Please enter resin name"               //请输入材料名称
#define CURING_TIME     "The curing time should be less than 480 min."  //固化总时间不能超过480分钟
#define MANY_FILES      "Too many files!"                       //文件已满
  

/*******************************************************************************/
#define NO_OTA_UPDATES              "No network connection, cannot perform OTA updates!" //无网络连接，不能执行OTA更新！
#define EN_WIFI_PASSWORD            "Please enter the WiFi password."             //请输入WiFi密码。
#define WIFI_CONNECTION_WAIT	    "Please wait for Wi-Fi connection..."         //请等待wifi连接
#define WIFI_CONNECTION_TIMEOUT     "Wi-Fi connection timed out..."               //wifi连接超时
#define WIFI_INIT_ERROR             "Wi-Fi module initialization failed!"         //WiFi模块初始化失败
#define NO_WIFI_                    "No wifi was found."                          //没有找到可连接的WiFi
#define SELECT_WIFI                 "Please select a Wi-Fi first."                //请先选中一个wifi
#define WIFI_DISCONNECTED           "The Wi-Fi connection has been disconnected." //wifi连接已断开
#define WIFI_GET_IP_ERR             "Failed to obtain IP address!"                //获取IP地址失败
#define RTC_INIT_ERROR              "If the RTC peripheral fails to be initialized, the time of data upload will be affected." //RTC外设初始化失败，会影响数据上传的时间记录。
#define CONFIGURE_NTP_FAILED        "Failed to configure the NTP server!"         //设置NTP服务器失败
#define WIFI_FILES_ERROR            "WiFi file reading error!"                    //WiFi文件读取错误！
#define WIFI_AUTO_CONNECT_FAILED    "WiFi automatic connection failed, please manually select the available WiFi connection!" //WiFi自动连接失败，请在手动选择可用的WiFi进行连接！
#define NET_TM_CAL_ERROR            "Network time calibration failed!"            //网络时间校准失败！
#define F_UPDATE_REQUEST_FAIL		"The file update request failed."             //更新文件请求失败。
/*********************************************************************************/
#define ETHERNET_INIT_FAIL          "Ethernet peripheral initialization failed, please check the Ethernet module hardware connection!"    //以太网外设初始化失败，请检查以太网模块的硬件连接！
#define ETHERNET_UPDATE_FAIL        "Data request failed, exit update!"           //数据请求失败，退出更新！
#define REQUEST_TIMED_OUT           "Request timed out, unable to update!"        //请求超时，无法更新

/*******************************************************************************/
//u disk
#define USB_DISK_CONNECTION      "USB flash drive insertion detected."   //USB设备连接
#define USB_DISK_DISCONNECT      "USB device unplugged!"                 //USB设备断开连接
#define USB_DISK_UNKNOWN		 "Unrecognized USB device."              //无法识别的USB设备
#define NO_USB_DISK              "No flash drive detected."              //没有检测到USB设备


//文件拷贝
#define FILE_IM_SUCCEEDED        "Parameter file imported successfully!"               //参数文件导入成功
#define FILE_IM_FAIL             "Parameter file import failed!"                       //参数文件导入失败
#define NO_FILE                  "No file!"                             //没有文件
#define MEMORY_ERROR             "memory error!"                        //内存错误
#define MEMORY_MOUNTFAIL         "Failed to mount memory!"              //挂载内存失败
#define FORMAT_FAILED            "Memory formatting failed!"            //内存格式化失败
#define USB_FILE_OPEN_FAIL       "Failed to open the USB flash drive file!"  //u盘文件打开失败
#define USB_FILE_READ_FAIL 		 "Failed to read files from the USB flash drive!"   //u盘文件读取失败
#define FILE_LARGE               "the file is too large!"               //文件过大
#define OTA_DOWNLOAD_MEM_ERROR   "The memory read/write error prevented OTA data download."     //内存读写错误，无法进行OTA数据下载。
#define DATA_LOAD_FAILED         "Data loading failed and the OTA was exited."  //数据加载失败，退出OTA。
#define MEM_SPACE_INSUFFF        "The memory space is insufficient."     //内存空间不足
 
#define SET_OK                   "OK!" 
#define SET_ERROR                "ERROR!" 
#define INPUT_PASSWORD           "Please enter password"           
#define PASSWORD_ERROR           "Enter the wrong password!"
#define ENTER_SN                 "Please enter a new SN"


#define SET_PTC_110V             "The operating voltage of the PTC is set to 110V." //PTC使用电压已设置为110V.
#define SET_PTC_220V             "The operating voltage of the PTC is set to 220V." //PTC使用电压已设置为110V.

//弹窗询问
#define IS_CLEAR_CURE_TIME       "Is the total cure time cleared?"   //是否清空总固化时长？
#define USER_PARA_DELETE         "Are you sure to delete?"           //是否删除？
#define CLEAR_TOTAL_TIME         "Total curing time record cleared!" //固化总时间记录清零
#define USER_PARA_CLEARED        "User parameters cleared!"          //用户参数清空
#define LOCAL_PARA_CLEARED       "Local parameters cleared!"         //本地参数清空

//不正常提示
#define	FILE_DELETE_ERR	         "Error: File deletion exception!"   //文件删除异常
#define	FILE_RW_ERR				 "Error: File read/write exception!" //错误提示 文件读写异常
#define WRITE_SN_ERR             "Exception: Writing SN failed!"     //异常：写入SN错误！
#define READ_SN_ERR              "Exception: SN reading error!"      //异常：读取SN错误！
#define MEMORY_UPGRADE_ERR       "Failed to upgrade due to memory error."  //内存挂载错误无法升级
#define UPGRADE_PATH_ERR         "Update file not detected in valid path!" //没有在有效路径下检测到更新文件!
#define MEM_WRITE_UPGRADE_ERROR  "Memory writing error, can not be updated properly!"     //内存写入错误，不能正常更新！
#define NO_LOCAL_PARA            "Local parameter does not exist!"   //本地参数不存在
#define NO_REQYEST_FIRMWARE      "No request to firmware program."   //没有请求到固件
#define EQYEST_UPDATE_FIRMWARE   "Request to update file, please wait..."    //正在请求更新文件，请等候...




#endif




