/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	模型		: 
	机能		: 
	作者		: 
	备注		:
			 
*****************************************************************/
/*===============================================================
头文件声明
================================================================*/
#include "../../Platform/CPU/Comm.h"		/*公共定义头文件*/
#include "./LCD_Driver/LCD_Dis.h"
#include "./LCD_Driver/LCD_Driver_User.h"
/*==============================================================
Global RAM声明
===============================================================*/




/*===============================================================
Static RAM声明
===============================================================*/


/*===============================================================
函数声明
===============================================================*/


/**************************************
Funcname	: Display_reset_init
Contents 	: 显示初始化函数
Input		: void
Return		: void
Remart		:	 

***************************************/
public void Display_reset_init( void )
{
	LCD_Init( );	/*LCD初始化*/
	ClrScreen(0);
}

/**************************************
Funcname	: disp_function
Contents 	: 显示函数
Input		: void
Return		: void
Remart		:	 

***************************************/
public void disp_function( void )
{
	FontSet(2,0xff);				//选择8X16的ASCII字符
	PutChar(0,0,2); 
	PutChar(16,0,1); 
}






/*===================================================================
Version	|	Date		|	Author	|	Contents  					|
---------------------------------------------------------------------
V1.00		2012-08-19		ZQ			New code write.

====================================================================*/

