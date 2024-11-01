#include "config.h"



//上电默认状态
s_cfg cfg_dft=
{
	.language = 0,          //语言

};



//固定的设置
file_save_config save_config =
{
	.id = 2,
	.weight_offset = 0,
};


int cfg_save(void) //将当前配置保存在Flash
{
	int ret = 0;
	ret = flash_eraseAwrite((u32*)&save_config,sizeof(save_config),(void*)CFG_FLASH_ADDR);
	return ret;
}

#define CFG  (*(file_save_config*)CFG_FLASH_ADDR)
#define CHECK_VALID(d,min,max,dft) ((d!=-1 && d>=min && d<=max)?d:dft)
void cfg_get(void) //读取设置
{
	if(CFG.id!=0xffff)
	{
		save_config=CFG;
	}
	else
	{
		cfg_save();    //初次写入
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

