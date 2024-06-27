#include "dwt_delay.h"

#define  DWT_CYCCNT  *(volatile unsigned int *)0xE0001004
#define  DWT_CR      *(volatile unsigned int *)0xE0001000
#define  DEM_CR      *(volatile unsigned int *)0xE000EDFC
#define  DBGMCU_CR   *(volatile unsigned int *)0xE0042004

#define  DEM_CR_TRCENA               (1 << 24)
#define  DWT_CR_CYCCNTENA            (1 <<  0)

void bsp_InitDWT(void)
{
        DEM_CR         |= (unsigned int)DEM_CR_TRCENA;   /* Enable Cortex-M4's DWT CYCCNT reg.  */
        DWT_CYCCNT      = (unsigned int)0u;
        DWT_CR         |= (unsigned int)DWT_CR_CYCCNTENA;
}


//΢����ʱ
void bsp_DelayUS(uint32_t _ulDelayTime)
{
	uint32_t tCnt, tDelayCnt;
	uint32_t tStart;
		   
	tStart = DWT_CYCCNT;                                     /* �ս���ʱ�ļ�����ֵ */
	tCnt = 0;
	tDelayCnt = _ulDelayTime * (SystemCoreClock / 1000000);         /* ��Ҫ�Ľ����� */                      

	while(tCnt < tDelayCnt)
	{
		tCnt = DWT_CYCCNT - tStart; /* ��������У����������һ��32λ���������¼�������Ȼ������ȷ���� */       
	}
}

//������ʱ
void bsp_DelayMS(uint32_t _ulDelayTime)
{
        bsp_DelayUS(1000*_ulDelayTime);
}


