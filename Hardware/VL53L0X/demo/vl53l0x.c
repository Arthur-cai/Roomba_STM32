#include "vl53l0x.h"

VL53L0X_Dev_t left_vl53l0x_dev;	//左边VL53模块设备I2C数据参数
VL53L0X_Dev_t right_vl53l0x_dev;//右边VL53模块设备I2C数据参数

uint8_t AjustOK = 0;//校准标志位，如果进行了Vl53l0x_data_init()初始化校准数据，该标志置1

//VL53L0X各精度模式参数
//0：默认;1:高精度;2:长距离;3:高速
mode_data Mode_data[] = {
    {(FixPoint1616_t)(0.25*65536), 
	 (FixPoint1616_t)(18*65536),
	 33000,
	 14,
	 10},//默认
		
	{(FixPoint1616_t)(0.25*65536) ,
	 (FixPoint1616_t)(18*65536),
	 200000, 
	 14,
	 10},//高精度
		
    {(FixPoint1616_t)(0.1*65536) ,
	 (FixPoint1616_t)(60*65536),
	 33000,
	 18,
	 14},//长距离
	
    {(FixPoint1616_t)(0.25*65536) ,
	 (FixPoint1616_t)(32*65536),
	 20000,
	 14,
	 10},//高速
};

/**
 * @brief	API错误信息打印
*/
void print_pal_error(VL53L0X_Error Status) {
	char buf[VL53L0X_MAX_STRING_LENGTH];

	VL53L0X_GetPalErrorString(Status, buf);//根据Status状态获取错误信息字符串
	OLED_ShowString(0, 7 * 8 + 1, (u8*)buf, 8, 1);
}

/** 
 * @brief	配置VL53L0X设备I2C地址
*/
VL53L0X_Error vl53l0x_Addr_set(VL53L0X_Dev_t* dev, uint8_t newaddr) {
	uint16_t Id;
	uint8_t FinalAddress;
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	//暂存地址
	FinalAddress = newaddr;
	//新设备I2C地址与旧地址一致,直接退出
	if (FinalAddress == dev->I2cDevAddr) return VL53L0X_ERROR_NONE;
	//在进行第一个寄存器访问之前设置I2C标准模式(400Khz)
	Status = VL53L0X_WrByte(dev, 0x88, 0x00);
	if (Status != VL53L0X_ERROR_NONE) return Status;
	//尝试使用默认的0x52地址读取VL53L0X_REG_IDENTIFICATION_MODEL_ID寄存器，默认值为0xEEAA
	Status = VL53L0X_RdWord(dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
	if (Status != VL53L0X_ERROR_NONE) return Status;
	if (Id == 0xEEAA) {
		//设置设备新的I2C地址
		Status = VL53L0X_SetDeviceAddress(dev, FinalAddress);
		if (Status != VL53L0X_ERROR_NONE) return Status;
		//修改参数结构体的I2C地址
		dev->I2cDevAddr = FinalAddress;
		//检查新的I2C地址读写是否正常
		Status = VL53L0X_RdWord(dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
		if (Status != VL53L0X_ERROR_NONE) return Status;
	}
	return Status;
}

/**
 * @brief	初始化dev结构体
 * @param	ldev,rdev	设备I2C结构体
*/
void dev_init(VL53L0X_Dev_t* ldev, VL53L0X_Dev_t* rdev) {
	ldev->I2cDevAddr = 0x52;	//I2C地址(上电默认0x52)
	ldev->comms_type = 1;		//I2C通信模式
	ldev->comms_speed_khz = 400;//I2C通信速率

	rdev->I2cDevAddr = 0x52;	//I2C地址(上电默认0x52)
	rdev->comms_type = 1;       //I2C通信模式
	rdev->comms_speed_khz = 400;//I2C通信速率
}

/**
 * @brief	初始化Vl53l0x_data校准数据
*/
void Vl53l0x_data_init(void) {
	Vl53l0x_data.adjustok = 0xAA;
	Vl53l0x_data.isApertureSpads = 0;
	Vl53l0x_data.VhvSettings = 23;
	Vl53l0x_data.PhaseCal = 12;
	Vl53l0x_data.XTalkCalDistance = 0;
	Vl53l0x_data.XTalkCompensationRateMegaCps = 0;
	Vl53l0x_data.CalDistanceMilliMeter = 6553600;
	Vl53l0x_data.OffsetMicroMeter = 39000;
	Vl53l0x_data.refSpadCount = 5;
}

/**
 * @brief	左边VL53模块地址设定
 * @param	ldev	设备I2C结构体
 * @param	newaddr	新地址
 * @retval	Status错误状态
*/
VL53L0X_Error left_vl53_addr_set(VL53L0X_Dev_t* ldev, uint8_t newaddr) {
	LeftOrRightFlag = 0;	//全局左右模块IIC总线切换标志置位0
	return vl53l0x_Addr_set(ldev, newaddr);
}

/**
 * @brief	右边VL53模块地址设定
 * @param	rdev	设备I2C结构体
 * @param	newaddr	新地址
 * @retval	Status错误状态
*/
VL53L0X_Error right_vl53_addr_set(VL53L0X_Dev_t* rdev, uint8_t newaddr) {
	LeftOrRightFlag = 1;	//全局左右模块IIC总线切换标志置位1
	return vl53l0x_Addr_set(rdev, newaddr);
}

/**
 * @brief	复位VL53L0X
*/
void vl53l0x_reset(VL53L0X_Dev_t* dev) {
	uint8_t addr;
	addr = dev->I2cDevAddr;//保存设备原I2C地址 
	if (addr == LEFT_VL53L0X_Addr) {
		LEFT_VL53L0X_Xshut = 0; //失能VL53L0X
		delay_ms(30);
		LEFT_VL53L0X_Xshut = 1; //使能VL53L0X,让传感器处于工作(I2C地址会恢复默认0X52)
		delay_ms(30);
		dev->I2cDevAddr = 0x52;
		left_vl53_addr_set(dev, addr); //设置VL53L0X传感器原来上电前原I2C地址 
		VL53L0X_DataInit(dev);
	}
	else if (addr == RIGHT_VL53L0X_Addr) {
		RIGHT_VL53L0X_Xshut = 0; //失能VL53L0X
		delay_ms(30);
		RIGHT_VL53L0X_Xshut = 1; //使能VL53L0X,让传感器处于工作(I2C地址会恢复默认0X52)
		delay_ms(30);
		dev->I2cDevAddr = 0x52;
		right_vl53_addr_set(dev, addr); //设置VL53L0X传感器原来上电前原I2C地址 
		VL53L0X_DataInit(dev);
	}
}

/**
 * @brief	左边VL53模块初始化，调用了left_vl53_addr_set()
 * @param	ldev	设备I2C结构体
 * @retval	Status错误状态
*/
VL53L0X_Error LEFT_VL53L0X_Init(VL53L0X_Dev_t* ldev) {
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	LEFT_VL53L0X_Xshut = 0;//失能VL53L0X
	delay_ms(30);
	LEFT_VL53L0X_Xshut = 1;//使能VL53L0X
	delay_ms(30);
	Status = left_vl53_addr_set(ldev, LEFT_VL53L0X_Addr); //设置左边VL53L0X传感器I2C地址 0x56
	if (Status != VL53L0X_ERROR_NONE || (left_vl53l0x_dev.I2cDevAddr != LEFT_VL53L0X_Addr)) {
		OLED_ShowString(0, 6 * 8, "left addr fail ", 8, 1);	OLED_Refresh();
		return Status;
	}
	Status = VL53L0X_DataInit(ldev); //左边设备初始化
	if (Status != VL53L0X_ERROR_NONE) {
		OLED_ShowString(0, 6 * 8, "left init fail ", 8, 1);	OLED_Refresh();
		return Status;
	}
	delay_ms(2);
	return Status;
}

/**
 * @brief	右边VL53模块初始化，调用了right_vl53_addr_set()
 * @param	rdev	设备I2C结构体
 * @retval	Status错误状态
*/
VL53L0X_Error RIGHT_VL53L0X_Init(VL53L0X_Dev_t* rdev) {
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	RIGHT_VL53L0X_Xshut = 0;
	delay_ms(30);
	RIGHT_VL53L0X_Xshut = 1;
	delay_ms(30);
	Status = right_vl53_addr_set(rdev, RIGHT_VL53L0X_Addr); //设置VL53L0X传感器I2C地址 0x58
	if (Status != VL53L0X_ERROR_NONE || (right_vl53l0x_dev.I2cDevAddr != RIGHT_VL53L0X_Addr)) {
		OLED_ShowString(0, 6 * 8, "right addr fail", 8, 1);	OLED_Refresh();
		return Status;
	}
	Status = VL53L0X_DataInit(rdev);//设备初始化
	if (Status != VL53L0X_ERROR_NONE) {
		OLED_ShowString(0, 6 * 8, "right init fail", 8, 1);	OLED_Refresh();
		return Status;
	}
	delay_ms(2);
	return Status;
}

/**
 * @brief	初始化VL53L0X，调用了dev_init()，VL53L0X_IIC_Init()，
			LEFT_VL53L0X_Init()，RIGHT_VL53L0X_Init()，Vl53l0x_data_init()
*/
VL53L0X_Error VL53L0X_Init(VL53L0X_Dev_t* ldev, VL53L0X_Dev_t* rdev) {
	GPIO_InitTypeDef  GPIO_InitStructure;
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOF, ENABLE); //先使能外设IO时钟		PD7和PF14

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	           //左边VL53模块XSHUT端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //IO口速度为50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;	           //右边VL53模块XSHUT端口配置
	GPIO_Init(GPIOF, &GPIO_InitStructure);				   //根据设定参数初始化GPIO

	//初始化 left_vl53l0x_dev，right_vl53l0x_dev 设备I2C结构体数据
	dev_init(ldev, rdev);
	//初始化IIC总线
	VL53L0X_IIC_Init(); 
	//使能并配置左边的VL53
	Status = LEFT_VL53L0X_Init(ldev);
	if (Status != VL53L0X_ERROR_NONE) goto error;
	//使能并配置右边的VL53
	Status = RIGHT_VL53L0X_Init(rdev);
	if (Status != VL53L0X_ERROR_NONE) goto error;
	//初始化校准数据
	Vl53l0x_data_init();	
	//已校准
	if (Vl53l0x_data.adjustok == 0xAA) AjustOK = 1; 
	//未校准
	else AjustOK = 0;
	//mode = 2，设置为长距离模式
	vl53l0x_set_mode(&left_vl53l0x_dev, &right_vl53l0x_dev, 2); 

error:
	if (Status != VL53L0X_ERROR_NONE) {
		print_pal_error(Status);//打印错误信息
		return Status;
	}
	return Status;
}

/**
 * @brief	初始化普通模式，长距测量
*/
void VL53L0X_General_LongRange_Init(void) {
	VL53L0X_Init(&left_vl53l0x_dev, &right_vl53l0x_dev); //初始化VL53L0X
	vl53l0x_set_mode(&left_vl53l0x_dev, &right_vl53l0x_dev, 2); //mode = 2，设置为长距离模式
}

/**
 * @brief	开启普通模式，长距测量
*/
void VL53L0X_General_LongRange_Start(void) {
	VL53L0X_General(&left_vl53l0x_dev, &right_vl53l0x_dev);
}
