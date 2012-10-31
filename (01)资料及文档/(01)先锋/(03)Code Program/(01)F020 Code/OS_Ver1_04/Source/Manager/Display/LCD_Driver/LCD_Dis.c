//========================================================================
// 文件名: LCD_Dis.c
// 描  述: 串行接口版LCD用户接口层程序集
//========================================================================
#include "LCD_Driver_User.h"					//LCD底层驱动声明头文件
#include "LCD_Config.h"							//LCD的配置文件，比如坐标轴是否倒置等的定义

extern FLASH Asii0608[];			//6X8的ASII字符库
extern FLASH Asii0816[];			//8X16的ASII字符库
extern FLASH GB1616[];				//16*16自定义的汉字库

DOTBYTE X_Witch;							//字符写入时的宽度
DOTBYTE Y_Witch;							//字符写入时的高度
DOTBYTE Font_Wrod;						//字体的每个字模占用多少个存储单元数
FLASH *Char_TAB;						//字库指针
LCDBYTE BMP_Color;
LCDBYTE Char_Color;

BYTE	Font_type=0;								//标识字符类型

//========================================================================
// 函数: void FontSet(BYTE Font_NUM,LCDBYTE Color)
// 描述: 文本字体设置
// 参数: Font_NUM 字体选择,以驱动所带的字库为准
//		 Color  文本颜色,仅作用于自带字库  
// 返回: 无
//========================================================================
void FontSet(BYTE Font_NUM,LCDBYTE Color)
{
	switch(Font_NUM)
	{
		case 0: Font_Wrod = 16;	//ASII字符A	8*16
				X_Witch = 8;
				Y_Witch = 2;
				Char_Color = Color;
				Char_TAB = Asii0816;
				Font_type = 1;
		break;	  
		case 1: Font_Wrod = 6;	//ASII字符B	 6*8
				X_Witch = 6;
				Y_Witch = 1;
				Char_Color = Color;
				Char_TAB = Asii0608;
				Font_type = 1;
		break;		
		case 2: Font_Wrod = 32;	//汉字A	   16*16
				X_Witch = 16;
				Y_Witch = 2;
				Char_Color = Color;
				Char_TAB = GB1616;
				Font_type = 0;
		break;
		default: break;
	}
}
//========================================================================
// 函数: void PutChar(DOTBYTE x,DOTBYTE y,char a)
// 描述: 写入一个标准字符
// 参数: x  X轴坐标     y  Y轴坐标(0~7页)
//		 a  要显示的字符在字库中的偏移量  
// 返回: 无
// 备注: ASCII字符可直接输入ASCII码即可
//========================================================================
void PutChar(DOTBYTE x,DOTBYTE y,char a)       
{
	unsigned char i,j;
	FLASH *p_data; 
	if(Font_type==1)
		p_data = Char_TAB + (a-32)*Font_Wrod;
	else
		p_data = Char_TAB + a*Font_Wrod;	//要写字符的首地址
	x = x;//+1;							//Edit by xinqiang
	for(j=0;j<Y_Witch;j++)
	{
		if((y+j) < (Dis_Y_MAX/8))
			LCD_RegWrite(y+j+0xb0);
		LCD_RegWrite(((x&0xf0) >> 4)|0x10);	//设置初始地址 
		LCD_RegWrite(x&0x0f);
		for(i=0;i<X_Witch;i++)
		{  
			if(((x+i) < Dis_X_MAX)&&((y+j) < (Dis_Y_MAX/8)))
				LCD_DataWrite(*p_data++);
		}
	}
}

//========================================================================
// 函数: void PutString(DOTBYTE x,DOTBYTE y,char *p)
// 描述: 在x、y为起始坐标处写入一串标准字符
// 参数: x  X轴坐标     y  Y轴坐标(0~7页)
//		 p  要显示的字符串  
// 返回: 无
// 备注: 仅能用于自带的ASCII字符串显示
//========================================================================
void PutString(DOTBYTE x,DOTBYTE y,char *p)
{
	while(*p!=0)
	{
		PutChar(x,y,*p);
		x += X_Witch;
		if((x + X_Witch) > Dis_X_MAX)
		{
			x = 0;
			if((Dis_Y_MAX - y) < Y_Witch) break;
			else y += Y_Witch;
		}
		p++;
	}
}
//========================================================================
// 函数: void ClrScreen(unsigned char Mode)
// 描述: 清屏函数，执行全屏幕清除或填充前景色
// 参数: Mode   0:全屏除屏
//				1:全屏填充前景色
// 返回: 无
//========================================================================
extern void ClrScreen(unsigned char Mode)
{
	if(Mode==0)
		LCD_Fill(LCD_INITIAL_COLOR);
	else
		LCD_Fill(BMP_Color);
}
