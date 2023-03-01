#include "vl53l0x.h"

VL53L0X_Dev_t left_vl53l0x_dev;	//���VL53ģ���豸I2C���ݲ���
VL53L0X_Dev_t right_vl53l0x_dev;//�ұ�VL53ģ���豸I2C���ݲ���

uint8_t AjustOK = 0;//У׼��־λ�����������Vl53l0x_data_init()��ʼ��У׼���ݣ��ñ�־��1

//VL53L0X������ģʽ����
//0��Ĭ��;1:�߾���;2:������;3:����
mode_data Mode_data[] = {
    {(FixPoint1616_t)(0.25*65536), 
	 (FixPoint1616_t)(18*65536),
	 33000,
	 14,
	 10},//Ĭ��
		
	{(FixPoint1616_t)(0.25*65536) ,
	 (FixPoint1616_t)(18*65536),
	 200000, 
	 14,
	 10},//�߾���
		
    {(FixPoint1616_t)(0.1*65536) ,
	 (FixPoint1616_t)(60*65536),
	 33000,
	 18,
	 14},//������
	
    {(FixPoint1616_t)(0.25*65536) ,
	 (FixPoint1616_t)(32*65536),
	 20000,
	 14,
	 10},//����
};

/**
 * @brief	API������Ϣ��ӡ
*/
void print_pal_error(VL53L0X_Error Status) {
	char buf[VL53L0X_MAX_STRING_LENGTH];

	VL53L0X_GetPalErrorString(Status, buf);//����Status״̬��ȡ������Ϣ�ַ���
	OLED_ShowString(0, 7 * 8 + 1, (u8*)buf, 8, 1);
}

/** 
 * @brief	����VL53L0X�豸I2C��ַ
*/
VL53L0X_Error vl53l0x_Addr_set(VL53L0X_Dev_t* dev, uint8_t newaddr) {
	uint16_t Id;
	uint8_t FinalAddress;
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	//�ݴ��ַ
	FinalAddress = newaddr;
	//���豸I2C��ַ��ɵ�ַһ��,ֱ���˳�
	if (FinalAddress == dev->I2cDevAddr) return VL53L0X_ERROR_NONE;
	//�ڽ��е�һ���Ĵ�������֮ǰ����I2C��׼ģʽ(400Khz)
	Status = VL53L0X_WrByte(dev, 0x88, 0x00);
	if (Status != VL53L0X_ERROR_NONE) return Status;
	//����ʹ��Ĭ�ϵ�0x52��ַ��ȡVL53L0X_REG_IDENTIFICATION_MODEL_ID�Ĵ�����Ĭ��ֵΪ0xEEAA
	Status = VL53L0X_RdWord(dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
	if (Status != VL53L0X_ERROR_NONE) return Status;
	if (Id == 0xEEAA) {
		//�����豸�µ�I2C��ַ
		Status = VL53L0X_SetDeviceAddress(dev, FinalAddress);
		if (Status != VL53L0X_ERROR_NONE) return Status;
		//�޸Ĳ����ṹ���I2C��ַ
		dev->I2cDevAddr = FinalAddress;
		//����µ�I2C��ַ��д�Ƿ�����
		Status = VL53L0X_RdWord(dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
		if (Status != VL53L0X_ERROR_NONE) return Status;
	}
	return Status;
}

/**
 * @brief	��ʼ��dev�ṹ��
 * @param	ldev,rdev	�豸I2C�ṹ��
*/
void dev_init(VL53L0X_Dev_t* ldev, VL53L0X_Dev_t* rdev) {
	ldev->I2cDevAddr = 0x52;	//I2C��ַ(�ϵ�Ĭ��0x52)
	ldev->comms_type = 1;		//I2Cͨ��ģʽ
	ldev->comms_speed_khz = 400;//I2Cͨ������

	rdev->I2cDevAddr = 0x52;	//I2C��ַ(�ϵ�Ĭ��0x52)
	rdev->comms_type = 1;       //I2Cͨ��ģʽ
	rdev->comms_speed_khz = 400;//I2Cͨ������
}

/**
 * @brief	��ʼ��Vl53l0x_dataУ׼����
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
 * @brief	���VL53ģ���ַ�趨
 * @param	ldev	�豸I2C�ṹ��
 * @param	newaddr	�µ�ַ
 * @retval	Status����״̬
*/
VL53L0X_Error left_vl53_addr_set(VL53L0X_Dev_t* ldev, uint8_t newaddr) {
	LeftOrRightFlag = 0;	//ȫ������ģ��IIC�����л���־��λ0
	return vl53l0x_Addr_set(ldev, newaddr);
}

/**
 * @brief	�ұ�VL53ģ���ַ�趨
 * @param	rdev	�豸I2C�ṹ��
 * @param	newaddr	�µ�ַ
 * @retval	Status����״̬
*/
VL53L0X_Error right_vl53_addr_set(VL53L0X_Dev_t* rdev, uint8_t newaddr) {
	LeftOrRightFlag = 1;	//ȫ������ģ��IIC�����л���־��λ1
	return vl53l0x_Addr_set(rdev, newaddr);
}

/**
 * @brief	��λVL53L0X
*/
void vl53l0x_reset(VL53L0X_Dev_t* dev) {
	uint8_t addr;
	addr = dev->I2cDevAddr;//�����豸ԭI2C��ַ 
	if (addr == LEFT_VL53L0X_Addr) {
		LEFT_VL53L0X_Xshut = 0; //ʧ��VL53L0X
		delay_ms(30);
		LEFT_VL53L0X_Xshut = 1; //ʹ��VL53L0X,�ô��������ڹ���(I2C��ַ��ָ�Ĭ��0X52)
		delay_ms(30);
		dev->I2cDevAddr = 0x52;
		left_vl53_addr_set(dev, addr); //����VL53L0X������ԭ���ϵ�ǰԭI2C��ַ 
		VL53L0X_DataInit(dev);
	}
	else if (addr == RIGHT_VL53L0X_Addr) {
		RIGHT_VL53L0X_Xshut = 0; //ʧ��VL53L0X
		delay_ms(30);
		RIGHT_VL53L0X_Xshut = 1; //ʹ��VL53L0X,�ô��������ڹ���(I2C��ַ��ָ�Ĭ��0X52)
		delay_ms(30);
		dev->I2cDevAddr = 0x52;
		right_vl53_addr_set(dev, addr); //����VL53L0X������ԭ���ϵ�ǰԭI2C��ַ 
		VL53L0X_DataInit(dev);
	}
}

/**
 * @brief	���VL53ģ���ʼ����������left_vl53_addr_set()
 * @param	ldev	�豸I2C�ṹ��
 * @retval	Status����״̬
*/
VL53L0X_Error LEFT_VL53L0X_Init(VL53L0X_Dev_t* ldev) {
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	LEFT_VL53L0X_Xshut = 0;//ʧ��VL53L0X
	delay_ms(30);
	LEFT_VL53L0X_Xshut = 1;//ʹ��VL53L0X
	delay_ms(30);
	Status = left_vl53_addr_set(ldev, LEFT_VL53L0X_Addr); //�������VL53L0X������I2C��ַ 0x56
	if (Status != VL53L0X_ERROR_NONE || (left_vl53l0x_dev.I2cDevAddr != LEFT_VL53L0X_Addr)) {
		OLED_ShowString(0, 6 * 8, "left addr fail ", 8, 1);	OLED_Refresh();
		return Status;
	}
	Status = VL53L0X_DataInit(ldev); //����豸��ʼ��
	if (Status != VL53L0X_ERROR_NONE) {
		OLED_ShowString(0, 6 * 8, "left init fail ", 8, 1);	OLED_Refresh();
		return Status;
	}
	delay_ms(2);
	return Status;
}

/**
 * @brief	�ұ�VL53ģ���ʼ����������right_vl53_addr_set()
 * @param	rdev	�豸I2C�ṹ��
 * @retval	Status����״̬
*/
VL53L0X_Error RIGHT_VL53L0X_Init(VL53L0X_Dev_t* rdev) {
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	RIGHT_VL53L0X_Xshut = 0;
	delay_ms(30);
	RIGHT_VL53L0X_Xshut = 1;
	delay_ms(30);
	Status = right_vl53_addr_set(rdev, RIGHT_VL53L0X_Addr); //����VL53L0X������I2C��ַ 0x58
	if (Status != VL53L0X_ERROR_NONE || (right_vl53l0x_dev.I2cDevAddr != RIGHT_VL53L0X_Addr)) {
		OLED_ShowString(0, 6 * 8, "right addr fail", 8, 1);	OLED_Refresh();
		return Status;
	}
	Status = VL53L0X_DataInit(rdev);//�豸��ʼ��
	if (Status != VL53L0X_ERROR_NONE) {
		OLED_ShowString(0, 6 * 8, "right init fail", 8, 1);	OLED_Refresh();
		return Status;
	}
	delay_ms(2);
	return Status;
}

/**
 * @brief	��ʼ��VL53L0X��������dev_init()��VL53L0X_IIC_Init()��
			LEFT_VL53L0X_Init()��RIGHT_VL53L0X_Init()��Vl53l0x_data_init()
*/
VL53L0X_Error VL53L0X_Init(VL53L0X_Dev_t* ldev, VL53L0X_Dev_t* rdev) {
	GPIO_InitTypeDef  GPIO_InitStructure;
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOF, ENABLE); //��ʹ������IOʱ��		PD7��PF14

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	           //���VL53ģ��XSHUT�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;	           //�ұ�VL53ģ��XSHUT�˿�����
	GPIO_Init(GPIOF, &GPIO_InitStructure);				   //�����趨������ʼ��GPIO

	//��ʼ�� left_vl53l0x_dev��right_vl53l0x_dev �豸I2C�ṹ������
	dev_init(ldev, rdev);
	//��ʼ��IIC����
	VL53L0X_IIC_Init(); 
	//ʹ�ܲ�������ߵ�VL53
	Status = LEFT_VL53L0X_Init(ldev);
	if (Status != VL53L0X_ERROR_NONE) goto error;
	//ʹ�ܲ������ұߵ�VL53
	Status = RIGHT_VL53L0X_Init(rdev);
	if (Status != VL53L0X_ERROR_NONE) goto error;
	//��ʼ��У׼����
	Vl53l0x_data_init();	
	//��У׼
	if (Vl53l0x_data.adjustok == 0xAA) AjustOK = 1; 
	//δУ׼
	else AjustOK = 0;
	//mode = 2������Ϊ������ģʽ
	vl53l0x_set_mode(&left_vl53l0x_dev, &right_vl53l0x_dev, 2); 

error:
	if (Status != VL53L0X_ERROR_NONE) {
		print_pal_error(Status);//��ӡ������Ϣ
		return Status;
	}
	return Status;
}

/**
 * @brief	��ʼ����ͨģʽ���������
*/
void VL53L0X_General_LongRange_Init(void) {
	VL53L0X_Init(&left_vl53l0x_dev, &right_vl53l0x_dev); //��ʼ��VL53L0X
	vl53l0x_set_mode(&left_vl53l0x_dev, &right_vl53l0x_dev, 2); //mode = 2������Ϊ������ģʽ
}

/**
 * @brief	������ͨģʽ���������
*/
void VL53L0X_General_LongRange_Start(void) {
	VL53L0X_General(&left_vl53l0x_dev, &right_vl53l0x_dev);
}
