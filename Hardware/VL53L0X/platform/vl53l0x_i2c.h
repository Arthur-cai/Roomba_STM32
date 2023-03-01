#ifndef __VL53L0X_I2C_H
#define __VL53L0X_I2C_H
#include "../../../Basic/sys/sys.h"
#include "../../Basic/delay/delay.h"
///////////////////////////////////////////////////////
//	模拟IIC总线传输，需自行设定SDA和SCL线，及IIC通讯标志
//	右边的VL53模块	SDA -> PF12		SCL -> PF11
//  左边的VL53模块	SDA -> PD5		SCL -> PD4
///////////////////////////////////////////////////////

#define CHOOSE_LEFT_FLAG	(u8)1	//选择左边的VL模块
#define CHOOSE_RIGHT_FLAG	(u8)2	//选择右边的VL模块

//设置右边的VL53模块IO方向
#define	RVL53L0X_SDA_IO_IN() {GPIOF->CRH &= 0xFFF0FFFF; GPIOF->CRH |= (u32)8<<4*4;}
#define	RVL53L0X_SDA_IO_OUT() {GPIOF->CRH &= 0xFFF0FFFF; GPIOF->CRH |= (u32)3<<4*4;}
//设置左边边的VL53模块IO方向
#define	LVL53L0X_SDA_IO_IN() {GPIOD->CRL &= 0xFF0FFFFF; GPIOD->CRL |= (u32)8<<5*4;}
#define	LVL53L0X_SDA_IO_OUT() {GPIOD->CRL &= 0xFF0FFFFF; GPIOD->CRL |= (u32)3<<5*4;}

//设置右边的VL53模块SDA和SCL线
#define RVL53L0X_SDA_OUT	PFout(12)
#define RVL53L0X_SCL_OUT	PFout(11)
#define RVL53L0X_SDA_IN		PFin(12)
//设置左边的VL53模块SDA和SCL线
#define LVL53L0X_SDA_OUT	PDout(5)
#define LVL53L0X_SCL_OUT	PDout(4)
#define LVL53L0X_SDA_IN		PDin(5)

/**
 * @brief	初始化VL53L0X的IIC总线
 * @note	右边的VL53模块	SDA -> PF12		SCL -> PF11
 * @note	左边的VL53模块	SDA -> PD5		SCL -> PD4
*/
void VL53L0X_IIC_Init(void);

/**
 * @brief	延时2us
*/
void VL53L0X_IIC_Delay(void);

/**
 * @brief	发送开始信号
 * @param	flag，左右模块IIC总线切换标志
*/
void VL53L0X_IIC_SendStart(u8 flag);

/**
 * @brief	发送停止信号
 * @param	flag，左右模块IIC总线切换标志
*/
void VL53L0X_IIC_SendStop(u8 flag);

/**
 * @brief	发送ACK信号
 * @param	flag，左右模块IIC总线切换标志
 */
void VL53L0X_IIC_SendAck(u8 flag);

/**
 * @brief	发送NACK信号
 * @param	flag，左右模块IIC总线切换标志
*/
void VL53L0X_IIC_SendNAck(u8 flag);

/**
 * @brief	等待ACK信号
 * @param	flag，左右模块IIC总线切换标志
 * @retval	接收到ACK返回0，否则返回1
*/
u8 VL53L0X_IIC_WaitAck(u8 flag);

/**
 * @brief	发送1个字节
 * @param	data，待发送数据
 * @param	flag，左右模块IIC总线切换标志
*/
void VL53L0X_IIC_SendByte(u8 data, u8 flag);

/**
 * @brief	读取1个字节
 * @param	ack，为1发送ACK，为0发送NACK
 * @param	flag，左右模块IIC总线切换标志
*/
u8 VL53L0X_IIC_ReadByte(u8 ack, u8 flag);

/**
 * @brief	写1个字节数据到VL53L0X
 * @param	slaveAddr	设备地址
 * @param	regAddr		寄存器地址
 * @param	data		待写入的字节
 * @param	flag		选择左边或右边的VL53模块
				 @arg 	CHOOSE_LEFT_FLAG 或 CHOOSE_RIGHT_FLAG
 * @retval	成功返回0，失败返回1
*/
u8 VL53L0X_WriteOneByte(u8 slaveAddr, u8 regAddr, u8 data, u8 flag);

/**
 * @brief	从VL53L0X读1个字节数据
 * @param	slaveAddr	设备地址
 * @param	regAddr		寄存器地址
 * @param	data		待读取的字节指针
 * @param	flag		选择左边或右边的VL53模块 
				 @arg 	CHOOSE_LEFT_FLAG 或 CHOOSE_RIGHT_FLAG
 * @retval	成功返回0，失败返回1
*/
u8 VL53L0X_ReadOneByte(u8 slaveAddr, u8 regAddr, u8* data, u8 flag);

/**
 * @brief	写多个字节数据到VL53L0X
 * @param	slaveAddr	设备地址
 * @param	regAddr		寄存器地址
 * @param	len			字节长度
 * @param	pbuf		待写入的数据缓冲区
 * @param	flag		选择左边或右边的VL53模块
				 @arg 	CHOOSE_LEFT_FLAG 或 CHOOSE_RIGHT_FLAG
 * @retval	成功返回0，失败返回1
*/
u8 VL53L0X_WriteNBytes(u8 slaveAddr, u8 regAddr, u16 len, u8* pbuf, u8 flag);

/**
 * @brief	从VL53L0X读多个字节数据
 * @param	slaveAddr	设备地址
 * @param	regAddr		寄存器地址
 * @param	len			字节长度
 * @param	pbuf		待读入的数据缓冲区
 * @param	flag		选择左边或右边的VL53模块
				 @arg 	CHOOSE_LEFT_FLAG 或 CHOOSE_RIGHT_FLAG
 * @retval	成功返回0，失败返回1
*/
u8 VL53L0X_ReadNBytes(u8 slaveAddr, u8 regAddr, u16 len, u8* pbuf, u8 flag);

/**
 * @brief	VL53L0X 读多个数据
 * @param	address		地址
 * @param	index		偏移地址
 * @param	pdata		数据指针
 * @param	count		长度 最大65535
 * @param	flag		选择左边或右边的VL53模块
				 @arg 	CHOOSE_LEFT_FLAG 或 CHOOSE_RIGHT_FLAG
 * @retval	成功返回0，失败返回1
*/
u8 VL53L0X_write_multi(u8 address, u8 index, u8* pdata, u16 count, u8 flag);

/**
 * @brief	VL53L0X 读多个数据
 * @param	address		地址
 * @param	index		偏移地址
 * @param	pdata		数据指针
 * @param	count		长度 最大65535
 * @param	flag		选择左边或右边的VL53模块
				 @arg 	CHOOSE_LEFT_FLAG 或 CHOOSE_RIGHT_FLAG
 * @retval	成功返回0，失败返回1
*/
u8 VL53L0X_read_multi(u8 address, u8 index, u8* pdata, u16 count, u8 flag);

/**
 * @brief	VL53L0X 写1个字节，8位数据
 * @param	address		地址
 * @param	index		偏移地址
 * @param	data		8位数据
 * @param	flag		选择左边或右边的VL53模块
				 @arg 	CHOOSE_LEFT_FLAG 或 CHOOSE_RIGHT_FLAG
 * @retval	成功返回0，失败返回1
*/
u8 VL53L0X_write_byte(u8 address, u8 index, u8 data, u8 flag);

/**
 * @brief	VL53L0X 写1个字，16位数据
 * @param	address		地址
 * @param	index		偏移地址
 * @param	data		16位数据
 * @param	flag		选择左边或右边的VL53模块
				 @arg 	CHOOSE_LEFT_FLAG 或 CHOOSE_RIGHT_FLAG
 * @retval	成功返回0，失败返回1
*/
u8 VL53L0X_write_word(u8 address, u8 index, u16 data, u8 flag);

/**
 * @brief	VL53L0X 写2个字，32位数据
 * @param	address		地址
 * @param	index		偏移地址
 * @param	data		32位数据
 * @param	flag		选择左边或右边的VL53模块
				 @arg 	CHOOSE_LEFT_FLAG 或 CHOOSE_RIGHT_FLAG
 * @retval	成功返回0，失败返回1
*/
u8 VL53L0X_write_dword(u8 address, u8 index, u32 data, u8 flag);

/**
 * @brief	VL53L0X 读1个字节，8位数据
 * @param	address		地址
 * @param	index		偏移地址
 * @param	pdata		8位数据缓冲区
 * @param	flag		选择左边或右边的VL53模块
				 @arg 	CHOOSE_LEFT_FLAG 或 CHOOSE_RIGHT_FLAG
 * @retval	成功返回0，失败返回1
*/
u8 VL53L0X_read_byte(u8 address, u8 index, u8* pdata, u8 flag);

/**
 * @brief	VL53L0X 读1个字，16位数据
 * @param	address		地址
 * @param	index		偏移地址
 * @param	pdata		16位数据缓冲区
 * @param	flag		选择左边或右边的VL53模块
				 @arg 	CHOOSE_LEFT_FLAG 或 CHOOSE_RIGHT_FLAG
 * @retval	成功返回0，失败返回1
*/
u8 VL53L0X_read_word(u8 address, u8 index, u16* data, u8 flag);

/**
 * @brief	VL53L0X 读2个字，32位数据
 * @param	address		地址
 * @param	index		偏移地址
 * @param	pdata		32位数据缓冲区
 * @param	flag		选择左边或右边的VL53模块
				 @arg 	CHOOSE_LEFT_FLAG 或 CHOOSE_RIGHT_FLAG
 * @retval	成功返回0，失败返回1
*/
u8 VL53L0X_read_dword(u8 address, u8 index, u32* data, u8 flag);


#endif // !__VL53L0X_I2C_H
