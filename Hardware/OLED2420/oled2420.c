#include "oled2420.h"
#include "stdlib.h"
#include "oledfont.h"  	
//#include "bmp.h"
#include "..\..\Basic\delay\delay.h"

//显存
u8 OLED_GRAM[144][8]; //144列，8大行

/**
 * @brief	反显函数 
*/
void OLED_ColorTurn(u8 mode) {
	if (mode) OLED_WR_Byte(0xA7, WRITE_CMD);//反色显示
	else OLED_WR_Byte(0xA6, WRITE_CMD);//正常显示
}

/**
 * @brief	屏幕旋转180°
 */
void OLED_DisplayTurn(u8 mode) {
	if (mode) {
		OLED_WR_Byte(0xC0, WRITE_CMD);//反转显示
		OLED_WR_Byte(0xA0, WRITE_CMD);
	}
	else {
		OLED_WR_Byte(0xC8, WRITE_CMD);//正常显示
		OLED_WR_Byte(0xA1, WRITE_CMD);
	}
}

/**
 * @brief	向OLED写1个字节
 */
void OLED_WR_Byte(u8 data, u8 CmdOrData) {
	u8 i;
	//设置IO方向为输出
	OLED_DC_IO_OUT();
	OLED_CS_IO_OUT();
	OLED_SCL_IO_OUT();
	OLED_SDA_IO_OUT();
	//如果cmd为1，说明是写数据
	if (CmdOrData == WRITE_DATA) OLED_DC_OUT = 1; //把DC（数据/命令）位设置为1
	//如果cmd为0，说明是写命令
	else if (CmdOrData == WRITE_CMD) OLED_DC_OUT = 0; //把DC（数据/命令）位设置为0
	//片选为0，选择器件
	OLED_CS_OUT = 0;
	//开始传输数据
	for (i = 0; i < 8; i++) {
		OLED_SCL_OUT = 0; //SCL先低电平
		OLED_SDA_OUT = (data & 0x80) >> 7; //传输高位
		data <<= 1;  //数据左移1位，等待下一次传输
		OLED_SCL_OUT = 1; //SCL再高电平
	}
	OLED_CS_OUT = 1;	//取消片选
	OLED_DC_OUT = 1;	//DC置高
}

/**
 * @brief	开启OLED显示
 */
void OLED_Display_On(void) {
	OLED_WR_Byte(0x8D, WRITE_CMD);//电荷泵使能
	OLED_WR_Byte(0x14, WRITE_CMD);//开启电荷泵
	OLED_WR_Byte(0xAF, WRITE_CMD);//点亮屏幕
}

/**
 * @brief	关闭OLED显示
 */
void OLED_Display_Off(void) {
	OLED_WR_Byte(0x8D, WRITE_CMD);//电荷泵使能
	OLED_WR_Byte(0x10, WRITE_CMD);//关闭电荷泵
	OLED_WR_Byte(0xAE, WRITE_CMD);//关闭屏幕
}

/**
 * @brief	更新OLED显存
 */
void OLED_Refresh(void) {
	u8 i, n;
	for (i = 0; i < 8; i++) {//共8大行
		OLED_WR_Byte(0xb0 + i, WRITE_CMD); //设置行起始地址
		OLED_WR_Byte(0x00, WRITE_CMD);   //设置低列起始地址
		OLED_WR_Byte(0x10, WRITE_CMD);   //设置高列起始地址
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(OLED_GRAM[n][i], WRITE_DATA);
	}
}

/**
 * @brief	清屏
 */
void OLED_Clear(void) {
	u8 i, n;
	for (i = 0; i < 8; i++) {
		for (n = 0; n < 128; n++)
			OLED_GRAM[n][i] = 0;//清除所有数据
	}
	OLED_Refresh();//更新显示
}

/**
 * @brief	画点
 */
void OLED_DrawPoint(u8 x, u8 y, u8 FillOrNot) {
	u8 i, m, n;
	i = y / 8;
	m = y % 8;
	n = 1 << m;
	if (FillOrNot) OLED_GRAM[x][i] |= n;
	else {
		OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
		OLED_GRAM[x][i] |= n;
		OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
	}
}

/**
 * @brief	画线
 */
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode) {
	u16 t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = x2 - x1; //计算坐标增量 
	delta_y = y2 - y1;
	uRow = x1;//画线起点坐标
	uCol = y1;
	if (delta_x > 0) incx = 1; //设置单步方向 
	else if (delta_x == 0) incx = 0;//垂直线 
	else incx = -1; delta_x = -delta_x;

	if (delta_y > 0) incy = 1;
	else if (delta_y == 0) incy = 0;//水平线 
	else incy = -1; delta_y = -delta_x;

	if (delta_x > delta_y) distance = delta_x; //选取基本增量坐标轴 
	else distance = delta_y;

	for (t = 0; t < distance + 1; t++) {
		OLED_DrawPoint(uRow, uCol, mode);//画点
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance) {
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance) {
			yerr -= distance;
			uCol += incy;
		}
	}
}

/**
 * @brief	画圆
 */
void OLED_DrawCircle(u8 x, u8 y, u8 r) {
	int a, b, num;
	a = 0;
	b = r;
	while (2 * b * b >= r * r) {
		OLED_DrawPoint(x + a, y - b, 1);
		OLED_DrawPoint(x - a, y - b, 1);
		OLED_DrawPoint(x - a, y + b, 1);
		OLED_DrawPoint(x + a, y + b, 1);

		OLED_DrawPoint(x + b, y + a, 1);
		OLED_DrawPoint(x + b, y - a, 1);
		OLED_DrawPoint(x - b, y - a, 1);
		OLED_DrawPoint(x - b, y + a, 1);

		a++;
		num = (a * a + b * b) - r * r; //计算画的点离圆心的距离
		if (num > 0) {
			b--;
			a--;
		}
	}
}

/**
 * @brief	在指定位置显示一个字符,包括部分字符
 */
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode) {
	u8 i, m, temp, size2, chr1;
	u8 x0 = x, y0 = y;
	if (size == 8) size2 = 6;
	else size2 = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);  //得到字体一个字符对应点阵集所占的字节数
	chr1 = chr - ' ';  //计算偏移后的值
	for (i = 0; i < size2; i++) {
		if (size == 8) temp = oled_asc2_0806[chr1][i]; //调用0806字体
		else if (size == 12) temp = oled_asc2_1206[chr1][i]; //调用1206字体
		else if (size == 16) temp = oled_asc2_1608[chr1][i]; //调用1608字体
		else if (size == 24) temp = oled_asc2_2412[chr1][i]; //调用2412字体
		else return;

		for (m = 0; m < 8; m++) {
			if (temp & 0x01) OLED_DrawPoint(x, y, mode);
			else OLED_DrawPoint(x, y, !mode);
			temp >>= 1;
			y++;
		}
		x++;
		if ((size != 8) && ((x - x0) == size / 2)) {
			x = x0; y0 = y0 + 8;
		}
		y = y0;
	}
	//OLED_Refresh();
}

/**
 * @brief	在指定位置显示一个字符串
 */
void OLED_ShowString(u8 x, u8 y, u8* chr, u8 size, u8 mode) {
	while ((*chr >= ' ') && (*chr <= '~')) { //判断是不是非法字符!
		OLED_ShowChar(x, y, *chr, size, mode);
		if (size == 8)x += 6;
		else x += size / 2;
		chr++;
	}
}

/**
 * @brief	m^n
 */
u32 OLED_Pow(u8 m, u8 n) {
	u32 result = 1;
	while (n--) {
		result *= m;
	}
	return result;
}

/**
 * @brief	在指定位置显示一个数字
 */
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size, u8 mode) {
	u8 t, temp, m = 0;
	//u8 flag = 0;

	if (size == 8) m = 2;
	for (t = 0; t < len; t++) {
		temp = (num / OLED_Pow(10, len - t - 1)) % 10;

		if (temp == 0) OLED_ShowChar(x + (size / 2 + m) * t, y, '0', size, mode);
		else OLED_ShowChar(x + (size / 2 + m) * t, y, temp + '0', size, mode);
	}
}

/**
 * @brief	在指定位置显示一个汉字
 */
void OLED_ShowChinese(u8 x, u8 y, u8 num, u8 size, u8 mode) {
	u8 m, temp;
	u8 x0 = x, y0 = y;
	u16 i, size3 = (size / 8 + ((size % 8) ? 1 : 0)) * size;  //得到字体一个字符对应点阵集所占的字节数
	for (i = 0; i < size3; i++) {
		if (size == 16) temp = Hzk1[num][i]; //调用16*16字体
		else if (size == 24) temp = Hzk2[num][i]; //调用24*24字体
		else if (size == 32) temp = Hzk3[num][i]; //调用32*32字体
		else if (size == 64) temp = Hzk4[num][i]; //调用64*64字体
		else return;

		for (m = 0; m < 8; m++) {
			if (temp & 0x01) OLED_DrawPoint(x, y, mode);
			else OLED_DrawPoint(x, y, !mode);
			temp >>= 1;
			y++;
		}
		x++;
		if ((x - x0) == size) {
			x = x0; y0 = y0 + 8;
		}
		y = y0;
	}
}

/**
 * @brief	OLED初始化
 */
void OLED_Init(void) {
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);	 //使能端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | \
		GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //速度50MHz
	GPIO_Init(GPIOF, &GPIO_InitStructure);	  //初始化GPIOF
	GPIO_SetBits(GPIOF, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | \
		GPIO_Pin_4);

	OLED_RES_IO_OUT();
	OLED_RES_OUT = 0;
	delay_ms(200);
	OLED_RES_OUT = 1;

	OLED_WR_Byte(0xae, WRITE_CMD);
	OLED_WR_Byte(0x00, WRITE_CMD);
	OLED_WR_Byte(0x10, WRITE_CMD);
	OLED_WR_Byte(0xd5, WRITE_CMD);
	OLED_WR_Byte(0x80, WRITE_CMD);
	OLED_WR_Byte(0xa8, WRITE_CMD);
	OLED_WR_Byte(0x3f, WRITE_CMD);
	OLED_WR_Byte(0xd3, WRITE_CMD);
	OLED_WR_Byte(0x00, WRITE_CMD);
	OLED_WR_Byte(0xb0, WRITE_CMD);
	OLED_WR_Byte(0x40, WRITE_CMD);
	OLED_WR_Byte(0x8d, WRITE_CMD);
	OLED_WR_Byte(0x14, WRITE_CMD);
	OLED_WR_Byte(0xa1, WRITE_CMD);
	OLED_WR_Byte(0xc8, WRITE_CMD);
	OLED_WR_Byte(0xda, WRITE_CMD);
	OLED_WR_Byte(0x12, WRITE_CMD);
	OLED_WR_Byte(0x81, WRITE_CMD);
	OLED_WR_Byte(0xbf, WRITE_CMD);
	OLED_WR_Byte(0xd9, WRITE_CMD);
	OLED_WR_Byte(0xf1, WRITE_CMD);
	OLED_WR_Byte(0xdb, WRITE_CMD);
	OLED_WR_Byte(0x30, WRITE_CMD);
	OLED_WR_Byte(0x20, WRITE_CMD);
	OLED_WR_Byte(0x00, WRITE_CMD);
	OLED_WR_Byte(0xa4, WRITE_CMD);
	OLED_WR_Byte(0xa6, WRITE_CMD);
	OLED_WR_Byte(0xaf, WRITE_CMD);
	//初始先清屏
	OLED_Clear();
}

//num 显示汉字的个数
//space 每一遍显示的间隔
//mode:0,反色显示;1,正常显示
void OLED_ScrollDisplay(u8 num, u8 space, u8 mode) {
	u8 i, n, t = 0, m = 0, r;
	while (1) {
		if (m == 0) {
			OLED_ShowChinese(128, 24, t, 16, mode); //写入一个汉字保存在OLED_GRAM[][]数组中
			t++;
		}
		if (t == num) {
			for (r = 0; r < 16 * space; r++) {      //显示间隔
				for (i = 1; i < 144; i++) {
					for (n = 0; n < 8; n++) {
						OLED_GRAM[i - 1][n] = OLED_GRAM[i][n];
					}
				}
				OLED_Refresh();
			}
			t = 0;
		}
		m++;
		if (m == 16) { m = 0; }
		for (i = 1; i < 144; i++) {  //实现左移
			for (n = 0; n < 8; n++) {
				OLED_GRAM[i - 1][n] = OLED_GRAM[i][n];
			}
		}
		OLED_Refresh();
	}
}

//x,y：起点坐标
//sizex,sizey,图片长宽
//BMP[]：要写入的图片数组
//mode:0,反色显示;1,正常显示
void OLED_ShowPicture(u8 x, u8 y, u8 sizex, u8 sizey, u8 BMP[], u8 mode) {
	u16 j = 0;
	u8 i, n, temp, m;
	u8 x0 = x, y0 = y;
	sizey = sizey / 8 + ((sizey % 8) ? 1 : 0);
	for (n = 0; n < sizey; n++) {
		for (i = 0; i < sizex; i++) {
			temp = BMP[j];
			j++;
			for (m = 0; m < 8; m++) {
				if (temp & 0x01)OLED_DrawPoint(x, y, mode);
				else OLED_DrawPoint(x, y, !mode);
				temp >>= 1;
				y++;
			}
			x++;
			if ((x - x0) == sizex) {
				x = x0;
				y0 = y0 + 8;
			}
			y = y0;
		}
	}
}


