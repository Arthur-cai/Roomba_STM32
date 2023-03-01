#ifndef __VL53L0X_PLATFORM_H
#define __VL53L0X_PLATFORM_H

#include "../core/vl53l0x_def.h"
#include "vl53l0x_i2c.h"
#include "vl53l0x_platform_log.h"

//vl53l0x设备I2C信息
typedef struct {
    VL53L0X_DevData_t Data;              /*!< embed ST Ewok Dev  data as "Data"*/
    /*!< user specific field */
    uint8_t   I2cDevAddr;                /*!< i2c device address user specific field */
    uint8_t   comms_type;                /*!< Type of comms : VL53L0X_COMMS_I2C or VL53L0X_COMMS_SPI */
    uint16_t  comms_speed_khz;           /*!< Comms speed [kHz] : typically 400kHz for I2C           */

} VL53L0X_Dev_t;

typedef VL53L0X_Dev_t* VL53L0X_DEV;

extern VL53L0X_Dev_t left_vl53l0x_dev;	//左边VL53模块设备I2C数据参数
extern VL53L0X_Dev_t right_vl53l0x_dev; //右边VL53模块设备I2C数据参数

extern u8 LeftOrRightFlag;          //左右模块IIC总线切换标志，0为左边VL53模块，1为右边VL53模块

#define LEFT_Addr  (uint8_t)0x56    //左边VL53模块地址
#define RIGHT_Addr (uint8_t)0x58    //右边VL53模块地址

#define VL53L0X_MAX_I2C_XFER_SIZE  64 //定义I2C写的最大字节数
#define PALDevDataGet(Dev, field) (Dev->Data.field)
#define PALDevDataSet(Dev, field, data) (Dev->Data.field)=(data)

/**
 * @brief	VL53L0X连续写数据
 * @param	Dev     设备I2C参数结构体
 * @param	index   偏移地址
 * @param	pdata   数据指针
 * @param	count   长度
 * @retval	Status错误状态
*/
VL53L0X_Error VL53L0X_WriteMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t* pdata, uint32_t count);

/**
 * @brief	VL53L0X连续读数据
 * @param	Dev     设备I2C参数结构体
 * @param	index   偏移地址
 * @param	pdata   数据指针
 * @param	count   长度
 * @retval	Status错误状态
*/
VL53L0X_Error VL53L0X_ReadMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t* pdata, uint32_t count);

/**
 * @brief	VL53L0X写1字节寄存器，8位数据
 * @param	Dev     设备I2C参数结构体
 * @param	index   偏移地址
 * @param	data    8位数据
 * @retval	Status错误状态
*/
VL53L0X_Error VL53L0X_WrByte(VL53L0X_DEV Dev, uint8_t index, uint8_t data);

/**
 * @brief	VL53L0X写2字节寄存器，16位数据
 * @param	Dev     设备I2C参数结构体
 * @param	index   偏移地址
 * @param	data    16位数据
 * @retval	Status错误状态
*/
VL53L0X_Error VL53L0X_WrWord(VL53L0X_DEV Dev, uint8_t index, uint16_t data);

/**
 * @brief	VL53L0X写4字节寄存器，32位数据
 * @param	Dev     设备I2C参数结构体
 * @param	index   偏移地址
 * @param	data    32位数据
 * @retval	Status错误状态
*/
VL53L0X_Error VL53L0X_WrDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t data);

/**
 * @brief	VL53L0X威胁安全更新(读/修改/写)单字节寄存器
 * @param	Dev     设备I2C参数结构体
 * @param	index   偏移地址
 * @param	AndData 8位与数据
 * @param	OrData  8位或数据
 * @retval	Status错误状态
*/
VL53L0X_Error VL53L0X_UpdateByte(VL53L0X_DEV Dev, uint8_t index, uint8_t AndData, uint8_t OrData);

/**
 * @brief	VL53L0X读1字节寄存器，8位数据
 * @param	Dev     设备I2C参数结构体
 * @param	index   偏移地址
 * @param	data    8位数据指针
 * @retval	Status错误状态
*/
VL53L0X_Error VL53L0X_RdByte(VL53L0X_DEV Dev, uint8_t index, uint8_t* data);

/**
 * @brief	VL53L0X读2字节寄存器，16位
 * @param	Dev     设备I2C参数结构体
 * @param	index   偏移地址
 * @param	data    16位数据指针
 * @retval	Status错误状态
*/
VL53L0X_Error VL53L0X_RdWord(VL53L0X_DEV Dev, uint8_t index, uint16_t* data);

/**
 * @brief	VL53L0X读4字节寄存器，32位
 * @param	Dev     设备I2C参数结构体
 * @param	index   偏移地址
 * @param	data    32位数据指针
 * @retval	Status错误状态
*/
VL53L0X_Error VL53L0X_RdDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t* data);

/**
 * @brief	VL53L0X底层延时函数
 * @param	Dev     设备I2C参数结构体
 * @retval	Status错误状态
*/
VL53L0X_Error VL53L0X_PollingDelay(VL53L0X_DEV Dev);


#endif 

