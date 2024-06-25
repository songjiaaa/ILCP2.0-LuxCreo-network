#include "avflash.h"
//均匀擦写，使用一个配置包结构，在一段flash中均匀擦写。
//包结构长度必须是页大小的公约数
//均匀擦写的区域起始必须与页对齐
//顺序写完后，全部擦除，然后从头开始再写

extern int flash_erase(void *pf); //擦MFLASH扇区,返回下一扇区的起始
extern int flash_write(u16 *p,int n,void *addr); //直接写入(按2字节)
////////////////////////////////////////////////////////////////////////////
//均匀擦写模式
u32 AV_FLASH_START=0x0800c800; //50K的位置
u32 AV_FLASH_LEN=(1024*10); //均匀擦写配置区域总长度(字节)
#define AV_FLASH_END (void*)(AV_FLASH_START+AV_FLASH_LEN)
#pragma pack(1)
typedef struct
{ //1 1：空闲，1 0：有效，0 0：失效
	u16 h0; //0xffff表示有效或空闲,0表示失效
	u16 h1; //0xffff表示空闲,0表示有效
	u16 data[124/2]; //可用于配置数据存储的区域
} S_AV_FLASH; //大小必须是页大小的公约数
#pragma pack()

void *av_flash_read(void) //在均匀擦写Flash中读取数据，返回有效数据指针
{
	S_AV_FLASH *p=(S_AV_FLASH*)AV_FLASH_START;
	for(;p<AV_FLASH_END;p++) //在flash段中找到有效的那个包
	{
		if(p->h0==0xffff && p->h1==0) //若有效
		{
			return p;
		}
	}
	return 0;
}
int av_flash_write(u16 *p16,int n) //写，输入数据首指针，长度单位字节,必须在规定的大小内
{
	int w_flag=0; //是否写了
	S_AV_FLASH *p;
	if(n>sizeof(S_AV_FLASH)-4) return -1; //长度错误//不能写太长
start:
	for(p=(S_AV_FLASH*)AV_FLASH_START;p<AV_FLASH_END;p++) //从头开始，看哪个块是当前有效的
	{
		u16 tmp=0;
		if(p->h0==0xffff && p->h1==0) //若有效
		{
			flash_write(&tmp,2,(void*)&(p->h0)); //写成无效
			if(p->h0!=0) return -2; //写无效失败
		}
		else if(p->h0==0xffff && p->h1==0xffff) //若空闲,写，不管是否已经擦了有效的
		{
			int i;
			flash_write(&tmp,2,(void*)&(p->h1));
			if(p->h1!=0) return -3; //写占用失败
			flash_write(p16,n,p->data);
			for(i=0;i<n/2;i++)
			{
				if(p->data[i]!=p16[i]) return -4; //数据校验错误
			}
			w_flag=1;
			break;
		}
	}
	if(w_flag==0) //若没找到空闲，就擦除所有
	{
		int i;
		u32 FLASH_PAGE=0;
		for(i=AV_FLASH_START;i<(int)AV_FLASH_END;i+=FLASH_PAGE)
		{
			FLASH_PAGE=flash_erase((void *)i);
			if(FLASH_PAGE==0) break; //若擦除失败
	 	}
		w_flag=2; //防止无限循环
		goto start;
	}
	else if(w_flag==2)
	{
		return -5; //擦除错误
	}
	return 0;
}

