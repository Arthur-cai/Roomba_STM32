///////////////////////// <头文件> //////////////////////////
#include "../CMSIS/stm32f10x.h"
#include "../Basic/sys/sys.h"
#include "../Basic/delay/delay.h"
#include "../Basic/usart/usart.h"
#include "../Basic/rtc/rtc.h"
#include "../Hardware/LED/led.h"
#include "../Hardware/OLED2420/oled2420.h"
#include "../Hardware/NRF24L01/24l01.h"
#include "../Hardware/L298N/l298n.h"
#include "../Hardware/MPU6050/mpu6050.h"
#include "../Hardware/VL53L0X/demo/vl53l0x.h"
#include "../Hardware/HC_SR04/hc_sr04.h"
#include "../Hardware/ESP8266/esp8266.h"
////////////////////////// <头文件> //////////////////////////

/////////////////////////// <变量> ///////////////////////////
u8 t = 0;									  //LED闪烁时间
u8 com[3] = {0};							  //NRF接收数组
u8 MPU_GET_DEGREES_STATE;					  //MPU角度获取标记
short mpuTemp = 0;							  //mpu数据暂存
vu16 frontDis = 0;							  //超声波模块读取距离
vu16 leftDis = 0;							  //左VL53L0X测距数据
vu16 rightDis = 0;							  //右VL53L0X测距数据
/////////////////////////// <变量> ///////////////////////////

///////////////////////// <全局变量> /////////////////////////
extern RTC_TIME time;						  //RTC时间结构体
extern Cloud_Parameter_Type Cloud_Parameters; //原子云设备参数结构体
extern Three_Degrees_Type degrees;			  //MPU角度值结构体
extern VL53L0X_RangingMeasurementData_t left_vl53l0x_data;  //左测量数据结构体
extern VL53L0X_RangingMeasurementData_t right_vl53l0x_data; //右测量数据结构体
///////////////////////// <全局变量> /////////////////////////

//////////////////////////////////////////////////////////////
//						-- 相关函数 --						//
//////////////////////////////////////////////////////////////

/**
 * @brief	主UI
*/
static void Show_MainUI(void) {
	u8 i;
	/* RTC及MPU角度UI显示 */
	for (i = 0; i < 128; i++)	OLED_DrawPoint(i, 0, 1);
	OLED_ShowString(0, 1, "    /  /    ", 8, 0);
	OLED_ShowString(12 * 6, 1, "  :  :    ", 8, 0);
	OLED_ShowString(0, 5 * 8, "pi:    .  ro:    .", 8, 1);
	OLED_ShowString(0, 6 * 8, "ya:    .", 8, 1);
	OLED_Refresh();
}

/**
 * @brief	LED翻转
*/
static void LED_Toggle(void) {
	t++;
	delay_ms(10);
	if (t > 50) {
		LED0 = !LED0;
		t = 0;
	}
}

/**
 * @brief	ESP8266控制模式下，接收到的命令包含指定内容
 * @param	str，原子云输入的内容
 * @retval	为真返回TRUE，否则返回FALSE
*/
static boolean Rec_Contain(char* str) {
	char* res;
	res = strstr((const char*)USART3_RX_BUF, str);
	if (res != 0) return TRUE;
	return FALSE;
}

/**
 * @brief	显示时间
*/
static void Show_RTC_Time(void) {
	if (RTC_GetTime()) {
		OLED_ShowNum(0, 1, (&time)->ryear, 4, 8, 0);
		OLED_ShowNum(5 * 6, 1, (&time)->rmon, 2, 8, 0);
		OLED_ShowNum(8 * 6, 1, (&time)->rday, 2, 8, 0);

		OLED_ShowNum(12 * 6, 1, (&time)->rhour, 2, 8, 0);
		OLED_ShowNum(15 * 6, 1, (&time)->rmin, 2, 8, 0);
		OLED_ShowNum(18 * 6, 1, (&time)->rsec, 2, 8, 0);
		OLED_Refresh();
	}
}

/**
 * @brief	显示MPU读取的角度
*/
static void Show_Three_Degrees(void) {
	/* 处理MPU6050数据 */
	if (MPU_GET_DEGREES_STATE == SUCCEED) {
		/* 显示pitch */
		mpuTemp = (&degrees)->pitch * 10;
		if (mpuTemp < 0) {
			OLED_ShowChar(3 * 6, 5 * 8, '-', 8, 1);
			mpuTemp = -mpuTemp;
		}
		else OLED_ShowChar(3 * 6, 40, ' ', 8, 1);
		OLED_ShowNum(4 * 6, 5 * 8, mpuTemp / 10, 3, 8, 1);
		OLED_ShowNum(8 * 6, 5 * 8, mpuTemp % 10, 1, 8, 1);
		/* 显示roll */
		mpuTemp = (&degrees)->roll * 10;
		if (mpuTemp < 0) {
			OLED_ShowChar(13 * 6, 5 * 8, '-', 8, 1);
			mpuTemp = -mpuTemp;
		}
		else OLED_ShowChar(13 * 6, 5 * 8, ' ', 8, 1);
		OLED_ShowNum(14 * 6, 5 * 8, mpuTemp / 10, 3, 8, 1);
		OLED_ShowNum(18 * 6, 5 * 8, mpuTemp % 10, 1, 8, 1);
		/* 显示yaw */
		mpuTemp = (&degrees)->yaw * 10;
		if (mpuTemp < 0) {
			OLED_ShowChar(3 * 6, 6 * 8, '-', 8, 1);
			mpuTemp = -mpuTemp;
		}
		else OLED_ShowChar(3 * 6, 6 * 8, ' ', 8, 1);
		OLED_ShowNum(4 * 6, 6 * 8, mpuTemp / 10, 3, 8, 1);
		OLED_ShowNum(8 * 6, 6 * 8, mpuTemp % 10, 1, 8, 1);

		OLED_Refresh();
	}
	mpuTemp = 0; /* 初始化mpu数据暂存 */
}

/**
 * @brief	右转微调
*/
static void Adjust_Position_Right(void) {
	Motion_Select(TURNRIGHT, 100, 100);
	OLED_ShowString(0, 3 * 8, "Turn Right", 8, 0); OLED_Refresh();
	delay_ms(800);
	Motion_Select(STOP, 0, 0);
	OLED_ShowString(0, 3 * 8, "STOP      ", 8, 0); OLED_Refresh();
	delay_ms(500);
	Motion_Select(FORWARD, 100, 110);
	OLED_ShowString(0, 3 * 8, "Forward   ", 8, 0); OLED_Refresh();
	delay_ms(1000);
	Motion_Select(STOP, 0, 0);
	OLED_ShowString(0, 3 * 8, "STOP      ", 8, 0); OLED_Refresh();
	delay_ms(500);
	Motion_Select(TURNRIGHT, 100, 100);
	OLED_ShowString(0, 3 * 8, "Turn Right", 8, 0); OLED_Refresh();
	delay_ms(725);
	Motion_Select(STOP, 0, 0);
	OLED_ShowString(0, 3 * 8, "STOP      ", 8, 0); OLED_Refresh();
	delay_ms(500);
}

/**
 * @brief	左转微调
*/
static void Adjust_Position_Left(void) {
	Motion_Select(TURNLEFT, 100, 100);
	OLED_ShowString(0, 3 * 8, "Turn Left ", 8, 0); OLED_Refresh();
	delay_ms(800);
	Motion_Select(STOP, 0, 0);
	OLED_ShowString(0, 3 * 8, "STOP      ", 8, 0); OLED_Refresh();
	delay_ms(500);
	Motion_Select(FORWARD, 100, 110);
	OLED_ShowString(0, 3 * 8, "Forward   ", 8, 0); OLED_Refresh();
	delay_ms(1000);
	Motion_Select(STOP, 0, 0);
	OLED_ShowString(0, 3 * 8, "STOP      ", 8, 0); OLED_Refresh();
	delay_ms(500);
	Motion_Select(TURNLEFT, 100, 100);
	OLED_ShowString(0, 3 * 8, "Turn Left ", 8, 0); OLED_Refresh();
	delay_ms(725);
	Motion_Select(STOP, 0, 0);
	OLED_ShowString(0, 3 * 8, "STOP      ", 8, 0); OLED_Refresh();
	delay_ms(500);
}

//////////////////////////////////////////////////////////////
//					   -- 3个控制模式 --					//
//////////////////////////////////////////////////////////////

/**
 * @brief	NRF24L01控制模式
*/
static void NRF24L01_Control_Mode() {
	OLED_ShowString(0, 2 * 8, "NRF24L01 Control Mode", 8, 1);
	while (TRUE) {
		NRF24L01_RX_Mode();											/* 设置NRF为接收模式 */
		MPU_GET_DEGREES_STATE = Get_ThreeDegrees(); 				/* 读取MPU数据 */
		if (NRF24L01_RxPacket(com) == 0) {							/* 如果接受到数据 */
			if (com[0] == CONTROL_BY_NRF24L01) {					/* 如果接收的命令一直是 CONTROL_BY_NRF24L01 */
				if (com[1] == FORWARD_COM) {						/* 如果收到前进 */
					Motion_Select(FORWARD, 100, 110);
					OLED_ShowString(0, 3 * 8, "Forward   ", 8, 0);
				}
				else if (com[1] == BACKWARD_COM) {					/* 如果收到后退 */
					Motion_Select(BACKWARD, 100, 110);
					OLED_ShowString(0, 3 * 8, "Backward  ", 8, 0);
				}
				else if (com[2] == LEFT_COM) {						/* 如果收到左转 */
					Motion_Select(TURNLEFT, 100, 100);
					OLED_ShowString(0, 3 * 8, "Turn Left ", 8, 0);
				}
				else if (com[2] == RIGHT_COM) {						/* 如果收到右转 */
					Motion_Select(TURNRIGHT, 100, 100);
					OLED_ShowString(0, 3 * 8, "Turn Right", 8, 0);
				}
				else {												/* 默认停止 */
					Motion_Select(STOP, 0, 0);
					OLED_ShowString(0, 3 * 8, "          ", 8, 1);
				}
				OLED_Refresh();
			}
			else {													/* 否则就退出当前模式 */
				Motion_Select(STOP, 0, 0);
				OLED_ShowString(0, 3 * 8, "          ", 8, 1);
				OLED_ShowString(0, 2 * 8, "                     ", 8, 1);
				OLED_Refresh();
				break;
			}
		}
		Show_Three_Degrees(); 										/* 显示角度值 */
		Show_RTC_Time();	  										/* 显示RTC时间 */
		LED_Toggle();		  										/* LED翻转 */
	}
}

/**
 * @brief	ESP8266控制模式
*/
static void ESP8266_Control_Mode(void) {
	while (ESP8266_Connect_YuanZiCloud(&Cloud_Parameters) != SUCCEED);	/* 等待成功连接原子云 */
	OLED_ShowString(0, 2 * 8, "ESP8266 Control Mode ", 8, 1);
	while (TRUE) {
		NRF24L01_RX_Mode();												/* 设置NRF为接收模式 */
		MPU_GET_DEGREES_STATE = Get_ThreeDegrees();						/* 读取MPU数据 */
		if (NRF24L01_RxPacket(com) == 0) {								/* 如果接收到数据 */
			if (com[0] == CONTROL_BY_ESP8266) {							/* 如果接收到的命令一直是 CONTROL_BY_ESP8266 */
				if ((&Cloud_Parameters)->CLOUD_CONNECTED && 			/* 如果已连接到云，且串口3收到数据 */
					USART3_RX_STA & 0x8000) {
					USART3_RX_BUF[USART3_RX_STA & 0x7fff] = '\0';		/* 添加结束符 */
					ESP8266_CloudSendData("received");					/* 返回已接收信息给原子云 */
					if (Rec_Contain("forward")) {						/* 如果收到前进 */
						Motion_Select(FORWARD, 100, 110);
						OLED_ShowString(0, 3 * 8, "Forward   ", 8, 0);
					}
					else if (Rec_Contain("backward")) {					/* 如果收到后退 */
						Motion_Select(BACKWARD, 100, 110);
						OLED_ShowString(0, 3 * 8, "Backward  ", 8, 0);
					}
					else if (Rec_Contain("left")) {						/* 如果收到左转 */
						Motion_Select(TURNLEFT, 100, 100);
						OLED_ShowString(0, 3 * 8, "Turn Left ", 8, 0);
					}
					else if (Rec_Contain("right")) {					/* 如果收到右转 */
						Motion_Select(TURNRIGHT, 100, 100);
						OLED_ShowString(0, 3 * 8, "Turn Right", 8, 0);
					}
					else if (Rec_Contain("stop")) {						/* 如果收到停止 */
						Motion_Select(STOP, 0, 0);
						OLED_ShowString(0, 3 * 8, "STOP      ", 8, 0);
					}
					else {												/* 默认保持静止 */
						Motion_Select(STOP, 0, 0);
						OLED_ShowString(0, 3 * 8, "          ", 8, 1);
					}
					OLED_Refresh();
					USART3_RX_BUF_CLEAR();								/* 清空串口3数据 */
				}
			}	
			else {														/* 否则就退出当前模式 */
				Motion_Select(STOP, 0, 0);
				ESP8266_Disconnect_YuanZiCloud();						/* 断开云连接 */
				USART3_RX_BUF_CLEAR();									/* 清空串口3数据 */
				OLED_ShowString(0, 3 * 8, "          ", 8, 1);
				OLED_ShowString(0, 2 * 8, "                     ", 8, 1);
				OLED_Refresh();
				break;
			}
		}
		Show_Three_Degrees(); 											/* 显示角度值 */
		Show_RTC_Time();	  											/* 显示RTC时间 */
		LED_Toggle();		  											/* LED翻转 */
	}
}

/**
 * @breif	自动控制模式
*/
static void Self_Control_Mode(void) {
	u8 LeftOrRight = 2; //左右转标记，为1表示左转，为2表示右转
	OLED_ShowString(0, 2 * 8, "Self Control Mode    ", 8, 1);
	while (TRUE) {
		NRF24L01_RX_Mode();											/* 设置NRF为接收模式 */
		MPU_GET_DEGREES_STATE = Get_ThreeDegrees();					/* 读取MPU数据 */
		if (NRF24L01_RxPacket(com) == 0) {							/* 如果接收到数据 */
			if (com[0] == CONTROL_BY_SELF) {						/* 如果接收到的命令一直是 CONTROL_BY_SELF */
				frontDis = HC_SR04_GetDistance();					/* 读取HC_SR04距离 */
				VL53L0X_General_LongRange_Start();					/* 读取VL53L0X距离 */
				leftDis = (&left_vl53l0x_data)->RangeMilliMeter;
				rightDis = (&right_vl53l0x_data)->RangeMilliMeter;
				if (frontDis >= FORWARD_DISTANCE_THRESHOULD) {		/* 如果距离大于停止阈值 */
					Motion_Select(FORWARD, 100, 110);
					OLED_ShowString(0, 3 * 8, "Forward   ", 8, 0);
				}
				else {												/* 否则就根据顺序左转或右转 */
					Motion_Select(STOP, 0, 0);
					switch (LeftOrRight) {
					case 1: Adjust_Position_Left(); LeftOrRight = 2; break;
					case 2: Adjust_Position_Right(); LeftOrRight = 1; break;
					}
				}
				OLED_Refresh();
			}
			else {													/* 否则就退出当前模式 */
				Motion_Select(STOP, 0, 0);
				OLED_ShowString(0, 3 * 8, "          ", 8, 1);
				OLED_ShowString(0, 2 * 8, "                     ", 8, 1);
				OLED_Refresh();
				break;
			}
		}
		Show_Three_Degrees(); 										/* 显示角度值 */
		Show_RTC_Time();	  										/* 显示RTC时间 */
		LED_Toggle();		  										/* LED翻转 */
	}
}

//////////////////////////////////////////////////////////////
//					 	 -- 主函数 --						//
//////////////////////////////////////////////////////////////

/**
 * @brief	主函数
*/
int main(void) {
	delay_init();									/* 延时初始化 */
	delay_ms(200);									/* 延时200ms，等待元件启动 */
	NVIC_Configuration();							/* NVIC中断优先级配置 */
	USART1_Init(115200);							/* 串口1初始化 */
	RTC_Configuration();							/* 初始化RTC */
	LED_Init();										/* 初始化LED */
	OLED_Init();									/* 初始化OLED */
	MPU_Init();										/* 初始化MPU6050 */
	NRF24L01_Init();								/* 初始化无线通信模块 */
	ESP8266_Init();									/* 初始化ESP8266 */
	HC_SR04_Init();									/* 初始化超声波测距模块 */
	VL53L0X_General_LongRange_Init(); 				/* 初始化VL53L0X */
	L298N_Init();									/* 初始化电机驱动模块 */
	Show_MainUI();									/* 显示主UI */
	LED0 = 0;										/* 点亮LED */

	while (TRUE) {
		NRF24L01_RX_Mode();							/* 设置NRF为接收模式 */
		if (NRF24L01_RxPacket(com) == 0) {			/* 如果接收到数据 */
			switch (com[0]) {						/* 判断控制命令 */
				case CONTROL_BY_NRF24L01:
					NRF24L01_Control_Mode();		/* 进入NRF控制模式 */
					break;
				case CONTROL_BY_ESP8266:
					ESP8266_Control_Mode(); 		/* 进入ESP控制模式 */
					break;
				case CONTROL_BY_SELF:
					Self_Control_Mode(); 			/* 进入自动控制模式 */
					break;
				default: break;
			}
		}
		MPU_GET_DEGREES_STATE = Get_ThreeDegrees();	/* 读取MPU数据 */
		Show_Three_Degrees(); 						/* 显示角度值 */
		Show_RTC_Time();	  						/* 显示RTC时间 */
		LED_Toggle();		  						/* LED翻转 */
	}
}
