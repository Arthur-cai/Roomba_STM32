#ifndef __RTC_H
#define __RTC_H	 
#include "../../Basic/sys/sys.h"
#include "../../Basic/usart/usart.h"

//RTC时间数值结构体
typedef struct _rtc_time_struct {
	u16 ryear;	//年
	u8 rmon;	//月
	u8 rday;	//日
	u8 rhour;	//时
	u8 rmin;	//分
	u8 rsec;	//秒
	u8 rweek;	//周
}RTC_TIME;
extern RTC_TIME time; //RTC时间结构体

/**
 * @brief 实时时钟初始化，调用了RTC_FirstConfig()
 */
void RTC_Configuration(void);

/**
 * @brief 判断是否为闰年
	月份  	   1  2  3  4  5  6  7  8  9  10 11 12
	闰年天数   31 29 31 30 31 30 31 31 30 31 30 31
	非闰年天数 31 28 31 30 31 30 31 31 30 31 30 31
	
 * @param year 输入的年份
 * @retval 是闰年返回TRUE，不是则返回FALSE
 */
boolean IsLeapYear(u16 year);

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
operateSta RTC_SetTime(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);

/**
 * @brief 读取当前时间
 * @retval 成功返回SUCCEED
 */
operateSta RTC_GetTime(void);

/** 
 * @brief  获取星期，按年月日计算星期，只允许1901-2099年。在RTC_GetTime()中被调用    
 * @retval 返回星期值
 */
u8 RTC_GetWeek(u16 year,u8 month,u8 day);

/**
 * @brief 通过串口设置时间
*/
void RTC_SetTimeByUSART(void);

#endif
