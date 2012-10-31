/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	模型		: 
	机能		: 
	作者		: 
	备注		:
			 
*****************************************************************/
#ifndef _KEYSELECT_H_
#define _KEYSELECT_H_
/*===============================================================
头文件声明
================================================================*/
#include "c8051f020.h"
#include "../../Platform/CPU/Comm.h"
//#define KEY_PORT_STATUS   (~(P1&0x07))
#define KEY_PORT_STATUS   (~P1)
#define KEY1_DEF	0x01    /*P1 & 0x02*/
#define KEY2_DEF   	0x02   	/*P1 & 0x04*/
#define KEY3_DEF   	0x04   	/*P1 & 0x08*/
#define KEY12_DEF  	0x03    /*P1 & 0x06*/
#define KEY23_DEF  	0x06   	/*P1 & 0x0c*/
#define KEY13_DEF  	0x05    /*P1 & 0x0b*/

public	_UBYTE get_precode_key( void );	/*Precode 获取*/
public void key_select_init( void );			/*key select初始化*/
public void matrixkey_8ms_task( void );/*Matrix 8ms函数*/


#endif/*_KEYSELECT_H_*/
/*===================================================================
Version	|	Date		|	Author	|	Contents  			|
---------------------------------------------------------------------
V1.00		2012-08-17		ZQ			New code write.

====================================================================*/
