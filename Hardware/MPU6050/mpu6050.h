#ifndef __MPU6050_H
#define __MPU6050_H
#include "..\..\Basic\sys\sys.h"
#include "..\..\Basic\i2c\myiic.h"
#include "..\MPU6050\eMPL\inv_mpu.h"
#include "..\MPU6050\eMPL\inv_mpu_dmp_motion_driver.h"
#include "..\..\Basic\delay\delay.h"

//MPU角度值结构体
typedef struct _three_degrees_struct {
	float pitch; //俯仰（绕Y轴）
	float roll;  //横滚（绕X轴）
	float yaw;   //航向（绕Z轴）
}Three_Degrees_Type;
extern Three_Degrees_Type degrees;	//MPU角度值结构体

//////////////////////////////////////////////////////////////////////
//						-- MPU6050硬件定义 --						//
//////////////////////////////////////////////////////////////////////

//MPU6050引脚AD0	 -> PG13
#define MPU_AD0_PIN		GPIO_Pin_13
#define	MPU_AD0_PORT	GPIOG
#define	MPU_AD0			PGout(13)	//控制AD0电平,从而控制MPU地址

#define MPU_SELF_TESTX_REG		0X0D	//自检寄存器X
#define MPU_SELF_TESTY_REG		0X0E	//自检寄存器Y
#define MPU_SELF_TESTZ_REG		0X0F	//自检寄存器Z
#define MPU_SELF_TESTA_REG		0X10	//自检寄存器A
#define MPU_SAMPLE_RATE_REG		0X19	//采样频率分频器
#define MPU_CFG_REG				0X1A	//配置寄存器
#define MPU_GYRO_CFG_REG		0X1B	//陀螺仪配置寄存器
#define MPU_ACCEL_CFG_REG		0X1C	//加速度计配置寄存器
#define MPU_MOTION_DET_REG		0X1F	//运动检测阀值设置寄存器
#define MPU_FIFO_EN_REG			0X23	//FIFO使能寄存器
#define MPU_I2CMST_CTRL_REG		0X24	//IIC主机控制寄存器
#define MPU_I2CSLV0_ADDR_REG	0X25	//IIC从机0器件地址寄存器
#define MPU_I2CSLV0_REG			0X26	//IIC从机0数据地址寄存器
#define MPU_I2CSLV0_CTRL_REG	0X27	//IIC从机0控制寄存器
#define MPU_I2CSLV1_ADDR_REG	0X28	//IIC从机1器件地址寄存器
#define MPU_I2CSLV1_REG			0X29	//IIC从机1数据地址寄存器
#define MPU_I2CSLV1_CTRL_REG	0X2A	//IIC从机1控制寄存器
#define MPU_I2CSLV2_ADDR_REG	0X2B	//IIC从机2器件地址寄存器
#define MPU_I2CSLV2_REG			0X2C	//IIC从机2数据地址寄存器
#define MPU_I2CSLV2_CTRL_REG	0X2D	//IIC从机2控制寄存器
#define MPU_I2CSLV3_ADDR_REG	0X2E	//IIC从机3器件地址寄存器
#define MPU_I2CSLV3_REG			0X2F	//IIC从机3数据地址寄存器
#define MPU_I2CSLV3_CTRL_REG	0X30	//IIC从机3控制寄存器
#define MPU_I2CSLV4_ADDR_REG	0X31	//IIC从机4器件地址寄存器
#define MPU_I2CSLV4_REG			0X32	//IIC从机4数据地址寄存器
#define MPU_I2CSLV4_DO_REG		0X33	//IIC从机4写数据寄存器
#define MPU_I2CSLV4_CTRL_REG	0X34	//IIC从机4控制寄存器
#define MPU_I2CSLV4_DI_REG		0X35	//IIC从机4读数据寄存器

#define MPU_I2CMST_STA_REG		0X36	//IIC主机状态寄存器
#define MPU_INTBP_CFG_REG		0X37	//中断/旁路设置寄存器
#define MPU_INT_EN_REG			0X38	//中断使能寄存器
#define MPU_INT_STA_REG			0X3A	//中断状态寄存器

#define MPU_ACCEL_XOUTH_REG		0X3B	//加速度值,X轴高8位寄存器
#define MPU_ACCEL_XOUTL_REG		0X3C	//加速度值,X轴低8位寄存器
#define MPU_ACCEL_YOUTH_REG		0X3D	//加速度值,Y轴高8位寄存器
#define MPU_ACCEL_YOUTL_REG		0X3E	//加速度值,Y轴低8位寄存器
#define MPU_ACCEL_ZOUTH_REG		0X3F	//加速度值,Z轴高8位寄存器
#define MPU_ACCEL_ZOUTL_REG		0X40	//加速度值,Z轴低8位寄存器

#define MPU_TEMP_OUTH_REG		0X41	//温度值高八位寄存器
#define MPU_TEMP_OUTL_REG		0X42	//温度值低8位寄存器

#define MPU_GYRO_XOUTH_REG		0X43	//陀螺仪值,X轴高8位寄存器
#define MPU_GYRO_XOUTL_REG		0X44	//陀螺仪值,X轴低8位寄存器
#define MPU_GYRO_YOUTH_REG		0X45	//陀螺仪值,Y轴高8位寄存器
#define MPU_GYRO_YOUTL_REG		0X46	//陀螺仪值,Y轴低8位寄存器
#define MPU_GYRO_ZOUTH_REG		0X47	//陀螺仪值,Z轴高8位寄存器
#define MPU_GYRO_ZOUTL_REG		0X48	//陀螺仪值,Z轴低8位寄存器

#define MPU_I2CSLV0_DO_REG		0X63	//IIC从机0数据寄存器
#define MPU_I2CSLV1_DO_REG		0X64	//IIC从机1数据寄存器
#define MPU_I2CSLV2_DO_REG		0X65	//IIC从机2数据寄存器
#define MPU_I2CSLV3_DO_REG		0X66	//IIC从机3数据寄存器

#define MPU_I2CMST_DELAY_REG	0X67	//IIC主机延时管理寄存器
#define MPU_SIGPATH_RST_REG		0X68	//信号通道复位寄存器
#define MPU_MDETECT_CTRL_REG	0X69	//运动检测控制寄存器
#define MPU_USER_CTRL_REG		0X6A	//用户控制寄存器
#define MPU_PWR_MGMT1_REG		0X6B	//电源管理寄存器1
#define MPU_PWR_MGMT2_REG		0X6C	//电源管理寄存器2 
#define MPU_FIFO_CNTH_REG		0X72	//FIFO计数寄存器高八位
#define MPU_FIFO_CNTL_REG		0X73	//FIFO计数寄存器低八位
#define MPU_FIFO_RW_REG			0X74	//FIFO读写寄存器
#define MPU_DEVICE_ID_REG		0X75	//器件ID寄存器

//如果AD0脚(9脚)接地,IIC地址为0X68(不包含最低位).
//如果接V3.3,则IIC地址为0X69(不包含最低位).
#define MPU_ADDR				0X68

////因为模块AD0默认接GND,所以转为读写地址后,为0XD1和0XD0(如果接VCC,则为0XD3和0XD2)  
//#define MPU_READ    0XD1
//#define MPU_WRITE   0XD0

//////////////////////////////////////////////////////////////////////
//						-- MPU6050相关操作函数 --					//
//////////////////////////////////////////////////////////////////////

/**
 * @brief	设置MPU6050陀螺仪传感器满量程范围
 * @param	fsr	可取值：0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
 * @retval	成功即返回SUCCEED，否则返回FAIL
*/
operateSta MPU_Set_Gyro_Fsr(u8 fullScaleRange);

/**
 * @brief	设置MPU6050加速度传感器满量程范围
 * @param	fsr	可取值：0,±2g;1,±4g;2,±8g;3,±16g
 * @retval	成功即返回SUCCEED，否则返回FAIL
*/
operateSta MPU_Set_Accel_Fsr(u8 fullScaleRange);

/**
 * @brief	设置MPU6050的数字低通滤波器
 * @param	lpf	数字低通滤波频率(Hz)
 * @retval	成功即返回SUCCEED，否则返回FAIL
*/
operateSta MPU_Set_LPF(u16 lpf);

/**
 * @brief	设置MPU6050的采样率(假定Fs=1KHz)
 * @param	rate	采样率 4~1000Hz
 * @retval	成功即返回SUCCEED，否则返回FAIL
*/
operateSta MPU_Set_Rate(u16 rate);

/**
 * @brief	得到温度值（带符号）
 * @retval	温度值(扩大了100倍)
*/
short MPU_Get_Temperature(void); //获取模块板载温度值

/**
 * @brief	得到陀螺仪值(原始值)
 * @param	gx,gy,gz	陀螺仪x,y,z轴的原始读数(带符号)
 * @retval	成功即返回SUCCEED，否则返回FAIL
*/
operateSta MPU_Get_Gyroscope(short* gx, short* gy, short* gz);

/**
 * @brief	得到加速度值(原始值)
 * @param	ax,ay,az	加速度计x,y,z轴的原始读数(带符号)
 * @retval	成功即返回SUCCEED，否则返回FAIL
*/
operateSta MPU_Get_Accelerometer(short* ax, short* ay, short* az);

/**
 * @brief	判断MPU6050地址是否为0x68
 * @retval	是即返回TRUE，否则返回FALSE
*/
boolean Is_MPU_ADDR_0x68(void);

/**
 * @brief	调用 inv_mpu.c 内的 mpu_dmp_init() 初始化DMP解算引擎来判断mpu是否存在
 * @retval	是即返回TRUE，否则返回FALSE
*/
boolean Is_MPU_Exist(void);	

/**
 * @brief	MPU6050初始化，调用了 Is_MPU_ADDR_0x68() 和 Is_MPU_Exist()
 * @retval	成功返回SUCCEED，否则返回FAIL
*/
operateSta MPU_Init(void);

/**
 * @brief	获取DMP解算后的全局变量pitch、roll、yaw
 * @param	degrees, MPU角度结构体指针
 * @retval	成功返回SUCCEED，否则返回FAIL
*/
operateSta Get_ThreeDegrees(void);

//////////////////////////////////////////////////////////////////////
//						   -- IIC相关函数 --			  			//
//////////////////////////////////////////////////////////////////////

/**
 * @breif	MPU6050写入一个字节
 * @param	reg		寄存器地址
 * @param	byte	待写入的字节
 * @retval	成功即返回SUCCEED，否则返回FAIL
*/
operateSta MPU_Write_Byte(u8 reg, u8 byte);

/**
 * @breif	MPU6050读取一个字节
 * @param	addr	器件地址
 * @param	reg		寄存器地址
 * @retval	读取到的字节
*/
u8 MPU_Read_Byte(u8 reg);

/**
 *@brief	IIC连续写
 *@param	addr:器件地址
 *@param	reg:要写入的寄存器地址
 *@param	len:要写入的长度
 *@param	buf:写入的数据存储区
 *@retval	返回值:0,正常。其他,错误代码
*/
u8 MPU_Write_Len(u8 addr, u8 reg, u8 len, u8* buf);

/**
 *@brief	IIC连续读
 *@param	addr:器件地址
 *@param	reg:要读取的寄存器地址
 *@param	len:要读取的长度
 *@param	buf:读取到的数据存储区
 *@retval	返回值:0,正常。其他,错误代码
*/
u8 MPU_Read_Len(u8 addr, u8 reg, u8 len, u8* buf);

#endif // !__MPU6050_H
