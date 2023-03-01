#include "24l01.h"

const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //接收地址

/**
 * @brief	初始化24L01的IO口
*/
void NRF24L01_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PA端口时钟

	GPIO_InitStructure.GPIO_Pin = CE_PIN | CSN_PIN;	 //CE和CSN引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(NRF24L01_PORT, &GPIO_InitStructure);	 //初始化指定IO

	GPIO_InitStructure.GPIO_Pin = IRQ_PIN;			 //IRQ引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 //PA8 IRQ 下拉输入  
	GPIO_Init(NRF24L01_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(NRF24L01_PORT, CE_PIN | CSN_PIN | IRQ_PIN);

	SPI2_Init(); //初始化SPI	 

	SPI_Cmd(SPI2, DISABLE); //SPI外设不使能

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master; //SPI主机
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; //发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; //时钟悬空低
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; //数据捕获于第1个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; //NSS信号由软件控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; //定义波特率预分频的值:波特率预分频值为16
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; //数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7; //CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure); //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

	SPI_Cmd(SPI2, ENABLE); //使能SPI外设

	NRF24L01_CE = 0; //使能24L01
	NRF24L01_CSN = 1; //SPI片选取消
}

/**
 * @brief	SPI写寄存器
 * @param	reg	指定寄存器地址
 * @param	value	写入的值
 * @retval	状态值
*/
static u8 NRF24L01_Write_Reg(u8 reg, u8 value) {
	u8 status;
	NRF24L01_CSN = 0;  //使能SPI传输
	status = SPI2_RWByte(reg);//发送寄存器号 
	SPI2_RWByte(value); //写入寄存器的值
	NRF24L01_CSN = 1; //禁止SPI传输	   
	return	status;
}

/**
 * @brief	读取SPI寄存器值
 * @param	reg	指定寄存器地址
 * @retval	接收到了数据就返回，没有数据失败就返回0
*/
static u8 NRF24L01_Read_Reg(u8 reg) {
	u8 reg_val;
	NRF24L01_CSN = 0;  //使能SPI传输		
	SPI2_RWByte(reg);  //发送寄存器号
	reg_val = SPI2_RWByte(0XFF);//读取寄存器内容
	NRF24L01_CSN = 1;  //禁止SPI传输	
	return	reg_val; 
}

/**
 * @brief	在指定位置读出指定长度的数据
 * @param	reg	指定寄存器地址
 * @param	pBuf	数据缓冲区
 * @param	len	数据长度
 * @retval	此次读到的状态寄存器值
*/
static u8 NRF24L01_Read_Buf(u8 reg, u8* pBuf, u8 len) {
	u8 status, u8_ctr;
	NRF24L01_CSN = 0; //使能SPI传输
	status = SPI2_RWByte(reg);//发送寄存器值(位置),并读取状态值   	   
	for (u8_ctr = 0; u8_ctr < len; u8_ctr++) {
		pBuf[u8_ctr] = SPI2_RWByte(0XFF);//读出数据
	}
	NRF24L01_CSN = 1; //关闭SPI传输
	return status;
}

/**
 * @brief	在指定位置写入指定长度的数据
 * @param	reg	指定寄存器地址
 * @param	pBuf	数据缓冲区
 * @param	len	数据长度
 * @retval	此次读到的状态寄存器值
*/
static u8 NRF24L01_Write_Buf(u8 reg, u8* pBuf, u8 len) {
	u8 status, u8_ctr;
	NRF24L01_CSN = 0; //使能SPI传输
	status = SPI2_RWByte(reg);//发送寄存器值(位置),并读取状态值
	for (u8_ctr = 0; u8_ctr < len; u8_ctr++) {
		SPI2_RWByte(*pBuf++); //写入数据	 
	}
	NRF24L01_CSN = 1; //关闭SPI传输
	return status;
}

/**
 * @brief	初始化NRF24L01为TX发送模式
			设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
			PWR_UP,CRC使能
			当CE变高后,长于10us,则启动发送.
*/
void NRF24L01_TX_Mode(void) {
	NRF24L01_CE = 0;
	NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR, (u8*)TX_ADDRESS, TX_ADR_WIDTH);//写TX节点地址 
	NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, (u8*)RX_ADDRESS, RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  

	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, 0x01);     //使能通道0的自动应答    
	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01); //使能通道0的接收地址  
	NRF24L01_Write_Reg(NRF_WRITE_REG + SETUP_RETR, 0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, 40);       //设置RF通道为40
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
	NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	NRF24L01_CE = 1;//CE为高,10us后启动发送
}

/**
 * @brief	初始化NRF24L01为RX接收模式
			设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
			当CE变高后,即进入RX模式,并可以接收数据了
*/
void NRF24L01_RX_Mode(void) {
	NRF24L01_CE = 0;
	NRF24L01_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, (u8*)RX_ADDRESS, RX_ADR_WIDTH);//写RX节点地址

	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_AA, 0x01);    //使能通道0的自动应答    
	NRF24L01_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);//使能通道0的接收地址  	 
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_CH, 40);	     //设置RF通信频率		  
	NRF24L01_Write_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 	    
	NRF24L01_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
	NRF24L01_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0f);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
	NRF24L01_CE = 1; //CE为高,进入接收模式 
}

/**
 * @brief	启动NRF24L01发送一次数据
 * @param	txbuf	待发送数据缓冲区
 * @retval	发送完成情况
*/
u8 NRF24L01_TxPacket(u8* txbuf) {
	u8 sta;
	SPI2_SetSpeed(SPI_BaudRatePrescaler_8);//spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   
	NRF24L01_CE = 0;
	NRF24L01_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
	NRF24L01_CE = 1;//启动发送	   
	while (NRF24L01_IRQ != 0);//等待发送完成
	sta = NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值	   
	NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, sta); //清除TX_DS或MAX_RT中断标志

	if (sta & MAX_TX) {	//达到最大重发次数 
		NRF24L01_Write_Reg(FLUSH_TX, 0xff);//清除TX FIFO寄存器 
		return MAX_TX;
	}
	if (sta & TX_OK) {//发送完成
		return TX_OK;
	}
	return 0xff;//其他原因发送失败
}

/**
 * @brief	启动NRF24L01接收一次数据
 * @param	rxbuf	待接收数据缓冲区
 * @retval	接收完成情况
*/
u8 NRF24L01_RxPacket(u8* rxbuf) {
	u8 sta;
	SPI2_SetSpeed(SPI_BaudRatePrescaler_8); //spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   
	sta = NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值
	NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS, sta); //清除TX_DS或MAX_RT中断标志
	if (sta & RX_OK) {//接收到数据
		NRF24L01_Read_Buf(RD_RX_PLOAD, rxbuf, RX_PLOAD_WIDTH);//读取数据
		NRF24L01_Write_Reg(FLUSH_RX, 0xff);//清除RX FIFO寄存器 
		return 0;
	}
	return 1;//没收到任何数据
}

/**
 * @brief	检测24L01是否存在
 * @retval	TRUE 存在，FALSE 不存在
*/
boolean Is_NRF24L01_Exist(void) {
	u8 buf[5] = { 0XA5,0XA5,0XA5,0XA5,0XA5 };
	u8 i;
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4); //spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   	 
	NRF24L01_Write_Buf(NRF_WRITE_REG + TX_ADDR, buf, 5);//写入5个字节的地址.	
	NRF24L01_Read_Buf(TX_ADDR, buf, 5); //读出写入的地址  
	for (i = 0; i < 5; i++) if (buf[i] != 0XA5) break;	//如果寄存器内的值不为0xA5就跳出
	if (i != 5) return FALSE;	//如果没把5个字节都查一遍，说明检测24L01错误，返回1
	return TRUE;
}

