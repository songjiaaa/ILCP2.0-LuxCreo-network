// SPDX-License-Identifier: GPL-3.0-only

#ifndef __UART_DEVICE_H
#define __UART_DEVICE_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "stm32f4xx.h" 
#include "queue.h" 
#include "semphr.h"
#include "FreeRTOS.h"
//#include "f4_uart.h"

//typedef struct 
//{
//    USART_TypeDef *huart;
//    GPIO_InitTypeDef* GPIOx_485;
//    QueueHandle_t xRxQueue;
//    SemaphoreHandle_t xTxSem;
//    uint8_t rxdata;
//}UART_Data;


struct UART_Device {
    char *name;
	int (*Init)( struct UART_Device *pDev, int baud, char parity, int data_bit, int stop_bit);
	int (*Send)( struct UART_Device *pDev, uint8_t *datas, uint32_t len, int timeout);
	int (*RecvByte)( struct UART_Device *pDev, uint8_t *data, int timeout);
	int (*Flush)(struct UART_Device *pDev);
    void *priv_data;
};

struct UART_Device *GetUARTDevice(char *name);

    
#endif /* __UART_DEVICE_H */

