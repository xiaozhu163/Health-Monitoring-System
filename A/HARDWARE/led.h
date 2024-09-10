#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED1 PAout(8)		   
#define LED2 PAout(11)	
#define LED PCout(13)	// LED接口	
#define LED_OFF		0
#define LED_ON		1
void LED_Init(void);	//初始化
		 				    
#endif

