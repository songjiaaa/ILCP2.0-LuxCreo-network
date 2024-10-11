#include "uart_device.h"


struct UART_Device g_uart4_dev;

struct UART_Data {
    S_UART *huart;
    GPIO_InitTypeDef* GPIOx_485;
    QueueHandle_t xRxQueue;
    SemaphoreHandle_t xTxSem;
    uint8_t rxdata;
};

static struct UART_Data g_uart4_data = {
    &uart4,
};


static int stm32_uart_init(struct UART_Device *pDev, int baud, char parity, int data_bit, int stop_bit)
{
    struct UART_Data * uart_data = pDev->priv_data;
    
	if (!uart_data->xRxQueue)
	{
		uart_data->xRxQueue = xQueueCreate(200, 1);
		uart_data->xTxSem   = xSemaphoreCreateBinary( );
		
        /* 配置RS485转换芯片的方向引脚,让它输出0表示接收 */
//        HAL_GPIO_WritePin(uart_data->GPIOx_485, uart_data->GPIO_Pin_485, GPIO_PIN_RESET);
//		
//		HAL_UART_Receive_IT(uart_data->huart, &uart_data->rxdata, 1);
	}
	return 0;
}

static int stm32_uart_send(struct UART_Device *pDev, uint8_t *datas, uint32_t len, int timeout)
{
    struct UART_Data * uart_data = pDev->priv_data;
    
    /* 配置RS485转换芯片的方向引脚,让它输出1表示发送 */
//    HAL_GPIO_WritePin(uart_data->GPIOx_485, uart_data->GPIO_Pin_485, GPIO_PIN_SET);
//    
//	HAL_UART_Transmit_IT(uart_data->huart, datas, len);
	
	uart_send(datas,len,uart_data->huart);
	/* 等待1个信号量(为何不用mutex? 因为在中断里Give mutex会出错) */
	if (pdTRUE == xSemaphoreTake(uart_data->xTxSem, timeout))
	{
//        HAL_GPIO_WritePin(uart_data->GPIOx_485, uart_data->GPIO_Pin_485, GPIO_PIN_RESET);
		return 0;
	}
	else
	{
//        HAL_GPIO_WritePin(uart_data->GPIOx_485, uart_data->GPIO_Pin_485, GPIO_PIN_RESET);
		return -1;
	}
}


static int stm32_uart_recv(struct UART_Device *pDev, uint8_t *pData, int timeout)
{
    struct UART_Data * uart_data = pDev->priv_data;
	if (pdPASS == xQueueReceive(uart_data->xRxQueue, pData, timeout))
	{
		return 0;
	}
	else
		return -1;
}


static int stm32_uart_flush(struct UART_Device *pDev)
{
    struct UART_Data * uart_data = pDev->priv_data;
	int cnt = 0;
	uint8_t data;
	while (1)
	{
		if (pdPASS != xQueueReceive(uart_data->xRxQueue, &data, 0))
			break;
		cnt++;
	}
	return cnt;
}


struct UART_Device g_uart4_dev = {"uart4", stm32_uart_init, stm32_uart_send, stm32_uart_recv, stm32_uart_flush, &g_uart4_data};


static struct UART_Device *g_uart_devices[] = {&g_uart4_dev};


struct UART_Device *GetUARTDevice(char *name)
{
	int i = 0;
	for (i = 0; i < sizeof(g_uart_devices)/sizeof(g_uart_devices[0]); i++)
	{
		if (!strcmp(name, g_uart_devices[i]->name))
			return g_uart_devices[i];
	}
	
	return NULL;
}



void UART4_IRQHandler(void)
{
	u16 t;
	S_UART *obj = &uart4;
	struct UART_Data *uart_data = g_uart4_dev.priv_data; 
	
	if(obj->uart->SR & 0x28)//接收和过载
	{
		t=(u8)(obj->uart->DR);    
		uart_data->rxdata = t;		
		xQueueSendFromISR(uart_data->xRxQueue, (const void *)&uart_data->rxdata, NULL);
	}
	if(obj->uart->CR1 & (1<<7) && obj->uart->SR & 0x80)//发送空中断TXE
	{
		u8 tmp;
		if(Queue_get_1(&tmp,&(obj->que_tx))==0)
		{
			obj->uart->DR = tmp;
		}
		else
		{
			obj->uart->CR1 &= ~(1<<7);//TXE
			xSemaphoreGiveFromISR(uart_data->xTxSem, NULL);
		}
	}
	
	if(uart4.uart->CR1 & (1<<4) && uart4.uart->SR & 0x10)  //空闲中断
	{
		uart4.uart->SR;
		uart4.uart->DR;	
	}
	
}


