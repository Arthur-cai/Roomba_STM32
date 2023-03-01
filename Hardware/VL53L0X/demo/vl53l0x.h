#ifndef __VL53L0X_H
#define __VL53L0X_H

#include "../core/vl53l0x_api.h"
#include "../platform/vl53l0x_platform.h"
#include "vl53l0x_gen.h"
#include "vl53l0x_cali.h"
//#include "vl53l0x_it.h"
#include "../../../Basic/sys/sys.h"
//#include "24cxx.h" 
#include "../../OLED2420/oled2420.h"
#include "../../KEY/key.h"
#include "../../../Basic/delay/delay.h"
//#include "../../LED/led.h"
#include <string.h>

//VL53L0X传感器上电默认IIC地址为0X52(不包含最低位)
#define LEFT_VL53L0X_Addr (u8)0x56
#define RIGHT_VL53L0X_Addr (u8)0x58

//控制Xshut电平,从而使能VL53L0X工作 1:使能 0:关闭
#define RIGHT_VL53L0X_Xshut PFout(14)	
#define LEFT_VL53L0X_Xshut PDout(7)

//使能2.8V IO电平模式
#define USE_I2C_2V8  1

//测量模式
#define Default_Mode   0// 默认
#define HIGH_ACCURACY  1//高精度
#define LONG_RANGE     2//长距离
#define HIGH_SPEED     3//高速

//vl53l0x模式配置参数集
typedef __packed struct {
	FixPoint1616_t signalLimit;    //Signal极限数值 
	FixPoint1616_t sigmaLimit;     //Sigmal极限数值
	uint32_t timingBudget;         //采样时间周期
	uint8_t preRangeVcselPeriod;   //VCSEL脉冲周期
	uint8_t finalRangeVcselPeriod; //VCSEL脉冲周期范围
}mode_data;

extern mode_data Mode_data[];
extern uint8_t AjustOK;	//全局校准标志位

/**
 * @brief	API错误信息打印
 * @param	Status，详情看VL53L0X_Error参数的定义
*/
void print_pal_error(VL53L0X_Error Status);

///**
// * @brief	模式字符串显示
// * @param	mode	0-默认;1-高精度;2-长距离;3-高速
// * @param	buf		字符串缓冲区
//*/
//void mode_string(u8 mode, char* buf);

/**
 * @brief	配置VL53L0X设备I2C地址
 * @param	dev		设备I2C参数结构体
 * @param	newaddr	设备新I2C地址
 * @retval	Status错误状态
*/
VL53L0X_Error vl53l0x_Addr_set(VL53L0X_Dev_t* dev, uint8_t newaddr);

/**
 * @brief	复位VL53L0X
 * @param	dev	设备I2C结构体
*/
void vl53l0x_reset(VL53L0X_Dev_t* dev);

/**
 * @brief	初始化VL53L0X，调用了dev_init()，VL53L0X_IIC_Init()，
			LEFT_VL53L0X_Init()，RIGHT_VL53L0X_Init()，Vl53l0x_data_init()
 * @param	ldev,rdev	设备I2C结构体
 * @retval	Status错误状态
*/
VL53L0X_Error VL53L0X_Init(VL53L0X_Dev_t* ldev, VL53L0X_Dev_t* rdev);

/**
 * @brief	启动普通模式，长距测量
*/
void VL53L0X_General_LongRange_Init(void);

/**
 * @brief	开启普通模式，长距测量
*/
void VL53L0X_General_LongRange_Start(void);

///**
// * @brief	主菜单界面
//*/
//void vl53l0x_mtest_ui(void);
//
///**
// * @brief	VL53L0X主测试程序
//*/
//void vl53l0x_test(void);

#endif
