#include "vl53l0x_platform.h"

u8 LeftOrRightFlag = 0;	//左右模块IIC总线切换标志，0为左边VL53模块，1为右边VL53模块

/**
 * @brief	VL53L0X连续写数据
*/
VL53L0X_Error VL53L0X_WriteMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t* pdata, uint32_t count) {
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	int32_t status_int = 0;
	uint8_t deviceAddress;
	//写的字节数不能超过64
	if (count >= VL53L0X_MAX_I2C_XFER_SIZE) Status = VL53L0X_ERROR_INVALID_PARAMS;
	//获取地址
	deviceAddress = Dev->I2cDevAddr;
	if (deviceAddress == LEFT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 0)) { //初始地址，左边模块
		status_int = VL53L0X_write_multi(deviceAddress, index, pdata, count, CHOOSE_LEFT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
	}
	else if (deviceAddress == RIGHT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 1)) { //初始地址，右边模块
		status_int = VL53L0X_write_multi(deviceAddress, index, pdata, count, CHOOSE_RIGHT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
	}
	return Status;
}

/**
 * @brief	VL53L0X连续读数据
*/
VL53L0X_Error VL53L0X_ReadMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t* pdata, uint32_t count) {
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	int32_t status_int;
	uint8_t deviceAddress; 
	//读的字节数不能超过64
	if (count >= VL53L0X_MAX_I2C_XFER_SIZE) Status = VL53L0X_ERROR_INVALID_PARAMS;
	//获取地址
	deviceAddress = Dev->I2cDevAddr;
	if (deviceAddress == LEFT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 0)) { //初始地址，左边模块
		status_int = VL53L0X_read_multi(deviceAddress, index, pdata, count, CHOOSE_LEFT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
	}
	else if (deviceAddress == RIGHT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 1)) { //初始地址，右边模块
		status_int = VL53L0X_read_multi(deviceAddress, index, pdata, count, CHOOSE_RIGHT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
	}
	return Status;
}

/**
 * @brief	VL53L0X写1字节寄存器
*/
VL53L0X_Error VL53L0X_WrByte(VL53L0X_DEV Dev, uint8_t index, uint8_t data) {
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	int32_t status_int;
	uint8_t deviceAddress;
	//获取地址
	deviceAddress = Dev->I2cDevAddr;
	if (deviceAddress == LEFT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 0)) { //初始地址，左边模块
		status_int = VL53L0X_write_byte(deviceAddress, index, data, CHOOSE_LEFT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
	}
	else if (deviceAddress == RIGHT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 1)) { //初始地址，右边模块
		status_int = VL53L0X_write_byte(deviceAddress, index, data, CHOOSE_RIGHT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
	}
	return Status;
}

/**
 * @brief	VL53L0X写2字节寄存器
*/
VL53L0X_Error VL53L0X_WrWord(VL53L0X_DEV Dev, uint8_t index, uint16_t data) {
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	int32_t status_int;
	uint8_t deviceAddress;
	//获取地址
	deviceAddress = Dev->I2cDevAddr;
	if (deviceAddress == LEFT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 0)) { //初始地址，左边模块
		status_int = VL53L0X_write_word(deviceAddress, index, data, CHOOSE_LEFT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
	}
	else if (deviceAddress == RIGHT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 1)) { //初始地址，右边模块
		status_int = VL53L0X_write_word(deviceAddress, index, data, CHOOSE_RIGHT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
	}
	return Status;
}

/**
 * @brief	VL53L0X写4字节寄存器
*/
VL53L0X_Error VL53L0X_WrDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t data) {
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	int32_t status_int;
	uint8_t deviceAddress;
	//获取地址
	deviceAddress = Dev->I2cDevAddr;
	if (deviceAddress == LEFT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 0)) { //初始地址，左边模块
		status_int = VL53L0X_write_dword(deviceAddress, index, data, CHOOSE_LEFT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
	}
	else if (deviceAddress == RIGHT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 1)) { //初始地址，右边模块
		status_int = VL53L0X_write_dword(deviceAddress, index, data, CHOOSE_RIGHT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
	}
	return Status;
}

/**
 * @brief	VL53L0X威胁安全更新(读/修改/写)单字节寄存器
*/
VL53L0X_Error VL53L0X_UpdateByte(VL53L0X_DEV Dev, uint8_t index, uint8_t AndData, uint8_t OrData) {
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	int32_t status_int;
	uint8_t deviceAddress;
	uint8_t data;
	//获取地址
	deviceAddress = Dev->I2cDevAddr;
	if (deviceAddress == LEFT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 0)) { //初始地址，左边模块
		status_int = VL53L0X_read_byte(deviceAddress, index, &data, CHOOSE_LEFT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
		if (Status == VL53L0X_ERROR_NONE) {
			data = (data & AndData) | OrData;
			status_int = VL53L0X_write_byte(deviceAddress, index, data, CHOOSE_LEFT_FLAG);
			if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
		}
	}
	else if (deviceAddress == RIGHT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 1)) { //初始地址，右边模块
		status_int = VL53L0X_read_byte(deviceAddress, index, &data, CHOOSE_RIGHT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
		if (Status == VL53L0X_ERROR_NONE) {
			data = (data & AndData) | OrData;
			status_int = VL53L0X_write_byte(deviceAddress, index, data, CHOOSE_RIGHT_FLAG);
			if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
		}
	}
	return Status;
}

/**
 * @brief	VL53L0X读1字节寄存器
*/
VL53L0X_Error VL53L0X_RdByte(VL53L0X_DEV Dev, uint8_t index, uint8_t* data) {
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	int32_t status_int;
	uint8_t deviceAddress;
	//获取地址
	deviceAddress = Dev->I2cDevAddr;
	if (deviceAddress == LEFT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 0)) { //初始地址，左边的模块
		status_int = VL53L0X_read_byte(deviceAddress, index, data, CHOOSE_LEFT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
	}
	else if (deviceAddress == RIGHT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 1)) { //初始地址，右边的模块
		status_int = VL53L0X_read_byte(deviceAddress, index, data, CHOOSE_RIGHT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
	}
	return Status;
}

/**
 * @brief	VL53L0X读2字节寄存器
*/
VL53L0X_Error VL53L0X_RdWord(VL53L0X_DEV Dev, uint8_t index, uint16_t* data) {
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	int32_t status_int;
	uint8_t deviceAddress;
	//获取地址
	deviceAddress = Dev->I2cDevAddr;
	if (deviceAddress == LEFT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 0)) { //初始地址，左边的模块
		status_int = VL53L0X_read_word(deviceAddress, index, data, CHOOSE_LEFT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
	}
	else if (deviceAddress == RIGHT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 1)) { //初始地址，右边的模块
		status_int = VL53L0X_read_word(deviceAddress, index, data, CHOOSE_RIGHT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
	}
	return Status;
}

/**
 * @brief	VL53L0X读4字节寄存器
*/
VL53L0X_Error  VL53L0X_RdDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t* data) {
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	int32_t status_int;
	uint8_t deviceAddress;
	//获取地址
	deviceAddress = Dev->I2cDevAddr;
	if (deviceAddress == LEFT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 0)) { //初始地址，左边模块
		status_int = VL53L0X_read_dword(deviceAddress, index, data, CHOOSE_LEFT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
	}
	else if (deviceAddress == RIGHT_Addr || (deviceAddress == 0x52 && LeftOrRightFlag == 1)) { //初始地址，右边模块
		status_int = VL53L0X_read_dword(deviceAddress, index, data, CHOOSE_RIGHT_FLAG);
		if (status_int != 0) Status = VL53L0X_ERROR_CONTROL_INTERFACE;
	}
	return Status;
}

/**
 * @brief	VL53L0X底层延时函数
*/
#define VL53L0X_POLLINGDELAY_LOOPNB  250
VL53L0X_Error VL53L0X_PollingDelay(VL53L0X_DEV Dev) {
	VL53L0X_Error status = VL53L0X_ERROR_NONE;
	volatile uint32_t i;

	for (i = 0; i < VL53L0X_POLLINGDELAY_LOOPNB; i++) {
		//Do nothing
	}
	return status;
}

