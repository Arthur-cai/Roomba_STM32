#include "vl53l0x_i2c.h"

/**
 * @brief	初始化VL53L0X的IIC总线
 * @note	右边的VL53模块	SDA -> PF12		SCL -> PF11
 * @note	左边的VL53模块	SDA -> PD5		SCL -> PD4
*/
void VL53L0X_IIC_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOF, ENABLE);
	//初始化左边的模块
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//初始化右边的模块
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	//初始拉高SDA和SCL
	GPIO_SetBits(GPIOF, GPIO_Pin_11 | GPIO_Pin_12);
	GPIO_SetBits(GPIOD, GPIO_Pin_4 | GPIO_Pin_5);
}

/**
 * @brief	延时2us
*/
void VL53L0X_IIC_Delay(void) {
	delay_us(2);
}

/**
 * @brief	发送开始信号
*/
void VL53L0X_IIC_SendStart(u8 flag) {
	if (flag == CHOOSE_RIGHT_FLAG) {
		RVL53L0X_SDA_IO_OUT();
		//开始SDA和SCL全拉高
		RVL53L0X_SDA_OUT = 1;	RVL53L0X_SCL_OUT = 1;	VL53L0X_IIC_Delay();
		//SDA出现下降沿时，此时SDA低电平，SCL高电平，说明是开始信号
		RVL53L0X_SDA_OUT = 0;	VL53L0X_IIC_Delay();
		//再拉低SCL，钳住总线，准备收发数据
		RVL53L0X_SCL_OUT = 0;
	}
	else if (flag == CHOOSE_LEFT_FLAG) {
		LVL53L0X_SDA_IO_OUT();
		//开始SDA和SCL全拉高
		LVL53L0X_SDA_OUT = 1;	LVL53L0X_SCL_OUT = 1;	VL53L0X_IIC_Delay();
		//SDA出现下降沿时，此时SDA低电平，SCL高电平，说明是开始信号
		LVL53L0X_SDA_OUT = 0;	VL53L0X_IIC_Delay();
		//再拉低SCL，钳住总线，准备收发数据
		LVL53L0X_SCL_OUT = 0;
	}
}

/**
 * @brief	发送停止信号
*/
void VL53L0X_IIC_SendStop(u8 flag) {
	if (flag == CHOOSE_RIGHT_FLAG) {
		RVL53L0X_SDA_IO_OUT();
		//开始SDA和SCL全拉低
		RVL53L0X_SCL_OUT = 0;	RVL53L0X_SDA_OUT = 0;	VL53L0X_IIC_Delay();
		//先拉高SCL，等待SDA上升沿，说明是停止信号
		RVL53L0X_SCL_OUT = 1;	RVL53L0X_SDA_OUT = 1;	VL53L0X_IIC_Delay();
	}
	else if (flag == CHOOSE_LEFT_FLAG) {
		LVL53L0X_SDA_IO_OUT();
		//开始SDA和SCL全拉低
		LVL53L0X_SCL_OUT = 0;	LVL53L0X_SDA_OUT = 0;	VL53L0X_IIC_Delay();
		//先拉高SCL，等待SDA上升沿，说明是停止信号
		LVL53L0X_SCL_OUT = 1;	LVL53L0X_SDA_OUT = 1;	VL53L0X_IIC_Delay();
	}
}

/**
 * @brief	发送ACK信号
 */
void VL53L0X_IIC_SendAck(u8 flag) {
	if (flag == CHOOSE_RIGHT_FLAG) {
		//SCL低电平
		RVL53L0X_SCL_OUT = 0;
		//SDA输出低电平，说明是ACK
		RVL53L0X_SDA_IO_OUT();
		RVL53L0X_SDA_OUT = 0;	VL53L0X_IIC_Delay();
		//SCL输出一个高低跳变
		RVL53L0X_SCL_OUT = 1;	VL53L0X_IIC_Delay();	RVL53L0X_SCL_OUT = 0;
	}
	else if (flag == CHOOSE_LEFT_FLAG) {
		//SCL低电平
		LVL53L0X_SCL_OUT = 0;
		//SDA输出低电平，说明是ACK
		LVL53L0X_SDA_IO_OUT();
		LVL53L0X_SDA_OUT = 0;	VL53L0X_IIC_Delay();
		//SCL输出一个高低跳变
		LVL53L0X_SCL_OUT = 1;	VL53L0X_IIC_Delay();	LVL53L0X_SCL_OUT = 0;
	}
}

/**
 * @brief	发送NACK信号
*/
void VL53L0X_IIC_SendNAck(u8 flag) {
	if (flag == CHOOSE_RIGHT_FLAG) {
		//SCL低电平
		RVL53L0X_SCL_OUT = 0;
		//SDA输出高电平，说明是NACK
		RVL53L0X_SDA_IO_OUT();
		RVL53L0X_SDA_OUT = 1;	VL53L0X_IIC_Delay();
		//SCL输出一个高低跳变
		RVL53L0X_SCL_OUT = 1;	VL53L0X_IIC_Delay();	RVL53L0X_SCL_OUT = 0;
	}
	else if (flag == CHOOSE_LEFT_FLAG) {
		//SCL低电平
		LVL53L0X_SCL_OUT = 0;
		//SDA输出高电平，说明是NACK
		LVL53L0X_SDA_IO_OUT();
		LVL53L0X_SDA_OUT = 1;	VL53L0X_IIC_Delay();
		//SCL输出一个高低跳变
		LVL53L0X_SCL_OUT = 1;	VL53L0X_IIC_Delay();	LVL53L0X_SCL_OUT = 0;
	}
}

/**
 * @brief	等待ACK信号
*/
u8 VL53L0X_IIC_WaitAck(u8 flag) {
	u8 time = 0;
	if (flag == CHOOSE_RIGHT_FLAG) {
		//拉高SDA和SCL
		RVL53L0X_SDA_OUT = 1;	VL53L0X_IIC_Delay();
		RVL53L0X_SCL_OUT = 1;	VL53L0X_IIC_Delay();
		//设置PB7为输入，接收SDA
		RVL53L0X_SDA_IO_IN();
		//当SDA一直是高电平，说明没有ACK信号发来
		while (RVL53L0X_SDA_IN) {
			time++;
			if (time > 250) {
				VL53L0X_IIC_SendStop(CHOOSE_RIGHT_FLAG);
				return 1;
			}
		}
		//拉低SCL
		RVL53L0X_SCL_OUT = 0;
		return 0;
	}
	else if (flag == CHOOSE_LEFT_FLAG) {
		//拉高SDA和SCL
		LVL53L0X_SDA_OUT = 1;	VL53L0X_IIC_Delay();
		LVL53L0X_SCL_OUT = 1;	VL53L0X_IIC_Delay();
		//设置PB7为输入，接收SDA
		LVL53L0X_SDA_IO_IN();
		//当SDA一直是高电平，说明没有ACK信号发来
		while (LVL53L0X_SDA_IN) {
			time++;
			if (time > 250) {
				VL53L0X_IIC_SendStop(CHOOSE_LEFT_FLAG);
				return 1;
			}
		}
		//拉低SCL
		LVL53L0X_SCL_OUT = 0;
		return 0;
	}
	return 1;
}

/**
 * @brief	发送1个字节
*/
void VL53L0X_IIC_SendByte(u8 data, u8 flag) {
	u8 i;
	if (flag == CHOOSE_RIGHT_FLAG) {
		//设置PB7输出
		RVL53L0X_SDA_IO_OUT();
		//拉低SCL开始传输
		RVL53L0X_SCL_OUT = 0;

		for (i = 0; i < 8; i++) {
			//从高位开始传
			RVL53L0X_SDA_OUT = (data & 0x80) >> 7;
			//data传完1位后左移1位，等下一次发送
			data <<= 1;
			//SCL输出一个高低跳变
			RVL53L0X_SCL_OUT = 1;	VL53L0X_IIC_Delay();
			RVL53L0X_SCL_OUT = 0;	VL53L0X_IIC_Delay();
		}
	}
	else if (flag == CHOOSE_LEFT_FLAG) {
		//设置PB7输出
		LVL53L0X_SDA_IO_OUT();
		//拉低SCL开始传输
		LVL53L0X_SCL_OUT = 0;

		for (i = 0; i < 8; i++) {
			//从高位开始传
			LVL53L0X_SDA_OUT = (data & 0x80) >> 7;
			//data传完1位后左移1位，等下一次发送
			data <<= 1;
			//SCL输出一个高低跳变
			LVL53L0X_SCL_OUT = 1;	VL53L0X_IIC_Delay();
			LVL53L0X_SCL_OUT = 0;	VL53L0X_IIC_Delay();
		}
	}
}

/**
 * @brief	读取1个字节
*/
u8 VL53L0X_IIC_ReadByte(u8 ack, u8 flag) {
	u8 i, res = 0;
	if (flag == CHOOSE_RIGHT_FLAG) {
		//设置PB7为输入
		RVL53L0X_SDA_IO_IN();
		for (i = 0; i < 8; i++) {
			//SCL输出一个低高跳变
			RVL53L0X_SCL_OUT = 0;	VL53L0X_IIC_Delay();
			RVL53L0X_SCL_OUT = 1;
			//res先左移1位再接收
			res <<= 1;
			if (RVL53L0X_SDA_IN) res |= RVL53L0X_SDA_IN;
			VL53L0X_IIC_Delay();
		}
		if (ack == 1) VL53L0X_IIC_SendAck(CHOOSE_RIGHT_FLAG); //ack为1则发送ACK
		else VL53L0X_IIC_SendNAck(CHOOSE_RIGHT_FLAG);	//发送NACK
		return res;
	}
	//设置PB7为输入
	LVL53L0X_SDA_IO_IN();
	for (i = 0; i < 8; i++) {
		//SCL输出一个低高跳变
		LVL53L0X_SCL_OUT = 0;	VL53L0X_IIC_Delay();
		LVL53L0X_SCL_OUT = 1;
		//res先左移1位再接收
		res <<= 1;
		if (LVL53L0X_SDA_IN) res |= LVL53L0X_SDA_IN;
		VL53L0X_IIC_Delay();
	}
	if (ack == 1) VL53L0X_IIC_SendAck(CHOOSE_LEFT_FLAG); //ack为1则发送ACK
	else VL53L0X_IIC_SendNAck(CHOOSE_LEFT_FLAG);	//发送NACK
	return res;
}

/**
 * @brief	写1个字节数据到VL53L0X
*/
u8 VL53L0X_WriteOneByte(u8 slaveAddr, u8 regAddr, u8 data, u8 flag) {
	VL53L0X_IIC_SendStart(flag);	//发送开始信号
	VL53L0X_IIC_SendByte(slaveAddr, flag); //发送7位器件地址 + LSB读写命令（0写，1读）
	//等待应答
	if (VL53L0X_IIC_WaitAck(flag) == 1) {
		VL53L0X_IIC_SendStop(flag);	//如果应答失败，则发送停止信号，并返回1
		return 1;
	}
	VL53L0X_IIC_SendByte(regAddr, flag); //写寄存器地址
	VL53L0X_IIC_WaitAck(flag);	//等待应答
	VL53L0X_IIC_SendByte(data, flag);	//发送字节
	VL53L0X_IIC_WaitAck(flag);
	VL53L0X_IIC_SendStop(flag);	//发送结束信号
	return 0;
}

/**
 * @brief	从VL53L0X读1个字节数据
*/
u8 VL53L0X_ReadOneByte(u8 slaveAddr, u8 regAddr, u8* data, u8 flag) {
	//先发送一个写信号，写入想读取的寄存器
	VL53L0X_IIC_SendStart(flag);	//发送开始信号
	VL53L0X_IIC_SendByte(slaveAddr, flag); //发送7位器件地址 + LSB读写命令（0写，1读）
	//等待应答
	if (VL53L0X_IIC_WaitAck(flag) == 1) {
		VL53L0X_IIC_SendStop(flag);	//如果应答失败，则发送停止信号，并返回1
		return 1;
	}
	VL53L0X_IIC_SendByte(regAddr, flag);	//发送寄存器地址
	VL53L0X_IIC_WaitAck(flag);	//等待应答

	//开始读周期的信号
	VL53L0X_IIC_SendStart(flag);	//发送开始信号
	VL53L0X_IIC_SendByte(slaveAddr | 0x01, flag); //发送7位器件地址 + LSB读写命令（0写，1读）
	VL53L0X_IIC_WaitAck(flag); //等待应答
	*data = VL53L0X_IIC_ReadByte(0, flag);	//读取字节，并发送NACK
	VL53L0X_IIC_SendStop(flag);	//发送停止信号
	return 0;
}

/**
 * @brief	写多个字节数据到VL53L0X
*/
u8 VL53L0X_WriteNBytes(u8 slaveAddr, u8 regAddr, u16 len, u8* pbuf, u8 flag) {
	VL53L0X_IIC_SendStart(flag);	//发送开始信号
	VL53L0X_IIC_SendByte(slaveAddr, flag); //发送7位器件地址 + LSB读写命令（0写，1读）
	//等待应答
	if (VL53L0X_IIC_WaitAck(flag) == 1) {
		VL53L0X_IIC_SendStop(flag);	//如果应答失败，则发送停止信号，并返回1
		return 1;
	}
	VL53L0X_IIC_SendByte(regAddr, flag); //写寄存器地址
	VL53L0X_IIC_WaitAck(flag);	//等待应答
	while (len--) {
		VL53L0X_IIC_SendByte(*pbuf, flag);	//循环发送字节
		VL53L0X_IIC_WaitAck(flag);	//等待应答
		pbuf++;
	}
	VL53L0X_IIC_SendStop(flag);	//发送结束信号
	return 0;
}

/**
 * @brief	从VL53L0X读多个字节数据
*/
u8 VL53L0X_ReadNBytes(u8 slaveAddr, u8 regAddr, u16 len, u8* pbuf, u8 flag) {
	//先发送一个写信号，写入想读取的寄存器
	VL53L0X_IIC_SendStart(flag);	//发送开始信号
	VL53L0X_IIC_SendByte(slaveAddr, flag); //发送7位器件地址 + LSB读写命令（0写，1读）
	//等待应答
	if (VL53L0X_IIC_WaitAck(flag) == 1) {
		VL53L0X_IIC_SendStop(flag);	//如果应答失败，则发送停止信号，并返回1
		return 1;
	}
	VL53L0X_IIC_SendByte(regAddr, flag);	//发送寄存器地址
	VL53L0X_IIC_WaitAck(flag);	//等待应答

	//开始读周期的信号
	VL53L0X_IIC_SendStart(flag);	//发送开始信号
	VL53L0X_IIC_SendByte(slaveAddr | 0x01, flag); //发送7位器件地址 + LSB读写命令（0写，1读）
	VL53L0X_IIC_WaitAck(flag); //等待应答
	while (len) {
		if (len == 1) 
			*pbuf = VL53L0X_IIC_ReadByte(0, flag);	//读到最后一个字节时发送NACK
		else 
			*pbuf = VL53L0X_IIC_ReadByte(1, flag);	//读取字节，并发送ACK
		pbuf++;	//指针加一
		len--;
	}
	VL53L0X_IIC_SendStop(flag);	//发送停止信号
	return 0;
}

/**
 * @brief	VL53L0X 写多个数据
*/
u8 VL53L0X_write_multi(u8 address, u8 index, u8* pdata, u16 count, u8 flag) {
	if (VL53L0X_WriteNBytes(address, index, count, pdata, flag) == 1)
		return 1;
	return 0;
}

/**
 * @brief	VL53L0X 读多个数据
*/
u8 VL53L0X_read_multi(u8 address, u8 index, u8* pdata, u16 count, u8 flag) {
	if (VL53L0X_ReadNBytes(address, index, count, pdata, flag) == 1)
		return 1;
	return 0;
}

/**
 * @brief	VL53L0X 写1个字节，8位数据
*/
u8 VL53L0X_write_byte(u8 address, u8 index, u8 data, u8 flag) {
	if (VL53L0X_WriteNBytes(address, index, 1, &data, flag) == 1)
		return 1;
	return 0;
}

/**
 * @brief	VL53L0X 写1个字，16位数据
*/
u8 VL53L0X_write_word(u8 address, u8 index, u16 data, u8 flag) {
	u8 temp[2];
	temp[0] = (u8)(data >> 8);
	temp[1] = (u8)(data & 0x00FF);

	if (VL53L0X_WriteNBytes(address, index, 2, temp, flag) == 1)
		return 1;
	return 0;
}

/**
 * @brief	VL53L0X 写2个字，32位数据
*/
u8 VL53L0X_write_dword(u8 address, u8 index, u32 data, u8 flag) {
	u8 temp[4];
	temp[0] = (u8)(data >> 24);
	temp[1] = (u8)((data & 0x00FF0000) >> 16);
	temp[2] = (u8)((data & 0x0000FF00) >> 8);
	temp[3] = (u8)(data & 0x000000FF);

	if (VL53L0X_WriteNBytes(address, index, 4, temp, flag) == 1)
		return 1;
	return 0;
}

/**
 * @brief	VL53L0X 读1个字节，8位数据
*/
u8 VL53L0X_read_byte(u8 address, u8 index, u8* pdata, u8 flag) {
	if (VL53L0X_ReadNBytes(address, index, 1, pdata, flag) == 1)
		return 1;
	return 0;
}

/**
 * @brief	VL53L0X 读1个字，16位数据
*/
u8 VL53L0X_read_word(u8 address, u8 index, u16* data, u8 flag) {
	u8 temp[2];

	if (VL53L0X_ReadNBytes(address, index, 2, temp, flag) == 1)
		return 1;
	else
		*data = ((u16)temp[0] << 8) | (u16)temp[1];
	return 0;
}

/**
 * @brief	VL53L0X 读2个字，32位数据
*/
u8 VL53L0X_read_dword(u8 address, u8 index, u32* data, u8 flag) {
	u8 temp[4];

	if (VL53L0X_ReadNBytes(address, index, 4, temp, flag) == 1)
		return 1;
	else
		*data = ((u32)temp[0] << 24) | ((u32)temp[1] << 16) | ((u32)temp[2] << 8) | ((u32)temp[3]);
	return 0;
}
