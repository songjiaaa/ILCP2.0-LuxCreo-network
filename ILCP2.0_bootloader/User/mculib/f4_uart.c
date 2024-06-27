/*
uart.c
同步方式的串口通信
*/
#include "f4_uart.h"

//串口结构
S_UART uart1=
{
	USART1,
};
static u8 uart1_tx_buf[2048];
static u8 uart1_rx_buf[2048];
S_UART uart2=
{
	USART2,
};
static u8 uart2_tx_buf[500];
static u8 uart2_rx_buf[500];
S_UART uart3=
{
	USART3,
};
static u8 uart3_tx_buf[500];
static u8 uart3_rx_buf[500];
S_UART uart4=
{
	UART4,
};
static u8 uart4_tx_buf[500];
static u8 uart4_rx_buf[500];
S_UART uart5=
{
	UART5,
};
static u8 uart5_tx_buf[500];
static u8 uart5_rx_buf[500];
S_UART uart6=
{
	USART6,
};
static u8 uart6_tx_buf[2048];
static u8 uart6_rx_buf[2048];

void uart_initial(S_UART *obj,int b)
{
	u32 nvic_num=USART1_IRQn;
	//b*=2; 
	switch((u32)obj->uart)
	{
	case (u32)USART1:
		RCC->APB2ENR|=RCC_APB2Periph_USART1;  //使能串口时钟 
		GPIO_Set(GPIOA,PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
		GPIO_AF_Set(GPIOA,9,GPIO_AF_USART1);	//PA9,AF7
		GPIO_AF_Set(GPIOA,10,GPIO_AF_USART1);//PA10,AF7
		b/=2; //1号串口主频高一倍
		nvic_num=USART1_IRQn;
		Queue_ini(uart1_tx_buf,sizeof(uart1_tx_buf),&(obj->que_tx));
		Queue_ini(uart1_rx_buf,sizeof(uart1_rx_buf),&(obj->que_rx));
		break;
	case (u32)USART2:
		RCC->APB1ENR|=RCC_APB1Periph_USART2;  //使能串口时钟 
		GPIO_Set(GPIOA,PIN2|PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PA2,PA3,复用功能,上拉输出
		GPIO_AF_Set(GPIOA,2,GPIO_AF_USART2);	//PA2,AF7
		GPIO_AF_Set(GPIOA,3,GPIO_AF_USART2);//PA3,AF7
		nvic_num=USART2_IRQn;
		Queue_ini(uart2_tx_buf,sizeof(uart2_tx_buf),&(obj->que_tx));
		Queue_ini(uart2_rx_buf,sizeof(uart2_rx_buf),&(obj->que_rx));
		break;
	case (u32)USART3:
		RCC->APB1ENR|=RCC_APB1Periph_USART3;  //使能串口时钟 
		GPIO_Set(GPIOB,PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
		GPIO_AF_Set(GPIOB,10,GPIO_AF_USART3);
		GPIO_AF_Set(GPIOB,11,GPIO_AF_USART3);
		nvic_num=USART3_IRQn;
		Queue_ini(uart3_tx_buf,sizeof(uart3_tx_buf),&(obj->que_tx));
		Queue_ini(uart3_rx_buf,sizeof(uart3_rx_buf),&(obj->que_rx));
		break;
	case (u32)UART4:
		RCC->APB1ENR|=RCC_APB1Periph_UART4;  //使能串口时钟 
		GPIO_Set(GPIOC,PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
		GPIO_AF_Set(GPIOC,10,GPIO_AF_UART4);
		GPIO_AF_Set(GPIOC,11,GPIO_AF_UART4);
		nvic_num=UART4_IRQn;
		Queue_ini(uart4_tx_buf,sizeof(uart4_tx_buf),&(obj->que_tx));
		Queue_ini(uart4_rx_buf,sizeof(uart4_rx_buf),&(obj->que_rx));
		break;
	case (u32)UART5:
		RCC->APB1ENR|=RCC_APB1Periph_UART5;  //使能串口时钟 
		GPIO_Set(GPIOC,PIN12,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
		GPIO_Set(GPIOD,PIN2,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
		GPIO_AF_Set(GPIOC,12,GPIO_AF_UART5);
		GPIO_AF_Set(GPIOD,2,GPIO_AF_UART5);
		nvic_num=UART5_IRQn;
		Queue_ini(uart5_tx_buf,sizeof(uart5_tx_buf),&(obj->que_tx));
		Queue_ini(uart5_rx_buf,sizeof(uart5_rx_buf),&(obj->que_rx));
		break;
	case (u32)USART6:
		RCC->APB2ENR|=RCC_APB2Periph_USART6;  //使能串口时钟 
		GPIO_Set(GPIOC,PIN6|PIN7,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
		GPIO_AF_Set(GPIOC,6,GPIO_AF_USART6);
		GPIO_AF_Set(GPIOC,7,GPIO_AF_USART6);
		b/=2; //1号串口主频高一倍
		nvic_num=USART6_IRQn;
		Queue_ini(uart6_tx_buf,sizeof(uart6_tx_buf),&(obj->que_tx));
		Queue_ini(uart6_rx_buf,sizeof(uart6_rx_buf),&(obj->que_rx));
		break;
	default:
		break;
	}
	obj->uart->SR=0;
	obj->uart->DR=0;
	obj->uart->BRR=42000000/b; //
	//obj->uart->CR1 = (u16)0x206c;	//使能发送中断
	obj->uart->CR1 = (u16)0x202c;
	obj->uart->SR = 0;

	MY_NVIC_Init(2,0,nvic_num,4);
}

void uart_send(u8 *p,int n,S_UART *obj) //发送数据
{
	int i;
	OS_CLOSE_INT; //进入保护区，防止中断取数时冲突
	for(i=0;i<n;i++)
	{
		Queue_set_1(p[i],&(obj->que_tx));
	}
	obj->uart->CR1 |= (1<<7);//TXE
	OS_OPEN_INT;
}
void uart_irq(S_UART *obj)
{
	u16 t;
	if(obj->uart->SR & 0x28)//接收和过载
	{
		t=(u8)(obj->uart->DR);
		Queue_set_1(t,&(obj->que_rx));
	}
	if(obj->uart->CR1 & (1<<7) && obj->uart->SR & 0x80)//发送空中断TXE
	{
		u8 tmp;
		if(Queue_get_1(&tmp,&(obj->que_tx))==0)
		{
			obj->uart->DR=tmp;
		}
		else
		{
			obj->uart->CR1 &= ~(1<<7);//TXE
		}
	}
}

void USART1_IRQHandler(void)
{
	uart_irq(&uart1);
}
void USART2_IRQHandler(void)
{
	uart_irq(&uart2);
}
void USART3_IRQHandler(void)
{
	uart_irq(&uart3);
}
void UART4_IRQHandler(void)
{
	uart_irq(&uart4);
}
void UART5_IRQHandler(void)
{
	uart_irq(&uart5);
}
void USART6_IRQHandler(void)
{
	uart_irq(&uart6);
}

//C标准库的重定向
//int uart1_putchar(int c)
//{
	//USART1->DR=c;
	//while(!(USART1->SR & 0x40));
	//USART1->SR &= ~0x00000040;
	//return c;	//IAR编译器里这里不能返回0
//}
int uart1_putchar(int c)
{
	u8 t=c;
	uart_send(&t,1,&uart1);
	return c;	//IAR编译器里这里不能返回0
}
int uart3_putchar(int c)
{
	u8 t=c;
	uart_send(&t,1,&uart3);
	return c;	//IAR编译器里这里不能返回0
}

