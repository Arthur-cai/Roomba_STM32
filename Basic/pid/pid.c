#include "pid.h"

///////////////// <全局变量> ///////////////
PID_Parameter_Type PID_Parameters; //PID相关参数结构体
///////////////// <全局变量> ///////////////
/**
 * @brief	设定KP，KI，KD
*/
static void PID_KP_KI_KD_Conf(PID_Parameter_Type* parameters) {
	parameters->KP = 5;
	parameters->KI = 0;
	parameters->KD = 0;
}

/**
 * @brief	初始化PID参数
*/
static void PID_Info_Init(PID_Parameter_Type* parameters) {
	parameters->yaw_target = 0;
	parameters->error_cur = 0;
	parameters->error_pre = 0;
	parameters->error_pre_pre = 0;
	parameters->pidValue = 0;
	parameters->FIRST_RECORD = 0;
}

/**
 * @brief	初始化PID相关参数
*/
void PID_Init(PID_Parameter_Type* parameters) {
	PID_KP_KI_KD_Conf(parameters);
	PID_Info_Init(parameters);
}

/**
 * @brief	增量式PID
*/
short Get_PID_Output(PID_Parameter_Type* parameters, float yaw_cur) {
	float temp = 0;
	//第一次先记录参考航向角
	if (!(parameters->FIRST_RECORD)) {
		parameters->yaw_target = yaw_cur;
		parameters->FIRST_RECORD = 1;
	}
	//if (FIRST_RECORD) OLED_ShowString(0, 56, "      ", 8, 1);
	//计算当前误差
	parameters->error_cur = parameters->yaw_target - yaw_cur;	
	//计算PID值
	parameters->pidValue = 
		parameters->KP * (parameters->error_cur - parameters->error_pre) + 
		parameters->KI * parameters->error_cur + 
		parameters->KD * (parameters->error_cur - 2 * parameters->error_pre + parameters->error_pre_pre);
	//更新前两次误差
	parameters->error_pre_pre = parameters->error_pre;	
	//更新前一次误差
	parameters->error_pre = parameters->error_cur;		
	temp = parameters->pidValue;
	return (short)temp;
}

/**
 * @brief	重置PID参数
*/
void Reset_PID_Parameters(PID_Parameter_Type* parameters) {
	parameters->FIRST_RECORD = 0;	//首次记录参考航向角标记
	//OLED_ShowString(0, 56, "Reset", 8, 1);	
	parameters->yaw_target = 0;		//当前航向角
	parameters->error_cur = 0;		//当前误差
	parameters->error_pre = 0;		//前一次误差
	parameters->error_pre_pre = 0;  //前两次误差
	parameters->pidValue = 0;		//PID值
}
