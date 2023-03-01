#include "key.h"
#include "..\..\Basic\delay\delay.h"

/**
 *@brief 按键初始化
*/
void KEY_Init(void){
	GPIO_InitTypeDef  GPIO_InitStructure; //定义GPIO的初始化枚举结构	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);  //使能GPIO口时钟
	//初始化key0 key1
	GPIO_InitStructure.GPIO_Pin = KEY0_PIN|KEY1_PIN; //选择端口号（0~15或all）                        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //选择IO接口工作方式上拉输入    
	GPIO_Init(KEYPORT,&GPIO_InitStructure);			
	
	//初始化key up
	GPIO_InitStructure.GPIO_Pin = KEY_UP_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	//IO口输入下拉
	GPIO_Init(KEY_UP_PORT,&GPIO_InitStructure);			
}

/**
 * @brief 获取按键按下情况
 * @param ContinueMode 连续模式，NOT_CONTINUE表示不记录连按，CONTINUE表示记录连按
					查看头文件的枚举定义
 * @retval 按键按下的代表值
*/
KeyFlag KEY_Status(u8 ContinueMode) {
	//连按标记定义为常变量，可以在修改后保持值而不会因为再次进入函数体而重新初始化
	static u8 continueFlag = CONTINUE;
	//如果是连按模式，每一次就都把标志位初始化为1，这样每次都能进入到下面的return方法区
	if(ContinueMode) continueFlag = CONTINUE;
	
	if(continueFlag && (KEY0||KEY1||KEY_UP)) {
		continueFlag = NOT_CONTINUE; //进入该分支后就把标志位置0，防止非连按时连续进入
		
		if(KEY0) {delay_ms(20); if(KEY0) return KEY0_PRESS;}
		else if(KEY1) {delay_ms(20); if(KEY1) return KEY1_PRESS;}
		else if(KEY_UP) {delay_ms(20); if(KEY_UP) return KEY_UP_PRESS;}
		
	} else if(!KEY0 && !KEY1 && !KEY_UP){
		//没有按键按下，或者当松开按键后，重新初始化标记为1
		continueFlag = 1;
	}
	
	return NONE;
}

/**
 * @brief 待机模式
*/
void Sys_Standby(void) { 
	//开启PWR时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_WakeUpPinCmd(ENABLE); //唤醒管脚
	//进入待机模式StandBy
	PWR_EnterSTANDBYMode();
}

/**
 * @brief 进入待机模式
*/
void Sys_Enter_Standby(void) {
	RCC_APB2PeriphResetCmd(0x01FC, DISABLE); //复位所有端口
	Sys_Standby();
}

/**
 * @brief 检测KEY_UP是否长按3s
 * @retval TRUE或FALSE
*/
boolean Check_KEY_UP(void) {	
	u8 time = 0;
	while (KEY_UP) {
		delay_ms(30);
		time++;
		if (time >= 100) return TRUE; //长按了3s
	}
	return FALSE;
}

/**
 * @brief KEY_UP的WK_UP功能初始化
*/
void WK_UP_Init(void) { 
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	//开启GPIOA时钟和复位AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin = KEY_UP_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannel = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//如果没有按下KEY_UP，就进入待机模式
	if (Check_KEY_UP() == FALSE) {
		Sys_Standby();
	}
}

/**
 * @brief KEY_UP的外部中断程序
*/
void EXTI0_IRQHandler(void) {
	EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志
	//如果长按KEYUP，就进入待机模式
	if (Check_KEY_UP() == TRUE) {
		Sys_Enter_Standby();
	}
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
