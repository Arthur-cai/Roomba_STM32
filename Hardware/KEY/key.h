#ifndef __KEY_H
#define __KEY_H	 
#include "..\..\Basic\sys\sys.h"

#define KEYPORT	GPIOE	//定义IO接口组
#define KEY_UP_PORT GPIOA

#define KEY0_PIN	GPIO_Pin_4  //按键引脚
#define KEY1_PIN  GPIO_Pin_3
#define KEY_UP_PIN  GPIO_Pin_0

#define KEY0	(!GPIO_ReadInputDataBit(KEYPORT, KEY0_PIN))	//读取按键值
#define KEY1  (!GPIO_ReadInputDataBit(KEYPORT, KEY1_PIN))
#define KEY_UP  GPIO_ReadInputDataBit(KEY_UP_PORT, KEY_UP_PIN)

/**
 *@brief  连按模式
*/
typedef enum _continue_mode_enum {
	NOT_CONTINUE = 0,
	CONTINUE
} ContinueMode;	//连按与否

/**
 *@brief 按键按下返回值
	KEY0_PRESS	1		//KEY0按下
	KEY1_PRESS	2		//KEY1按下
	KEY_UP_PRESS	3	//KEY_UP按下
*/
typedef enum _key_flag_enum {
	NONE = 0, 
	KEY0_PRESS, 
	KEY1_PRESS, 
	KEY_UP_PRESS
} KeyFlag;

void KEY_Init(void);//初始化
KeyFlag KEY_Status(u8 ContinueMode); //获取按键按下情况

void WK_UP_Init(void); //KEY_UP的WK_UP功能初始化
void Sys_Standby(void); //待机模式
void Sys_Enter_Standby(void); //系统进入待机模式
boolean Check_KEY_UP(void); //检查KEY_UP是否长按3s

#endif
