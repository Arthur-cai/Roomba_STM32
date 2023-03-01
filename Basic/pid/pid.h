#ifndef __PID_H
#define	__PID_H
#include "..\..\Basic\sys\sys.h"
#include "..\..\Hardware\OLED2420\oled2420.h"

//PID相关参数结构体
typedef struct _pid_params_struct {
	float KP;
	float KI;
	float KD;
	u8 FIRST_RECORD;	//是否记录初始航向角标记
	float yaw_target;	//目标值
	float error_cur;	//当前误差
	float error_pre;	//前一次误差
	float error_pre_pre;//前两次误差
	float pidValue;		//PID值
}PID_Parameter_Type;
extern PID_Parameter_Type PID_Parameters; //PID相关参数结构体

/**
 * @brief	初始化PID相关参数
 * @param	parameters, PID参数结构体指针
*/
void PID_Init(PID_Parameter_Type* parameters);

/**
 * @brief	增量式PID
 * @param	parameters, PID参数结构体指针
 * @param	yaw_cur，当前航向角
 * @retval	返回增量值，用于PWM输出
*/
short Get_PID_Output(PID_Parameter_Type* parameters, float yaw_cur);

/**
 * @brief	重置PID参数
 * @param	parameters, PID参数结构体指针
*/
void Reset_PID_Parameters(PID_Parameter_Type* parameters);


#endif // !__PID_H
