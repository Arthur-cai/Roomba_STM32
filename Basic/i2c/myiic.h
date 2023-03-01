#ifndef __MYIIC_H
#define __MYIIC_H
#include "..\..\Basic\sys\sys.h"

///////////////////////////////////////////////////////
//	模拟IIC总线传输，需自行设定SDA和SCL线，及IIC通讯标志
//	SDA -> PB7		SCL -> PB6
///////////////////////////////////////////////////////

//设置IO方向
#define	SDA_IO_IN() {GPIOB->CRL &= 0x0FFFFFFF; GPIOB->CRL |= (u32)8<<7*4;}
#define	SDA_IO_OUT() {GPIOB->CRL &= 0x0FFFFFFF; GPIOB->CRL |= (u32)3<<7*4;}

//设置SDA和SCL线
#define SDA_OUT	PBout(7)
#define SCL_OUT	PBout(6)
#define SDA_IN	PBin(7)

/**
 * @brief	初始化IIC
*/
void IIC_Init(void);

/**
 * @brief	发送开始信号
*/
void IIC_SendStart(void);

/**
 * @brief	发送停止信号
*/
void IIC_SendStop(void);

/**
 * @brief	发送ACK信号
*/
void IIC_SendAck(void);

/**
 * @brief	发送NACK信号
*/
void IIC_SendNAck(void);

/**
 * @brief	等待ACK信号
 * @retval	接收到ACK返回SUCCEED，否则返回FAIL
*/
operateSta IIC_WaitAck(void);

/**
 * @brief	发送1个字节
*/
void IIC_SendByte(u8 data);

/**
 * @brief	读取1个字节
 * @param ack	1表示发送ACK，0表示发送NACK
 * @retval	读取到的字节
*/
u8 IIC_ReadByte(u8 ack);

#endif // !__MYIIC_H
