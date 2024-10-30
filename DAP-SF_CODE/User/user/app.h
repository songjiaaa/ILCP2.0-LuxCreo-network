#ifndef APP_H
#define APP_H

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"


#include "stm32f4_sys.h"
#include "stm32f4xx.h" 
#include "main.h"
#include "comm_rec.h"
#include "com_fun.h"
#include "f4_uart.h"
#include "my_queue.h"

#include "cmd.h"
#include "config.h"
#include "f4_led.h"
#include "gpio_init.h"
#include "dwt_delay.h"
#include "tim.h"

#include "adc.h"
#include "ntc.h"  

//#include "rtc.h"


#include "w25qxx.h"  
#include "spi.h"  
#include "sdio_sdcard.h"    
#include "ws2812b.h"

#include "usbh_usr.h" 

#include "ff.h"
#include "diskio.h"
#include "file_handing.h"

#include "config.h"
#include "version.h"

#include "hx711.h"
#include "rfid.h"

extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef RTC_DateStruct;


extern USBH_HOST USB_Host;
extern USB_OTG_CORE_HANDLE  USB_OTG_Core;
extern SD_Error SD_state;




extern TaskHandle_t debug_task_handler;
void debug_task(void *pvParameters);

extern TaskHandle_t rfid1_task_handler;
void rfid1_task(void *pvParameters);

extern TaskHandle_t rfid2_task_handler;
void rfid2_task(void *pvParameters);

void modbus_pro_task(void * pvParameters);

extern TaskHandle_t start_task_handler;
void start_task(void *pvParameters);

extern TaskHandle_t software_timer_handler;
void software_timer_task( void * pvParameters );

extern TaskHandle_t get_sensor_data_handler;
void get_sensor_data_task( void * pvParameters );

extern TaskHandle_t init_task_handler;
void init_task(void * pvParameters);







//modbus addr  ��Ӧ���ݶ�Ӧ��modbus�Ĵ�����ַ
#define  REG_START_ADDR   0x05A0          //��ʼ��ַ

#define  BAND_ADDR        0x05A0          //����Ʒ����
#define  RESIN_ADDR       0x05E0          //��֬��
#define  VOLUME_ADDR      0x061E          //��ƿ����

#define  WEIGHT_ADDR      0x0620          //��֬������
#define  LEAP_MAX_NUM_ADDR     0x0622          //����Ĥ���ʹ�ô���
#define  LEAP_REMAIN_NUM_ADDR  0x0624          //����Ĥʣ��ʹ�ô���
#define  LEAP_NUM_ADDR    0x0626               //Ĥʹ�ô���
#define  LEAP_NAME_ADDR   0x0630               //Ĥ����
#define  REMAIN_RESIN_WEIGHT   0x0660          //д��ʣ����֬����
#define  WEIGHT_ZERO_ADDR      0x0662          //���ع���
#define  GET_SN_ADDR           0x0670          //SN

#define  IO_INPUT_STATE_ADDR   0x0690          //IO������
#define  IO_OUT_STATE_ADDR     0x0691          //IO������

#define  HEARTBEAT_FRAME_ADDR  0x0688          //�������

#define  PROGRAM_VERSION_ADDR  0x06A0          //�汾��Ϣ��ѯ

#define  FILE_TRANSFER_FLAGS_ADDR   0x0800          //�ļ������־   1�����ļ�   0 �����ļ�����
#define  RECEIVED_FILE_SIZE_ADDR    0x06FF          //���յ��ļ���С
#define  RECEIVED_FILE_DATA_ADDR    0x0700          //�����ļ�����
#define  START_UPDATE_COMMAND_ADDR  0x0801          //�������±�־


//��Ȧ�Ĵ���
#define  IP_CAMERA_POWER_REG   0x0680        //IP�����Դ
#define  LIQUID_IN_SWITCH_REG  0x0681        //עҺ���Ͽ���



#define  BAND_REG        (BAND_ADDR - REG_START_ADDR)          //����Ʒ����
#define  RESIN_REG        (RESIN_ADDR - REG_START_ADDR)          //��֬��
#define  VOLUME_REG     (VOLUME_ADDR - REG_START_ADDR)          //��ƿ����
#define  WEIGHT_REG       (WEIGHT_ADDR - REG_START_ADDR)          //��֬������
#define  REMAIN_RESIN_WEIGHT_REG    (REMAIN_RESIN_WEIGHT - REG_START_ADDR)          //д��ʣ����֬����


#define  LEAP_MAX_NUM_REG      (LEAP_MAX_NUM_ADDR - REG_START_ADDR)          //����Ĥ���ʹ�ô���
#define  LEAP_REMAIN_NUM_REG   (LEAP_REMAIN_NUM_ADDR - REG_START_ADDR)          //����Ĥʣ��ʹ�ô���
#define  LEAP_NUM_REG     (LEAP_NUM_ADDR - REG_START_ADDR)               //д��Ĥʹ�ô���
#define  LEAP_NAME_REG    (LEAP_NAME_ADDR - REG_START_ADDR)               //Ĥ����

#define  GET_SN_REG            (GET_SN_ADDR - REG_START_ADDR)           //SN
#define  WEIGHT_ZERO_REG       (WEIGHT_ZERO_ADDR - REG_START_ADDR)          //���ع���

#define  IO_INPUT_STATE_REG    (IO_INPUT_STATE_ADDR - REG_START_ADDR)          //IO������)          //IO������
#define  IO_OUT_STATE_REG      (IO_OUT_STATE_ADDR - REG_START_ADDR)          //IO������

#define  HEARTBEAT_FRAME_REG   (HEARTBEAT_FRAME_ADDR - REG_START_ADDR)          //�������

#define  PROGRAM_VERSION_REG   (PROGRAM_VERSION_ADDR - REG_START_ADDR)          //�汾��Ϣ��ѯ

#define  FILE_TRANSFER_FLAGS_REG    (FILE_TRANSFER_FLAGS_ADDR - REG_START_ADDR)          //�ļ������־   1�����ļ�   0 �����ļ�����
#define  RECEIVED_FILE_SIZE_REG     (RECEIVED_FILE_SIZE_ADDR - REG_START_ADDR)          //���յ��ļ���С
#define  RECEIVED_FILE_DATA_REG     (RECEIVED_FILE_DATA_ADDR - REG_START_ADDR)          //�����ļ�����
#define  START_UPDATE_COMMAND_REG   (START_UPDATE_COMMAND_ADDR - REG_START_ADDR)          //�������±�־




//IO������
#pragma pack(1)
typedef struct
{
    u16 material:             1,   //�Ϻе�λ���
        bucket:               1,   //��Ͳ��λ���
        liquid_max:           1,   //עҺ�����
        liquid_overflow:      1,   //עҺ������
        surplus_material:     1,   //�Ϻ�ʣ����
        surplus_bucket:       1,   //��Ͳʣ����
        empty_cantilever:     1,   //�����ۼ��
        bucket_rfid_stete:    1,   //��ͲRFID״̬
        material_rfid_stete:  1,   //�Ϻ�RFID״̬
        reserved:             7;  //����λ
}in_state_t;
#pragma pack()

//IO������
#pragma pack(1)
typedef struct
{
    u16 ip_camera_power:      1,   //IP�����Դ
        liquid_in_switch:     1,   //עҺ���Ͽ���
        reserved:             14;   //����λ
}out_state_t;
#pragma pack()


#pragma pack(1)
typedef struct
{
	int resin_weight;    //��֬����
    u32 write_resin_weight;   //д����֬����
	u32 resin_volume;    //��֬�ݻ�
    u8 resin_band[30];   //��֬Ʒ����
	u8 resin_name[30];   //��֬��

	u32 leap_max_num;    //����Ĥ���ʹ�ô���
	u32 leap_remain_num; //����Ĥʣ��ʹ�ô���
	u32 writeleap_num;   //д��Ĥʹ�ô���
	u8 leap_name[30];    //Ĥ����

	u8 sn[30];   //SN
    int weight_zero;   //���ع���

	in_state_t io_input_state;   //IO������
	out_state_t io_out_state;   //IO������
	

	u8 heartbeat_frame;   //�������
	u8 program_version[30];   //�汾��Ϣ��ѯ
	u8 file_transfer_flags;   //�ļ������־   1�����ļ�   0 �����ļ�����
	u32 received_file_size;   //���յ��ļ���С
	u8 received_file_data[256];   //�����ļ�����
	u8 start_update_command;   //�������±�־
}modbus_data_t;
#pragma pack()

extern modbus_data_t m_data_t;

#endif

