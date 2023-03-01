#ifndef __SYS_H
#define __SYS_H	 
#include "..\..\CMSIS\stm32f10x.h"
#include "..\..\CMSIS\core_cm3.h"
#include "..\..\CMSIS\system_stm32f10x.h"
#include "..\..\Lib\inc\misc.h"
#include "..\..\Lib\inc\stm32f10x_adc.h"
#include "..\..\Lib\inc\stm32f10x_bkp.h"
#include "..\..\Lib\inc\stm32f10x_can.h"
#include "..\..\Lib\inc\stm32f10x_cec.h"
#include "..\..\Lib\inc\stm32f10x_crc.h"
#include "..\..\Lib\inc\stm32f10x_dac.h"
#include "..\..\Lib\inc\stm32f10x_dbgmcu.h"
#include "..\..\Lib\inc\stm32f10x_dma.h"
#include "..\..\Lib\inc\stm32f10x_flash.h"
#include "..\..\Lib\inc\stm32f10x_exti.h"
#include "..\..\Lib\inc\stm32f10x_fsmc.h"
#include "..\..\Lib\inc\stm32f10x_gpio.h"
#include "..\..\Lib\inc\stm32f10x_i2c.h"
#include "..\..\Lib\inc\stm32f10x_iwdg.h"
#include "..\..\Lib\inc\stm32f10x_pwr.h"
#include "..\..\Lib\inc\stm32f10x_rcc.h"
#include "..\..\Lib\inc\stm32f10x_rtc.h"
#include "..\..\Lib\inc\stm32f10x_sdio.h"
#include "..\..\Lib\inc\stm32f10x_spi.h"
#include "..\..\Lib\inc\stm32f10x_tim.h"
#include "..\..\Lib\inc\stm32f10x_usart.h"
#include "..\..\Lib\inc\stm32f10x_wwdg.h"

//定义布尔类型
typedef enum _boolean_enum {
	FALSE = 0,
	TRUE
}boolean; 

//操作状态
typedef enum _operate_sta_enum {
	FAIL = 0,
	SUCCEED
}operateSta; 

#define FREE_RTOS_SUPPORT		0		//为1支持OS，为0不支持


//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址

void NVIC_Configuration(void);
void RCC_Configuration(void); //RCC时钟类的设置
#endif
