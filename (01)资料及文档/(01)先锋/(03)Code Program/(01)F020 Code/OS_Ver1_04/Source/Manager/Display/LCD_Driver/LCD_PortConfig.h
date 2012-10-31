//note:如果您使用C语言编写LCD的底层接口程序的话，这里的定义才会有用
// this file for MCU I/O port or the orther`s hardware config 
// for LCD Display 
#include "c8051f020.h"
#include "intrins.h"			//包含此头文件可直接操作内核的寄存器以及一些定义好的宏

sbit LCD_CS	 =		P2^2;
sbit LCD_RES =		P2^3;
sbit LCD_A0  =		P2^4;
sbit LCD_CLK =		P2^5;
sbit LCD_SDI =		P2^6;

#define LCD_Ctrl_GPIO()		//PINSEL1 &= ~(0x00003cfc)
#define LCD_Ctrl_Out()		//IODIR0 |= (LCD_CS+LCD_RE+LCD_A0+LCD_CLK+LCD_SDI)
#define LCD_Ctrl_Set(n)		//IOSET0 = n
#define LCD_Ctrl_Clr(n)		//IOCLR0 = n

//#define LCD_CS				(0x01<<22)
#define LCD_CS_SET()		LCD_CS = 1
#define LCD_CS_CLR()		LCD_CS = 0

//#define LCD_RE				(0x01<<21)
#define LCD_RE_SET()		LCD_RES = 1
#define LCD_RE_CLR()		LCD_RES = 0

//#define LCD_A0				(0x01<<19)
#define LCD_A0_SET()		LCD_A0 = 1
#define LCD_A0_CLR()		LCD_A0 = 0

//#define LCD_CLK				(0x01<<18)
#define LCD_CLK_SET()		LCD_CLK = 1
#define LCD_CLK_CLR()		LCD_CLK = 0

//#define LCD_SDI				(0x01<<17)
#define LCD_SDI_SET()		LCD_SDI = 1
#define LCD_SDI_CLR()		LCD_SDI = 0


