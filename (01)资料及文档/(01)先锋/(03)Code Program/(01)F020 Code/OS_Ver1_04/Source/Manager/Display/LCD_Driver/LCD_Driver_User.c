//========================================================================
// 文件名: LCD_Driver_User.c
// 描  述: 底层接口驱动扩展程序集
//========================================================================
#include "LCD_Config.h"
#include "LCD_PortConfig.h"

//========================================================================
// 函数: void LCD_DataWrite(unsigned int Data)
// 描述: 写一个字节的显示数据至LCD中的显示缓冲RAM当中
// 参数: Data 写入的数据 
// 返回: 无
//========================================================================
void LCD_DataWrite(LCDBYTE Dat)
{
	unsigned char Num;
	LCD_CS_CLR();// = 0;
	LCD_A0_SET();// = 1;
	for(Num=0;Num<8;Num++)
	{
		if((Dat&0x80) == 0)	LCD_SDI_CLR();// = 0;
		else LCD_SDI_SET();// = 1;
		Dat = Dat << 1;
		LCD_CLK_CLR();// = 0;
		LCD_CLK_SET();// = 1;
	}
	LCD_CS_SET();// = 1;
}
//========================================================================
// 函数: void LCD_RegWrite(unsigned char Command)
// 描述: 写一个字节的数据至LCD中的控制寄存器当中
// 参数: Command		写入的数据，低八位有效（byte） 
// 返回: 无
//========================================================================
void LCD_RegWrite(LCDBYTE Command)
{
	unsigned char Num;
	LCD_CS_CLR();// = 0;
	LCD_A0_CLR();// = 0;
	for(Num=0;Num<8;Num++)
	{
		if((Command&0x80) == 0)	LCD_SDI_CLR();// = 0;
		else LCD_SDI_SET();// = 1;
		Command = Command << 1;
		LCD_CLK_CLR();// = 0;
		LCD_CLK_SET();// = 1;
	}
	LCD_CS_SET();// = 1;
}
//========================================================================
// 函数: void LCD_Fill(unsigned int Data)
// 描述: 会屏填充以Data的数据至各点中
// 参数: Data   要填充的颜色数据
// 返回: 无
// 备注: 仅在LCD初始化程序当中调用
//========================================================================
void LCD_Fill(LCDBYTE Data)
{
	unsigned char i,j;
	unsigned char uiTemp;
	uiTemp = Dis_Y_MAX;
	uiTemp = uiTemp>>3;
	for(i=0;i<=uiTemp;i++)								//往LCD中填充初始化的显示数据
	{
		LCD_RegWrite(0xb0+i);
		LCD_RegWrite(0x00);//1);
		LCD_RegWrite(0x10);
		for(j=0;j<=Dis_X_MAX;j++)
		{
			LCD_DataWrite(Data);
		}
	}
}
//========================================================================
// 函数:TimeDelay
// 描述: LCD初始化程序，在里面会完成LCD初始所需要设置的许多寄存器，具体如果
//		 用户想了解，建议查看DataSheet当中各个寄存器的意义
// 参数: 无 
// 返回: 无
// 备注:
//========================================================================
//延时程序

void TimeDelay(int Time)
{
	int i;
	while(Time > 0)
	{
		for(i = 0;i < 8000;i++)
		{
			;//_nop_();
		}
		Time --;
	}
}
//========================================================================
// 函数: LCD_Init
// 描述: LCD初始化
// 参数: void
// 返回: void
// 备注: 
//========================================================================
extern void LCD_Init(void)
{
	//LCD驱动所使用到的端口的初始化（如果有必要的话）
	LCD_Ctrl_GPIO();
	LCD_Ctrl_Out();

	TimeDelay(200);
	LCD_RE_CLR();// = 0;
	TimeDelay(200);
	LCD_RE_SET();// = 1;
	TimeDelay(20);
		
	LCD_RegWrite(M_LCD_ON);							//LCD On
	LCD_RegWrite(M_LCD_POWER_ALL);					//设置上电控制模式
	
	LCD_RegWrite(M_LCD_ELE_VOL);					//电量设置模式（显示亮度）
	LCD_RegWrite(0x1f);								//指令数据0x0000~0x003f
	
	LCD_RegWrite(M_LCD_VDD_SET);					//V5内部电压调节电阻设置
	LCD_RegWrite(M_LCD_VDD);						//LCD偏压设置，V3时选
	
	LCD_RegWrite(M_LCD_COM_REV);					//Com 扫描方式设置
	LCD_RegWrite(M_LCD_SEG_NOR);					//Segment方向选择
	LCD_RegWrite(M_LCD_ALL_LOW);					//全屏点亮/变暗指令
	LCD_RegWrite(M_LCD_ALL_NOR);					//正向反向显示控制指令
	
	LCD_RegWrite(M_LCD_STATIC_OFF);					//关闭静态指示器
	LCD_RegWrite(0x00);								//指令数据
	
	LCD_RegWrite(M_LCD_BEGIN_LINE);					//设置显示起始行对应RAM
	LCD_Fill(LCD_INITIAL_COLOR);
//	ClrScreen(0);									/*清屏*/
}

