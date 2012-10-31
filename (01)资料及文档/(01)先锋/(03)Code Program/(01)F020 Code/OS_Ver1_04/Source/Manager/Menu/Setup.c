/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	模型		: 全模型
	机能		: 菜单功能实现方法文件
	作者		: Zq
	备注		: 产品的设定，menu相关
*****************************************************************/
/*===============================================================
头文件声明
================================================================*/
#include "../../Platform/CPU/Comm.h"
#include "./Setup.h"
#include "./Menu.h"
#include "../../Controller/Key/keycode.h"		/* 按键定义*/

#define KEY_DEBUG

#ifdef KEY_DEBUG
#include "../Display/Display.h"
#endif
/*==============================================================
Global RAM声明
===============================================================*/




/*===============================================================
private RAM声明
===============================================================*/
private _UINT setup_last_menu;		/*系统设定最后菜单记忆变量*/
private _UBYTE setup_key_buf;		/*Setup Menu最后按键存储变量*/
/*===============================================================
private函数声明
===============================================================*/
private void sys_setup_menu_keychange( unsigned char newkey );
private unsigned char sys_system_setup_enable_check( void );
/*==============================================================
系统SETUP菜单登录的结构体
===============================================================*/
 const MenuTbl sys_system_setup_mode ={	
	 &setup_last_menu
	,&sys_system_setup_enable_check
	,MENUST_SETUP_START
	,MENUST_SETUP_END
};


/**************************************
Funcname	: set_menu_reset_init
Contents 	: 相关变量的初始值定义
Input		: 无
Return		: 无
Remart		: 	 

***************************************/
public void set_menu_reset_init( void )
{
	 setup_key_buf = 0x00;			/*Setup最后按键变量初始化*/
	 setup_last_menu = 0x00;		/*Setup Menu最后菜单变量初始化*/
}


/**************************************
Funcname		: sys_system_setup_enable_check
Contents 	: 系统SETUP Menu使能函数
Input		: void
Return		: 1:Ok;0:NG
Remart		:	 

***************************************/
private unsigned char sys_system_setup_enable_check( void )
{
	unsigned char enable_flag = 1;
	return ( enable_flag );
}



/**************************************
Funcname	: setup_menu_keychange
Contents 	: Setup Menu按键处理函数
Input		: newkey
Return		: void
Remart		:	 
               按键操作函数。
               
***************************************/
public void setup_menu_keychange( unsigned char newkey )
{

	if( menust.one.attribute == MENUATTR_OFF ){
		switch( newkey )
		{
			case KEY0_KEY:
			{
				PutChar(16,4,0); 
			}
			break;
			case KEY1_KEY:
			{
				PutChar(16,4,1); 
			}
			break;
			case KEY2_KEY:
			{
				PutChar(16,4,2); 
			}
			break;
			default:
				break;
					
		}
	}
}


/**************************************
Funcname	: sys_setup_menu_keychange
Contents 	: system setup menu中按键动作
Input		: newkey
Return		: void
Remart		:	 

***************************************/
private void sys_setup_menu_keychange( unsigned char newkey )
{
	switch( newkey )
	{

	}
}


/**************************************
Funcname	: menu_disp_mode_set
Contents 	: disp mode set
Input		: newkey
Return		: void
Remart		: 返回当前的显示mode	 

***************************************/
public unsigned char  menu_disp_mode_set( void )
{

	private char disp_mode_temp = 0x00;
	switch( menust.all )
	{

	}
	return( disp_mode_temp );
}


/*===================================================================
Version	|	Date		|	Author	|	Contents  					|
---------------------------------------------------------------------
V1.00		2012-08-19		ZQ			New code write.

====================================================================*/

