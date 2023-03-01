#include "led.h"

void LED_Init(void) { //LED灯的接口初始化
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(LED_RCC_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = LED_PIN;				//选择端口号（0~15或all）                        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	//选择IO接口工作方式       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//设置IO接口速度（2/10/50MHz）    
	GPIO_Init(LED0PORT, &GPIO_InitStructure);			//初始化LED0
	GPIO_SetBits(LED0PORT, LED_PIN);					//置高是熄灭，置低是点亮

	GPIO_InitStructure.GPIO_Pin = LED_PIN;
	GPIO_Init(LED1PORT, &GPIO_InitStructure);			//初始化LED1
	GPIO_SetBits(LED1PORT, LED_PIN);					//置高是熄灭，置低是点亮
}

/*
选择IO接口工作方式：
GPIO_Mode_AIN 模拟输入
GPIO_Mode_IN_FLOATING 浮空输入
GPIO_Mode_IPD 下拉输入
GPIO_Mode_IPU 上拉输入
GPIO_Mode_Out_PP 推挽输出
GPIO_Mode_Out_OD 开漏输出
GPIO_Mode_AF_PP 复用推挽输出
GPIO_Mode_AF_OD 复用开漏输出
*/
