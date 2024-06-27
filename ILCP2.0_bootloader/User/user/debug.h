#ifndef _DEBUG_H_
#define _DEBUG_H_

//#include "f4_uart.h"
#include "app.h"

	#define __DEBUG
	#ifdef __DEBUG
		#define DEBUG(info)    printf(info)
	#else
		#define DEBUG(info)
	#endif


#endif





