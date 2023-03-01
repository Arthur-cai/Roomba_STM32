#ifndef __SPI_H
#define __SPI_H
#include "../sys/sys.h"

/**
 * @brief SPI2读写
 * @param data 用于发送的数据
 * @retval 接收到了数据就返回，没有数据失败就返回0
*/
u8 SPI2_RWByte(u8 data);

/**
 * @brief SPI2初始化
*/
void SPI2_Init(void);

/**
 * @brief 	设置SPI2的速度
 * @param	SPI_BaudRatePrescaler, 预分频值
 * @arg		SPI_BaudRatePrescaler_2, SPI_BaudRatePrescaler_8, 
 * 			SPI_BaudRatePrescaler_16, SPI_BaudRatePrescaler_256
*/
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler);

#endif // !__SPI_H
