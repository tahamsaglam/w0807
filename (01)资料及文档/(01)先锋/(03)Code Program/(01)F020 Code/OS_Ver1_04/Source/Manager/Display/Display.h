/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	模型		: 
	机能		: 
	作者		: 
	备注		:
			 
*****************************************************************/
#ifndef _DISPLAY_H_
#define _DISPLAY_H_
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
函数声明
===============================================================*/
/**************************************
Funcname	: Display_reset_init
Contents 	: 显示初始化函数
Input		: void
Return		: void
Remart		:	 

***************************************/
public void Display_reset_init( void );
/**************************************
Funcname	: disp_function
Contents 	: 显示函数
Input		: void
Return		: void
Remart		:	 

***************************************/
public void disp_function( void );
#endif
/*===================================================================
Version	|	Date		|	Author	|	Contents  					|
---------------------------------------------------------------------
V1.00		2012-08-19		ZQ			New code write.

====================================================================*/

