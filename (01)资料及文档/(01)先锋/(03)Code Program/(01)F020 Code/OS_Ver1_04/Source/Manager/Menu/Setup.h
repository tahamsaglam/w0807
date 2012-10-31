/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	模型		: 
	机能		: 
	作者		: 
	备注		:
			 
*****************************************************************/
#ifndef SET_MENU_H_INCLUDE
#define SET_MENU_H_INCLUDE

/*===============================================================
头文件声明
================================================================*/
#include "../../Platform/CPU/Comm.h"
#include "./menu.h"
/**************************************
Funcname	: setup_menu_keychange
Contents 	: 新按键更新
Input		: newkey
Return		: void
Remart		:	 
               按键操作函数。
               
***************************************/
public void setup_menu_keychange( unsigned char newkey );


/**************************************
Funcname	: set_menu_reset_init
Contents 	: 相关变量的初始值定义
Input		: 无
Return		: 无
Remart		: 	 

***************************************/
public void set_menu_reset_init( void );


/**************************************
Funcname		: menu_disp_mode_set
Contents 	: disp mode set
Input		: newkey
Return		: void
Remart		: 返回当前的显示mode	 

***************************************/
public unsigned char  menu_disp_mode_set( void );

enum{
	MENUST_SETUP_START = MENUATTR_SYSTEM_SETUP * 0x100
,	MENUST_SETUP_NUMBER 			/*Menu set number	*/
,	MENUST_SETUP_END
};


#endif/*SET_MENU_H_INCLUDE*/

/*======================================================
Version	Date			Remart.
--------------------------------------------------------
Ver1_00	2012-07-22	新规做成 by 周前


========================================================*/


