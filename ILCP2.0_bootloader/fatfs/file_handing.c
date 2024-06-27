#include "file_handing.h"



#define FILE_MAX_TYPE_NUM		7	//最多FILE_MAX_TYPE_NUM个大类
#define FILE_MAX_SUBT_NUM		4	//最多FILE_MAX_SUBT_NUM个小类

 //文件类型列表
u8*const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
{"BIN"},			//BIN文件
{"LRC"},			//LRC文件
{"NES"},			//NES文件
{"TXT","C","H"},	//文本文件
{"WAV","MP3","APE","FLAC"},//支持的音乐文件
{"BMP","JPG","JPEG","GIF"},//图片文件
{"AVI"},			//视频文件
};
///////////////////////////////公共文件区,使用malloc的时候////////////////////////////////////////////
FATFS *fs[_VOLUMES];//逻辑磁盘工作区.	 
FIL *file;	  		//文件1
FIL *ftemp;	  		//文件2.
UINT br,bw;			//读写变量
FILINFO fileinfo;	//文件信息
DIR dir;  			//目录

u8 *fatbuf;			//SD卡数据缓存区


FIL fdst;
FIL f_user;
FATFS f_fs[3];
UINT b_read, b_write;
DSTATUS stat = 0;

void mem_mount(void)
{
	int i = 0;
	//逻辑指针   磁盘号   ，是否立即挂载
	stat = f_mount(&f_fs[1],"1:",1);  //W25Qxx
	
	while( stat != FR_OK )
	{
		W25QXX_Init();
		bsp_DelayMS(100);	
		stat = f_mount(&f_fs[1],"1:",1);  //W25Qxx	
		if(i++ > 100)
		{
			DEBUG("boot:W25Qxx mounting error! \r\n");
			break;
		}
	}
	cfg_get(); //得到flash的数据
	DEBUG("boot:The W25Qxx card is successfully mounted. \r\n");


	stat = f_mount(&f_fs[2],"2:",1);  //U盘
	if(stat == FR_OK)
	{
		DEBUG("boot:The U_Disk card is successfully mounted. \r\n");
	} 
}

//测试文件系统，从sd卡读取bin文件，并打印出来
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
//为exfuns申请内存
//返回值:0,成功
////1,失败
//u8 exfuns_init(void)
//{
//	u8 i;
//	for(i=0;i<_VOLUMES;i++)
//	{
//		fs[i]=(FATFS*)mymalloc(SRAMIN,sizeof(FATFS));	//为磁盘i工作区申请内存	
//		if(!fs[i])break;
//	}
//	file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//为file申请内存
//	ftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//为ftemp申请内存
//	fatbuf=(u8*)mymalloc(SRAMIN,512);				//为fatbuf申请内存
//	if(i==_VOLUMES&&file&&ftemp&&fatbuf)return 0;  //申请有一个失败,即失败.
//	else return 1;	
//}

//将小写字母转为大写字母,如果是数字,则保持不变.
u8 char_upper(u8 c)
{
	if(c<'A')return c;//数字,保持不变.
	if(c>='a')return c-0x20;//变为大写.
	else return c;//大写,保持不变
}	      
//报告文件的类型
//fname:文件名
//返回值:0XFF,表示无法识别的文件类型编号.
//		 其他,高四位表示所属大类,低四位表示所属小类.
u8 f_typetell(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr='\0';//后缀名
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;//偏移到了最后了.
		fname++;
	}
	if(i==250)return 0xff;//错误的字符串.
 	for(i=0;i<5;i++)//得到后缀名
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
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);//全部变为大写 
	for(i=0;i<FILE_MAX_TYPE_NUM;i++)	//大类对比
	{
		for(j=0;j<FILE_MAX_SUBT_NUM;j++)//子类对比
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;//此组已经没有可对比的成员了.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//找到了
			{
				return (i<<4)|j;
			}
		}
	}
	return 0xff;//没找到		 			   
}	 

//得到磁盘剩余容量
//drv:磁盘编号("0:"/"1:")
//total:总容量	 （单位KB）
//free:剩余容量	 （单位KB）
//返回值:0,正常.其他,错误代码
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //得到磁盘信息及空闲簇数量
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//得到总扇区数
	    fre_sect=fre_clust*fs1->csize;			//得到空闲扇区数	   
#if _MAX_SS!=512				  				//扇区大小不是512字节,则转换为512字节
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//单位为KB
		*free=fre_sect>>1;	//单位为KB 
 	}
	return res;
}	


//用户处理程序
u8 USH_User_App(void)
{
//	u32 total,free;
	static u8 usb_stat= 0;
	u8 res=0;
	 
//	res=exf_getfree((u8 *)"2:",&total,&free);
//	if(res==0)
//	{
//		printf("Total capacity: %d MB",total>>10);
//	    printf("Remaining capacity: %d MB",free>>10);
////		LCD_ShowNum(174,180,total>>10,5,16); //显示U盘总容量 MB
////		LCD_ShowNum(174,200,free>>10,5,16);	
//	} 
	
//	mf_scan_files("2:");
//	mf_showfree("2:");
	if(HCD_IsDeviceConnected(&USB_OTG_Core))//设备连接成功
	{
		if(!usb_stat)
			printf("设备已连接\r\n");
		usb_stat = 1;
		cfg_dft.u_disk_state = 99;
	}
	else
	{

	}
	return res;
}


 //遍历文件
 //path:路径
 //返回值:执行结果
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

    res = f_opendir(&dir,(const TCHAR*)path); //打开一个目录
    if (res == FR_OK) 
	{	
		printf("\r\n"); 
		while(1)
		{
	        res = f_readdir(&dir, &fileinfo);                   //读取目录下的一个文件
	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //错误了/到末尾了,退出
	        //if (fileinfo.fname[0] == '.') continue;             //忽略上级目录
#if _USE_LFN
        	fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else							   
        	fn = fileinfo.fname;
#endif	                                              /* It is a file. */
			printf("%s/", path);//打印路径	
			printf("%s\r\n",  fn);//打印文件名	  
		} 
    }	  
    return res;	  
}
//显示剩余容量
//drv:盘符
//返回值:剩余容量(字节)
u32 mf_showfree(u8 *drv)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //得到磁盘信息及空闲簇数量
    res = f_getfree((const TCHAR*)drv,(DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect = (fs1->n_fatent - 2) * fs1->csize;//得到总扇区数
	    fre_sect = fre_clust * fs1->csize;			//得到空闲扇区数	   
#if _MAX_SS!=512
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		if(tot_sect<20480)//总容量小于10M
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n磁盘总容量:%d KB\r\n"
		           "可用空间:%d KB\r\n",
		           tot_sect>>1,fre_sect>>1);
		}else
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n磁盘总容量:%d MB\r\n"
		           "可用空间:%d MB\r\n",
		           tot_sect>>11,fre_sect>>11);
		}
	}
	return fre_sect;
}	




