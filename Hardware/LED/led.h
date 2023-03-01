#ifndef __LED_H
#define __LED_H	 
#include "..\..\Basic\sys\sys.h"


#define LED0            PBout(5)// PB0
#define LED1            PEout(5)// PB1	
#define LED0PORT        GPIOB	//定义IO端口
#define LED1PORT        GPIOE   //定义IO端口
#define LED_PIN         GPIO_Pin_5  //定义引脚
#define LED_RCC_CLOCK   (RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE)   //定义时钟

void LED_Init(void);//初始化

#endif
