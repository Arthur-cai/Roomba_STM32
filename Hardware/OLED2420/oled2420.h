#ifndef __OLED2420_H
#define __OLED2420_H 
#include "..\..\Basic\sys\sys.h"
//#include "stdlib.h"	
/*PF0~7*/

//-----------------OLED端口定义---------------- 
//SCL时钟线 PF0
#define OLED_SCL_IO_OUT() {GPIOF->CRL &= 0xFFFFFFF0; GPIOF->CRL |= (u32)3<<0*4;} //PF0
//SDA数据线 PF1
#define OLED_SDA_IO_OUT() {GPIOF->CRL &= 0xFFFFFF0F; GPIOF->CRL |= (u32)3<<1*4;} //PF1
#define OLED_SDA_IO_IN() {GPIOF->CRL &= 0xFFFFFF0F; GPIOF->CRL |= (u32)8<<1*4;}
//RES复位 PF2
#define OLED_RES_IO_OUT() {GPIOF->CRL &= 0xFFFFF0FF; GPIOF->CRL |= (u32)3<<2*4;} //PF2
//DC（数据/命令）选择脚 PF3
#define OLED_DC_IO_OUT() {GPIOF->CRL &= 0xFFFF0FFF; GPIOF->CRL |= (u32)3<<3*4;} //PF3
//CS片选 PF4
#define OLED_CS_IO_OUT() {GPIOF->CRL &= 0xFFF0FFFF; GPIOF->CRL |= (u32)3<<4*4;} //PF4

#define OLED_SCL_OUT PFout(0)
#define OLED_SDA_OUT PFout(1)
#define OLED_SDA_IN PFin(1)
#define OLED_RES_OUT PFout(2)
#define OLED_DC_OUT PFout(3)
#define OLED_CS_OUT PFout(4)


#define WRITE_CMD  0	//写命令
#define WRITE_DATA 1	//写数据

/**
 * @brief	反显函数
 * @param	mode 0正常, 1反色
*/
void OLED_ColorTurn(u8 i);

/**
 * @brief	屏幕旋转180°
 * @param	mode 0正常，1旋转
 */
void OLED_DisplayTurn(u8 i);

/**
 * @brief	向OLED写1个字节
 * @param	data 数据或命令
 * @param	CmdOrData 选择写入命令或者数据，
 * @arg		WRITE_CMD  0	//写命令
			WRITE_DATA 1	//写数据
 */
void OLED_WR_Byte(u8 dat, u8 mode);

/**
 * @brief	开启OLED显示
 */
void OLED_Display_On(void);		

/**
 * @brief	关闭OLED显示
 */
void OLED_Display_Off(void);

/**
 * @brief	更新OLED显存
 */
void OLED_Refresh(void);	

/**
 * @brief	清屏
 */
void OLED_Clear(void);

/**
 * @brief	画点
 * @param	x(0~127)
 * @param	y(0~63)
 * @param	FillOrNot	是否填充，1填充，0清空
 */
void OLED_DrawPoint(u8 x, u8 y, u8 t);

/**
 * @brief	画线
 * @param	x1，y1	起点坐标
 * @param	x2，y2	终点坐标
 * @param	mode	是否填充，1填充，0清空
 */
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode);

/**
 * @brief	画圆
 * @param	x，y		圆心坐标
 * @param	r		半径
 */
void OLED_DrawCircle(u8 x, u8 y, u8 r);

/**
 * @brief	在指定位置显示一个字符,包括部分字符
 * @param	x，y		起点坐标
 * @param	size	字体大小（6x8/6x12/8x16/12x24）
 * @param	mode	是否填充，1填充，0清空
 */
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode);

/**
 * @brief	在指定位置显示一个字符串
 * @param	x，y		起点坐标
 * @param	chr		字符串
 * @param	size	字体大小（6x8/6x12/8x16/12x24）
 * @param	mode	是否填充，1填充，0清空
 */
void OLED_ShowString(u8 x, u8 y, u8* chr, u8 size, u8 mode);

/**
 * @brief	在指定位置显示一个数字
 * @param	x，y		起点坐标
 * @param	num		数字
 * @param	len		数字位数
 * @param	size	字体大小（6x8/6x12/8x16/12x24）
 * @param	mode	是否填充，1填充，0清空
 */
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size, u8 mode);

/**
 * @brief	在指定位置显示一个汉字
 * @param	x，y		起点坐标
 * @param	num		汉字的序号
 * @param	size	字体大小（6x8/6x12/8x16/12x24）
 * @param	mode	是否填充，1填充，0清空
 */
void OLED_ShowChinese(u8 x, u8 y, u8 num, u8 size, u8 mode);

/**
 * @brief	OLED初始化
 */
void OLED_Init(void);

void OLED_ScrollDisplay(u8 num, u8 space, u8 mode);
void OLED_ShowPicture(u8 x, u8 y, u8 sizex, u8 sizey, u8 BMP[], u8 mode);


#endif

