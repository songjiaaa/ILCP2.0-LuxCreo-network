#include "app.h"

u32 test_flash_w = 0x12345678;
u32 test_flash_r = 0;

int main(void)
{
	Stm32_Clock_Init(336,8,2,7);//,8MHz*336/8/2=168Mhz 
//	MY_NVIC_SetVectorTable(0x08000000,0x10000);

	RCC->AHB1ENR|=0x7ff;
	bsp_InitDWT();      //
//	gpio_init();
	OS_CLOSE_INT;
	uart_initial(&uart1,115200);  //dbug
//	uart_initial(&uart2,115200);  //ethernet
//	uart_initial(&uart3,115200);  //wifi
//	uart_initial(&uart6,115200);  //lcd
	cmd_ini(); //

//	ADC1_Config();
	IO_putchar=uart1_putchar;
	version_release();                   
//	if( 0 == flash_eraseAwrite(&test_flash_w,4,(void*)CFG_FLASH_ADDR) )
//		test_flash_r = (*(u32*)CFG_FLASH_ADDR);
	
	xTaskCreate( dap_data_pro_task, "dap_data_pro_task", 256, NULL, 11, &dap_data_pro_handler );
	xTaskCreate( lcd_data_pro_task, "lcd_data_pro_task", 1024, NULL, 10, &lcd_data_pro_handler );
	xTaskCreate( enthernet_data_pro_task, "enthernet_data_pro_task", 1024, NULL, 9, &enthernet_data_pro_handler );
	xTaskCreate( wifi_data_pro_task, "wifi_data_pro_task", 1024, NULL, 8, &wifi_data_pro_handler );	
	xTaskCreate( usb_disk_task, "usb_disk_task", 1024, NULL, 7, &usb_task_handler );	
	xTaskCreate( run_task, "run_task", 512, NULL, 6, &run_task_handler );
	xTaskCreate( get_sensor_data_task, "get_sensor_data_task", 256, NULL, 5, &get_sensor_data_handler );
	xTaskCreate( software_timer_task, "software_timer_task", 256, NULL, 4, &software_timer_handler );
	xTaskCreate( init_task, "init_task",512, NULL, 3, &init_task_handler );	

	OS_OPEN_INT; 
	vTaskStartScheduler();
	return 0;
}





