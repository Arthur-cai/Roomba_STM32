#include "esp8266.h"

////////////////////// <全局变量声明> //////////////////////
STA_Parameter_Type STA_Parameters;				//STA模式参数
AP_Parameter_Type AP_Parameters;				//AP模式参数
TCP_Server_Parameter_Type TCP_Server_Parameters;//TCP Server参数
#if FIRMWARE_YUANZI_SUPPORT	//如果为刷了原子云固件的ESP8266
Cloud_Parameter_Type Cloud_Parameters;			//原子云设备参数
#endif // FIRMWARE_YUANZI_SUPPORT
////////////////////// <全局变量声明> //////////////////////

/////////////////////////////////////////////////////////////////////////
//						-- 结构体参数初始化 --				   		    //
/////////////////////////////////////////////////////////////////////////

//STA参数初始化
static void STA_Info_Init(STA_Parameter_Type* parameters) {
	parameters->STA_SSID = "201";
	parameters->STA_PWD = "22112622";
	parameters->WIFI_CONNECTED = 0;
}

//AP参数初始化
static void AP_Info_Init(AP_Parameter_Type* parameters) {
	parameters->AP_SSID = "ESP_8266";
	parameters->AP_PWD = "123456789";
	parameters->AP_CHANNEL = 5;
	parameters->AP_ENCRYPTION = WPA_WPA2_PSK;
	parameters->MAX_CLIENT = MAX_CLIENT_NUMBER_1;
	parameters->BROADCAST = 0;
}

//TCP_Server参数初始化
static void TCP_Server_Info_Init(TCP_Server_Parameter_Type* parameters) {
	parameters->serverIP = "192.168.0.10";
	parameters->serverPort = "9090";
	parameters->TCP_SERVER_CONNECTED = 0;
	parameters->TRANSPARENT_MODE = 0;
}

#if FIRMWARE_YUANZI_SUPPORT	//如果为刷了原子云固件的ESP8266
//Cloud参数初始化
static void Cloud_Info_Init(Cloud_Parameter_Type* parameters) {
	parameters->cloudID = "60002472859501205274";
	parameters->cloadPwd = "12345678";
	parameters->CLOUD_CONNECTED = 0;
}
#endif // FIRMWARE_YUANZI_SUPPORT


/////////////////////////////////////////////////////////////////////////
//							-- 相关操作函数 --				   		    //
/////////////////////////////////////////////////////////////////////////

/**
 * @brief	检查收到的应答
 * @param	res	预期应答字符串
 * @retval	res首次出现的位置指针，如果为0则说明响应无该字符串
*/
u8* ESP8266_Check_Response(u8* res) {
	char* index = 0;
	//接收到数据
	if (USART3_RX_STA & 0x8000) {
		USART3_RX_BUF[USART3_RX_STA & 0x3FFF] = '\0'; //添加结束符
		index = strstr((const char*)USART3_RX_BUF, (const char*)res);
	}
	return (u8*)index;
}

/**
 * @brief	发送命令给ESP8266
 * @param	cmd	命令
 * @param	ack 期待的应答结果，如果为空则不等待应答
 * @param	waitTime	等待时间(单位ms)
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_SendCmd(u8* cmd, u8* ack, u16 waitTime) {
	USART3_RX_STA = 0;			  //初始化串口3接收标记
	USART3_printf("%s\r\n", cmd); //发送命令
	if (ack && waitTime) {
		while (waitTime--) {
			if (USART3_RX_STA & 0x8000) {
				if (ESP8266_Check_Response(ack) != 0) return SUCCEED;
				USART3_RX_STA = 0;
			}
			delay_ms(1);
		}
	}
	if (ack == NULL) return SUCCEED;
	return FAIL;
}

/**
 * @brief	ESP8266发送数据内容。
 *			如果连接了原子云，消息收发会被云占用，无法和TCP服务器收发
 * @param	data	数据
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_SendData(u8* data) {
	u8 state;
	u8 cmd[12];
	u16 len = strlen((const char*)data) + 2; //要发送的数据长度+2个结束字符0d0a
	//如果TCP服务器未连接成功则直接返回FAIL
	if (!(&TCP_Server_Parameters)->TCP_SERVER_CONNECTED) return FAIL;
#if FIRMWARE_YUANZI_SUPPORT	//如果为刷了原子云固件的ESP8266
	if ((&Cloud_Parameters)->CLOUD_CONNECTED) return FAIL;
#endif // FIRMWARE_YUANZI_SUPPORT
	sprintf((char*)cmd, "AT+CIPSEND=%d", len);
	state = ESP8266_SendCmd(cmd, ">", 500);
	if (state == FAIL)	return FAIL;
	state = ESP8266_SendCmd(data, "SEND OK", 1000);
	if (state == SUCCEED)	return SUCCEED;
	return FAIL;
}

/**
 * @brief	ESP8266进入透传模式
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Enter_TransparentTransmit(void) {
	u8 state;
	state = ESP8266_SendCmd("AT+CIPMODE=1", "OK", 500);
	if (state == FAIL) return FAIL;
	state = ESP8266_SendCmd("AT+CIPSEND", ">", 500);
	if (state == SUCCEED) return SUCCEED;
	return FAIL;
}

/**
 * @brief	ESP8266通过透传发送数据内容。
 *			如果连接了原子云，消息收发会被云占用，无法和TCP服务器收发
 * @note	进入透传模式后，直接发送数据即可，无需先发送AT+CIPSEND=%d说明要发送的长度
 * @param	data	数据
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_TransparentSendData(u8* data) {
	u8 state;
	//如果TCP服务器未连接成功则直接返回FAIL
	if (!(&TCP_Server_Parameters)->TCP_SERVER_CONNECTED) return FAIL;
#if FIRMWARE_YUANZI_SUPPORT	//如果为刷了原子云固件的ESP8266
	if ((&Cloud_Parameters)->CLOUD_CONNECTED) return FAIL;
#endif // FIRMWARE_YUANZI_SUPPORT
	if (!(&TCP_Server_Parameters)->TRANSPARENT_MODE) {
		state = ESP8266_Enter_TransparentTransmit();
		if (state != SUCCEED) return FAIL;
		(&TCP_Server_Parameters)->TRANSPARENT_MODE = 1;
	}
	state = ESP8266_SendCmd(data, NULL, 500);
	if (state == SUCCEED) return SUCCEED;
	return FAIL;
}

/**
 * @brief	ESP8266退出透传模式
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Exit_TransparentTransmit(void) {
	u8 state;
	if (!(&TCP_Server_Parameters)->TRANSPARENT_MODE) return SUCCEED;
	USART3_printf("+++");
	delay_ms(1500);
	state = ESP8266_SendCmd("AT+CIPMODE=0", "OK", 500);
	if (state == SUCCEED) {
		(&TCP_Server_Parameters)->TRANSPARENT_MODE = 0;
		return SUCCEED;
	}
	return FAIL;
}

#if FIRMWARE_YUANZI_SUPPORT	//如果为刷了原子云固件的ESP8266
/**
 * @brief	ESP8266连接原子云
 * @note	只有刷了原子云固件的才可以使用
 * @param	parameters, 原子云相关参数结构体指针
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Connect_YuanZiCloud(Cloud_Parameter_Type* parameters) {
	u8 state;
	u8 cmd[50];

	sprintf((char*)cmd, "AT+ATKCLDSTA=\"%s\",\"%s\"",
		parameters->cloudID,
		parameters->cloadPwd);
	state = ESP8266_SendCmd(cmd, "CLOUD CONNECTED", 10000);
	if (state == SUCCEED) {
		parameters->CLOUD_CONNECTED = 1;
		return SUCCEED;
	}
	return FAIL;
}

/**
 * @brief	ESP8266发送数据内容给原子云。
 *			如果连接了原子云，消息收发会被云占用，无法和TCP服务器收发
 * @note	只有刷了原子云固件的才可以使用
 * @param	data	数据
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta  ESP8266_CloudSendData(u8* data) {
	u8 state;

	if (!(&Cloud_Parameters)->CLOUD_CONNECTED) return FAIL;
	state = ESP8266_SendCmd(data, NULL, 500);
	if (state == SUCCEED) return SUCCEED;
	return FAIL;
}

/**
 * @brief	ESP8266断开与原子云的连接
 * @note	只有刷了原子云固件的才可以使用
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Disconnect_YuanZiCloud(void) {
	u8 state;
	state = ESP8266_SendCmd("AT+ATKCLDCLS", "CLOUD DISCONNECT", 500);
	if (state == SUCCEED) {
		(&Cloud_Parameters)->CLOUD_CONNECTED = 0;
		return SUCCEED;
	}
	return FAIL;
}
#endif // FIRMWARE_YUANZI_SUPPORT

/**
 * @brief	AT指令测试
 * @note	单独发送"AT"给ESP8266，会返回"OK"
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_ATCmd_Test(void) {
	u8 i;
	u8 state;
	for (i = 0; i < 10; i++) {
		state = ESP8266_SendCmd("AT", "OK", 500);
		if (state == SUCCEED) return SUCCEED;
	}
	return FAIL;
}

/**
 * @brief	查询AT固件版本信息
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Check_FirmWare_Info(void) {
	u8 state;
	state = ESP8266_SendCmd("AT+GMR", "OK", 500);
	if (state == SUCCEED) return SUCCEED;
	return FAIL;
}

/**
 * @brief	设置ESP8266工作模式
 * @param	mode 工作模式
			@arg	STA_MODE, AP_MODE, AP_STA_MODE
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_WorkMode_Conf(ESP8266_WORK_MODE mode) {
	u8 state;
	switch (mode) {
		case STA_MODE: {
			state = ESP8266_SendCmd("AT+CWMODE_DEF=1", "OK", 500); break;
		}
		case AP_MODE: {
			state = ESP8266_SendCmd("AT+CWMODE_DEF=2", "OK", 500); break;
		}
		case AP_STA_MODE: {
			state = ESP8266_SendCmd("AT+CWMODE_DEF=3", "OK", 500); break;
		}
		default: return FAIL;
	}
	if (state == SUCCEED) return SUCCEED;
	return FAIL;
}

/**
 * @brief	设置ESP8266软路由相关参数
 * @param	parameters, AP模式相关参数结构体指针
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_SoftAP_Conf(AP_Parameter_Type* parameters) {
	u8 state;
	u8 cmd[50];

	sprintf((char*)cmd, "AT+CWSAP_DEF=\"%s\",\"%s\",%d,%d,%d,%d", 
		parameters->AP_SSID, 
		parameters->AP_PWD,
		parameters->AP_CHANNEL,
		parameters->AP_ENCRYPTION,
		parameters->MAX_CLIENT,
		parameters->BROADCAST);
	state = ESP8266_SendCmd(cmd, "OK", 5000);
	if (state == SUCCEED) return SUCCEED;
	return FAIL;
}

/**
 * @brief	ESP8266设置AP参数后，查询与ESP8266建立连接的客户端信息
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Check_Connected_ClientInfo(void) {
	u8 state;
	state = ESP8266_SendCmd("AT+CWLIF", "192.", 5000);
	if (state == SUCCEED) return SUCCEED;
	return FAIL;
}

/**
 * @brief	ESP8266连接WIFI
 * @param	parameters, STA模式下的WIFI相关参数结构体指针
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Connect_WIFI(STA_Parameter_Type* parameters) {
	u8 state;
	u8 cmd[50];

	sprintf((char*)cmd, "AT+CWJAP_DEF=\"%s\",\"%s\"", 
		parameters->STA_SSID, 
		parameters->STA_PWD);
	state = ESP8266_SendCmd(cmd, "WIFI GOT IP", 10000);
	if (state == SUCCEED) {
		parameters->WIFI_CONNECTED = 1;
		return SUCCEED;
	}
	return FAIL;
}

/**
 * @brief	ESP8266连接TCP服务器
 * @param	parameters, TCP服务器相关参数结构体指针
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Connect_TCP_Server(TCP_Server_Parameter_Type* parameters) {
	u8 state;
	u8 cmd[50];

	state = ESP8266_SendCmd("AT+CIPMUX=0", "OK", 500);
	if (state != SUCCEED) return FAIL;
	sprintf((char*)cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", 
		parameters->serverIP,
		parameters->serverPort);
	state = ESP8266_SendCmd(cmd, "CONNECT", 5000);
	if (state == SUCCEED) {
		parameters->TCP_SERVER_CONNECTED = 1;
		return SUCCEED;
	}
	return FAIL;
}

/**
 * @brief	断开与TCP服务器的连接
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Disconnect_TCP_Server(void) {
	u8 state;
	state = ESP8266_SendCmd("AT+CIPCLOSE", "CLOSED", 500);
	if (state == SUCCEED) {
		(& TCP_Server_Parameters)->TCP_SERVER_CONNECTED = 0;
		return SUCCEED;
	}
	return FAIL;
}

/**
 * @brief	获取ip地址
 * @param	ipbuf	ip地址字符串缓冲区
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_GetIP(u8* ipbuf) {
	u8* startIndex;
	u8* endIndex;
	u8 state;

	state = ESP8266_SendCmd("AT+CIFSR", "OK", 500);
	//如果获取IP地址失败
	if (state != SUCCEED) return FAIL;
	//如果获取IP成功
	startIndex = ESP8266_Check_Response("\""); //找到开始的("符号)指针位置
	endIndex = (u8*)strstr((const char*)(startIndex + 1), "\""); //找到结束的("符号)指针位置
	*endIndex = '\0'; //把最后一个"变为结束符
	sprintf((char*)ipbuf, "%s", startIndex + 1); //把""内的ip地址字符串赋给ipbuf
	return SUCCEED;
}

////////////////////////////////////////////////////////////////////////
//							-- 初始化相关函数 --				   	   //
////////////////////////////////////////////////////////////////////////

/**
 * @brief	ESP8266硬件初始化
*/
static void ESP8266_HW_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(ESP8266_RST_PORT_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = ESP8266_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ESP8266_RST_PORT, &GPIO_InitStructure);
}

/**
 * @brief	ESP8266硬件复位
*/
static void ESP8266_HW_Reset(void) {
	ESP8266_RST_OUT = 0;
	delay_ms(100);
	ESP8266_RST_OUT = 1;
	delay_ms(500);
}

/**
 * @brief	ESP8266软件复位
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_SW_Reset(void) {
	u8 state;
	state = ESP8266_SendCmd("AT+RST", "OK", 500);
	if (state == SUCCEED) return SUCCEED;
	return FAIL;
}

/**
 * @brief	ESP8266恢复出厂设置
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Restore_Default(void) {
	u8 state;
	state = ESP8266_SendCmd("AT+RESTORE", "ready", 3000);
	if (state == SUCCEED) return SUCCEED;
	return FAIL;
}

/**
 * @brief	ESP8266初始化
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Init(void) {
	u8 state;

	USART3_Init(115200); //初始化串口3
	ESP8266_HW_Init();   //硬件初始化
	ESP8266_HW_Reset();  //硬件复位
	STA_Info_Init(&STA_Parameters);	//初始化STA模式相关参数
	AP_Info_Init(&AP_Parameters);	//初始化AP模式相关参数
	TCP_Server_Info_Init(&TCP_Server_Parameters); //初始化TCP Server相关参数
#if FIRMWARE_YUANZI_SUPPORT	//如果为刷了原子云固件的ESP8266
	Cloud_Info_Init(&Cloud_Parameters);	//初始化云平台相关参数
#endif // FIRMWARE_YUANZI_SUPPORT
	state = ESP8266_ATCmd_Test();	//测试AT指令
	if (state == SUCCEED) return SUCCEED;
	return FAIL;
}
