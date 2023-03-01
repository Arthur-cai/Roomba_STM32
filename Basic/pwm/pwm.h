#ifndef __PWM_H
#define __PWM_H
#include "..\..\Basic\sys\sys.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	设置频率为50Hz，公式为：溢出时间Tout（单位秒） = (arr + 1)(psc + 1) / Tclk	 20ms = (199 + 1) * (7199 + 1) / 72000000
//	Tclk为通用定时器的时钟，如果APB1没有分频，则就为系统时钟，72MHZ
//	PWM时钟频率 = 72000000 / [(199+1)*(7199+1)] = 50Hz (20ms), 设置自动装载值199，预分频系数7199
// 
//	psc表示计数一次需要多少时间，arr表示总共计数多少次
//	psc = 72M /（7200-1）= 0.1ms		计数一次0.1ms
//  arr = （200-1）					总共计200次，总时间200*0.1 = 20ms，即50Hz
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define WHEEL_PWM_PORT			GPIOB
#define LEFT_WHEEL_PWM_PIN		GPIO_Pin_1	//PB1对应TIM3_Channel4	PB5对应重映射channel2
#define RIGHT_WHEEL_PWM_PIN		GPIO_Pin_0  //PB0对应TIM3_Channel3

/**
 * @brief	定时器3 PWM初始化
 * @note	psc = 72M / 7200 = 0.1ms，arr = (200-1)，psc = (7200-1)
 * @param	arr		重装载值, 表示总共计数多少次
 * @param	psc		预分频系数, 表示计数一次需要多少时间
*/
void TIM3_PWM_Init(u16 arr, u16 psc);

/**
 * @brief	开启或关闭TIM3，用于控制开启或关闭PWM生成
*/
void TIM3_PWM_Operation(FunctionalState EnOrDis);

#endif // !__PWM_H
