#include "config.h"



//上电默认状态
s_cfg cfg_dft=
{
	.language = 0,          //语言
	.light_strip = 1,       //RGB灯条 默认开
	.heat_sw = 0,           //加热默认关
	.w_data = 0,            //写测试数据
	.n2_enable = 0,         //氮气功能 默认关闭
	.head_lamp = 0,         //顶灯
	.init_step = 0,         //初始化步骤
	.network_selet = 0,
	.m_ctrl = 0,            //电机运行状态
	.u_disk_state = 0,      //u盘状态
	.heat_temp = -300,      //加热温度 不加热
	.ali_domain_name = NULL,
	.lux_domain_name = NULL
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

	return 0;
}

#define CFG  (*(file_save_config*)CFG_FLASH_ADDR)
#define CHECK_VALID(d,min,max,dft) ((d!=-1 && d>=min && d<=max)?d:dft)
void cfg_get(void) //读取设置
{
//	FILINFO fno = {0};
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

