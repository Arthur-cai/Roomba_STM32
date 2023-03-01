#include "vl53l0x_gen.h"

VL53L0X_RangingMeasurementData_t left_vl53l0x_data;//测距测量结构体
VL53L0X_RangingMeasurementData_t right_vl53l0x_data;

/**
 * @brief	vl53l0x测量模式配置
*/
VL53L0X_Error vl53l0x_set_mode(VL53L0X_Dev_t* ldev, VL53L0X_Dev_t* rdev, u8 mode) {

	VL53L0X_Error status = VL53L0X_ERROR_NONE;
	uint8_t VhvSettings;
	uint8_t PhaseCal;
	uint32_t refSpadCount;
	uint8_t isApertureSpads;

	vl53l0x_reset(ldev);//复位vl53l0x(频繁切换工作模式容易导致采集距离数据不准，需加上这一代码) 
	vl53l0x_reset(rdev);//复位vl53l0x(频繁切换工作模式容易导致采集距离数据不准，需加上这一代码) 
	status = VL53L0X_StaticInit(ldev);
	status = VL53L0X_StaticInit(rdev);

	if (AjustOK != 0) { //已校准好了,写入校准值
		status = VL53L0X_SetReferenceSpads(ldev, Vl53l0x_data.refSpadCount, Vl53l0x_data.isApertureSpads);//设定Spads校准值 
		if (status != VL53L0X_ERROR_NONE) {
			//LED0 = 0; 
			goto error;
		}
		delay_ms(2);
		status = VL53L0X_SetRefCalibration(ldev, Vl53l0x_data.VhvSettings, Vl53l0x_data.PhaseCal);//设定Ref校准值 
		if (status != VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
		status = VL53L0X_SetOffsetCalibrationDataMicroMeter(ldev, Vl53l0x_data.OffsetMicroMeter);//设定偏移校准值 
		if (status != VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
		status = VL53L0X_SetXTalkCompensationRateMegaCps(ldev, Vl53l0x_data.XTalkCompensationRateMegaCps);//设定串扰校准值
		if (status != VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);

		status = VL53L0X_SetReferenceSpads(rdev, Vl53l0x_data.refSpadCount, Vl53l0x_data.isApertureSpads);//设定Spads校准值 
		if (status != VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
		status = VL53L0X_SetRefCalibration(rdev, Vl53l0x_data.VhvSettings, Vl53l0x_data.PhaseCal);//设定Ref校准值 
		if (status != VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
		status = VL53L0X_SetOffsetCalibrationDataMicroMeter(rdev, Vl53l0x_data.OffsetMicroMeter);//设定偏移校准值 
		if (status != VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
		status = VL53L0X_SetXTalkCompensationRateMegaCps(rdev, Vl53l0x_data.XTalkCompensationRateMegaCps);//设定串扰校准值
		if (status != VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);

	} else {
		status = VL53L0X_PerformRefCalibration(ldev, &VhvSettings, &PhaseCal);//Ref参考校准
		status = VL53L0X_PerformRefCalibration(rdev, &VhvSettings, &PhaseCal);//Ref参考校准
		if (status != VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
		status = VL53L0X_PerformRefSpadManagement(ldev, &refSpadCount, &isApertureSpads);//执行参考SPAD管理 
		status = VL53L0X_PerformRefSpadManagement(rdev, &refSpadCount, &isApertureSpads);//执行参考SPAD管理 
		if (status != VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
	}
	status = VL53L0X_SetDeviceMode(ldev, VL53L0X_DEVICEMODE_SINGLE_RANGING);//使能单次测量模式 
	status = VL53L0X_SetDeviceMode(rdev, VL53L0X_DEVICEMODE_SINGLE_RANGING);//使能单次测量模式 
	if (status != VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status = VL53L0X_SetLimitCheckEnable(ldev, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);//使能SIGMA范围检查 
	status = VL53L0X_SetLimitCheckEnable(rdev, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);//使能SIGMA范围检查 
	if (status != VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status = VL53L0X_SetLimitCheckEnable(ldev, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);//使能信号速率范围检查 
	status = VL53L0X_SetLimitCheckEnable(rdev, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);//使能信号速率范围检查 
	if (status != VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status = VL53L0X_SetLimitCheckValue(ldev, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, Mode_data[mode].sigmaLimit);//设定SIGMA范围 
	status = VL53L0X_SetLimitCheckValue(rdev, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, Mode_data[mode].sigmaLimit);//设定SIGMA范围 
	if (status != VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status = VL53L0X_SetLimitCheckValue(ldev, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, Mode_data[mode].signalLimit);//设定信号速率范围范围 
	status = VL53L0X_SetLimitCheckValue(rdev, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, Mode_data[mode].signalLimit);//设定信号速率范围范围 
	if (status != VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(ldev, Mode_data[mode].timingBudget);//设定完整测距最长时间 
	status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(rdev, Mode_data[mode].timingBudget);//设定完整测距最长时间 
	if (status != VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status = VL53L0X_SetVcselPulsePeriod(ldev, VL53L0X_VCSEL_PERIOD_PRE_RANGE, Mode_data[mode].preRangeVcselPeriod);//设定VCSEL脉冲周期 
	status = VL53L0X_SetVcselPulsePeriod(rdev, VL53L0X_VCSEL_PERIOD_PRE_RANGE, Mode_data[mode].preRangeVcselPeriod);//设定VCSEL脉冲周期 
	if (status != VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	status = VL53L0X_SetVcselPulsePeriod(ldev, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, Mode_data[mode].finalRangeVcselPeriod);//设定VCSEL脉冲周期范围
	status = VL53L0X_SetVcselPulsePeriod(rdev, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, Mode_data[mode].finalRangeVcselPeriod);//设定VCSEL脉冲周期范围

error://错误信息
	if (status != VL53L0X_ERROR_NONE) {
		print_pal_error(status);
		return status;
	}
	return status;
}

/**
 * @brief	vl53l0x单次距离测量函数
*/
VL53L0X_Error vl53l0x_start_single_test(VL53L0X_Dev_t* ldev, VL53L0X_RangingMeasurementData_t* ldata, 
										VL53L0X_Dev_t* rdev, VL53L0X_RangingMeasurementData_t* rdata) {
	VL53L0X_Error status = VL53L0X_ERROR_NONE;
	
	status = VL53L0X_PerformSingleRangingMeasurement(ldev, ldata);//执行单次测距并获取测距测量数据 
	if (status != VL53L0X_ERROR_NONE) return status;

	status = VL53L0X_PerformSingleRangingMeasurement(rdev, rdata);//执行单次测距并获取测距测量数据 
	if (status != VL53L0X_ERROR_NONE) return status;

	//left_Distance_data = ldata->RangeMilliMeter;//保存最近一次测距测量数据
	//right_Distance_data = rdata->RangeMilliMeter;//保存最近一次测距测量数据
	return status;
}

/**
 * @brief	启动普通测量
*/
VL53L0X_Error VL53L0X_General(VL53L0X_Dev_t* ldev, VL53L0X_Dev_t* rdev) {
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;//工作状态

	Status = vl53l0x_start_single_test(ldev, &left_vl53l0x_data, rdev, &right_vl53l0x_data); //执行一次测量

	if (Status != VL53L0X_ERROR_NONE) return Status;
	return Status;
}
