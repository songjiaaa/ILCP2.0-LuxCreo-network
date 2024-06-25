#include "dap_app.h"

dap_state d_state = {0};

static u8 dap_pack_buf[100];
static u8 syn_buf[]={0x01};
int dap_pre_cb(u8 *b,int len);//������������
int dap_pro(u8 * b,int len);//�����Ƿ���ȷ����
COMM_HEAD dap_pack= //�������
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

int dap_pre_cb(u8 *b,int len)//������������
{
	switch(b[1])
	{
	case 0x03: //���������ݰ���22�ֽ�
		return sizeof(dap_03cmd);
	case 0x10: //�����̬���ݣ���26�ֽ�
		return sizeof(dap_10cmd);
	default:
		return sizeof(dap_10cmd);
	}
}



u8 test_buf[50] = {0};
int dap_pro(u8 * b,int len)//�����Ƿ���ȷ����
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
//����ָ���   cmd ָ������  
void dap_cmd_process(u8 * b,int len)
{
	if(DAP03.cmd == 0x03)
	{
		if( DAP03.data_addr == CHANGE_END16(0x03F2) && DAP03.data == CHANGE_END16(0x0001) )      //������ӡ��״̬��ѯ
		{
			if(d_state.dap_state == 0)
			{
//				u8 res_cmdbuf[] = {0x01,0x03,0x02,0x00,0x00,0xb8,0x44};
//				uart_send(res_cmdbuf,7,&uart1);
				d_state.reset_state = 1;
				if( 0 == reset_motor() ) 
				{					
					responsde_cmd(0x03,0x02);
					d_state.dap_state = 2;      //��λ��� DAP׼������
				}
				else
				{
					d_state.dap_state = 1;
					responsde_cmd(0x03,0x01);
				}
			}
			else if( d_state.dap_state == 1 )        //���ڸ�λ״̬
			{
				d_state.reset_state = 1;
				if( 0 == reset_motor()) 
				{
					responsde_cmd(0x03,0x02);
					d_state.dap_state = 2;      //��λ��� DAP׼������
					test_cmd_res++;
				}
				else
				{
					responsde_cmd(0x03,0x01);
					test_cmd_res++;
				}
			}
			else if(d_state.dap_state == 2)    //��λ���״̬ 
			{
				responsde_cmd(0x03,0x02);
				test_cmd_res++;
			}
			else if(d_state.dap_state == 3)    //��ʼ�˶�״̬
			{
				if(d_state.auto_state == 3 && platform_motor.run == 0)    //�������˶���λ
				{
					responsde_cmd(0x03,0x04);   //�������˶���λ �ظ�
					test_cmd_res++;
				}
				else
				{
					responsde_cmd(0x03,0x03);   //�������˶��� �ظ�
					test_cmd_res++;
				}
			}
			else if(d_state.dap_state == 4) 
			{
				//������ 
				responsde_cmd(0x03,0x05);       //�����лظ�
				test_cmd_res++;

			}
			else if(d_state.dap_state == 5) 
			{
				//�������
				responsde_cmd(0x03,0x06);       //�������
				test_cmd_res++;
			}
			else if(d_state.dap_state == 6)     //
			{
				responsde_cmd(0x03,0x07);      //DAP��λ��
				test_cmd_res++;
			}
			else if(d_state.dap_state == 7)    //DAP��λ���
			{
				responsde_cmd(0x03,0x08);      //�ظ�������� ���Կ�ʼ��һ��
				test_cmd_res++;
			}
		}
		else if( DAP03.data_addr == CHANGE_END16(0x03FD) && DAP03.data == CHANGE_END16(0x0001) )
		{
					memcpy(test_buf,b,len);
		}
		else if( DAP03.data_addr == CHANGE_END16(0x0406) && DAP03.data == CHANGE_END16(0x0001) )  //PLC����״̬1 
		{
			in_io_state1 io_state1 = {0x01,0x03,0x02,};
			io_state1.addr = 0x01; io_state1.cmd = 0x03;io_state1.len = 0x02;
			io_state1.scraper_r = S_MOTOR_R;  io_state1.scraper_z = 0;  io_state1.scraper_l = S_MOTOR_L;
			io_state1.main_r = ~M_MOTOR_R;  io_state1.main_z = 0;  io_state1.main_l = ~M_MOTOR_L;
			io_state1.cb_sensor_l = ~HOPPER_L; io_state1.cb_sensor_r = ~HOPPER_R;
			io_state1.printer_loc = ~PRINTER_LOC;
			io_state1.material_det = 1;   //ȱ�ϼ��
			io_state1.in_material_det = IN_MATERIALS;
			io_state1.door_det = ~FRAME_DOOR;
			io_state1.e_stop = ~EG_STOP;
			io_state1.res = 0;
			io_state1.scraper_alm = ~S_MOTOR_ALM;
			io_state1.main_alm = ~M_MOTOR_ALM;
			io_state1.crc =  GetModbusCRC16((u8*)&io_state1,sizeof(in_io_state1) - 2);
			uart_send((u8*)&io_state1,sizeof(in_io_state1),&uart1);
		}
		else if( DAP03.data_addr == CHANGE_END16(0x0407) && DAP03.data == CHANGE_END16(0x0001) )  //����״̬2
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
		else if( DAP03.data_addr == CHANGE_END16(0x0408) && DAP03.data == CHANGE_END16(0x0800) )  //���״̬
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
				IN_MATERIALS_SW = 1;    //�Զ�עҺ��ֹ
				d_state.dap_state = 0;   //
				d_state.reset_state	= 0;	
				responsde_cmd(0x03,0x02);   //DAP	��־���� �Զ�עҺ��ֹ			
			}
			else if( DAP10.data2 == CHANGE_END16(0x0200) && DAP10.data3 == 0x01 )   //��ʼ��λָ��
			{
				u8 res_buf[] = {0x01,0x10,0x03,0xE8,0x00,0x01,0x81,0xB9};
				uart_send(res_buf,sizeof(res_buf),&uart1);
				
				d_state.reset_state = 1;      //������λ
				reset_motor();                //DAP�����λ
				d_state.dap_state = 1;        //DAP���ڸ�λ״̬
			}
			else if( DAP10.data2 == CHANGE_END16(0x0200) && DAP10.data3 == 0x02 )   //��ʼ�˶�ָ��
			{
				u8 res_buf[] = {0x01,0x10,0x03,0xE8,0x00,0x01,0x81,0xB9};
				uart_send(res_buf,sizeof(res_buf),&uart1);
				IN_MATERIALS_SW = 0;    //�Զ�עҺ����
				d_state.dap_state = 3;        //DAP����̨��ʼ�˶�   �����Զ�עҺ
				d_state.auto_state = 1;       //�Զ����Ϲ��̿�ʼ
			}
			else if( DAP10.data2 == CHANGE_END16(0x0200) && DAP10.data3 == 0x03 )   //��ʼ����ָ��
			{
				u8 res_buf[] = {0x01,0x10,0x03,0xE8,0x00,0x01,0x81,0xB9};
				uart_send(res_buf,sizeof(res_buf),&uart1);
				d_state.auto_state = 4;       //ִ���Զ����ϵ��Ĳ�
				d_state.dap_state = 4;
			}
			else if( DAP10.data2 == CHANGE_END16(0x0200) && DAP10.data3 == 0x04 )   //������� ����̨�Ѿ��ص���ߵ�
			{
				u8 res_buf[] = {0x01,0x10,0x03,0xE8,0x00,0x01,0x81,0xB9};
				uart_send(res_buf,sizeof(res_buf),&uart1);
				d_state.dap_state = 6;
				d_state.auto_state = 6;   //������λ
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

//��Ӧָ���
void responsde_cmd(u8 cmd,u8 state)
{
	u8 res_cmd[20] = {0x01,};   
	if(cmd == 0x03)
	{
		res_cmd[0] = 0x01;       //��ַλ 01
		res_cmd[1] = 0x03;
		res_cmd[2] = 0x02;
		res_cmd[3] = 0x00;
		res_cmd[4] = state;
		*(u16*)&res_cmd[5] = GetModbusCRC16(res_cmd,5);
		uart_send(res_cmd,7,&uart1);
	}
	else if(cmd == 0x10)
	{
		res_cmd[0] = 0x01;       //��ַλ 01
		res_cmd[1] = 0x10;
		res_cmd[2] = 0x02;
		res_cmd[3] = 0x00;
		res_cmd[4] = state;
		res_cmd[5] = 0x01;
		*(u16*)&res_cmd[6] = GetModbusCRC16(res_cmd,6);
		uart_send(res_cmd,8,&uart1);		
	}
}


//�Զ����Ϲ���
void automatic_shoveling_pro(void)
{
	if(d_state.auto_state == 1)        //�����λ
	{
		d_state.reset_state = 1;
		if(0 == reset_motor())
		{
			d_state.auto_state = 2;   //��λ���
		}
	}
	else if(d_state.auto_state == 2)
	{
		if(M_MOTOR_R == 0 && platform_motor.run == 0)
		{
			platform_motor.motor_sn = PLATFORM_MOTOR;
			platform_motor.dir = MAIN_FORWAED;               //����
			platform_motor.pulse_cnt = 0;
			platform_motor.dis =  303000;                    //����  um  
			platform_motor.acc_tm = 3;                       //����ʱ��
			platform_motor.dec_tm = 3;                       //����ʱ��
			platform_motor.start_v = 0;                      //�����ٶ� um/s
			platform_motor.max_v =  20200;                   //����� um/s  
			platform_motor.end_v = 0;                        //ĩ�ٶ�
			motor_move(&platform_motor);	
			d_state.auto_state = 3;
		}
	}
	else if(d_state.auto_state == 3)
	{
		if(M_MOTOR_L == 0)                                //���Ǵ�������λֹͣ���
			motor_stop(PLATFORM_MOTOR);
	}
	else if(d_state.auto_state == 4)                      //��ʼ����
	{
		if( platform_motor.run == 0 )                      //��������ֹͣ ������ ��ס����̨
		{
			platform_motor.motor_sn = PLATFORM_MOTOR;
			platform_motor.dir = MAIN_BACKWARD;               //����
			platform_motor.pulse_cnt = 0;
			platform_motor.dis =  3300;                    //����  um  
			platform_motor.acc_tm = 1;                       //����ʱ��
			platform_motor.dec_tm = 1;                       //����ʱ��
			platform_motor.start_v = 0;                      //�����ٶ� um/s
			platform_motor.max_v =  2000;                   //����� um/s  
			platform_motor.end_v = 0;                        //ĩ�ٶ�

			motor_move(&platform_motor);

			d_state.auto_state = 5;
			scraper_motor.stat = 1;                    //������ʼ�˶�
		}
	}
	else if(d_state.auto_state == 5)                   //�����˶�
	{
		if(platform_motor.run == 0 )                   //��������ֹͣ
		{
			if(scraper_motor.stat == 1)
			{
				if(S_MOTOR_R != 1)           //�������������ұ�   ��ع����ұ�
				{
					scraper_motor.motor_sn = SCRAPER_MOTOR;
					scraper_motor.dir = SCRAPER_BACKWARD;              //����
					scraper_motor.pulse_cnt = 0;
					scraper_motor.dis =  scraper_motor.max_len;     //����  um  
					scraper_motor.acc_tm = 1;                       //����ʱ��
					scraper_motor.dec_tm = 1;                       //����ʱ��
					scraper_motor.start_v = 0;                      //�����ٶ� um/s
					scraper_motor.max_v =  3000;                    //����� um/s  
					scraper_motor.end_v = 0;                        //ĩ�ٶ�
					motor_move(&scraper_motor);
				}
				scraper_motor.stat = 2;
			}
			else if(scraper_motor.stat == 2)
			{
				if(S_MOTOR_R == 1 && scraper_motor.run == 0)           //�������ֹͣ�Ҵ�������λ
				{
					responsde_cmd(0x03,0x05);       //����
					
					scraper_motor.motor_sn = SCRAPER_MOTOR;
					scraper_motor.dir = SCRAPER_FORWAED;               //����
					scraper_motor.pulse_cnt = 0;
					scraper_motor.dis =  150000;                     //����  um    
					scraper_motor.acc_tm = 2;                       //����ʱ��
					scraper_motor.dec_tm = 2;                       //����ʱ��
					scraper_motor.start_v = 0;                      //�����ٶ� um/s
					scraper_motor.max_v =  1800;                   //����� um/s  
					scraper_motor.end_v = 0;                        //ĩ�ٶ�
					motor_move(&scraper_motor);						
					scraper_motor.stat = 3;
				}
			}
			else if(scraper_motor.stat == 3)
			{
				if( scraper_motor.run == 0 )                        //�ȴ��������ֹͣ
				{
//					static u32 tick = 0;
//					if(tick++ %100 == 0)
//						responsde_cmd(0x03,0x06);       //�������  ����̨�ص���ߵ�
					
					d_state.dap_state = 5;
				}
			}

		}
	}
	else if(d_state.auto_state == 6)                          //������λ ���϶�����
	{
		scraper_motor.stat = 0;  //����״̬���
		if(M_MOTOR_R != 0)          //�����������ڸ���λ �ұ� ��ع�����λ
		{
			if(platform_motor.run == 0)
			{
				platform_motor.motor_sn = PLATFORM_MOTOR;
				platform_motor.dir = MAIN_BACKWARD;              //���� ����
				platform_motor.pulse_cnt = 0;
				platform_motor.dis =  303000 - 2000;             //����  um   ����Զ����  �ع����Ϊ������Զ�����ȥ������̨�ߵľ��� ���
				platform_motor.acc_tm = 1;                       //����ʱ��
				platform_motor.dec_tm = 1;                       //����ʱ��
				platform_motor.start_v = 0;                      //�����ٶ� um/s
				platform_motor.max_v =  20200;                   //����� um/s  
				platform_motor.end_v = 0;                        //ĩ�ٶ�
				motor_move(&platform_motor); 
			}
		}
		if(S_MOTOR_R != 1)           //����Ϊ1    ������������ڸ���λ
		{
			if(scraper_motor.run == 0)                      //�������
			{
				scraper_motor.motor_sn = SCRAPER_MOTOR;
				scraper_motor.dir = SCRAPER_BACKWARD;              //���� ����
				scraper_motor.pulse_cnt = 0;
				scraper_motor.dis =  scraper_motor.max_len;     //����  um  
				scraper_motor.acc_tm = 1;                       //����ʱ��
				scraper_motor.dec_tm = 1;                       //����ʱ��
				scraper_motor.start_v = 0;                      //�����ٶ� um/s
				scraper_motor.max_v =  6000;                    //����� um/s  
				scraper_motor.end_v = 0;                        //ĩ�ٶ�
				motor_move(&scraper_motor);
			}
		}
		if(M_MOTOR_R == 0)         //������������λ
		{
			if( 0 == hopper_dump(2) )
			{
				vTaskDelay(3000);  //�ӳ�3��
				if( 0 == hopper_dump(1) )
				{
					//���
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
	if(CB_H_DET == 0)            //��Ĥ���ϸ߶ȼ�⣬����Ϊ�͵�ƽ
	{
		lamp_flickering();       //�ȵ���˸
		return;
	}
	if(d_state.auto_state == 0)  //���ǿ���
	{
		LOG_COLOUR_GREEN;        //�̵Ƴ���
	}
	else                         //������������
	{
		if(FRAME_DOOR == 1)      //������� ����
		{
			lamp_flickering();   //�ȵ���˸
		}
		else
		{
			LOG_COLOUR_GREEN;    //������Ϊ�̵�
		}
	}
}

//����Ƶ��100hz 
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
//�����λ
u8 reset_motor(void)
{
	if( d_state.reset_state == 1)   //ִ�е����㸴λ
	{
		if(M_MOTOR_R != 0)          //�����������ڸ���λ �ұ� ��ع�����λ
		{
			if(platform_motor.run == 0)
			{
				platform_motor.motor_sn = PLATFORM_MOTOR;
				platform_motor.dir = MAIN_BACKWARD;              //���� ����
				platform_motor.pulse_cnt = 0;
				platform_motor.dis =  platform_motor.max_len;    //����  um   ����Զ����
				platform_motor.acc_tm = 1;                       //����ʱ��
				platform_motor.dec_tm = 1;                       //����ʱ��
				platform_motor.start_v = 0;                      //�����ٶ� um/s
				platform_motor.max_v =  20200;                   //����� um/s  
				platform_motor.end_v = 0;                        //ĩ�ٶ�
				motor_move(&platform_motor); 
			}
		}
		
		if(S_MOTOR_R != 1)           //����Ϊ1    ������������ڸ���λ
		{
			if(scraper_motor.run == 0)                      //�������
			{
				scraper_motor.motor_sn = SCRAPER_MOTOR;
				scraper_motor.dir = SCRAPER_BACKWARD;              //���� ����
				scraper_motor.pulse_cnt = 0;
				scraper_motor.dis =  scraper_motor.max_len;     //����  um  
				scraper_motor.acc_tm = 1;                       //����ʱ��
				scraper_motor.dec_tm = 1;                       //����ʱ��
				scraper_motor.start_v = 0;                      //�����ٶ� um/s
				scraper_motor.max_v =  2000;                    //����� um/s  
				scraper_motor.end_v = 0;                        //ĩ�ٶ�
				motor_move(&scraper_motor);
			}
		}
		if(HOPPER_L == 0 && HOPPER_R == 1)                  //����λ���� ����λû�д�����Ϊ��λ״̬
		{
			if(M_MOTOR_R == 0 && S_MOTOR_R == 1)
			{
				d_state.reset_state = 0;
				return 0;
			}
		}
		else
		{
			if( 0 == hopper_dump(1) )   //�϶���λ
			{
				hopper_dump(0);
			}			
		}
	}
	return 1;
}


