#include "rtc.h"

//////// -- <全局变量> -- ////////
RTC_TIME time; //RTC时间全局变量
//////// -- <全局变量> -- ////////

//月份数据表
const u8 weekTable[12] = {0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表  
const u8 monTable[12] = {31,28,31,30,31,30,31,31,30,31,30,31}; //平年的月份日期表

/**
 * @brief 首次启用RTC的设置, 在RTC_Configuration()中被调用
 */
static void RTC_FirstConfig(void) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);//启用PWR和BKP的时钟（from APB1）
	PWR_BackupAccessCmd(ENABLE);//后备域解锁
	BKP_DeInit();//备份寄存器模块复位
	RCC_LSEConfig(RCC_LSE_ON);//外部低速32.768KHZ晶振开启   
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);//等待外部低速晶振就绪  
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//RTC时钟源配置成LSE（外部低速晶振32.768KHZ）    
	RCC_RTCCLKCmd(ENABLE);//使能RTCCLK
	RTC_WaitForSynchro();//开启后需要等待APB1时钟与RTC时钟同步，才能读写寄存器    
	RTC_WaitForLastTask();//读写寄存器前，要确定上一个操作已经结束
	RTC_SetPrescaler(32767);//设置RTC分频器，使RTC时钟为1Hz,RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)   
	RTC_WaitForLastTask();//等待寄存器写入完成	
	//当不使用RTC秒中断，可以屏蔽下面2条
	/*RTC_ITConfig(RTC_IT_SEC, ENABLE);//使能秒中断
	RTC_WaitForLastTask();//等待写入完成*/
}

/**
 * @brief 实时时钟初始化，调用了RTC_FirstConfig()
 */
void RTC_Configuration(void) {
	//在BKP的后备寄存器1中，存了一个特殊字符0xA5A5
	//第一次上电或后备电源掉电后，该寄存器数据丢失，表明RTC数据丢失，需要重新配置
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5) {//判断寄存数据是否丢失       
		RTC_FirstConfig();//重新配置RTC        
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);//配置完成后，向后备寄存器中写特殊字符0xA5A5
	}
	else {
		//若后备寄存器没有掉电，则无需重新配置RTC
		//这里我们可以利用RCC_GetFlagStatus()函数查看本次复位类型
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {
			//这是上电复位
		}
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) {
			//这是外部RST管脚复位
		}
		RCC_ClearFlag();//清除RCC中复位标志

		//虽然RTC模块不需要重新配置，且掉电后依靠后备电池依然运行
		//但是每次上电后，还是要使能RTCCLK
		RCC_RTCCLKCmd(ENABLE);//使能RTCCLK        
		RTC_WaitForSynchro();//等待RTC时钟与APB1时钟同步

		//当不使用RTC秒中断，可以屏蔽下面2条
		/*RTC_ITConfig(RTC_IT_SEC, ENABLE);//使能秒中断
		RTC_WaitForLastTask();//等待写入完成*/
	}
#ifdef RTCClockOutput_Enable   
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	BKP_TamperPinCmd(DISABLE);
	BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
#endif
}

// /**
//  * @brief RTC秒中断服务程序
// */
// void RTC_IRQHandler(void) { //RTC时钟1秒触发中断函数（名称固定不可修改）
// 	if (RTC_GetITStatus(RTC_IT_SEC) != RESET) {
// 		//如果触发了秒中断，就进行操作
// 	}
// 	RTC_ClearITPendingBit(RTC_IT_SEC);  //清除中断标志
// 	RTC_WaitForLastTask();
// }

// void RTCAlarm_IRQHandler(void) {	//闹钟中断处理（启用时必须调高其优先级）
// 	if (RTC_GetITStatus(RTC_IT_ALR) != RESET) {

// 	}
// 	RTC_ClearITPendingBit(RTC_IT_ALR); //清除中断标志
// 	RTC_WaitForLastTask();
// }

/**
 * @brief 判断是否为闰年
	月份  	   1  2  3  4  5  6  7  8  9  10 11 12
	闰年天数   31 29 31 30 31 30 31 31 30 31 30 31
	非闰年天数 31 28 31 30 31 30 31 31 30 31 30 31
	
 * @param year 输入的年份
 * @retval 是闰年返回TRUE，不是则返回FALSE
 */
boolean IsLeapYear(u16 year) {
	if (year % 4 == 0) { //必须能被4整除
		if (year % 100 == 0) {
			if (year % 400 == 0) return TRUE;//如果以00结尾,还要能被400整除          
			else return FALSE;
		}
		else return TRUE;
	}
	else return FALSE;
}

/**
 * @brief 自定义设定时间，1970~2099年有效
	利用RTC_SetCounter(secSum)函数，把年月日时分秒的总秒数写入来实现设定时间
	
 * @param syear 设定的年
 * @param smon 设定的月
 * @param sday 设定的日
 * @param hour 设定的小时
 * @param min 设定的分钟
 * @param sec 设定的秒
 * @retval 成功返回1，失败返回0
 */
operateSta RTC_SetTime(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec) {
	u16 i;
	//秒数之和
	u32 secSum = 0;
	//syear范围1970-2099，此处设置范围为2000-2099 
	if (syear < 2000 || syear > 2099) return FAIL;

	for (i = 1970; i < syear; i++) { //把所有年份的秒相加
		if (IsLeapYear(i)) secSum += 31622400;	//闰年的秒钟数
		else secSum += 31536000;                //平年的秒钟数
	}
	smon -= 1;
	for (i = 0; i < smon; i++) { //把直到设定月份之前所有月的秒数相加
		//月份秒数相加，一天86400秒，月天数*一天秒数
		secSum += (u32)monTable[i] * 86400;
		//如果是闰年且为2月份，需要多增加一天的秒数    
		if (IsLeapYear(syear) && i == 1) secSum += 86400;
	}
	secSum += (u32)(sday - 1) * 86400;	//把直到设定日之前的天的秒数相加
	secSum += (u32)hour * 3600;	//小时秒钟数
	secSum += (u32)min * 60;      //分钟秒钟数
	secSum += sec;		//最后的秒钟加上去

	RTC_FirstConfig(); //重新初始化时钟
	BKP_WriteBackupRegister(BKP_DR1, 0xA5A5); //配置完成后，向后备寄存器中写特殊字符0xA5A5
	RTC_SetCounter(secSum); //写入累加好的总秒数
	RTC_WaitForLastTask(); //等待写入完成
	return SUCCEED;
}

/**
 * @brief 读取当前时间
 * @retval 成功返回SUCCEED
 */
operateSta RTC_GetTime(void) {
	static u16 dayCount = 0;
	u32 timeCount = 0;

	u32 dayTemp = 0; //日期缓存
	u16 yearTemp = 0; //年份缓存
	u8	monTemp = 0; //月份缓存
	u32	secTemp = 0; //秒缓存

	//调用RTC_GetCounter()获取总秒数计数
	timeCount = RTC_GetCounter();
	//除以一天的秒数，换算成天数
	dayTemp = timeCount / 86400;

	if (dayCount != dayTemp) {//超过一天了
		dayCount = dayTemp;
		yearTemp = 1970;  //yearTemp从1970年开始
		//当日期缓存大于365天时，不断换算成年数
		while (dayTemp >= 365) {
			//如果yearTemp年份缓存是闰年
			if (IsLeapYear(yearTemp)) {
				if (dayTemp >= 366) dayTemp -= 366;
				else { yearTemp++; break; }
			}
			//平年
			else dayTemp -= 365;
			yearTemp++;
		}
		(&time)->ryear = yearTemp; //得到年份

		//当剩余的天数超过一个月时
		while (dayTemp >= 28) {
			//如果是闰年且为2月份
			if (IsLeapYear((&time)->ryear) && monTemp == 1) {
				if (dayTemp >= 29) dayTemp -= 29;
				else break;
			}
			else {
				if (dayTemp >= monTable[monTemp]) dayTemp -= monTable[monTemp];//平年
				else break;
			}
			monTemp++;
		}
		(&time)->rmon = monTemp + 1; //得到月份
		(&time)->rday = dayTemp + 1; //得到日期
	}
	secTemp = timeCount % 86400; //得到秒数      
	(&time)->rhour = secTemp / 3600;     //小时
	(&time)->rmin = (secTemp % 3600) / 60; //分钟
	(&time)->rsec = (secTemp % 3600) % 60; //秒钟
	(&time)->rweek = RTC_GetWeek((&time)->ryear, (&time)->rmon, (&time)->rday); //获取星期  
	return SUCCEED;
}

/** 
 * @brief  获取星期，按年月日计算星期，只允许1901-2099年。在RTC_GetTime()中被调用    
 * @retval 返回星期值
 */
u8 RTC_GetWeek(u16 year, u8 month, u8 day) {
	u16 dayTemp2;
	u8 yearH, yearL;
	yearH = year / 100;
	yearL = year % 100;
	// 如果为21世纪,年份数加100 
	if (yearH > 19) yearL += 100;
	// 所过闰年数只算1900年之后的 
	dayTemp2 = yearL + yearL / 4;
	dayTemp2 = dayTemp2 % 7;
	dayTemp2 = dayTemp2 + day + weekTable[month - 1];
	if (yearL % 4 == 0 && month < 3)dayTemp2--;
	return(dayTemp2 % 7); //返回星期值
}

/**
 * @brief 通过串口设置时间
*/
void RTC_SetTimeByUSART(void) {
	u16 len, syear;
	u8 smon, sday, shour, smin, ssec;

	if (USART1_RX_STA & 0x8000) {
		len = USART1_RX_STA & 0x3fff;
		//20221124223530
		if (len == 14) {
			syear = (USART1_RX_BUF[0] - 0x30) * 1000 + (USART1_RX_BUF[1] - 0x30) * 100 + \
				(USART1_RX_BUF[2] - 0x30) * 10 + (USART1_RX_BUF[3] - 0x30);
			smon = (USART1_RX_BUF[4] - 0x30) * 10 + (USART1_RX_BUF[5] - 0x30);
			sday = (USART1_RX_BUF[6] - 0x30) * 10 + (USART1_RX_BUF[7] - 0x30);
			shour = (USART1_RX_BUF[8] - 0x30) * 10 + (USART1_RX_BUF[9] - 0x30);
			smin = (USART1_RX_BUF[10] - 0x30) * 10 + (USART1_RX_BUF[11] - 0x30);
			ssec = (USART1_RX_BUF[12] - 0x30) * 10 + (USART1_RX_BUF[13] - 0x30);
			if (RTC_SetTime(syear, smon, sday, shour, smin, ssec)) printf("成功 \r\n\r\n");
			else printf("失败 \r\n\r\n");
		}
		USART1_RX_STA = 0;
	}
}
