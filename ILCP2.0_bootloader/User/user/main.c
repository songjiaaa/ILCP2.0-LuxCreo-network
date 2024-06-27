#include "app.h"

int main(void)
{
	Stm32_Clock_Init(336,8,2,7);//设置时钟,8MHz*336/8/2=168Mhz Fusb是除以7
	RCC->AHB1ENR|=0x7ff;//使能所有PORT口时钟
	systime_initial(1000);//Hz
	bsp_InitDWT();      //DWT时间戳
	gpio_init();

	uart_initial(&uart1,115200); //调试串口
	uart_initial(&uart3,115200);
	uart_initial(&uart6,115200);
	ws2812_init();	
	cmd_ini(); //使用串口1作为命令串口
	OS_OPEN_INT; 
	IO_putchar=uart1_putchar;
	W25QXX_Init();	
    USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);	
	mem_mount();

	printf("boot:test_uart\r\n");
	printf("boot:save_config.upgrade_state : %d\r\n",save_config.upgrade_state);  
	
	while(1)
	{
		u8 tt;
		if(uart1.que_rx.dlen>0) //这个只读不用锁
		{
			while(get_que_data(&tt,&uart1.que_rx)==0)
			{
				cmd_fun(tt);
			}
		}
		if(uart6.que_rx.dlen>0) //这个只读不用锁
		{
			while(get_que_data(&tt,&uart6.que_rx)==0)
			{
				rec_sync(tt,&button_ctrl_pack);
			}
		}
		if(SYS_task & TASK_POOL_1000) //1000Hz
		{
			SYS_task &= ~TASK_POOL_1000;
			app_poll();
		}
	}
}

void SysTick_Handler(void) //1000Hz
{
	SYS_time++;
	START_TASK(TASK_POOL_1000);
}



