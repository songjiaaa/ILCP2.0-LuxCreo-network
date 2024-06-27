#include "usb_bsp.h"
#include "dwt_delay.h"


ErrorStatus HSEStartUpStatus;


void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE * pdev)
{

  GPIO_InitTypeDef GPIO_InitStructure;


  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  /* Configure SOF ID DM DP Pins */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_12;

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_OTG1_FS);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_OTG1_FS);


  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE);


//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;

//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);

//  GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_OTG2_FS);
//  GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_OTG2_FS);
//  GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_OTG2_FS);

//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);

//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_OTG_HS, ENABLE);

}

/**
  * @brief  USB_OTG_BSP_EnableInterrupt
  *         Configures USB Global interrupt
  * @param  None
  * @retval None
  */
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE * pdev)
{
  NVIC_InitTypeDef NVIC_InitStructure;

//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
#ifdef USE_USB_OTG_HS
  NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_IRQn;
#else
  NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
#endif
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_EP1_OUT_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_EP1_IN_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
}

/**
  * @brief  BSP_Drive_VBUS
  *         Drives the Vbus signal through IO
  * @param  state : VBUS states
  * @retval None
  */

void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE * pdev, uint8_t state)
{
  /* 
   * On-chip 5 V VBUS generation is not supported. For this reason, a charge
   * pump or, if 5 V are available on the application board, a basic power
   * switch, must be added externally to drive the 5 V VBUS line. The external 
   * charge pump can be driven by any GPIO output. When the application
   * decides to power on VBUS using the chosen GPIO, it must also set the port 
   * power bit in the host port control and status register (PPWR bit in
   * OTG_FS_HPRT).
   * 
   * Bit 12 PPWR: Port power The application uses this field to control power
   * to this port, and the core clears this bit on an overcurrent condition. */
#if defined(USE_STM324x9I_EVAL)

#if defined( USE_USB_OTG_FS)

  if (state == 0)
  {
    /* Configure Low Charge pump */
    IOE16_SetIOPin(HOST_POWERSW_FS1_VBUS, IOE16_BitReset);
  }
  else
  {
    /* Configure Low Charge pump */
    IOE16_SetIOPin(HOST_POWERSW_FS1_VBUS, IOE16_BitSet);
  }

#endif                          /* defined( USE_USB_OTG_FS) */

#ifdef USE_USB_OTG_HS
  if (state == 0)
  {
    /* Configure Low Charge pump */
    IOE16_SetIOPin(HOST_POWERSW_FS2_VBUS, IOE16_BitReset);
  }
  else
  {
    /* Drive High Charge pump */
    IOE16_SetIOPin(HOST_POWERSW_FS2_VBUS, IOE16_BitSet);
  }
#endif                          /* USE_USB_OTG_HS */

#else
#if defined( USE_USB_OTG_FS)

  if (0 == state)
  {
    /* DISABLE is needed on output of the Power Switch */
//    GPIO_SetBits(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
  }
  else
  {
    /* ENABLE the Power Switch by driving the Enable LOW */
//    GPIO_ResetBits(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
  }
#endif                          /* defined( USE_USB_OTG_FS) */
#endif                          /* defined(USE_STM324x9I_EVAL) */
}

/**
  * @brief  USB_OTG_BSP_ConfigVBUS
  *         Configures the IO for the Vbus and OverCurrent
  * @param  None
  * @retval None
  */

void USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE * pdev)
{

}



/**
  * @brief  USB_OTG_BSP_uDelay
  *         This function provides delay time in micro sec
  * @param  usec : Value of delay required in micro sec
  * @retval None
  */
void USB_OTG_BSP_uDelay(const uint32_t usec)
{
	bsp_DelayUS(usec);
}


/**
  * @brief  USB_OTG_BSP_mDelay
  *          This function provides delay time in milli sec
  * @param  msec : Value of delay required in milli sec
  * @retval None
  */
void USB_OTG_BSP_mDelay(const uint32_t msec)
{
	bsp_DelayMS(msec);
}




/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
