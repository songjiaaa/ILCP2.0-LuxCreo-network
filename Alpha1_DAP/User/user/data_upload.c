#include "data_upload.h"
//������ͷ
upload_frame_head head = {0};

//����״̬���ϴ�������
idle_upload_frame idle_data = {0};           //���л�̻�����״̬���ϴ����ݰ�

//�̻����ϴ�������
cure_upload_front cure_data_head = {0};      //�̻��л�̻���ͣ�ϴ������ݰ�
char upload_cure_para_data[500] = {0};       //�̻����������
cure_upload_tail cure_data_tail = {0};       //�̻���ʽ��β

upload_end end = {0};

u8 upload_data_buf[1000] = {0};
//t_mode����ģʽ��0x00 WiFi�ϴ�  0x01 ��̫���ϴ�
//type ֡���ͣ�0x01 ����״̬���ϴ�   0x02 �̻�����ͣ״̬���ϴ�   0x03 �̻����������õ���   0x04 OTA����    0x05 �����ϱ�
void netword_package_upload(u8 t_mode,u8 type)
{

	head.head = 0xa55a;
	head.len = 0;

	head.t_mode = t_mode;
	head.t_dir = UPLOADING_DIRECTION;
	head.type = type;
	memcpy(head.version,cfg_dft.version,sizeof(head.version));   //�汾��
	
	memset(upload_data_buf,0x00,sizeof(upload_data_buf));
	if(type == 0x01)          //���л����״̬�ϴ�����
	{

	}
	else if(type == 0x02)     //�̻��л���ͣ �ϴ�����
	{

	}
	else if(type == 0x03 || type == 0x04)
	{

	}
	else if(type == 0x05)
	{
		
	}

	if(head.t_mode == 0)        //WiFi�ϴ�
	{
		esp8266_uart_tx(upload_data_buf,head.len);
		head.run_num ++;	
	}
	else if(head.t_mode == 1)   //��̫���ϴ�
	{
		usr_k5_tx(upload_data_buf,head.len);
		head.run_num ++;
	}
}


//�ϴ�����
void p_data_upload(void)
{

}







































