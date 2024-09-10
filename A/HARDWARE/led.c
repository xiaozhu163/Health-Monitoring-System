#include "led.h"


void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能GPIOA端口时钟
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //LED>PA.8 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);	
 


 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				 //LED>PA.9 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);	
 
		

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	    		 //端口配置, 推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
	
	LED = LED_OFF;
 GPIO_ResetBits(GPIOA,GPIO_Pin_8);//输出0，
 GPIO_ResetBits(GPIOA,GPIO_Pin_11);//输出0，
}
