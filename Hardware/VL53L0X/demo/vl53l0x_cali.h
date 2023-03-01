#ifndef __VL53L0X_CAIL_H
#define __VL53L0X_CAIL_H

#include "vl53l0x.h"

//////////////////////////////////////////////////////////////////////////////////	 
//ALIENTEK 精英STM32开发板
//VL53L0X-校准模式 驱动代码	   							  
////////////////////////////////////////////////////////////////////////////////// 

//vl53l0x传感器校准信息结构体定义
typedef __packed struct
{
	uint8_t  adjustok;                    //校准成功标志，0XAA，已校准;其他，未校准
	uint8_t  isApertureSpads;             //ApertureSpads值
	uint8_t  VhvSettings;                 //VhvSettings值
	uint8_t  PhaseCal;                    //PhaseCal值
	uint32_t XTalkCalDistance;            //XTalkCalDistance值
	uint32_t XTalkCompensationRateMegaCps;//XTalkCompensationRateMegaCps值
	uint32_t CalDistanceMilliMeter;       //CalDistanceMilliMeter值
	int32_t  OffsetMicroMeter;            //OffsetMicroMeter值
	uint32_t refSpadCount;                //refSpadCount值
	
}_vl53l0x_adjust;

extern _vl53l0x_adjust Vl53l0x_data;

void vl53l0x_calibration_test(VL53L0X_Dev_t *dev);

#endif
