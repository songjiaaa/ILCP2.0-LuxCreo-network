#include "data_upload.h"
//整包包头
upload_frame_head head = {0};

//空闲状态下上传的数据
idle_upload_frame idle_data = {0};           //空闲或固化结束状态下上传数据包

//固话中上传的数据
cure_upload_front cure_data_head = {0};      //固话中或固化暂停上传的数据包
char upload_cure_para_data[500] = {0};       //固化中数据组包
cure_upload_tail cure_data_tail = {0};       //固化格式结尾

upload_end end = {0};

u8 upload_data_buf[1000] = {0};
//t_mode传输模式：0x00 WiFi上传  0x01 以太网上传
//type 帧类型：0x01 空闲状态下上传   0x02 固化或暂停状态下上传   0x03 固化参数或配置导入   0x04 OTA升级    0x05 错误上报
void netword_package_upload(u8 t_mode,u8 type)
{

	head.head = 0xa55a;
	head.len = 0;

	head.t_mode = t_mode;
	head.t_dir = UPLOADING_DIRECTION;
	head.type = type;
	memcpy(head.version,cfg_dft.version,sizeof(head.version));   //版本号
	
	memset(upload_data_buf,0x00,sizeof(upload_data_buf));
	if(type == 0x01)          //空闲或结束状态上传数据
	{
		memset(idle_data.device_name,0x00,sizeof(idle_data.device_name));
		memset(idle_data.SN,0x00,sizeof(idle_data.SN));

		memcpy(idle_data.device_name,DEVICE_NAME,strlen(DEVICE_NAME));
		memcpy(idle_data.SN,cfg_dft.sn,sizeof(idle_data.SN));

		idle_data.status = cure.mode;
		idle_data.uv_power_cal = save_config.uv_cal_k;
		idle_data.up_uv_k = save_config.up_uv_k;
		idle_data.down_uv_k = save_config.down_uv_k;
		idle_data.heat_cal = save_config.heat_cal;
		idle_data.uv_heat_cal = save_config.uv_heat_cal;
		idle_data.door_state = DOOR_DET;
		idle_data.mac_lock_sta = MAC_DOOR_LOCK;
		idle_data.uv_total_time = save_time.uv_total_time;
		
		head.len = sizeof(upload_frame_head) + sizeof(idle_upload_frame) + sizeof(upload_end);

		memcpy(upload_data_buf,&head,sizeof(upload_frame_head));
		memcpy(&upload_data_buf[sizeof(upload_frame_head)],&idle_data,sizeof(idle_upload_frame));	
		end.crc = cal_crc(upload_data_buf,sizeof(upload_frame_head) + sizeof(idle_upload_frame));
		end.end = 0xFFFF;
		memcpy(&upload_data_buf[sizeof(upload_frame_head) + sizeof(idle_upload_frame)],&end,sizeof(upload_end));
	}
	else if(type == 0x02)     //固化中或暂停 上传数据
	{
		extern page_record custom_page;
		memset(cure_data_head.device_name,0x00,sizeof(cure_data_head.device_name));
		memset(cure_data_head.SN,0x00,sizeof(cure_data_head.SN));

		//前段字段
		memcpy(cure_data_head.device_name,DEVICE_NAME,strlen(DEVICE_NAME));
		memcpy(cure_data_head.SN,cfg_dft.sn,sizeof(cure_data_head.SN));
		cure_data_head.status = cure.mode;
		//参数字段      //此处需要识别从Cure固化 还是Custom固化
		memset(upload_cure_para_data,0x00,sizeof(upload_cure_para_data));
		if(custom_page.select_num != -1) //custom
		{
			sprintf( upload_cure_para_data,"Resin:%s\r\nStep:01  Up_UV:%d  Down_UV:%d  Heat:%d  Time:%d\r\nStep:02  Up_UV:%d  Down_UV:%d  Heat:%d  Time:%d\r\n",
			exe_parameter.resin,\
			exe_parameter.up_uv[0],exe_parameter.down_uv[0],exe_parameter.heat[0],exe_parameter.time[0],\
			exe_parameter.up_uv[1],exe_parameter.down_uv[1],exe_parameter.heat[1],exe_parameter.time[1]);
			memcpy(cure_data_tail.app_area,"Custom",sizeof("Custom"));			
		}
		else
		{
			sprintf( upload_cure_para_data,"%s\r\nResin:%s\r\nStep:01  Up_UV:%d  Down_UV:%d  Heat:%d  Time:%d\r\nStep:02  Up_UV:%d  Down_UV:%d  Heat:%d  Time:%d\r\n",
			(char*)resin_app_area[scene_mode],exe_parameter.resin,\
			exe_parameter.up_uv[0],exe_parameter.down_uv[0],exe_parameter.heat[0],exe_parameter.time[0],\
			exe_parameter.up_uv[1],exe_parameter.down_uv[1],exe_parameter.heat[1],exe_parameter.time[1]);		
			memcpy(cure_data_tail.app_area,resin_app_area[scene_mode],sizeof(cure_data_tail.app_area));
		}
		//后段字段
		memcpy(cure_data_tail.resin,exe_parameter.resin,sizeof(cure_data_tail.resin));

		cure_data_tail.step = cure.step + 1;
		cure_data_tail.step_time = exe_parameter.time[cure.step];
		cure_data_tail.up_uv_per = exe_parameter.up_uv[cure.step];
		cure_data_tail.down_uv_per = exe_parameter.down_uv[cure.step];
		cure_data_tail.step_temp = exe_parameter.heat[cure.step];
		cure_data_tail.cure_total_time = cure.total_time;
		cure_data_tail.r_time = cure.r_time;
		cure_data_tail.ptc_temp = sensor_data.ptc_t;
		cure_data_tail.up_uv_temp = sensor_data.uv_led1_t;
		cure_data_tail.down_uv_temp = sensor_data.uv_led2_t;
		cure_data_tail.head_lamp = WLED_PWR_EN;
		cure_data_tail.motor_state = MOTOR_DIR;
		head.len = sizeof(upload_frame_head) + sizeof(cure_upload_front) + strlen(upload_cure_para_data) + sizeof(cure_data_tail) + sizeof(upload_end);
		
		memcpy(upload_data_buf,&head,sizeof(upload_frame_head));    //包头
		memcpy(&upload_data_buf[sizeof(upload_frame_head)],&cure_data_head,sizeof(cure_upload_front));  //固化数据头
		memcpy(&upload_data_buf[sizeof(upload_frame_head) + sizeof(cure_upload_front)],upload_cure_para_data,strlen(upload_cure_para_data)); //固化参数
		memcpy(&upload_data_buf[sizeof(upload_frame_head) + sizeof(cure_upload_front) + strlen(upload_cure_para_data)],&cure_data_tail,sizeof(cure_upload_tail)); //固化数据尾
		
		end.crc = cal_crc(upload_data_buf,head.len - sizeof(upload_end));
		end.end = 0xFFFF;
		memcpy(&upload_data_buf[sizeof(upload_frame_head) + sizeof(cure_upload_front) + strlen(upload_cure_para_data) + sizeof(cure_upload_tail)],&end,sizeof(upload_end));
	}
	else if(type == 0x03 || type == 0x04)
	{
//		memset(request_data.device_name,0x00,sizeof(request_data.device_name));
//		memset(request_data.SN,0x00,sizeof(request_data.SN));
//		memset(request_data.version,0x00,sizeof(request_data.version));
//		
//		memcpy(request_data.device_name,DEVICE_NAME,strlen(DEVICE_NAME));
//		memcpy(request_data.SN,cfg_dft.sn,sizeof(idle_data.SN));
//		memcpy(request_data.version,cfg_dft.version,sizeof(idle_data.version));

//		request_data.file_data_num = 0;      //向上传输的帧序号  暂时为0 没有意义
//		//拼包
//		head.len = sizeof(upload_frame_head) + sizeof(data_load_request) + sizeof(upload_end);
//		
//		memcpy(upload_data_buf,&head,sizeof(upload_frame_head));
//		memcpy(&upload_data_buf[sizeof(upload_frame_head)],&request_data,sizeof(data_load_request));	
//		end.crc = cal_crc(upload_data_buf,sizeof(upload_frame_head) + sizeof(data_load_request));
//		end.end = 0xFFFF;
//		memcpy(&upload_data_buf[sizeof(upload_frame_head) + sizeof(data_load_request)],&end,sizeof(upload_end));
	}
	else if(type == 0x05)
	{
//		memset(error_code.device_name,0x00,sizeof(error_code.device_name));
//		memset(error_code.SN,0x00,sizeof(error_code.SN));
//		memset(error_code.version,0x00,sizeof(error_code.version));
//		
//		memcpy(error_code.device_name,DEVICE_NAME,strlen(DEVICE_NAME));
//		memcpy(error_code.SN,cfg_dft.sn,sizeof(error_code.SN));
//		memcpy(error_code.version,cfg_dft.version,sizeof(error_code.version));	
////		error_code.error 
//		//拼包
//		head.len = sizeof(upload_frame_head) + sizeof(error_load) + sizeof(upload_end);
//		
//		memcpy(upload_data_buf,&head,sizeof(upload_frame_head));
//		memcpy(&upload_data_buf[sizeof(upload_frame_head)],&error_code,sizeof(error_load));	
//		end.crc = cal_crc(upload_data_buf,sizeof(upload_frame_head) + sizeof(error_load));
//		end.end = 0xFFFF;
//		memcpy(&upload_data_buf[sizeof(upload_frame_head) + sizeof(error_load)],&end,sizeof(upload_end));		
	}

	if(head.t_mode == 0)        //WiFi上传
	{
		esp8266_uart_tx(upload_data_buf,head.len);
		head.run_num ++;	
	}
	else if(head.t_mode == 1)   //以太网上传
	{
		usr_k5_tx(upload_data_buf,head.len);
		head.run_num ++;
	}
}


//上传数据
void p_data_upload(void)
{
	static u32 upload_tick = 0;
	upload_tick++;
	if(upload_tick % 300 == 0)
	{
		if( cfg_dft.init_step == 4 )
		{
			if(cfg_dft.network_selet == 1 || wifi_flow.connect_state == 1)      //有网络连接
			{
				if(wifi_flow.execution_step == 6 || enthernet_flow.execution_step == 4)  //模块初始化连接完成
				{
					if(cure.mode == 1 || cure.mode == 2)                      // 若在固化或暂停固化中
						netword_package_upload(cfg_dft.network_selet,CURED_UPLOAD_TYPE);			
					else
						netword_package_upload(cfg_dft.network_selet,IDLE_UPLOAD_TYPE); 	
				}				
			}
		}
	} 
}







































