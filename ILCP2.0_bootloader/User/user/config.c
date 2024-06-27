#include "config.h"
//#include "pid.h"

DSTATUS write_user_parameter(const TCHAR* path,const void *buff,u32 len)
{
	DSTATUS f_sta = 0;

	f_sta = f_open(&f_user, path, FA_CREATE_ALWAYS | FA_WRITE);  
	if(f_sta == FR_OK)
	{
		f_sta = f_write(&f_user,buff,len,&b_write);
	}
	f_close(&f_user);

	return f_sta;
}



DSTATUS read_user_parameter(const TCHAR* path,void *buff,u32 len)
{
	DSTATUS f_sta = 0;

	f_sta = f_open(&f_user,path, FA_READ); 
	if(f_sta == FR_OK)
	{
		f_sta = f_read(&f_user,buff,len,&b_read);
	}
	f_close(&f_user);

	return f_sta;
}


//上电默认状态
s_cfg cfg_dft=
{
	.language = 0,          //语言
	.light_strip = 1,       //RGB灯条 默认开
	.heat_sw = 0,           //加热默认关
	.w_data = 0,            //内部温度显示
	.n2_enable = 0,         //氮气功能 默认关闭
	.head_lamp = 0,         //顶灯
	.u_disk_state = 0,      //u盘状态
	.heat_temp = -300,      //加热温度 不加热
};

//总时间保存
save_total_time save_time = 
{
	.uv_total_time = 0,
	.cure_total_time = 0,
	.run_total_time = 0
};

//固定的设置
file_save_config save_config =
{
	.id = 1, 
	.upgrade_state = 0,      //0空闲启动 1
	.uv_power_max =  0.85f,
	.uv_cal_k = 0.80f,
	.up_uv_k = 1,
	.down_uv_k = 1,
	.heat_cal = 0,
	.uv_heat_cal = 0
};


int cfg_save(void) //将当前配置保存在Flash
{
//	return flash_eraseAwrite((u32*)&save_config,sizeof(save_config),(void*)CFG_FLASH_ADDR);
	if(FR_OK != write_user_parameter(CONFIG_FILE,(const void*)&save_config,sizeof(save_config)))
	{
		//弹框 写入配置失败，发出警告
		pop_tips(30,(u8*)WARNING,(u8*)"Error: File read/write exception!"); 
		return 1;
	}
	return 0;
}

#define CFG  (*(file_save_config*)CFG_FLASH_ADDR)
#define CHECK_VALID(d,min,max,dft) ((d!=-1 && d>=min && d<=max)?d:dft)
void cfg_get(void) //读取设置
{
	//config.id=CHECK_VALID(CFG.id,1,65534,config_dft.id);
//	if(CFG.id!=0xffff)
//	{
//		save_config=CFG;
//	}
//	else
//	{
//		cfg_save();
//	}
	FILINFO fno;
	if( FR_OK == f_stat(TT_FILE, &fno) )  //检查总时间记录文件是否存在 
	{
		if(FR_OK != read_user_parameter(TT_FILE,(void*)&save_time,sizeof(save_time)))
		{
			//错误提示
		}
	}
	else
	{
		if(FR_OK != write_user_parameter(TT_FILE,(const void*)&save_time,sizeof(save_time)))
		{
			//弹框 写入配置失败
		}
	}
	
	if( FR_OK == f_stat(CONFIG_FILE, &fno) )  //检查配置文件是否存在
	{
		if(FR_OK != read_user_parameter(CONFIG_FILE,(void*)&save_config,sizeof(save_config)))
		{
			//错误提示
		}		
	}
	else
	{
		if(FR_OK != write_user_parameter(CONFIG_FILE,(const void*)&save_config,sizeof(save_config)))
		{
			//弹框 写入配置失败，发出警告
		}
	}
	
	if( FR_OK == f_stat(SN_FILE, &fno) )  //检查SN文件是否存在
	{
		if(FR_OK != read_user_parameter(SN_FILE,(void*)cfg_dft.sn,sizeof(cfg_dft.sn)))
		{
			//读取SN错误提示
		}		
	}
	else
	{
//		if(FR_OK != write_user_parameter(SN_FILE,(const void*)cfg_dft.sn,sizeof(cfg_dft.sn)))
//		{
//			//弹框 写入配置失败，发出警告
//		}
	}
}

void cfg_print(void)
{
//	printf("id:%04X \r\n",config.id);
//	printf("v_pid_k_in %04X \r\n",config.v_pid_k_in);
//	printf("v_pid_p %04X \r\n",config.v_pid_p);
//	printf("v_pid_i %04X \r\n",config.v_pid_i);
//	printf("v_pid_d %04X \r\n",config.v_pid_d);
//	printf("f_pid_k_in %04X \r\n",config.f_pid_k_in);
//	printf("f_pid_i %04X \r\n",config.f_pid_i);
//	printf("f_pid_d %04X \r\n",config.f_pid_d);

}

