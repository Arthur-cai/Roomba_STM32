#ifndef __L298N_H
#define __L298N_H
#include "..\..\Basic\sys\sys.h"
#include "..\..\Basic\pwm\pwm.h"
#include "..\..\Basic\delay\delay.h"
#include "..\..\Basic\pid\pid.h"
#include "..\..\Hardware\MPU6050\mpu6050.h"
#include "../OLED2420/oled2420.h"

#define L298N_PORT	GPIOA
#define	L298N_OUT1	GPIO_Pin_7	//模块上IN1 -> PA7	控制左轮后退
#define	L298N_OUT2	GPIO_Pin_6	//模块上IN2 -> PA6	控制左轮前进
#define	L298N_OUT3	GPIO_Pin_5	//模块上IN3 -> PA5	控制右轮前进
#define	L298N_OUT4	GPIO_Pin_4	//模块上IN4 -> PA4	控制右轮后退

//运动类型
typedef	enum _motion_type_enum {
	FORWARD = 1,
	BACKWARD,
	TURNLEFT,
	TURNRIGHT,
	STOP
}motionType;

/**
 * @brief 初始化L298N
*/
void L298N_Init(void);

/**
 * @brief	选择运动形式
 * @param	motion	运动形式
 * @arg		可取值
			STOP,
			FORWARD,
			BACKWARD,
			TURNLEFT,
			TURNRIGHT
 * @param	lspeed，左轮速度
 * @param	rspeed，右轮速度
*/
void Motion_Select(motionType motion, u16 lspeed, u16 rspeed);


#endif // !__L298N_H
