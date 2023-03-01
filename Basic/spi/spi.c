#include "spi.h"

/**
 * @brief SPI2读写
 * @param data 用于发送的数据
 * @retval 接收到了数据就返回，没有数据失败就返回0
*/
u8 SPI2_RWByte(u8 data) {
	u8 time; //用于超时计数
	//等待发送缓存空位TXE置位
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) != SET) {
		time++;
		//超时返回0，发送缓存区还有数据，不能发送
		if (time > 200) return 0;
	}
	time = 0;
	//发送缓存空位TXE置位，可以开始发送数据了
	SPI_I2S_SendData(SPI2, data);

	//等待接收缓存区非空位RXNE置位
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != SET) {
		time++;
		//超时返回0，接收缓存区还没满，不能接收
		if (time > 200) return 0;
	}
	//接收缓存区非空位RXNE置位，接收缓存已满，可以提取数据
	return SPI_I2S_ReceiveData(SPI2);
}

/**
 * @brief SPI2初始化
*/
void SPI2_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI2_InitStructure;
	//开启GPIOB和SPI2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //设置复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//初始先拉高PB13，PB14，PB15
	GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);

	SPI2_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //设置SPI为双线全双工
	SPI2_InitStructure.SPI_Mode = SPI_Mode_Master; //设置为SPI主机
	SPI2_InitStructure.SPI_DataSize = SPI_DataSize_8b;	//设置为8bit的数据帧格式
	SPI2_InitStructure.SPI_CPOL = SPI_CPOL_High;	//设置时钟极性为1，空闲状态时为高电平
	SPI2_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; //设置时钟相位为第二个跳变沿时，采样数据
	SPI2_InitStructure.SPI_NSS = SPI_NSS_Soft;	//设置软件配置NSS从选择脚
	SPI2_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; //设置波特率预分频256
	SPI2_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//设置数据传输格式高位在前
	SPI2_InitStructure.SPI_CRCPolynomial = 7; //CRC值计算的多项式
	SPI_Init(SPI2, &SPI2_InitStructure);

	SPI_Cmd(SPI2, ENABLE); //使能SPI2
	SPI2_RWByte(0xff); //开始传输
}

/**
 * @brief 	设置SPI2的速度
 * @param	SPI_BaudRatePrescaler, 预分频值
 * @arg		SPI_BaudRatePrescaler_2, SPI_BaudRatePrescaler_8, 
 * 			SPI_BaudRatePrescaler_16, SPI_BaudRatePrescaler_256
*/
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler) {
	//检查输入参数
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));

	SPI2->CR1 &= 0xFFC7; //(0b1111111111000111) 取bit[5:3]
	SPI2->CR1 |= SPI_BaudRatePrescaler; //配置上预分频值

	SPI_Cmd(SPI2, ENABLE); //使能SPI2
}


