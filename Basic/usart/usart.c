#include "usart.h"	  

#if EN_USART3	//如果使用串口3
#include "../../Hardware/ESP8266/esp8266_timer.h"
#endif // EN_USART3

#if FREE_RTOS_SUPPORT //如果使用FreeRTOS
#include "../../FreeRTOS/include/FreeRTOS.h"
#include "../../FreeRTOS/include/task.h"
#include "../../FreeRTOS/include/queue.h"
#include "../../FreeRTOS/include/semphr.h"
#endif // FREE_RTOS_SUPPORT

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数
struct __FILE {
	int handle;
};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) {
	x = x;
}

//重定义fputc函数 
int fputc(int ch, FILE* f) {
	while ((printfByUSARTx->SR & 0X40) == 0);//循环发送,直到发送完毕   
	printfByUSARTx->DR = (u8)ch;
	return ch;
}
#endif

//////////////////////////////////////////////////////////////
//					    -- <USART1> --						//
//////////////////////////////////////////////////////////////
#if EN_USART1   //如果使能了接收
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART1_RX_BUF[USART1_REC_LEN];	//接收缓冲,最大USART1_REC_LEN个字节.
/* 接收状态
	bit15	接收完成标志
	bit14	接收到0x0d
	bit13~0	接收到的有效字节数目
*/
u16 USART1_RX_STA = 0;       //接收状态标记

/**
 * @brief 串口1初始化
*/
void USART1_Init(u32 baudRate) {
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟

	USART_DeInit(USART1); //复位USART1

	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
#if FREE_RTOS_SUPPORT
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;//抢占优先级7
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
#else
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
#endif // FREE_RTOS_SUPPORT
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = baudRate;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART1, ENABLE);                    //使能串口1 
}

/**
 * @brief	串口1中断服务程序
*/
void USART1_IRQHandler(void) {
	u8 res;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) { //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		res = USART_ReceiveData(USART1);	//读取接收到的数据
		if ((USART1_RX_STA & 0x8000) == 0) {//最高位为0，还未接受完成
			if (USART1_RX_STA & 0x4000) {//接收到了0x0d
				if (res != 0x0a) USART1_RX_STA = 0;//接收错误,重新开始
				else USART1_RX_STA |= 0x8000;	//如果最后一位是0a，则标记为接受完成
			}
			else { //还没收到0d
				if (res == 0x0d) USART1_RX_STA |= 0x4000;
				else {
					USART1_RX_BUF[USART1_RX_STA & 0X3FFF] = res;
					USART1_RX_STA++;
					if (USART1_RX_STA > (USART1_REC_LEN - 1)) USART1_RX_STA = 0;//接收数据错误,重新开始接收	  
				}
			}
		}
	}
}

/**
 * @brief	USART1_RX_BUF串口3数据缓冲清空
*/
void USART1_RX_BUF_CLEAR(void) {
	memset(USART1_RX_BUF, 0, USART1_REC_LEN);
	USART1_RX_STA = 0;
}
#endif // EN_USART1

//////////////////////////////////////////////////////////////
//					    -- <USART3> --						//
//////////////////////////////////////////////////////////////
#if EN_USART3
u8  USART3_RX_BUF[USART3_REC_LEN];	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
u8  USART3_TX_BUF[USART3_SEN_LEN];	//发送缓冲
u16 USART3_RX_STA = 0;				//接收状态标记	

/**
 * @brief	串口3初始化
*/
void USART3_Init(u32 baudRate) {
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3，GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	USART_DeInit(USART3); //复位USART3

	//USART3_TX   GPIOB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//USART3_RX	  GPIOB.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//Usart3 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
#if FREE_RTOS_SUPPORT
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;//抢占优先级8
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
#else
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
#endif // FREE_RTOS_SUPPORT
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = baudRate;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART3, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART3, ENABLE);	//使能串口 

	TIM7_Int_Init(100 - 1, 7200 - 1);	//TIM7初始化，每10ms触发中断
	TIM_Cmd(TIM7, DISABLE);	//关闭定时器7
}

/**
 * @brief	串口3中断服务程序
*/
void USART3_IRQHandler(void) {
	u8 res;
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) { //串口3产生中断
		res = USART_ReceiveData(USART3); //暂存数据
		if ((USART3_RX_STA & 0x8000) == 0) { //最高位接收完成标志还为0时
			if (USART3_RX_STA < USART3_REC_LEN) { //如果USART3接收缓存仍有余量
				TIM_SetCounter(TIM7, 0); //清空TIM7计数值
				if (USART3_RX_STA == 0)	 TIM_Cmd(TIM7, ENABLE);	//使能TIM7
				USART3_RX_BUF[USART3_RX_STA++] = res; //存储数据
			}
			//否则当达到最大接受缓冲时强制结束存储
			else USART3_RX_STA |= 0x8000; //强制标记接收完成，最高位赋值为1
		}
	}
}

/**
 * @brief	串口3 printf
*/
void USART3_printf(char* fmt, ...) {
	u16 i, j;
	va_list ap;
	va_start(ap, fmt);
	vsprintf((char*)USART3_TX_BUF, fmt, ap);
	va_end(ap);
	i = strlen((const char*)USART3_TX_BUF);//此次发送数据的长度
	for (j = 0; j < i; j++) { //循环发送
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) != SET);  //等待上次传输完成 
		USART_SendData(USART3, (uint8_t)USART3_TX_BUF[j]); 	 //发送数据到串口3 
	}
}

/**
 * @brief	USART3_RX_BUF串口3数据缓冲清空
*/
void USART3_RX_BUF_CLEAR(void) {
	memset(USART3_RX_BUF, 0, USART3_REC_LEN);
	USART3_RX_STA = 0;
}
#endif //EN_USART3

