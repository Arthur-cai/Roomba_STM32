#include "mpu6050.h"

Three_Degrees_Type degrees;	//MPU角度值结构体

//////////////////////////////////////////////////////////////////////
//						   -- IIC相关函数 --			  			    //
//////////////////////////////////////////////////////////////////////

/**
 * @breif	MPU6050写入一个字节
*/
operateSta MPU_Write_Byte(u8 reg, u8 byte) {
	IIC_SendStart();	//发送开始信号
	IIC_SendByte((MPU_ADDR << 1) | 0); //发送7位器件地址 + LSB读写命令（0写，1读）
	//等待应答
	if (IIC_WaitAck() == FAIL) {
		IIC_SendStop();	//如果应答失败，则发送停止信号，并返回FAIL
		return FAIL;
	}
	IIC_SendByte(reg); //写寄存器地址
	IIC_WaitAck();	//等待应答
	IIC_SendByte(byte);	//发送字节
	if (IIC_WaitAck() == FAIL) {
		IIC_SendStop();	//如果应答失败，则发送停止信号，并返回FAIL
		return FAIL;
	}
	IIC_SendStop();	//发送结束信号
	return SUCCEED;
}

/**
 * @breif	MPU6050读取一个字节
*/
u8 MPU_Read_Byte(u8 reg) {
	u8 res;

	IIC_SendStart();	//发送开始信号
	IIC_SendByte((MPU_ADDR << 1) | 0); //发送7位器件地址 + LSB读写命令（0写，1读）
	IIC_WaitAck(); //等待应答
	IIC_SendByte(reg);	//发送寄存器地址
	IIC_WaitAck(); //等待应答

	//开始读周期的信号
	IIC_SendStart();	//发送开始信号
	IIC_SendByte((MPU_ADDR << 1) | 1); //发送7位器件地址 + LSB读写命令（0写，1读）
	IIC_WaitAck(); //等待应答
	res = IIC_ReadByte(0);	//读取字节，并发送NACK
	IIC_SendStop();	//发送停止信号
	return res;
}

/**
 *@brief	IIC连续写
*/
u8 MPU_Write_Len(u8 addr, u8 reg, u8 len, u8* buf) {
	u8 i;
	IIC_SendStart();
	IIC_SendByte((addr << 1) | 0);//发送器件地址+写命令	
	if (IIC_WaitAck() == FAIL) {//等待应答
		IIC_SendStop();
		return 1;
	}
	IIC_SendByte(reg);	//写寄存器地址
	IIC_WaitAck();		//等待应答
	for (i = 0; i < len; i++) {
		IIC_SendByte(buf[i]);	//发送数据
		if (IIC_WaitAck() == FAIL) {	//等待ACK
			IIC_SendStop();
			return 1;
		}
	}
	IIC_SendStop();
	return 0;
}

/**
 *@brief	IIC连续读
*/
u8 MPU_Read_Len(u8 addr, u8 reg, u8 len, u8* buf) {
	IIC_SendStart();
	IIC_SendByte((addr << 1) | 0);//发送器件地址+写命令	
	if (IIC_WaitAck() == FAIL) {//等待应答
		IIC_SendStop();
		return 1;
	}
	IIC_SendByte(reg);	//写寄存器地址
	IIC_WaitAck();		//等待应答
	IIC_SendStart();
	IIC_SendByte((addr << 1) | 1);//发送器件地址+读命令	
	IIC_WaitAck();		//等待应答 
	while (len) {
		if (len == 1) *buf = IIC_ReadByte(0);//读到最后一个字节数据,发送nACK 
		else *buf = IIC_ReadByte(1);		//读数据,发送ACK  
		len--;
		buf++;
	}
	IIC_SendStop();	//产生一个停止条件 
	return 0;
}

//////////////////////////////////////////////////////////////////////
//						-- MPU6050相关API函数 --						//
////////////////////////////////////////////////////////////////////// 

/**
 * @brief	设置MPU6050陀螺仪传感器满量程范围
*/
operateSta MPU_Set_Gyro_Fsr(u8 fullScaleRange) {
	return MPU_Write_Byte(MPU_GYRO_CFG_REG, fullScaleRange << 3); 
}

/**
 * @brief	设置MPU6050加速度传感器满量程范围
*/
operateSta MPU_Set_Accel_Fsr(u8 fullScaleRange) {
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG, fullScaleRange << 3);
}

/**
 * @brief	设置MPU6050的数字低通滤波器
*/
operateSta MPU_Set_LPF(u16 lpf) {
	u8 data;
	if (lpf >= 188) data = 1;
	else if (lpf >= 98) data = 2;
	else if (lpf >= 42) data = 3;
	else if (lpf >= 20) data = 4;
	else if (lpf >= 10) data = 5;
	else data = 6;
	return MPU_Write_Byte(MPU_CFG_REG, data); //设置数字低通滤波器  
}

/**
 * @brief	设置MPU6050的采样率(假定Fs=1KHz)
*/
operateSta MPU_Set_Rate(u16 rate) {
	u8 data;
	if (rate > 1000) rate = 1000;
	else if (rate < 4) rate = 4;
	data = 1000 / rate - 1;
	MPU_Set_LPF(rate / 2);	//自动设置LPF为采样率的一半
	return MPU_Write_Byte(MPU_SAMPLE_RATE_REG, data);	//设置数字低通滤波器
}

/**
 * @brief	得到扩大了100倍的温度值（带符号）
*/
short MPU_Get_Temperature(void) {
	u8 buf[2];
	short raw;
	float temp;
	MPU_Read_Len(MPU_ADDR, MPU_TEMP_OUTH_REG, 2, buf);
	raw = ((u16)buf[0] << 8) | buf[1];
	temp = 36.53 + ((double)raw) / 340;
	return temp * 100;
}

/**
 * @brief	得到陀螺仪值(原始值)
*/
operateSta MPU_Get_Gyroscope(short* gx, short* gy, short* gz) {
	u8 buf[6];
	if (MPU_Read_Len(MPU_ADDR, MPU_GYRO_XOUTH_REG, 6, buf) == 0) {
		*gx = ((u16)buf[0] << 8) | buf[1];
		*gy = ((u16)buf[2] << 8) | buf[3];
		*gz = ((u16)buf[4] << 8) | buf[5];
		return SUCCEED;
	}
	return FAIL;
}

/**
 * @brief	得到加速度值(原始值)
*/
operateSta MPU_Get_Accelerometer(short* ax, short* ay, short* az) {
	u8 buf[6];
	if (MPU_Read_Len(MPU_ADDR, MPU_ACCEL_XOUTH_REG, 6, buf) == 0) {
		*ax = ((u16)buf[0] << 8) | buf[1];
		*ay = ((u16)buf[2] << 8) | buf[3];
		*az = ((u16)buf[4] << 8) | buf[5];
		return SUCCEED;
	}
	return FAIL;
}

/**
 * @brief	判断MPU6050地址是否为0x68
*/
boolean Is_MPU_ADDR_0x68(void) {
	u8 id = MPU_Read_Byte(MPU_DEVICE_ID_REG);	//读设备ID
	if (id == MPU_ADDR) {
		MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X01);//设置CLKSEL,PLL X轴为参考
		MPU_Write_Byte(MPU_PWR_MGMT2_REG, 0X00);//加速度与陀螺仪都工作
		MPU_Set_Rate(50);						//设置采样率为50Hz
		return TRUE;
	}
	return FALSE;
}

/**
 * @brief	调用 inv_mpu.c 内的 mpu_dmp_init() 初始化DMP解算引擎来判断mpu是否存在
*/
boolean Is_MPU_Exist(void) {
	if (mpu_dmp_init() == 0) return TRUE;
	return FALSE;
}

/**
 * @brief	MPU6050初始化
*/
operateSta MPU_Init(void) {
	GPIO_InitTypeDef	GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);

	//初始化AD0引脚，输出低电平，以控制IIC通讯地址为0x68
	GPIO_InitStructure.GPIO_Pin = MPU_AD0_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MPU_AD0_PORT, &GPIO_InitStructure);

	MPU_AD0 = 0; //控制AD0输出低电平
	IIC_Init(); //初始化IIC
	MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0x80); //复位MPU6050
	delay_ms(100);
	
	MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0x00); //唤醒MPU6050
	MPU_Set_Gyro_Fsr(3);					 //陀螺仪传感器,±2000dps
	MPU_Set_Accel_Fsr(0);					 //加速度传感器,±2g
	MPU_Set_Rate(50);						 //设置采样率50Hz
	MPU_Write_Byte(MPU_INT_EN_REG, 0X00);	 //关闭所有中断
	MPU_Write_Byte(MPU_USER_CTRL_REG, 0X00); //I2C主模式关闭
	MPU_Write_Byte(MPU_FIFO_EN_REG, 0X00);	 //关闭FIFO
	MPU_Write_Byte(MPU_INTBP_CFG_REG, 0X80); //INT引脚低电平有效
	//调用Is_MPU_ADDR_0x68和Is_MPU_Exist以确保初始化成功
	if (Is_MPU_ADDR_0x68() && Is_MPU_Exist()) return SUCCEED;	
	return FAIL;
}

/**
 * @brief	获取DMP解算后的全局变量pitch、roll、yaw
*/
operateSta Get_ThreeDegrees(void) {
	u8 state;
	state = mpu_dmp_get_data();
	if (state == 0) return SUCCEED;
	return FAIL;
}
