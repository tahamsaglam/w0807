/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	模型		: 
	机能		: 
	作者		: 
	备注		:
			 
*****************************************************************/
#ifndef MENU_H_INCLUDE
#define MENU_H_INCLUDE
/*===============================================================
头文件声明
================================================================*/
#include "../../Platform/CPU/Comm.h"		/*公共定义头文件*/
/******************************************************
Menu 联合体声明
*****************************************************/
typedef union{
	struct {
		_UBYTE status;		/*Menu状态*/
		_UBYTE attribute;	/*Menu属性*/
	}one;
	_UINT all;	
}MenuStatus;


/******************************************************
Menu 结构体声明
*****************************************************/
typedef struct{
	_UINT *last_menu;
	_UBYTE ( * enable_chk )( _UINT );
	_UINT menu_start;
	_UINT menu_end;
}MenuTbl;


/******************************************************
Menu attribute enum 定义
*****************************************************/
enum {
	MENUATTR_OFF			= 0x00
,	MENUATTR_SYSTEM_SETUP
,	MENUATTR_FUNCTION
,	MENUATTR_MENU1
,	MENUATTR_MENU2
};

/******************************************************
Menu 旋转方向定义
*****************************************************/
enum{
	MENU_DIRECTION_NORMAL = 0x00		/*正向旋转Menu*/
,	MENU_DIRECTION_REVERSE			/*反向旋转Menu*/
};


/******************************************************
Menu 宏定义
*****************************************************/
#define MENUST_MENU_OFF  0x00 			/*Menu.all清空*/
#define MENU_EXIT_TIME_30S  0xf0		/*Menu 30s自动退出时间设置*/
#define MENU_EXIT_TIME_CLEAR 0x00		/*Menu 自动退出时间清除*/


/******************************************************
Menu 阶层定义
*****************************************************/
enum{
	MENU_LAYER_UNKNOW	= 0x00	/*Menu未知层*/
,	MENU_LAYER_FIRST			/*Menu第一阶层*/
,	MENU_LAYER_SECOND			/*Menu第二阶层*/
,	MENU_LAYER_THREE			/*Menu第三阶层*/
,	MENU_RQ_LAYER_INC	= 0xf0	/*Menu阶层数增加*/
,	MENU_RQ_LAYER_DEC			/*Menu阶层数减少*/
};


/******************************************************
Menu 外部变量声明
*****************************************************/
public MenuStatus menust ;			/*menu全局变量*/


/******************************************************
Menu 外部函数声明
*****************************************************/

/**************************************
Funcname		: menu_reset_init
Contents 	: Menu初始化
Input		: void
Return		: void
Remart		: menu初始化参数	 

***************************************/
public void menu_reset_init( void );


/**************************************
Funcname		: menu_exit_requst
Contents 	: 菜单解除
Input		: void
Return		: void
Remart		: 调用该函数，解除menu

***************************************/
public void menu_exit_requst( void );


/**************************************
Funcname		: menu_get_layer
Contents		: 获得当前的阶层数
Input		: void
Return		:
			  MENU_LAYER_FIRST  第一层
			  MENU_LAYER_SECOND 第二层
			  MENU_LAYER_THREE  第三层
Remart		: 获取当前阶层
***************************************/
public _UBYTE menu_get_layer( void );


/*************************************
Funcname		: menu_set_layer
Contents		: Menu阶层数变更
Input		: request_layer范围
			  第1层到已定义的最大层
Return		: void
Remart		:
			  传入的参数:
			  MENU_RQ_LAYER_INC 当前层数加1
			  MENU_RQ_LAYER_DEC 当前层数减1
*************************************/
public void menu_set_layer( _UBYTE requst_layer );

/********************************************************
Funcname		: menu_exit_timer_reload
Contents		: 30SMenu自动退出时间重载
Input		: void
Return		: void
Remart		: menu中30s自动退出时间重载

********************************************************/
public void menu_exit_timer_reload( void );


/**************************************
Funcname		: menu_128mtimer_task
Contents 	: 菜单登录函数
Input		: MenuTbl
Return		: void
Remart		: 	 

***************************************/
public void menu_128mtimer_task( void );


/**************************************
Funcname		: menu_method_function
Contents 	: 菜单登录函数
Input		: MenuTbl
Return		: void
Remart		: 	 
			  Menu登录函数，每个Menu进场，
			  都需要通过该函数注册。
***************************************/
public void menu_method(MenuTbl *unit_tbl,_UBYTE menu_direct );

#endif /*MENU_H_INCLUDE*/
/*======================================================
Version	Date			Remart.
--------------------------------------------------------
Ver1_00	2012-07-22	新规做成 by 周前




========================================================*/

