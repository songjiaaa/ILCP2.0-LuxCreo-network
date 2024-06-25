#include "f4_can2.h"

//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:1~3;
//tbs2:时间段2的时间单元.范围:1~8;
//tbs1:时间段1的时间单元.范围:1~16;
//brp :波特率分频器.范围:1~1024;(实际要加1,也就是1~1024) tq=(brp)*tpclk1
//注意以上参数任何一个都不能设为0,否则会乱.
//波特率=Fpclk1/((tbs1+tbs2+1)*brp);
//mode:0,普通模式;1,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Normal_Init(1,8,9,4,1);
//则波特率为:36M/((8+9+1)*4)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败;
int can2_mode_init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp)
{
	u8 mode=0;
	u16 i=0;
 	if(tsjw==0||tbs2==0||tbs1==0||brp==0)return 1;
	tsjw-=1;//先减去1.再用于设置
	tbs2-=1;
	tbs1-=1;
	brp-=1;

	RCC->AHB1ENR|=RCC_AHB1Periph_GPIOB;
	GPIO_Set(GPIOB,PIN12 | PIN13,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
	GPIO_AF_Set(GPIOB,12,GPIO_AF_CAN2);
	GPIO_AF_Set(GPIOB,13,GPIO_AF_CAN2);
					    
	RCC->APB1ENR|=RCC_APB1Periph_CAN2;//使能CAN时钟 CAN使用的是APB1的时钟(max:36M)
	CAN2->MCR=0x0000;		//退出睡眠模式(同时设置所有位为0)
	CAN2->MCR|=1<<0;		//请求CAN进入初始化模式
	while((CAN2->MSR&1<<0)==0)
	{
		i++;
		if(i>100)return 2;	//进入初始化模式失败
	}
	CAN2->MCR|=0<<7;		//非时间触发通信模式
	CAN2->MCR|=1<<6;		//软件自动离线管理
	CAN2->MCR|=0<<5;		//睡眠模式通过软件唤醒(清除CAN2->MCR的SLEEP位)
	CAN2->MCR|=0<<4;		//1禁止报文自动传送,0重传
	CAN2->MCR|=0<<3;		//报文不锁定,新的覆盖旧的
	CAN2->MCR|=1<<2;		//优先级由报文标识符决定
	CAN2->BTR=0x00000000;	//清除原来的设置.
	CAN2->BTR|=mode<<30;	//模式设置 0,普通模式;1,回环模式;
	CAN2->BTR|=tsjw<<24; 	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位
	CAN2->BTR|=tbs2<<20; 	//Tbs2=tbs2+1个时间单位
	CAN2->BTR|=tbs1<<16;	//Tbs1=tbs1+1个时间单位
	CAN2->BTR|=brp<<0;  	//分频系数(Fdiv)为brp+1
							//波特率:Fpclk1/((Tbs1+Tbs2+1)*Fdiv)
	CAN2->MCR&=~(1<<0);		//请求CAN退出初始化模式
	while((CAN2->MSR&1<<0)==1)
	{
		i++;
		if(i>0XFFF0)return 3;//退出初始化模式失败
	}
	//过滤器初始化
	CAN2->FMR|=1<<0;		//过滤器组工作在初始化模式
	CAN2->FA1R&=~(1<<0);	//过滤器0不激活
	CAN2->FS1R|=1<<0; 		//过滤器位宽为32位.
	CAN2->FM1R|=0<<0;		//过滤器0工作在标识符屏蔽位模式
	CAN2->FFA1R|=0<<0;		//过滤器0关联到FIFO0
	CAN2->sFilterRegister[0].FR1=0X00000000;//32位ID
	CAN2->sFilterRegister[0].FR2=0X00000000;//32位MASK
	CAN2->FA1R|=1<<0;		//激活过滤器0
	CAN2->FMR&=0<<0;		//过滤器组进入正常模式

#if CAN_RX0_INT_ENABLE
 	//使用中断接收
	CAN2->IER|=1<<1;		//FIFO0消息挂号中断允许.	    
	MY_NVIC_Init(1,0,USB_LP_CAN2_RX0_IRQn,2);//组2
#endif
	return 0;
}   
//得到在FIFO0/FIFO1中接收到的报文个数.
//fifox:0/1.FIFO编号;
//返回值:FIFO0/FIFO1中的报文个数.
static u8 CAN_Msg_Pend(u8 fifox)
{
	if(fifox==0)return CAN2->RF0R&0x03; 
	else if(fifox==1)return CAN2->RF1R&0x03; 
	else return 0;
}

#if CAN_RX0_INT_ENABLE	//使能RX0中断
//中断服务函数			    
void USB_LP_CAN2_RX0_IRQHandler(void)
{
}
#endif

/////////////////////////////////////////////////////////////////////
//收发接口
CAN_MSG can2_rx_buf; //接收缓存
int can2_rx(void) //接收，返回收到的数据数量
{
	u8 fifox=0;
	if(CAN_Msg_Pend(0)==0)return 0;	//没有接收到数据,直接退出
	can2_rx_buf.ext_std=CAN2->sFIFOMailBox[fifox].RIR&0x04?1:0;//得到标识符选择位的值
 	if(can2_rx_buf.ext_std==0)//标准标识符
	{
		can2_rx_buf.can_id=CAN2->sFIFOMailBox[fifox].RIR>>21;
	}
	else	   //扩展标识符
	{
		can2_rx_buf.can_id=CAN2->sFIFOMailBox[fifox].RIR>>3;
	}
	can2_rx_buf.rtr=CAN2->sFIFOMailBox[fifox].RIR&0x02?1:0;	//得到远程发送请求值.
	can2_rx_buf.len=CAN2->sFIFOMailBox[fifox].RDTR&0x0F;	//得到DLC
 	//*fmi=(CAN2->sFIFOMailBox[FIFONumber].RDTR>>8)&0xFF;//得到FMI
	//接收数据
	((u32*)(can2_rx_buf.data))[0]=CAN2->sFIFOMailBox[fifox].RDLR;
	((u32*)(can2_rx_buf.data))[1]=CAN2->sFIFOMailBox[fifox].RDHR;
  	if(fifox==0)CAN2->RF0R|=0X20;//释放FIFO0邮箱
	else if(fifox==1)CAN2->RF1R|=0X20;//释放FIFO1邮箱	 
	return can2_rx_buf.len;
}
int can2_can_send(void)
{
	u8 mbox;
	if(CAN2->TSR&(1<<26))mbox=0;		//邮箱0为空
	else if(CAN2->TSR&(1<<27))mbox=1;	//邮箱1为空
	else if(CAN2->TSR&(1<<28))mbox=2;	//邮箱2为空
	else return -1;					//无空邮箱,无法发送 
	return mbox;
}
int can2_tx(CAN_MSG* buf) //发送,返回0成功
{	
	s8 mbox;
	u32 tid=buf->can_id;
	mbox=can2_can_send();
	if(mbox<0) return 1;
	CAN2->sTxMailBox[mbox].TIR=0;		//清除之前的设置
	if(buf->ext_std==0)	//标准帧
	{
		tid&=0x7ff;//取低11位stdid
		tid<<=21;
	}else		//扩展帧
	{
		tid&=0X1FFFFFFF;//取低32位extid
		tid<<=3;
	}
	CAN2->sTxMailBox[mbox].TIR|=tid;
	CAN2->sTxMailBox[mbox].TIR|=buf->ext_std<<2;
	CAN2->sTxMailBox[mbox].TIR|=buf->rtr<<1;
	buf->len&=0X0F;//得到低四位
	CAN2->sTxMailBox[mbox].TDTR&=~(0X0000000F);
	CAN2->sTxMailBox[mbox].TDTR|=buf->len;	//设置DLC.
	//待发送数据存入邮箱.
	CAN2->sTxMailBox[mbox].TDLR=((u32*)(buf->data))[0];
	CAN2->sTxMailBox[mbox].TDHR=((u32*)(buf->data))[1];
	CAN2->sTxMailBox[mbox].TIR|=1<<0; 	//请求发送邮箱数据
	return 0;
}
void can2_check_err(void) //检查是否有错误
{
	//return ;
	if(!(CAN2->TSR & (1<<1))) //若发送尝试失败
	{
		CAN2->TSR |= (1<<7); //终止发送
	}
	if(!(CAN2->TSR & (1<<9)))
	{
		CAN2->TSR |= (1<<15);
	}
	if(!(CAN2->TSR & (1<<17)))
	{
		CAN2->TSR |= (1<<23);
	}
}

