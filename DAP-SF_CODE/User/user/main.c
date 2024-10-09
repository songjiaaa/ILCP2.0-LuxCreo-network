#include "app.h"

int main(void)
{
	Stm32_Clock_Init(336,8,2,7);//,8MHz*336/8/2=168Mhz 
//	MY_NVIC_SetVectorTable(0x08000000,0x10000);
//	RCC->AHB1ENR|=0x7ff;
//	bsp_InitDWT();              //初始化DWT计数器
//	gpio_init();
//	PBout(0) = 1;
	
	OS_CLOSE_INT;

	IO_putchar=uart1_putchar;
	version_release();                   

	xTaskCreate( start_task, "start_task", 128, NULL, 1, &start_task_handler );	

	OS_OPEN_INT; 
	vTaskStartScheduler();
	return 0;
}





