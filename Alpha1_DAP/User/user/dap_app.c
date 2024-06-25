#include "dap_app.h"

dap_state d_state = {0};

static u8 dap_pack_buf[100];
static u8 syn_buf[]={0x01};
int dap_pre_cb(u8 *b,int len);//返回整包长度
int dap_pro(u8 * b,int len);//返回是否正确接收
COMM_HEAD dap_pack= //组包对象
{
	dap_pack_buf,
	sizeof(dap_pack_buf),
	(u8 *)syn_buf,
	sizeof(syn_buf),
	sizeof(dap_03cmd),
	1,
	0,
	dap_pre_cb,
	dap_pro
};

int dap_pre_cb(u8 *b,int len)//返回整包长度
{
	switch(b[1])
	{
	case 0x03: //传感器数据包共22字节
		return sizeof(dap_03cmd);
	case 0x10: //输出姿态数据，共26字节
		return sizeof(dap_10cmd);
	default:
		return sizeof(dap_10cmd);
	}
}



u8 test_buf[50] = {0};
int dap_pro(u8 * b,int len)//返回是否正确接收
{
	u16 crc=GetModbusCRC16(b,len-2);
	if( crc != *(u16*)&b[len-2] ) return 1;
	dap_cmd_process(b,len);

	return 0;
}


//dap_03cmd ctrl_03cmd = {0};
//dap_10cmd ctrl_10cmd = {0};
u32 test_cmd_res = 0;
#define DAP03 (*(dap_03cmd*)b)
#define DAP10 (*(dap_10cmd*)b)
//接收指令处理   cmd 指令类型  
void dap_cmd_process(u8 * b,int len)
{
	if(DAP03.cmd == 0x03)
	{
		if( DAP03.data_addr == CHANGE_END16(0x03F2) && DAP03.data == CHANGE_END16(0x0001) )      //杨明打印机状态查询
		{
			if(d_state.dap_state == 0)
			{
//				u8 res_cmdbuf[] = {0x01,0x03,0x02,0x00,0x00,0xb8,0x44};
//				uart_send(res_cmdbuf,7,&uart1);
				d_state.reset_state = 1;
				if( 0 == reset_motor() ) 
				{					
					responsde_cmd(0x03,0x02);
					d_state.dap_state = 2;      //复位完毕 DAP准备就绪
				}
				else
				{
					d_state.dap_state = 1;
					responsde_cmd(0x03,0x01);
				}
			}
			else if( d_state.dap_state == 1 )        //若在复位状态
			{
				d_state.reset_state = 1;
				if( 0 == reset_motor()) 
				{
					responsde_cmd(0x03,0x02);
					d_state.dap_state = 2;      //复位完毕 DAP准备就绪
					test_cmd_res++;
				}
				else
				{
					responsde_cmd(0x03,0x01);
					test_cmd_res++;
				}
			}
			else if(d_state.dap_state == 2)    //复位完成状态 
			{
				responsde_cmd(0x03,0x02);
				test_cmd_res++;
			}
			else if(d_state.dap_state == 3)    //开始运动状态
			{
				if(d_state.auto_state == 3 && platform_motor.run == 0)    //主体电机运动到位
				{
					responsde_cmd(0x03,0x04);   //主体电机运动到位 回复
					test_cmd_res++;
				}
				else
				{
					responsde_cmd(0x03,0x03);   //主体电机运动中 回复
					test_cmd_res++;
				}
			}
			else if(d_state.dap_state == 4) 
			{
				//铲料中 
				responsde_cmd(0x03,0x05);       //铲料中回复
				test_cmd_res++;

			}
			else if(d_state.dap_state == 5) 
			{
				//铲料完成
				responsde_cmd(0x03,0x06);       //铲料完毕
				test_cmd_res++;
			}
			else if(d_state.dap_state == 6)     //
			{
				responsde_cmd(0x03,0x07);      //DAP回位中
				test_cmd_res++;
			}
			else if(d_state.dap_state == 7)    //DAP回位完成
			{
				responsde_cmd(0x03,0x08);      //回复倒料完毕 可以开始下一轮
				test_cmd_res++;
			}
		}
		else if( DAP03.data_addr == CHANGE_END16(0x03FD) && DAP03.data == CHANGE_END16(0x0001) )
		{
					memcpy(test_buf,b,len);
		}
		else if( DAP03.data_addr == CHANGE_END16(0x0406) && DAP03.data == CHANGE_END16(0x0001) )  //PLC输入状态1 
		{
			in_io_state1 io_state1 = {0x01,0x03,0x02,};
			io_state1.addr = 0x01; io_state1.cmd = 0x03;io_state1.len = 0x02;
			io_state1.scraper_r = S_MOTOR_R;  io_state1.scraper_z = 0;  io_state1.scraper_l = S_MOTOR_L;
			io_state1.main_r = ~M_MOTOR_R;  io_state1.main_z = 0;  io_state1.main_l = ~M_MOTOR_L;
			io_state1.cb_sensor_l = ~HOPPER_L; io_state1.cb_sensor_r = ~HOPPER_R;
			io_state1.printer_loc = ~PRINTER_LOC;
			io_state1.material_det = 1;   //缺料检测
			io_state1.in_material_det = IN_MATERIALS;
			io_state1.door_det = ~FRAME_DOOR;
			io_state1.e_stop = ~EG_STOP;
			io_state1.res = 0;
			io_state1.scraper_alm = ~S_MOTOR_ALM;
			io_state1.main_alm = ~M_MOTOR_ALM;
			io_state1.crc =  GetModbusCRC16((u8*)&io_state1,sizeof(in_io_state1) - 2);
			uart_send((u8*)&io_state1,sizeof(in_io_state1),&uart1);
		}
		else if( DAP03.data_addr == CHANGE_END16(0x0407) && DAP03.data == CHANGE_END16(0x0001) )  //输入状态2
		{
			in_io_state2 io_state2 = {0x01,0x03,0x02,};
			io_state2.addr = 0x01; io_state2.cmd = 0x03;io_state2.len = 0x02;
			io_state2.scr_self_l = 1;
			io_state2.scr_self_r = 1;
			io_state2.cb_loc = ~CB_LOC;
			io_state2.cb_h_det = ~CB_H_DET;
			io_state2.crc =  GetModbusCRC16((u8*)&io_state2,sizeof(in_io_state2) - 2);
			uart_send((u8*)&io_state2,sizeof(in_io_state2),&uart1);
		}
		else if( DAP03.data_addr == CHANGE_END16(0x0408) && DAP03.data == CHANGE_END16(0x0800) )  //输出状态
		{
			out_io_state out_state = {0x01,0x03,0x02,};
			out_state.scraper_pwm = scraper_motor.run;
			out_state.scraper_dir = SCRAPER_MOTOR_DIR;
			out_state.main_pwm = platform_motor.run;
			out_state.main_dir = MAIN_MOTOR_DIR;
			out_state.rec_material = HOPPER_L;
			out_state.pour_material = HOPPER_R;
			out_state.in_material = IN_MATERIALS;
			out_state.led_r = LED_R;
			out_state.led_g = LED_G;
			out_state.led_b = LED_Y;
			out_state.crc =  GetModbusCRC16((u8*)&out_state,sizeof(out_io_state) - 2);
			uart_send((u8*)&out_state,sizeof(out_io_state),&uart1);
		}			
	}
	else if(DAP10.cmd == 0x10)
	{
		if( DAP10.data_addr == CHANGE_END16(0x03E8) && DAP10.data1 == CHANGE_END16(0x0001) )
		{
			if( DAP10.data2 == CHANGE_END16(0x0200) && DAP10.data3 == 0x00 )
			{
				IN_MATERIALS_SW = 1;    //自动注液禁止
				d_state.dap_state = 0;   //
				d_state.reset_state	= 0;	
				responsde_cmd(0x03,0x02);   //DAP	标志清零 自动注液禁止			
			}
			else if( DAP10.data2 == CHANGE_END16(0x0200) && DAP10.data3 == 0x01 )   //开始复位指令
			{
				u8 res_buf[] = {0x01,0x10,0x03,0xE8,0x00,0x01,0x81,0xB9};
				uart_send(res_buf,sizeof(res_buf),&uart1);
				
				d_state.reset_state = 1;      //启动复位
				reset_motor();                //DAP电机复位
				d_state.dap_state = 1;        //DAP处于复位状态
			}
			else if( DAP10.data2 == CHANGE_END16(0x0200) && DAP10.data3 == 0x02 )   //开始运动指令
			{
				u8 res_buf[] = {0x01,0x10,0x03,0xE8,0x00,0x01,0x81,0xB9};
				uart_send(res_buf,sizeof(res_buf),&uart1);
				IN_MATERIALS_SW = 0;    //自动注液允许
				d_state.dap_state = 3;        //DAP刮料台开始运动   允许自动注液
				d_state.auto_state = 1;       //自动铲料过程开始
			}
			else if( DAP10.data2 == CHANGE_END16(0x0200) && DAP10.data3 == 0x03 )   //开始铲料指令
			{
				u8 res_buf[] = {0x01,0x10,0x03,0xE8,0x00,0x01,0x81,0xB9};
				uart_send(res_buf,sizeof(res_buf),&uart1);
				d_state.auto_state = 4;       //执行自动铲料第四步
				d_state.dap_state = 4;
			}
			else if( DAP10.data2 == CHANGE_END16(0x0200) && DAP10.data3 == 0x04 )   //铲料完成 成形台已经回到最高点
			{
				u8 res_buf[] = {0x01,0x10,0x03,0xE8,0x00,0x01,0x81,0xB9};
				uart_send(res_buf,sizeof(res_buf),&uart1);
				d_state.dap_state = 6;
				d_state.auto_state = 6;   //铲刀回位
			}
		}
		if( DAP10.data_addr == CHANGE_END16(0x03FC) && DAP10.data1 == CHANGE_END16(0x0001) )
		{
			if( DAP10.data2 == CHANGE_END16(0x0200) && DAP10.data3 == 0x00 )
			{
				
			}
			else if( DAP10.data2 == CHANGE_END16(0x0200) && DAP10.data3 == 0x01 )
			{
				
			}
		}
	}
}

//响应指令处理
void responsde_cmd(u8 cmd,u8 state)
{
	u8 res_cmd[20] = {0x01,};   
	if(cmd == 0x03)
	{
		res_cmd[0] = 0x01;       //地址位 01
		res_cmd[1] = 0x03;
		res_cmd[2] = 0x02;
		res_cmd[3] = 0x00;
		res_cmd[4] = state;
		*(u16*)&res_cmd[5] = GetModbusCRC16(res_cmd,5);
		uart_send(res_cmd,7,&uart1);
	}
	else if(cmd == 0x10)
	{
		res_cmd[0] = 0x01;       //地址位 01
		res_cmd[1] = 0x10;
		res_cmd[2] = 0x02;
		res_cmd[3] = 0x00;
		res_cmd[4] = state;
		res_cmd[5] = 0x01;
		*(u16*)&res_cmd[6] = GetModbusCRC16(res_cmd,6);
		uart_send(res_cmd,8,&uart1);		
	}
}


//自动铲料过程
void automatic_shoveling_pro(void)
{
	if(d_state.auto_state == 1)        //电机复位
	{
		d_state.reset_state = 1;
		if(0 == reset_motor())
		{
			d_state.auto_state = 2;   //复位完毕
		}
	}
	else if(d_state.auto_state == 2)
	{
		if(M_MOTOR_R == 0 && platform_motor.run == 0)
		{
			platform_motor.motor_sn = PLATFORM_MOTOR;
			platform_motor.dir = MAIN_FORWAED;               //方向
			platform_motor.pulse_cnt = 0;
			platform_motor.dis =  303000;                    //距离  um  
			platform_motor.acc_tm = 3;                       //加速时间
			platform_motor.dec_tm = 3;                       //减速时间
			platform_motor.start_v = 0;                      //启动速度 um/s
			platform_motor.max_v =  20200;                   //最高速 um/s  
			platform_motor.end_v = 0;                        //末速度
			motor_move(&platform_motor);	
			d_state.auto_state = 3;
		}
	}
	else if(d_state.auto_state == 3)
	{
		if(M_MOTOR_L == 0)                                //若是触发正限位停止电机
			motor_stop(PLATFORM_MOTOR);
	}
	else if(d_state.auto_state == 4)                      //开始铲料
	{
		if( platform_motor.run == 0 )                      //待主体电机停止 往回缩 顶住成型台
		{
			platform_motor.motor_sn = PLATFORM_MOTOR;
			platform_motor.dir = MAIN_BACKWARD;               //方向
			platform_motor.pulse_cnt = 0;
			platform_motor.dis =  3300;                    //距离  um  
			platform_motor.acc_tm = 1;                       //加速时间
			platform_motor.dec_tm = 1;                       //减速时间
			platform_motor.start_v = 0;                      //启动速度 um/s
			platform_motor.max_v =  2000;                   //最高速 um/s  
			platform_motor.end_v = 0;                        //末速度

			motor_move(&platform_motor);

			d_state.auto_state = 5;
			scraper_motor.stat = 1;                    //铲刀开始运动
		}
	}
	else if(d_state.auto_state == 5)                   //铲刀运动
	{
		if(platform_motor.run == 0 )                   //待主体电机停止
		{
			if(scraper_motor.stat == 1)
			{
				if(S_MOTOR_R != 1)           //铲刀若不在最右边   则回归最右边
				{
					scraper_motor.motor_sn = SCRAPER_MOTOR;
					scraper_motor.dir = SCRAPER_BACKWARD;              //方向
					scraper_motor.pulse_cnt = 0;
					scraper_motor.dis =  scraper_motor.max_len;     //距离  um  
					scraper_motor.acc_tm = 1;                       //加速时间
					scraper_motor.dec_tm = 1;                       //减速时间
					scraper_motor.start_v = 0;                      //启动速度 um/s
					scraper_motor.max_v =  3000;                    //最高速 um/s  
					scraper_motor.end_v = 0;                        //末速度
					motor_move(&scraper_motor);
				}
				scraper_motor.stat = 2;
			}
			else if(scraper_motor.stat == 2)
			{
				if(S_MOTOR_R == 1 && scraper_motor.run == 0)           //铲刀电机停止且触发右限位
				{
					responsde_cmd(0x03,0x05);       //铲料
					
					scraper_motor.motor_sn = SCRAPER_MOTOR;
					scraper_motor.dir = SCRAPER_FORWAED;               //方向
					scraper_motor.pulse_cnt = 0;
					scraper_motor.dis =  150000;                     //距离  um    
					scraper_motor.acc_tm = 2;                       //加速时间
					scraper_motor.dec_tm = 2;                       //减速时间
					scraper_motor.start_v = 0;                      //启动速度 um/s
					scraper_motor.max_v =  1800;                   //最高速 um/s  
					scraper_motor.end_v = 0;                        //末速度
					motor_move(&scraper_motor);						
					scraper_motor.stat = 3;
				}
			}
			else if(scraper_motor.stat == 3)
			{
				if( scraper_motor.run == 0 )                        //等待铲刀电机停止
				{
//					static u32 tick = 0;
//					if(tick++ %100 == 0)
//						responsde_cmd(0x03,0x06);       //铲料完毕  成型台回到最高点
					
					d_state.dap_state = 5;
				}
			}

		}
	}
	else if(d_state.auto_state == 6)                          //铲刀回位 ，料斗倒料
	{
		scraper_motor.stat = 0;  //铲刀状态清空
		if(M_MOTOR_R != 0)          //若主体电机不在负限位 右边 则回归右限位
		{
			if(platform_motor.run == 0)
			{
				platform_motor.motor_sn = PLATFORM_MOTOR;
				platform_motor.dir = MAIN_BACKWARD;              //方向 后退
				platform_motor.pulse_cnt = 0;
				platform_motor.dis =  303000 - 2000;             //距离  um   走最远距离  回归距离为行走最远距离减去顶成形台走的距离 多点
				platform_motor.acc_tm = 1;                       //加速时间
				platform_motor.dec_tm = 1;                       //减速时间
				platform_motor.start_v = 0;                      //启动速度 um/s
				platform_motor.max_v =  20200;                   //最高速 um/s  
				platform_motor.end_v = 0;                        //末速度
				motor_move(&platform_motor); 
			}
		}
		if(S_MOTOR_R != 1)           //触发为1    若铲刀电机不在负限位
		{
			if(scraper_motor.run == 0)                      //铲刀电机
			{
				scraper_motor.motor_sn = SCRAPER_MOTOR;
				scraper_motor.dir = SCRAPER_BACKWARD;              //方向 后退
				scraper_motor.pulse_cnt = 0;
				scraper_motor.dis =  scraper_motor.max_len;     //距离  um  
				scraper_motor.acc_tm = 1;                       //加速时间
				scraper_motor.dec_tm = 1;                       //减速时间
				scraper_motor.start_v = 0;                      //启动速度 um/s
				scraper_motor.max_v =  6000;                    //最高速 um/s  
				scraper_motor.end_v = 0;                        //末速度
				motor_move(&scraper_motor);
			}
		}
		if(M_MOTOR_R == 0)         //主体电机在右限位
		{
			if( 0 == hopper_dump(2) )
			{
				vTaskDelay(3000);  //延迟3秒
				if( 0 == hopper_dump(1) )
				{
					//完成
//					responsde_cmd(0x03,0x08); 
					d_state.auto_state = 0;
					d_state.dap_state = 7;
				}
			}
		}
	}

	if(S_MOTOR_ALM || M_MOTOR_ALM)
	{
		LOG_COLOUR_RED;
		return;
	}
	if(CB_H_DET == 0)            //牙膜铲料高度检测，触发为低电平
	{
		lamp_flickering();       //橙灯闪烁
		return;
	}
	if(d_state.auto_state == 0)  //若是空闲
	{
		LOG_COLOUR_GREEN;        //绿灯常亮
	}
	else                         //若是在运行中
	{
		if(FRAME_DOOR == 1)      //若外框门 开门
		{
			lamp_flickering();   //橙灯闪烁
		}
		else
		{
			LOG_COLOUR_GREEN;    //若关门为绿灯
		}
	}
}

//调用频率100hz 
void lamp_flickering(void)
{
	static u32 tick = 0;
	tick++;
	if(tick == 1)
		LOG_COLOUR_YELLOW;
	else if(tick == 100)
	{
		LOG_COLOUR_BLACK;
	}
	else if(tick == 180)
	{
		tick = 0;
	}
}
//电机归位
u8 reset_motor(void)
{
	if( d_state.reset_state == 1)   //执行电机零点复位
	{
		if(M_MOTOR_R != 0)          //若主体电机不在负限位 右边 则回归右限位
		{
			if(platform_motor.run == 0)
			{
				platform_motor.motor_sn = PLATFORM_MOTOR;
				platform_motor.dir = MAIN_BACKWARD;              //方向 后退
				platform_motor.pulse_cnt = 0;
				platform_motor.dis =  platform_motor.max_len;    //距离  um   走最远距离
				platform_motor.acc_tm = 1;                       //加速时间
				platform_motor.dec_tm = 1;                       //减速时间
				platform_motor.start_v = 0;                      //启动速度 um/s
				platform_motor.max_v =  20200;                   //最高速 um/s  
				platform_motor.end_v = 0;                        //末速度
				motor_move(&platform_motor); 
			}
		}
		
		if(S_MOTOR_R != 1)           //触发为1    若铲刀电机不在负限位
		{
			if(scraper_motor.run == 0)                      //铲刀电机
			{
				scraper_motor.motor_sn = SCRAPER_MOTOR;
				scraper_motor.dir = SCRAPER_BACKWARD;              //方向 后退
				scraper_motor.pulse_cnt = 0;
				scraper_motor.dis =  scraper_motor.max_len;     //距离  um  
				scraper_motor.acc_tm = 1;                       //加速时间
				scraper_motor.dec_tm = 1;                       //减速时间
				scraper_motor.start_v = 0;                      //启动速度 um/s
				scraper_motor.max_v =  2000;                    //最高速 um/s  
				scraper_motor.end_v = 0;                        //末速度
				motor_move(&scraper_motor);
			}
		}
		if(HOPPER_L == 0 && HOPPER_R == 1)                  //正限位触发 负限位没有触发则为回位状态
		{
			if(M_MOTOR_R == 0 && S_MOTOR_R == 1)
			{
				d_state.reset_state = 0;
				return 0;
			}
		}
		else
		{
			if( 0 == hopper_dump(1) )   //料斗回位
			{
				hopper_dump(0);
			}			
		}
	}
	return 1;
}


