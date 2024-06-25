#ifndef _FILE_HANDING_H_
#define _FILE_HANDING_H_ 

#include "stm32f4xx.h"
#include "ff.h"
#include "string.h"
#include "app.h" 
#include "debug.h"

extern FATFS *fs[_VOLUMES];  
extern FIL *file;	 
extern FIL *ftemp;	 
extern UINT br,bw;
extern FILINFO fileinfo;
extern DIR dir;
extern u8 *fatbuf;//SD卡数据缓存区


//f_typetell返回的类型定义
//根据表FILE_TYPE_TBL获得.在exfuns.c里面定义
#define T_BIN		0X00	//bin文件
#define T_LRC		0X10	//lrc文件
#define T_NES		0X20	//nes文件

#define T_TEXT		0X30	//.txt文件
#define T_C			0X31	//.c文件
#define T_H			0X32    //.h文件

#define T_WAV		0X40	//WAV文件
#define T_MP3		0X41	//MP3文件 
#define T_APE		0X42	//APE文件
#define T_FLAC		0X43	//FLAC文件

#define T_BMP		0X50	//bmp文件
#define T_JPG		0X51	//jpg文件
#define T_JPEG		0X52	//jpeg文件		 
#define T_GIF		0X53	//gif文件   

#define T_AVI		0X60	//avi文件  



#define	HMI_FR_OK				    "Succeeded." 
#define	HMI_FR_DISK_ERR			    "A hard error occurred in the low level disk I/O layer."
#define	HMI_FR_INT_ERR				"Assertion failed."
#define	HMI_FR_NOT_READY			"The physical drive cannot work."
#define	HMI_FR_NO_FILE				"Could not find the file."
#define	HMI_FR_NO_PATH				"Could not find the path."
#define	HMI_FR_INVALID_NAME		    "The path name format is invalid."
#define	HMI_FR_DENIED				"Access denied due to prohibited access or directory full."
#define	HMI_FR_EXIST				"Access denied due to prohibited access."
#define	HMI_FR_INVALID_OBJECT		"The file/directory object is invalid."
#define	HMI_FR_WRITE_PROTECTED		"The physical drive is write protected."
#define	HMI_FR_INVALID_DRIVE		"The logical drive number is invalid."
#define	HMI_FR_NOT_ENABLED			"The volume has no work area."
#define	HMI_FR_NO_FILESYSTEM		"There is no valid FAT volume."
#define	HMI_FR_MKFS_ABORTED		    "The f_mkfs() aborted due to any parameter error."
#define	HMI_FR_TIMEOUT				"Could not get a grant to access the volume within defined period."
#define	HMI_FR_LOCKED				"The operation is rejected according to the file sharing policy."
#define	HMI_FR_NOT_ENOUGH_CORE		"LFN working buffer could not be allocated."
#define	HMI_FR_TOO_MANY_OPEN_FILES	"Number of open files > _FS_SHARE."
#define	HMI_FR_INVALID_PARAMETER	"Given parameter is invalid."

static const char f_result[20][70] = 
{
	HMI_FR_OK,				    /* (0) Succeeded */
	HMI_FR_DISK_ERR,			/* (1) A hard error occurred in the low level disk I/O layer */
	HMI_FR_INT_ERR,				/* (2) Assertion failed */
	HMI_FR_NOT_READY,			/* (3) The physical drive cannot work */
	HMI_FR_NO_FILE,				/* (4) Could not find the file */
	HMI_FR_NO_PATH,				/* (5) Could not find the path */
	HMI_FR_INVALID_NAME,		/* (6) The path name format is invalid */
	HMI_FR_DENIED,				/* (7) Access denied due to prohibited access or directory full */
	HMI_FR_EXIST,				/* (8) Access denied due to prohibited access */
	HMI_FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	HMI_FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	HMI_FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
	HMI_FR_NOT_ENABLED,			/* (12) The volume has no work area */
	HMI_FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
	HMI_FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any parameter error */
	HMI_FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	HMI_FR_LOCKED,				/* (16) The operation is rejected according to the file sharing policy */
	HMI_FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	HMI_FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_SHARE */
	HMI_FR_INVALID_PARAMETER	/* (19) Given parameter is invalid */
};







extern FIL fdst;
extern FIL f_user;
extern FATFS f_fs[3];
extern UINT b_read, b_write;


void mem_mount(void); 
//u8 exfuns_init(void);							//申请内存
u8 f_typetell(u8 *fname);						//识别文件类型
u8 exf_getfree(u8 *drv,u32 *total,u32 *free);	//得到磁盘总容量和剩余容量
u32 exf_fdsize(u8 *fdname);						//得到文件夹大小		

u8 USH_User_App(void);
u8 mf_scan_files(u8 * path);
u32 mf_showfree(u8 *drv);

FRESULT my_copyfile(char* destin_path,char* source_path);

FRESULT DeleteDirectoryFiles(const char *path);

#endif



