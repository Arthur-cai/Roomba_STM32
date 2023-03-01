#include "myiic.h"
#include "..\..\Basic\delay\delay.h"

/**
 * @brief	初始化IIC
 *			SDA -> PB7		SCL -> PB6
*/
void IIC_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//初始拉高SDA和SCL
	GPIO_SetBits(GPIOB, GPIO_Pin_6|GPIO_Pin_7);
}

/**
 * @brief	延时函数
*/
static void IIC_Delay(void) {
	delay_us(2);
}

/**
 * @brief	发送开始信号
 *			SDA -> PB7		SCL -> PB6
*/
void IIC_SendStart(void) {
	//设置PB7为输出
	SDA_IO_OUT();
	//开始SDA和SCL全拉高
	SDA_OUT = 1;	SCL_OUT = 1;	IIC_Delay();
	//SDA出现下降沿时，此时SDA低电平，SCL高电平，说明是开始信号
	SDA_OUT = 0;	IIC_Delay();
	//再拉低SCL，钳住总线，准备收发数据
	SCL_OUT = 0;
}

/**
 * @brief	发送停止信号
 *			SDA -> PB7		SCL -> PB6
*/
void IIC_SendStop(void) {
	//设置PB7为输出
	SDA_IO_OUT();
	//开始SDA和SCL全拉低
	SCL_OUT = 0;	SDA_OUT = 0;	IIC_Delay();
	//先拉高SCL，等待SDA上升沿，说明是停止信号
	SCL_OUT = 1;	SDA_OUT = 1;	IIC_Delay();
}

/**
 * @brief	发送ACK信号
 *			SDA -> PB7		SCL -> PB6
*/
void IIC_SendAck(void) {
	//SCL低电平
	SCL_OUT = 0;
	//SDA输出低电平，说明是ACK
	SDA_IO_OUT();
	SDA_OUT = 0;	IIC_Delay();
	//SCL输出一个高低跳变
	SCL_OUT = 1;	IIC_Delay();	SCL_OUT = 0;
}

/**
 * @brief	发送NACK信号
 *			SDA -> PB7		SCL -> PB6
*/
void IIC_SendNAck(void) {
	//SCL低电平
	SCL_OUT = 0;
	//SDA输出高电平，说明是NACK
	SDA_IO_OUT();
	SDA_OUT = 1;	IIC_Delay();
	//SCL输出一个高低跳变
	SCL_OUT = 1;	IIC_Delay();	SCL_OUT = 0;
}

/**
 * @brief	等待ACK信号
 *			SDA -> PB7		SCL -> PB6
 * @retval	接收到ACK返回SUCCEED，否则返回FAIL
*/
operateSta IIC_WaitAck(void) {
	u8 time = 0;
	
	//拉高SDA和SCL
	SDA_OUT = 1;	IIC_Delay();
	SCL_OUT = 1;	IIC_Delay();
	//设置PB7为输入，接收SDA
	SDA_IO_IN();
	//当SDA一直是高电平，说明没有ACK信号发来
	while (SDA_IN) {
		time++;
		if (time > 250) {
			IIC_SendStop();
			return FAIL;
		}
	}
	//拉低SCL
	SCL_OUT = 0;
	return SUCCEED;
}

/**
 * @brief	发送1个字节
 *			SDA -> PB7		SCL -> PB6
*/
void IIC_SendByte(u8 data) {
	u8 i;
	//设置PB7输出
	SDA_IO_OUT();
	//拉低SCL开始传输
	SCL_OUT = 0;

	for (i = 0; i < 8; i++) {
		//从高位开始传
		SDA_OUT = (data & 0x80) >> 7;
		//data传完1位后左移1位，等下一次发送
		data <<= 1;	
		//SCL输出一个高低跳变
		SCL_OUT = 1;	IIC_Delay();
		SCL_OUT = 0;	IIC_Delay();
	}
}

/**
 * @brief	读取1个字节
 *			SDA -> PB7		SCL -> PB6
 * @param ack	1表示发送ACK，0表示发送NACK
 * @retval	读取到的字节
*/
u8 IIC_ReadByte(u8 ack) {
	u8 i, res = 0;
	//设置PB7为输入
	SDA_IO_IN();

	for (i = 0; i < 8; i++) {
		//SCL输出一个低高跳变
		SCL_OUT = 0;	IIC_Delay();
		SCL_OUT = 1;	
		//res先左移1位再接收
		res <<= 1;
		if (SDA_IN) res |= SDA_IN;
		IIC_Delay();
	}
	if (ack == 1) IIC_SendAck(); //ack为1则发送ACK
	else IIC_SendNAck();	//发送NACK
	return res;
}

