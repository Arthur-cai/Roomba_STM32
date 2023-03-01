#ifndef __VL53L0X_GEN_H
#define __VL53L0X_GEN_H

#include "vl53l0x.h"

extern VL53L0X_RangingMeasurementData_t left_vl53l0x_data;  //左测量数据结构体
extern VL53L0X_RangingMeasurementData_t right_vl53l0x_data; //右测量数据结构体

/**
 * @brief	vl53l0x测量模式配置
 * @param	dev:设备I2C参数结构体
 * @param	mode: 0:默认;1:高精度;2:长距离;3:高速
 * @retval	Status错误状态
*/
VL53L0X_Error vl53l0x_set_mode(VL53L0X_Dev_t* ldev, VL53L0X_Dev_t* rdev, u8 mode);

/**
 * @brief	vl53l0x单次距离测量函数
 * @param	ldev,rdev	设备I2C参数结构体
 * @param	ldata,rdata	保存测量数据结构体
 * @param	lbuf，rbuf	保存测量状态字符串
 * @retval	Status错误状态
*/
VL53L0X_Error vl53l0x_start_single_test(VL53L0X_Dev_t* ldev, VL53L0X_RangingMeasurementData_t* ldata,
										VL53L0X_Dev_t* rdev, VL53L0X_RangingMeasurementData_t* rdata);

/**
 * @brief	启动普通测量
 * @param	ldev,rdev	设备I2C参数结构体
 * @retval	Status错误状态
*/
VL53L0X_Error VL53L0X_General(VL53L0X_Dev_t* ldev, VL53L0X_Dev_t* rdev);

///**
// * @brief	vl53l0x普通测量模式UI
// */
//void general_ui(void);

///**
// * @brief	启动普通测量
// * @param	ldev,rdev	设备I2C参数结构体
// * @param	mode: 0:默认;1:高精度;2:长距离;3:高速
//*/
//void vl53l0x_general_start(VL53L0X_Dev_t* ldev, VL53L0X_Dev_t* rdev, u8 mode);
//
///**
// * @brief	vl53l0x普通测量模式测试
// * @param	ldev,rdev	设备I2C参数结构体
//*/
//void vl53l0x_general_test(VL53L0X_Dev_t* ldev, VL53L0X_Dev_t* rdev);

#endif
