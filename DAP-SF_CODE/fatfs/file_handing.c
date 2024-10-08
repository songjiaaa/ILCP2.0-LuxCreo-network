#include "file_handing.h"



#define FILE_MAX_TYPE_NUM		7	//���FILE_MAX_TYPE_NUM������
#define FILE_MAX_SUBT_NUM		4	//���FILE_MAX_SUBT_NUM��С��

 //�ļ������б�
u8*const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
{"BIN"},			//BIN�ļ�
{"LRC"},			//LRC�ļ�
{"NES"},			//NES�ļ�
{"TXT","C","H"},	//�ı��ļ�
{"WAV","MP3","APE","FLAC"},//֧�ֵ������ļ�
{"BMP","JPG","JPEG","GIF"},//ͼƬ�ļ�
{"AVI"},			//��Ƶ�ļ�
};




///////////////////////////////�����ļ���,ʹ��malloc��ʱ��////////////////////////////////////////////
FATFS *fs[_VOLUMES];//�߼����̹�����.	 
FIL *file;	  		//�ļ�1
FIL *ftemp;	  		//�ļ�2.
UINT br,bw;			//��д����
FILINFO fileinfo;	//�ļ���Ϣ
DIR dir;  			//Ŀ¼

u8 *fatbuf;			//SD�����ݻ�����


FIL fdst;
FIL f_user;
FATFS f_fs[3];
UINT b_read, b_write;
DSTATUS stat = 0;
#define EMMC_MOUNT_FLAG   "1:w25_mount_ok_flag.TXT"
void mem_mount(void)
{
	int i = 0;
	stat = f_mount(&f_fs[1],"1:",1);  //W25Qxx
	
	while( stat != FR_OK )
	{
		W25QXX_Init();
		W25QXX_CS = 0;
		vTaskDelay(50);	
		stat = f_mount(&f_fs[1],"1:",1);  
		if(i++ > 10)
		{
			DEBUG("W25Qxx mounting error! \r\n");
			break;
		}
	}
	
	if(stat == FR_OK)
	{
		DEBUG("The W25Qxx card is successfully mounted. \r\n");
	}
	else
	{
		//��ʽ���ⲿflash
		stat = f_mkfs("1:",1,4096);       //�߼��������ţ�����1 �ⲿflash�� ���̺� 1  FAT32 �ļ�ϵͳÿ�ش�СΪ4096�ֽ�
		if(stat != FR_OK)
		{
			return;
		}
		stat = f_mount(&f_fs[1],"1:",1);  //���¹���
		if(stat != FR_OK)
		{

		}	
	}
}

//�����ļ�ϵͳ����sd����ȡbin�ļ�������ӡ����
//void test_fatfs(void)
//{
//	FRESULT result;
//	DWORD bin_size = 0;
//	u8 buffer[256] = {0}; 

//	result = f_open(&f_user, "0:/iLuxCure.bin", FA_READ);
//	if(FR_OK == result)
//	{
//		bin_size = f_size(&f_user); 
//		while(bin_size)
//		{
//			bin_size -=  sizeof(buffer);
//			if(bin_size < sizeof(buffer))
//			{
//				bin_size = 0;
//			}
//			stat = f_read(&f_user, buffer, sizeof(buffer),&b_read);   
//			uart_send(buffer,sizeof(buffer),&uart1);
//		}			
//	}
//	f_close(&f_user); 
//}
///////////////////////////////////////////////////////////////////////////////////////
//Ϊexfuns�����ڴ�
//����ֵ:0,�ɹ�
////1,ʧ��
//u8 exfuns_init(void)
//{
//	u8 i;
//	for(i=0;i<_VOLUMES;i++)
//	{
//		fs[i]=(FATFS*)mymalloc(SRAMIN,sizeof(FATFS));	//Ϊ����i�����������ڴ�	
//		if(!fs[i])break;
//	}
//	file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//Ϊfile�����ڴ�
//	ftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//Ϊftemp�����ڴ�
//	fatbuf=(u8*)mymalloc(SRAMIN,512);				//Ϊfatbuf�����ڴ�
//	if(i==_VOLUMES&&file&&ftemp&&fatbuf)return 0;  //������һ��ʧ��,��ʧ��.
//	else return 1;	
//}

//��Сд��ĸתΪ��д��ĸ,���������,�򱣳ֲ���.
u8 char_upper(u8 c)
{
	if(c<'A')return c;//����,���ֲ���.
	if(c>='a')return c-0x20;//��Ϊ��д.
	else return c;//��д,���ֲ���
}	      
//�����ļ�������
//fname:�ļ���
//����ֵ:0XFF,��ʾ�޷�ʶ����ļ����ͱ��.
//		 ����,����λ��ʾ��������,����λ��ʾ����С��.
u8 f_typetell(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr='\0';//��׺��
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;//ƫ�Ƶ��������.
		fname++;
	}
	if(i==250)return 0xff;//������ַ���.
 	for(i=0;i<5;i++)//�õ���׺��
	{
		fname--;
		if(*fname=='.')
		{
			fname++;
			attr=fname;
			break;
		}
  	}
	strcpy((char *)tbuf,(const char*)attr);//copy
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);//ȫ����Ϊ��д 
	for(i=0;i<FILE_MAX_TYPE_NUM;i++)	//����Ա�
	{
		for(j=0;j<FILE_MAX_SUBT_NUM;j++)//����Ա�
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;//�����Ѿ�û�пɶԱȵĳ�Ա��.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//�ҵ���
			{
				return (i<<4)|j;
			}
		}
	}
	return 0xff;//û�ҵ�		 			   
}	 

//�õ�����ʣ������
//drv:���̱��("0:"/"1:")
//total:������	 ����λKB��
//free:ʣ������	 ����λKB��
//����ֵ:0,����.����,�������
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //�õ�������Ϣ�����д�����
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//�õ���������
	    fre_sect=fre_clust*fs1->csize;			//�õ�����������	   
#if _MAX_SS!=512				  				//������С����512�ֽ�,��ת��Ϊ512�ֽ�
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//��λΪKB
		*free=fre_sect>>1;	//��λΪKB 
 	}
	return res;
}	


//�û��������
u8 USH_User_App(void)
{
//	u32 total,free;
//	static u8 usb_stat= 0;
	u8 res=0;

	if(HCD_IsDeviceConnected(&USB_OTG_Core))//�豸���ӳɹ�
	{
		if(cfg_dft.u_disk_state != 99)
		{
			stat = f_mount(&f_fs[2],"2:",1);  //U��
			if(stat == FR_OK)
			{
				cfg_dft.u_disk_state = 99;
				printf("�豸������\r\n");
			}
		}
	}
	else
	{
		
	}
	return res;
}


 //�����ļ�
 //path:·��
 //����ֵ:ִ�н��
u8 mf_scan_files(u8 * path)
{
	FRESULT res;	  
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
	static char lfn[_MAX_LFN];
    fileinfo.lfname = lfn;
    fileinfo.lfsize = sizeof(lfn);
// 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
//	fileinfo.lfname = mymalloc(SRAMIN,fileinfo.lfsize);
#endif		  

    res = f_opendir(&dir,(const TCHAR*)path); //��һ��Ŀ¼
    if (res == FR_OK) 
	{	
		printf("\r\n"); 
		while(1)
		{
	        res = f_readdir(&dir, &fileinfo);                   //��ȡĿ¼�µ�һ���ļ�
	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //������/��ĩβ��,�˳�
	        //if (fileinfo.fname[0] == '.') continue;             //�����ϼ�Ŀ¼
#if _USE_LFN
        	fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else							   
        	fn = fileinfo.fname;
#endif	                                              /* It is a file. */
			printf("%s/", path);//��ӡ·��	
			printf("%s\r\n",  fn);//��ӡ�ļ���	  
		} 
    }	  
    return res;	  
}
//��ʾʣ������
//drv:�̷�
//����ֵ:ʣ������(�ֽ�)
u32 mf_showfree(u8 *drv)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //�õ�������Ϣ�����д�����
    res = f_getfree((const TCHAR*)drv,(DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect = (fs1->n_fatent - 2) * fs1->csize;//�õ���������
	    fre_sect = fre_clust * fs1->csize;			//�õ�����������	   
#if _MAX_SS!=512
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		if(tot_sect<20480)//������С��10M
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n����������:%d KB\r\n"
		           "���ÿռ�:%d KB\r\n",
		           tot_sect>>1,fre_sect>>1);
		}else
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n����������:%d MB\r\n"
		           "���ÿռ�:%d MB\r\n",
		           tot_sect>>11,fre_sect>>11);
		}
	}
	return fre_sect;
}	


//��һ��·�������ļ�����һ��·��
FRESULT my_copyfile(char* destin_path,char* source_path)
{
	static FILINFO fno = {0};
	static FIL fsrc = {0};
	static FIL fdst = {0};
	FRESULT result = FR_OK;
	UINT br = 0, bw = 0;
	char buf[512] = {0};
	result = f_stat(source_path, &fno);             //���Դ·�����Ƿ�����ļ�
	if(FR_OK == result)
	{
		result = f_open(&fsrc,source_path,FA_READ);   //��Դ·���ļ�
		if(FR_OK == result)
		{
			result = f_open(&fdst,destin_path, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);   //����Ŀ��·�����ļ� 
			if(FR_OK == result)
			{
				while(!f_eof(&fsrc))
				{
					result = f_read(&fsrc,buf,sizeof(buf),&br);
					if(FR_OK == result)	
					{
						result = f_write(&fdst,buf,br,&bw);
						if(FR_OK != result)
							break;
					}
//					vTaskDelay(1);              
				}			
			}
			f_close(&fdst); 
		}
		f_close(&fsrc); 
	}
	return result;
}

// �ݹ�ɾ��ָ��·���������ļ������ļ���
FRESULT DeleteDirectoryFiles(const char *path) 
{
    FRESULT fr;
    FILINFO fno;
    DIR dir;
    char filepath[_MAX_LFN + 1];
 
    // ��Ŀ¼
    fr = f_opendir(&dir, path);
    if (fr != FR_OK) return fr;
 
    for (;;) {
        fr = f_readdir(&dir, &fno); // ��ȡĿ¼��
        if (fr != FR_OK || fno.fname[0] == 0) break; // ��������������
 
        if (fno.fname[0] == '.') continue; // ������ǰĿ¼���ϼ�Ŀ¼
 
        // �����ļ���ȫ·��
        sprintf(filepath, "%s/%s", path, fno.fname);
 
        if (fno.fattrib & AM_DIR) { // ��Ŀ¼
            DeleteDirectoryFiles(filepath); // �ݹ�ɾ����Ŀ¼�������ļ�
            f_unlink(filepath); // ɾ������Ŀ¼
        } else { // �ļ�
            f_unlink(filepath); // ɾ���ļ�
        }
    }
 
    f_closedir(&dir); // �ر�Ŀ¼
 
    // ɾ��Ŀ¼����
    return f_unlink(path);
}


