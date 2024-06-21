/*
uart.h
同步方式的串口通信
*/
#ifndef F4_UART_H
#define F4_UART_H

#include "main.h"
#include "stm32f4_sys.h"
#include "my_queue.h"
#include "app.h"


typedef struct
{
	USART_TypeDef *uart;
	Queue que_tx;
	Queue que_rx;
} S_UART; //串口设备结构

extern S_UART uart1;
extern S_UART uart2;
extern S_UART uart3;
extern S_UART uart4;
extern S_UART uart5;
extern S_UART uart6;

void uart_initial(S_UART *obj,int b);

void uart1_tx(u8 *addr,u16 len);
void uart_send(u8 *p,int n,S_UART *obj); //发送数据

int uart1_putchar(int c);
int uart3_putchar(int c);

#endif
