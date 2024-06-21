#ifndef STM32F4_SYS_H
#define STM32F4_SYS_H	 

#include "stm32f4xx.h" 
#include "main.h"

//位带操作
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014     

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入
////////////////////////////////////////////////////////////////////////////////// 
//Ex_NVIC_Config专用定义
#define GPIO_A 				0
#define GPIO_B 				1
#define GPIO_C				2
#define GPIO_D 				3
#define GPIO_E 				4
#define GPIO_F 				5
#define GPIO_G 				6 
#define GPIO_H 				7 
#define GPIO_I 				8 

#define FTIR   				1  		//下降沿触发
#define RTIR   				2  		//上升沿触发

//GPIO设置专用宏定义
#define GPIO_MODE_IN    	0		//普通输入模式
#define GPIO_MODE_OUT		1		//普通输出模式
#define GPIO_MODE_AF		2		//AF功能模式
#define GPIO_MODE_AIN		3		//模拟输入模式

#define GPIO_SPEED_2M		0		//GPIO速度2Mhz
#define GPIO_SPEED_25M		1		//GPIO速度25Mhz
#define GPIO_SPEED_50M		2		//GPIO速度50Mhz
#define GPIO_SPEED_100M		3		//GPIO速度100Mhz

#define GPIO_PUPD_NONE		0		//不带上下拉
#define GPIO_PUPD_PU		1		//上拉
#define GPIO_PUPD_PD		2		//下拉
#define GPIO_PUPD_RES		3		//保留 

#define GPIO_OTYPE_PP		0		//推挽输出
#define GPIO_OTYPE_OD		1		//开漏输出 

//GPIO引脚编号定义
#define PIN0				(1<<0)
#define PIN1				(1<<1)
#define PIN2				(1<<2)
#define PIN3				(1<<3)
#define PIN4				(1<<4)
#define PIN5				(1<<5)
#define PIN6				(1<<6)
#define PIN7				(1<<7)
#define PIN8				(1<<8)
#define PIN9				(1<<9)
#define PIN10				(1<<10)
#define PIN11				(1<<11)
#define PIN12				(1<<12)
#define PIN13				(1<<13)
#define PIN14				(1<<14)
#define PIN15				(1<<15)

//GPIO
#define PAi      GPIOA->IDR
#define PAo      GPIOA->ODR
#define PBi      GPIOB->IDR
#define PBo      GPIOB->ODR
#define PCi      GPIOC->IDR
#define PCo      GPIOC->ODR
#define PDi      GPIOD->IDR
#define PDo      GPIOD->ODR
#define PEi      GPIOE->IDR
#define PEo      GPIOE->ODR
#define PFi      GPIOF->IDR
#define PFo      GPIOF->ODR

////////////////////////////////////////////////////////////////////////////////// 
//各设备的时钟
#define RCC_AHB1Periph_GPIOA             ((uint32_t)0x00000001)
#define RCC_AHB1Periph_GPIOB             ((uint32_t)0x00000002)
#define RCC_AHB1Periph_GPIOC             ((uint32_t)0x00000004)
#define RCC_AHB1Periph_GPIOD             ((uint32_t)0x00000008)
#define RCC_AHB1Periph_GPIOE             ((uint32_t)0x00000010)
#define RCC_AHB1Periph_GPIOF             ((uint32_t)0x00000020)
#define RCC_AHB1Periph_GPIOG             ((uint32_t)0x00000040)
#define RCC_AHB1Periph_GPIOH             ((uint32_t)0x00000080)
#define RCC_AHB1Periph_GPIOI             ((uint32_t)0x00000100) 
#define RCC_AHB1Periph_GPIOJ             ((uint32_t)0x00000200)
#define RCC_AHB1Periph_GPIOK             ((uint32_t)0x00000400)
#define RCC_AHB1Periph_CRC               ((uint32_t)0x00001000)
#define RCC_AHB1Periph_FLITF             ((uint32_t)0x00008000)
#define RCC_AHB1Periph_SRAM1             ((uint32_t)0x00010000)
#define RCC_AHB1Periph_SRAM2             ((uint32_t)0x00020000)
#define RCC_AHB1Periph_BKPSRAM           ((uint32_t)0x00040000)
#define RCC_AHB1Periph_SRAM3             ((uint32_t)0x00080000)
#define RCC_AHB1Periph_CCMDATARAMEN      ((uint32_t)0x00100000)
#define RCC_AHB1Periph_DMA1              ((uint32_t)0x00200000)
#define RCC_AHB1Periph_DMA2              ((uint32_t)0x00400000)
#define RCC_AHB1Periph_DMA2D             ((uint32_t)0x00800000)
#define RCC_AHB1Periph_ETH_MAC           ((uint32_t)0x02000000)
#define RCC_AHB1Periph_ETH_MAC_Tx        ((uint32_t)0x04000000)
#define RCC_AHB1Periph_ETH_MAC_Rx        ((uint32_t)0x08000000)
#define RCC_AHB1Periph_ETH_MAC_PTP       ((uint32_t)0x10000000)
#define RCC_AHB1Periph_OTG_HS            ((uint32_t)0x20000000)
#define RCC_AHB1Periph_OTG_HS_ULPI       ((uint32_t)0x40000000)

#define RCC_AHB2Periph_DCMI              ((uint32_t)0x00000001)
#define RCC_AHB2Periph_CRYP              ((uint32_t)0x00000010)
#define RCC_AHB2Periph_HASH              ((uint32_t)0x00000020)
#define RCC_AHB2Periph_RNG               ((uint32_t)0x00000040)
#define RCC_AHB2Periph_OTG_FS            ((uint32_t)0x00000080)

#define RCC_APB1Periph_TIM2              ((uint32_t)0x00000001)
#define RCC_APB1Periph_TIM3              ((uint32_t)0x00000002)
#define RCC_APB1Periph_TIM4              ((uint32_t)0x00000004)
#define RCC_APB1Periph_TIM5              ((uint32_t)0x00000008)
#define RCC_APB1Periph_TIM6              ((uint32_t)0x00000010)
#define RCC_APB1Periph_TIM7              ((uint32_t)0x00000020)
#define RCC_APB1Periph_TIM12             ((uint32_t)0x00000040)
#define RCC_APB1Periph_TIM13             ((uint32_t)0x00000080)
#define RCC_APB1Periph_TIM14             ((uint32_t)0x00000100)
#define RCC_APB1Periph_WWDG              ((uint32_t)0x00000800)
#define RCC_APB1Periph_SPI2              ((uint32_t)0x00004000)
#define RCC_APB1Periph_SPI3              ((uint32_t)0x00008000)
#define RCC_APB1Periph_USART2            ((uint32_t)0x00020000)
#define RCC_APB1Periph_USART3            ((uint32_t)0x00040000)
#define RCC_APB1Periph_UART4             ((uint32_t)0x00080000)
#define RCC_APB1Periph_UART5             ((uint32_t)0x00100000)
#define RCC_APB1Periph_I2C1              ((uint32_t)0x00200000)
#define RCC_APB1Periph_I2C2              ((uint32_t)0x00400000)
#define RCC_APB1Periph_I2C3              ((uint32_t)0x00800000)
#define RCC_APB1Periph_CAN1              ((uint32_t)0x02000000)
#define RCC_APB1Periph_CAN2              ((uint32_t)0x04000000)
#define RCC_APB1Periph_PWR               ((uint32_t)0x10000000)
#define RCC_APB1Periph_DAC               ((uint32_t)0x20000000)
#define RCC_APB1Periph_UART7             ((uint32_t)0x40000000)
#define RCC_APB1Periph_UART8             ((uint32_t)0x80000000)

#define RCC_APB2Periph_TIM1              ((uint32_t)0x00000001)
#define RCC_APB2Periph_TIM8              ((uint32_t)0x00000002)
#define RCC_APB2Periph_USART1            ((uint32_t)0x00000010)
#define RCC_APB2Periph_USART6            ((uint32_t)0x00000020)
#define RCC_APB2Periph_ADC               ((uint32_t)0x00000100)
#define RCC_APB2Periph_ADC1              ((uint32_t)0x00000100)
#define RCC_APB2Periph_ADC2              ((uint32_t)0x00000200)
#define RCC_APB2Periph_ADC3              ((uint32_t)0x00000400)
#define RCC_APB2Periph_SDIO              ((uint32_t)0x00000800)
#define RCC_APB2Periph_SPI1              ((uint32_t)0x00001000)
#define RCC_APB2Periph_SPI4              ((uint32_t)0x00002000)
#define RCC_APB2Periph_SYSCFG            ((uint32_t)0x00004000)
#define RCC_APB2Periph_TIM9              ((uint32_t)0x00010000)
#define RCC_APB2Periph_TIM10             ((uint32_t)0x00020000)
#define RCC_APB2Periph_TIM11             ((uint32_t)0x00040000)
#define RCC_APB2Periph_SPI5              ((uint32_t)0x00100000)
#define RCC_APB2Periph_SPI6              ((uint32_t)0x00200000)
#define RCC_APB2Periph_SAI1              ((uint32_t)0x00400000)
#define RCC_APB2Periph_LTDC              ((uint32_t)0x04000000)

////////////////////////////////////////////////////////////////////////////////// 
#define GPIO_AF_RTC_50Hz      ((uint8_t)0x00)  /* RTC_50Hz Alternate Function mapping */
#define GPIO_AF_MCO           ((uint8_t)0x00)  /* MCO (MCO1 and MCO2) Alternate Function mapping */
#define GPIO_AF_TAMPER        ((uint8_t)0x00)  /* TAMPER (TAMPER_1 and TAMPER_2) Alternate Function mapping */
#define GPIO_AF_SWJ           ((uint8_t)0x00)  /* SWJ (SWD and JTAG) Alternate Function mapping */
#define GPIO_AF_TRACE         ((uint8_t)0x00)  /* TRACE Alternate Function mapping */
#define GPIO_AF_TIM1          ((uint8_t)0x01)  /* TIM1 Alternate Function mapping */
#define GPIO_AF_TIM2          ((uint8_t)0x01)  /* TIM2 Alternate Function mapping */
#define GPIO_AF_TIM3          ((uint8_t)0x02)  /* TIM3 Alternate Function mapping */
#define GPIO_AF_TIM4          ((uint8_t)0x02)  /* TIM4 Alternate Function mapping */
#define GPIO_AF_TIM5          ((uint8_t)0x02)  /* TIM5 Alternate Function mapping */
#define GPIO_AF_TIM8          ((uint8_t)0x03)  /* TIM8 Alternate Function mapping */
#define GPIO_AF_TIM9          ((uint8_t)0x03)  /* TIM9 Alternate Function mapping */
#define GPIO_AF_TIM10         ((uint8_t)0x03)  /* TIM10 Alternate Function mapping */
#define GPIO_AF_TIM11         ((uint8_t)0x03)  /* TIM11 Alternate Function mapping */
#define GPIO_AF_I2C1          ((uint8_t)0x04)  /* I2C1 Alternate Function mapping */
#define GPIO_AF_I2C2          ((uint8_t)0x04)  /* I2C2 Alternate Function mapping */
#define GPIO_AF_I2C3          ((uint8_t)0x04)  /* I2C3 Alternate Function mapping */
#define GPIO_AF_SPI1          ((uint8_t)0x05)  /* SPI1/I2S1 Alternate Function mapping */
#define GPIO_AF_SPI2          ((uint8_t)0x05)  /* SPI2/I2S2 Alternate Function mapping */
#define GPIO_AF5_SPI3         ((uint8_t)0x05)  /* SPI3/I2S3 Alternate Function mapping (Only for STM32F411xE Devices) */
#define GPIO_AF_SPI4          ((uint8_t)0x05)  /* SPI4/I2S4 Alternate Function mapping */
#define GPIO_AF_SPI5          ((uint8_t)0x05)  /* SPI5 Alternate Function mapping      */
#define GPIO_AF_SPI6          ((uint8_t)0x05)  /* SPI6 Alternate Function mapping      */
#define GPIO_AF_SPI3          ((uint8_t)0x06)  /* SPI3/I2S3 Alternate Function mapping */
#define GPIO_AF6_SPI2         ((uint8_t)0x06)  /* SPI2 Alternate Function mapping (Only for STM32F411xE Devices) */
#define GPIO_AF6_SPI4         ((uint8_t)0x06)  /* SPI4 Alternate Function mapping (Only for STM32F411xE Devices) */
#define GPIO_AF6_SPI5         ((uint8_t)0x06)  /* SPI5 Alternate Function mapping (Only for STM32F411xE Devices) */
#define GPIO_AF_SAI1          ((uint8_t)0x06)  /* SAI1 Alternate Function mapping      */
#define GPIO_AF_USART1         ((uint8_t)0x07)  /* USART1 Alternate Function mapping  */
#define GPIO_AF_USART2         ((uint8_t)0x07)  /* USART2 Alternate Function mapping  */
#define GPIO_AF_USART3         ((uint8_t)0x07)  /* USART3 Alternate Function mapping  */
#define GPIO_AF7_SPI3          ((uint8_t)0x07)  /* SPI3/I2S3ext Alternate Function mapping */
#define GPIO_AF_I2S3ext   GPIO_AF7_SPI3
#define GPIO_AF_UART4         ((uint8_t)0x08)  /* UART4 Alternate Function mapping  */
#define GPIO_AF_UART5         ((uint8_t)0x08)  /* UART5 Alternate Function mapping  */
#define GPIO_AF_USART6        ((uint8_t)0x08)  /* USART6 Alternate Function mapping */
#define GPIO_AF_UART7         ((uint8_t)0x08)  /* UART7 Alternate Function mapping  */
#define GPIO_AF_UART8         ((uint8_t)0x08)  /* UART8 Alternate Function mapping  */
#define GPIO_AF_CAN1          ((uint8_t)0x09)  /* CAN1 Alternate Function mapping  */
#define GPIO_AF_CAN2          ((uint8_t)0x09)  /* CAN2 Alternate Function mapping  */
#define GPIO_AF_TIM12         ((uint8_t)0x09)  /* TIM12 Alternate Function mapping */
#define GPIO_AF_TIM13         ((uint8_t)0x09)  /* TIM13 Alternate Function mapping */
#define GPIO_AF_TIM14         ((uint8_t)0x09)  /* TIM14 Alternate Function mapping */

#define GPIO_AF9_I2C2         ((uint8_t)0x09)  /* I2C2 Alternate Function mapping (Only for STM32F401xx/STM32F411xE Devices) */
#define GPIO_AF9_I2C3         ((uint8_t)0x09)  /* I2C3 Alternate Function mapping (Only for STM32F401xx/STM32F411xE Devices) */
#define GPIO_AF_OTG_FS         ((uint8_t)0xA)  /* OTG_FS Alternate Function mapping */
#define GPIO_AF_OTG_HS         ((uint8_t)0xA)  /* OTG_HS Alternate Function mapping */
#define GPIO_AF_ETH             ((uint8_t)0x0B)  /* ETHERNET Alternate Function mapping */
#define GPIO_AF_FSMC             ((uint8_t)0xC)  /* FSMC Alternate Function mapping                     */
#define GPIO_AF_FMC              ((uint8_t)0xC)  /* FMC Alternate Function mapping                      */
#define GPIO_AF_OTG_HS_FS        ((uint8_t)0xC)  /* OTG HS configured in FS, Alternate Function mapping */
#define GPIO_AF_SDIO             ((uint8_t)0xC)  /* SDIO Alternate Function mapping                     */
#define GPIO_AF_DCMI          ((uint8_t)0x0D)  /* DCMI Alternate Function mapping */
#define GPIO_AF_LTDC          ((uint8_t)0x0E)  /* LCD-TFT Alternate Function mapping */
#define GPIO_AF_EVENTOUT      ((uint8_t)0x0F)  /* EVENTOUT Alternate Function mapping */
////////////////////////////////////////////////////////////////////////////////// 
u8 Sys_Clock_Set(u32 plln,u32 pllm,u32 pllp,u32 pllq);		//系统时钟设置
void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq); //时钟初始化  
void Sys_Soft_Reset(void);      							//系统软复位
void Sys_Standby(void);         							//待机模式 	
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset);	//设置偏移地址
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group);			//设置NVIC分组
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group);//设置中断
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM);				//外部中断配置函数(只对GPIOA~I)
void GPIO_AF_Set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx);		//GPIO复用功能设置
void GPIO_Set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD);//GPIO设置函数  
void wtd_ini(u8 div); //按默认0，200~600ms
void wtd_tri(void);
void systime_initial(u32 frq);

//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
#define OS_OPEN_INT		__ASM volatile("cpsie i")//开启所有中断
#define OS_CLOSE_INT	__ASM volatile("cpsid i")//关闭所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址 
void switch_2_addr(u32 *pc); //切换到指定的程序指针
//////////////////////////////////////////////////////////////////////////////

//系统变量
extern u32 SYS_task;
extern u16 SYS_idle;
extern u16 SYS_idle_dis;
extern u32 SYS_time;

#define START_TASK(x)       SYS_task |= x

#define TASK_POOL_100			  (1) //周期任务 100 hz
#define TASK_POOL_1000			(2)	//1000 hz
#define TASK_POOL_1         (4) //1 hz
//#define TASK_                 //必须是2的幂次
void wait(vu32 i);
int void_putchar(int c);
extern int (*IO_putchar)(int);	//初始化的函数

#endif

