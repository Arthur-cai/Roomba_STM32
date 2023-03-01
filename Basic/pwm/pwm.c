#include "pwm.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	设置频率为50Hz，公式为：溢出时间Tout（单位秒） = (arr + 1)(psc + 1) / Tclk	 20ms = (199 + 1) * (7199 + 1) / 72000000
//	Tclk为通用定时器的时钟，如果APB1没有分频，则就为系统时钟，72MHZ
//	PWM时钟频率 = 72000000 / [(199+1)*(7199+1)] = 50Hz (20ms), 设置自动装载值199，预分频系数7199
// 
//	psc表示计数一次需要多少时间，arr表示总共计数多少次
//	psc = 72M /（7200-1）= 0.1ms		计数一次0.1ms
//  arr = （200-1）					总共计200次，总时间200*0.1 = 20ms，即50Hz
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief	定时器3初始化
*/
void TIM3_PWM_Init(u16 arr, u16 psc) {
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	GPIO_InitStructure.GPIO_Pin = RIGHT_WHEEL_PWM_PIN | LEFT_WHEEL_PWM_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(WHEEL_PWM_PORT,&GPIO_InitStructure);

	TIM_DeInit(TIM3); //复位定时器3

	//定时器3配置
	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0/*TIM_CKD_DIV1*/;	//分频因子
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

	//PWM配置
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	//设置有效电平位高电平
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //输出使能
	//定时器3通道3，4 PWM初始化
	TIM_OC3Init(TIM3, &TIM_OCInitStructure); 
	TIM_OC4Init(TIM3, &TIM_OCInitStructure); 
	//定时器3通道3，4 预装载
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
}

/**
 * @brief	开启或关闭TIM3，用于控制开启或关闭PWM生成
*/
void TIM3_PWM_Operation(FunctionalState EnOrDis) {
	TIM_Cmd(TIM3, EnOrDis);
}
