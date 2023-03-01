#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "stdarg.h"
#include <string.h>
#include "../sys/sys.h"

#define printfByUSARTx  USART1  //定义支持printf函数的串口

#define USART1_REC_LEN	200  	//定义最大接收字节数 200
#define	USART3_REC_LEN	200		//定义最大接收字节数 400
#define USART3_SEN_LEN	200		//定义最大发送字节数 400

#define EN_USART1		1		//1使能,0禁止  串口1接收
#define EN_USART2		0		//1使能,0禁止  串口1接收
#define EN_USART3		1		//1使能,0禁止  串口1接收

//////////////////////////////////////////////////////////////
//					    -- <USART1> --						//
//////////////////////////////////////////////////////////////
#if EN_USART1
extern u8  USART1_RX_BUF[USART1_REC_LEN];	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART1_RX_STA;         			//接收状态标记

/**
 * @brief	初始化串口1
 * @param	baudRate，波特率
*/
void USART1_Init(u32 baudRate);

/**
 * @brief	USART1_RX_BUF串口3数据缓冲清空
*/
void USART1_RX_BUF_CLEAR(void);
#endif // EN_USART1

//////////////////////////////////////////////////////////////
//					    -- <USART2> --						//
//////////////////////////////////////////////////////////////
#if EN_USART2
extern u8  USART2_RX_BUF[USART2_REC_LEN];	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART2_RX_STA;         			//接收状态标记	
#endif // EN_USART2

//////////////////////////////////////////////////////////////
//					    -- <USART3> --						//
//////////////////////////////////////////////////////////////
#if EN_USART3
extern u8  USART3_RX_BUF[USART3_REC_LEN];	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u8  USART3_TX_BUF[USART3_SEN_LEN];	//发送缓冲
extern u16 USART3_RX_STA;         			//接收状态标记

/**
 * @brief	串口3初始化
*/
void USART3_Init(u32 baudRate);

/**
 * @brief	串口3 printf
*/
void USART3_printf(char* fmt, ...);

/**
 * @brief	USART3_RX_BUF串口3数据缓冲清空
*/
void USART3_RX_BUF_CLEAR(void);
#endif // EN_USART3

#endif //!__USART_H
