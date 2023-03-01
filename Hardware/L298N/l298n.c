#include "l298n.h"
#include <math.h>
////////////////// <全局变量> //////////////////
extern Three_Degrees_Type degrees;	//MPU角度值结构体
extern PID_Parameter_Type PID_Parameters; //PID参数结构体
////////////////// <全局变量> //////////////////

/**
 * @brief 初始化L298N
*/
void L298N_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//初始化PA7~PA4，对应OUT1~OUT4
	GPIO_InitStructure.GPIO_Pin = L298N_OUT1 | L298N_OUT2 | L298N_OUT3 | L298N_OUT4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(L298N_PORT, &GPIO_InitStructure);
	//初始先置低
	GPIO_ResetBits(L298N_PORT, L298N_OUT1 | L298N_OUT2 | L298N_OUT3 | L298N_OUT4);
	
	//初始化PID相关设定
	PID_Init(&PID_Parameters);
	//初始化PWM
	TIM3_PWM_Init(199, 7199); //输出频率50hz 
	TIM3_PWM_Operation(ENABLE); //使能TIM3
}

/**
 * @brief 设置右轮速度
*/
static void SetRightSpeed(u16 speed) {
	//设置定时器3通道4比较值
	TIM_SetCompare3(TIM3, speed);
}

/**
 * @brief 设置左轮速度
*/
static void SetLeftSpeed(u16 speed) {
	//设置定时器3通道3比较值
	TIM_SetCompare4(TIM3, speed);
}

/**
 * @brief	选择运动形式
*/
void Motion_Select(motionType motion, u16 lspeed, u16 rspeed) {
	short offset;
	//Get_ThreeDegrees();
	//TIM3_PWM_Operation(ENABLE); //开启TIM3
	if (motion <= 2) {
		offset = (Get_PID_Output(&PID_Parameters, (&degrees)->yaw) / 2); //计算PID值
		//OLED_ShowNum(0, 7 * 8, offset, 5, 8, 1);
		if (motion == FORWARD) {
			SetRightSpeed(rspeed + offset); //设置右轮速度，即PWM占空比
			SetLeftSpeed(lspeed - offset);  //设置左轮速度，即PWM占空比
			GPIO_ResetBits(L298N_PORT, L298N_OUT1 | L298N_OUT4);
			GPIO_SetBits(L298N_PORT, L298N_OUT2 | L298N_OUT3);
		}
		else {
			SetRightSpeed(rspeed + offset); //设置右轮速度，即PWM占空比
			SetLeftSpeed(lspeed - 10 - offset);  //设置左轮速度，即PWM占空比
			GPIO_ResetBits(L298N_PORT, L298N_OUT2 | L298N_OUT3);
			GPIO_SetBits(L298N_PORT, L298N_OUT1 | L298N_OUT4);
		}
	}
	else {
		Reset_PID_Parameters(&PID_Parameters);	//重置PID参数
		if (motion == TURNLEFT) {
			SetRightSpeed(rspeed); //设置右轮速度，即PWM占空比
			SetLeftSpeed(lspeed); //设置左轮速度，即PWM占空比
			GPIO_ResetBits(L298N_PORT, L298N_OUT2 | L298N_OUT4);
			GPIO_SetBits(L298N_PORT, L298N_OUT1 | L298N_OUT3);
		}
		else if (motion == TURNRIGHT) {
			SetRightSpeed(rspeed); //设置右轮速度，即PWM占空比
			SetLeftSpeed(lspeed); //设置左轮速度，即PWM占空比
			GPIO_ResetBits(L298N_PORT, L298N_OUT1 | L298N_OUT3);
			GPIO_SetBits(L298N_PORT, L298N_OUT2 | L298N_OUT4);
		}
		else {
			GPIO_ResetBits(L298N_PORT, L298N_OUT1 | L298N_OUT2 | L298N_OUT3 | L298N_OUT4);
			SetRightSpeed(0);
			SetLeftSpeed(0);
		}
	}
}


