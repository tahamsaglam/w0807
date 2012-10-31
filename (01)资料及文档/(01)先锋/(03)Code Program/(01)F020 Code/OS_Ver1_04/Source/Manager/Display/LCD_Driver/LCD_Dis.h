#include "LCD_Config.h"
//LCD 初始化函数，一般用于LCD屏的寄存器初始化
extern void LCD_Init(void);	//在LCD_Driver_User.c中定义的函数，如有必要该函数还会包含端口初始化、复位等操作


//标准字符设置，包括两型号ASCII码的大小尺寸，以及字体颜色的设置
extern void FontSet(unsigned char Font_NUM,LCDBYTE Color);
//于x、y的坐标上写入一个标准字符
extern void PutChar(DOTBYTE x,DOTBYTE y,char a);
//于x、y的坐标为起始写入一串标准字符串
extern void PutString(DOTBYTE x,DOTBYTE y,char *p);
//清屏函数，执行全屏幕清除或填充前景色
extern void ClrScreen(unsigned char Mode);

//以下函数以及变量的声明一般建议用户不要调用，仅供高级用户在自行编写特殊显示效果以及特性应用程序时使用
//
//extern unsigned char code Asii16[];		//8X16的ASII字符库
//extern unsigned char code GB32[];		//自定义的32X29汉字库
//extern unsigned char code GB48[];		//自定义的48X55汉字库

extern DOTBYTE X_Witch;					//字符写入时的宽度
extern DOTBYTE Y_Witch;					//字符写入时的高度
extern DOTBYTE Font_Wrod;				//字体的大
extern unsigned char *Char_TAB;		//字库指针
//extern unsigned char Plot_Mode;				//绘图模式
extern LCDBYTE BMP_Color;
extern LCDBYTE Char_Color;

//extern void Pos_Switch(unsigned int * x,unsigned int * y);
//extern void Writ_Dot(int x,int y,unsigned int Color);
//extern void Line_f(int s_x,int s_y,int e_x);
//extern unsigned int Get_Dot(int x,int y);
//extern void Clear_Dot(int x,int y);
//extern void Set_Dot_Addr(int x,int y);
