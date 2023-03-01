#ifndef __VL53L0X_H
#define __VL53L0X_H

#include "../core/vl53l0x_api.h"
#include "../platform/vl53l0x_platform.h"
#include "vl53l0x_gen.h"
#include "vl53l0x_cali.h"
//#include "vl53l0x_it.h"
#include "../../../Basic/sys/sys.h"
//#include "24cxx.h" 
#include "../../OLED2420/oled2420.h"
#include "../../KEY/key.h"
#include "../../../Basic/delay/delay.h"
//#include "../../LED/led.h"
#include <string.h>

//VL53L0X�������ϵ�Ĭ��IIC��ַΪ0X52(���������λ)
#define LEFT_VL53L0X_Addr (u8)0x56
#define RIGHT_VL53L0X_Addr (u8)0x58

//����Xshut��ƽ,�Ӷ�ʹ��VL53L0X���� 1:ʹ�� 0:�ر�
#define RIGHT_VL53L0X_Xshut PFout(14)	
#define LEFT_VL53L0X_Xshut PDout(7)

//ʹ��2.8V IO��ƽģʽ
#define USE_I2C_2V8  1

//����ģʽ
#define Default_Mode   0// Ĭ��
#define HIGH_ACCURACY  1//�߾���
#define LONG_RANGE     2//������
#define HIGH_SPEED     3//����

//vl53l0xģʽ���ò�����
typedef __packed struct {
	FixPoint1616_t signalLimit;    //Signal������ֵ 
	FixPoint1616_t sigmaLimit;     //Sigmal������ֵ
	uint32_t timingBudget;         //����ʱ������
	uint8_t preRangeVcselPeriod;   //VCSEL��������
	uint8_t finalRangeVcselPeriod; //VCSEL�������ڷ�Χ
}mode_data;

extern mode_data Mode_data[];
extern uint8_t AjustOK;	//ȫ��У׼��־λ

/**
 * @brief	API������Ϣ��ӡ
 * @param	Status�����鿴VL53L0X_Error�����Ķ���
*/
void print_pal_error(VL53L0X_Error Status);

///**
// * @brief	ģʽ�ַ�����ʾ
// * @param	mode	0-Ĭ��;1-�߾���;2-������;3-����
// * @param	buf		�ַ���������
//*/
//void mode_string(u8 mode, char* buf);

/**
 * @brief	����VL53L0X�豸I2C��ַ
 * @param	dev		�豸I2C�����ṹ��
 * @param	newaddr	�豸��I2C��ַ
 * @retval	Status����״̬
*/
VL53L0X_Error vl53l0x_Addr_set(VL53L0X_Dev_t* dev, uint8_t newaddr);

/**
 * @brief	��λVL53L0X
 * @param	dev	�豸I2C�ṹ��
*/
void vl53l0x_reset(VL53L0X_Dev_t* dev);

/**
 * @brief	��ʼ��VL53L0X��������dev_init()��VL53L0X_IIC_Init()��
			LEFT_VL53L0X_Init()��RIGHT_VL53L0X_Init()��Vl53l0x_data_init()
 * @param	ldev,rdev	�豸I2C�ṹ��
 * @retval	Status����״̬
*/
VL53L0X_Error VL53L0X_Init(VL53L0X_Dev_t* ldev, VL53L0X_Dev_t* rdev);

/**
 * @brief	������ͨģʽ���������
*/
void VL53L0X_General_LongRange_Init(void);

/**
 * @brief	������ͨģʽ���������
*/
void VL53L0X_General_LongRange_Start(void);

///**
// * @brief	���˵�����
//*/
//void vl53l0x_mtest_ui(void);
//
///**
// * @brief	VL53L0X�����Գ���
//*/
//void vl53l0x_test(void);

#endif
