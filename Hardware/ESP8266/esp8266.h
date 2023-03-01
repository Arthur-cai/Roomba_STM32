#ifndef __ESP8266_H
#define __ESP8266_H
////////////////////// <头文件> //////////////////////
#include "../../Basic/sys/sys.h"
#include "../../Basic/usart/usart.h"
#include "../../Basic/delay/delay.h"
#include <string.h>
////////////////////// <头文件> //////////////////////

#define	FIRMWARE_YUANZI_SUPPORT		1	//是否为刷了原子云固件的ESP8266

/////////////////////////////////////////////////////////////////////////
//							-- 枚举类型定义 --				   		    //
/////////////////////////////////////////////////////////////////////////

//ESP8266 工作模式
typedef enum _esp8266_workmode_enum {
	STA_MODE = 1, /* STA模式 */
	AP_MODE,	  /* AP模式 */
	AP_STA_MODE	  /* AP+STA模式 */
}ESP8266_WORK_MODE;

//ESP8266 加密方式
typedef enum _esp8266_encryption_enum {
	OPEN = 0,
	WPA_PSK = 2,
	WPA2_PSK,
	WPA_WPA2_PSK
}ESP8266_ENCRYPTION_TYPE;

//AP模式下最大可连接客户端数量
typedef enum _max_client_number_enum {
	MAX_CLIENT_NUMBER_1 = 1, /* 设置最大可连接的客户端数量为1 */
	MAX_CLIENT_NUMBER_2,	 /* 设置最大可连接的客户端数量为2 */
	MAX_CLIENT_NUMBER_3,	 /* 设置最大可连接的客户端数量为3 */
	MAX_CLIENT_NUMBER_4		 /* 设置最大可连接的客户端数量为4 */
}MAX_CLIENT_NUMBER;

/////////////////////////////////////////////////////////////////////////
//							-- 相关结构体定义 --				   		//
/////////////////////////////////////////////////////////////////////////

//STA相关参数
typedef struct _sta_params_struct {
	const u8* STA_SSID;		//要连接的 WIFI SSID
	const u8* STA_PWD;		//要连接的 WIFI 密码
	u8 WIFI_CONNECTED;		//WIFI连接标志位
}STA_Parameter_Type;
extern STA_Parameter_Type STA_Parameters; //WIFI参数

//AP相关参数
typedef struct _ap_params_struct {
	const u8* AP_SSID;		//软路由 AP SSID
	const u8* AP_PWD; 		//软路由 AP 密码(至少8位)
	u8 AP_CHANNEL;			//通道号
	ESP8266_ENCRYPTION_TYPE AP_ENCRYPTION; //加密方式
	MAX_CLIENT_NUMBER MAX_CLIENT; //最大客户端数量
	u8 BROADCAST;			//是否广播AP的SSID
}AP_Parameter_Type;
extern AP_Parameter_Type AP_Parameters; //软路由参数

//需连接的TCP服务器相关参数
typedef struct _server_params_struct {
	const u8* serverIP;		//TCP服务器IP
	const u8* serverPort;	//TCP服务器连接端口
	u8 TCP_SERVER_CONNECTED;//TCP服务器连接标志位
	u8 TRANSPARENT_MODE;	//透传模式标志位
}TCP_Server_Parameter_Type;
extern TCP_Server_Parameter_Type TCP_Server_Parameters; //TCP参数

#if FIRMWARE_YUANZI_SUPPORT	//如果为刷了原子云固件的ESP8266
//原子云设备ID及密码
typedef struct _cloud_params_struct {
	const u8* cloudID;		//原子云设备ID
	const u8* cloadPwd;		//原子云设备密码
	u8 CLOUD_CONNECTED;		//原子云连接标志位
}Cloud_Parameter_Type;
extern Cloud_Parameter_Type Cloud_Parameters;
#endif // FIRMWARE_YUANZI_SUPPORT

////////////////////////////////////////////////////////////////////////
//							-- 硬件相关定义 --				   		   //
//				USART3_TX引脚为PB10，USART3_RX引脚为PB11			   //
////////////////////////////////////////////////////////////////////////

#define ESP8266_RST_PIN			GPIO_Pin_9				//ESP8266模块RST引脚
#define ESP8266_RST_PORT		GPIOB					//ESP8266模块RST端口
#define ESP8266_RST_OUT			PBout(9)				//ESP8266模块RST输出
#define	ESP8266_RST_PORT_CLOCK	RCC_APB2Periph_GPIOB	//ESP8266模块RST引脚端口时钟

////////////////////////////////////////////////////////////////////////
//							-- 相关操作函数 --				   		   //
////////////////////////////////////////////////////////////////////////

/**
 * @brief	检查收到的应答
 * @param	res	预期应答字符串
 * @retval	res首次出现的位置指针，如果为0则说明响应无该字符串
*/
u8* ESP8266_Check_Response(u8* res);

/**
 * @brief	发送命令给ESP8266
 * @param	cmd	命令
 * @param	ack 期待的应答结果，如果为空则不等待应答
 * @param	waitTime	等待时间(单位ms)
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_SendCmd(u8* cmd, u8* ack, u16 waitTime);

/**
 * @brief	ESP8266发送数据内容。
 *			如果连接了原子云，消息收发会被云占用，无法和TCP服务器收发
 * @param	data	数据
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_SendData(u8* data);

/**
 * @brief	ESP8266进入透传模式
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Enter_TransparentTransmit(void);

/**
 * @brief	ESP8266通过透传发送数据内容。
 *			如果连接了原子云，消息收发会被云占用，无法和TCP服务器收发
 * @note	进入透传模式后，直接发送数据即可，无需先发送AT+CIPSEND=%d说明要发送的长度
 * @param	data	数据
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_TransparentSendData(u8* data);

/**
 * @brief	ESP8266退出透传模式
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Exit_TransparentTransmit(void);

#if FIRMWARE_YUANZI_SUPPORT	//如果为刷了原子云固件的ESP8266
/**
 * @brief	ESP8266连接原子云
 * @note	只有刷了原子云固件的才可以使用
 * @param	parameters, 原子云相关参数结构体指针
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Connect_YuanZiCloud(Cloud_Parameter_Type* parameters);

/**
 * @brief	ESP8266发送数据内容给原子云。
 *			如果连接了原子云，消息收发会被云占用，无法和TCP服务器收发
 * @note	只有刷了原子云固件的才可以使用
 * @param	data	数据
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta  ESP8266_CloudSendData(u8* data);

/**
 * @brief	ESP8266断开与原子云的连接
 * @note	只有刷了原子云固件的才可以使用
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Disconnect_YuanZiCloud(void);
#endif // FIRMWARE_YUANZI_SUPPORT

/**
 * @brief	AT指令测试
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_ATCmd_Test(void);

/**
 * @brief	查询AT固件版本信息
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Check_FirmWare_Info(void);

/**
 * @brief	设置ESP8266工作模式
 * @param	mode 工作模式.
			@arg	取值为: STA_MODE, AP_MODE, AP_STA_MODE
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_WorkMode_Conf(ESP8266_WORK_MODE mode);

/**
 * @brief	设置ESP8266软路由相关参数
 * @param	parameters, AP模式相关参数结构体指针
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_SoftAP_Conf(AP_Parameter_Type* parameters);

/**
 * @brief	ESP8266设置AP参数后，查询与ESP8266建立连接的客户端信息
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Check_Connected_ClientInfo(void);

/**
 * @brief	ESP8266连接WIFI
 * @param	parameters, STA模式下的WIFI相关参数结构体指针
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Connect_WIFI(STA_Parameter_Type* parameters);

/**
 * @brief	ESP8266连接TCP服务器
 * @param	parameters, TCP服务器相关参数结构体指针
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Connect_TCP_Server(TCP_Server_Parameter_Type* parameters);

/**
 * @brief	断开与TCP服务器的连接
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Disconnect_TCP_Server(void);

/**
 * @brief	获取ip地址
 * @param	ipbuf	ip地址字符串缓冲区
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_GetIP(u8* ipbuf);

////////////////////////////////////////////////////////////////////////
//							-- 初始化相关函数 --				   	   //
////////////////////////////////////////////////////////////////////////

/**
 * @brief	ESP8266恢复出厂设置
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Restore_Default(void);

/**
 * @brief	ESP8266软件复位
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_SW_Reset(void);

/**
 * @brief	ESP8266初始化
 * @retval	成功SUCCEED，失败FAIL
*/
operateSta ESP8266_Init(void);

#endif // !__ESP8266_H
