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
#include "../../Manager/Menu/Menu.h"
#include "../../Platform/CPU/Comm.h"
#include "./Timer.h"
/*==============================================================
Global RAM声明
===============================================================*/




/*===============================================================
Static RAM声明
===============================================================*/
private _UBYTE timer_128ms;

/*===============================================================
函数声明
===============================================================*/
static void timer_128m_task(void);

/**************************************
Funcname	: timer_reset_init
Contents 	: timer 初始化函数
Input		: void
Return		: void
Remart		:	 

***************************************/
public void timer_reset_init( void )
{
	timer_128ms = Time_128ms_char8;
}


/**************************************
Funcname	:tcount
Contents 	:计数函数
Input		:需要计数的变量
Return		:
			 1:计数到
			 0:未开始计数或者计数未到
Remart		:	 

***************************************/
public _UBYTE tcount( _UBYTE *counter )
{
	if( *counter != 0x00 ){
		/* Timer Incriment rootin  */
		-- *counter;
		if( *counter == 0x00 ){
			return 1;
		}
	}
	return 0;
}

/**************************************
Funcname	: tcount_int
Contents 	: 计数函数
Input		: 需要计数的变量
Return		:
			  1:计数到
			  0:未开始计数或者计数未到
Remart		:	 

***************************************/
public _UBYTE tcount_int( _UINT *counter )
{
	if( *counter != 0x00 ){

		/* Timer Incriment rootin  */
		-- *counter;
		if( *counter == 0x00 ){
			return 1;
		}
	}
	return 0;
}



/**************************************
Funcname	: timer_8ms_task
Contents 	: 计数任务
Input		:
			  void
Return		:
			  void
Remart		:	 

***************************************/
public void timer_8ms_task( void )
{
	if( timer_128ms != Time_Clear ){
		timer_128ms --;
	}
	else{
		timer_128ms = Time_128ms_char8;
		timer_128m_task();
	}
}

/**************************************
Funcname	:timer_128m_task
Contents 	:128ms计数任务
Input		:
			 void
Return		:
			 void
Remart		:	 

***************************************/
static void timer_128m_task(void)
{
	menu_128mtimer_task();
		/*Add 128ms timer*/	
}




/*===================================================================
Version	|	Date		|	Author	|	Contents  					|
---------------------------------------------------------------------
V1.00		2012-08-19		ZQ			New code write.

====================================================================*/


