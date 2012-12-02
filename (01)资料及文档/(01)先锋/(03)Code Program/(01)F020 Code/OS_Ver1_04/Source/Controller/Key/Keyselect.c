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
#include <string.h>			/* memcpy(),memset()etc */
#include "./Keyselect.h"
#include "./Precode.h"
#include "../../Common/Comm_soft.def"
/****************************************************************
Key底层数据结构
*****************************************************************/
typedef struct{
#ifdef SYSTEM_SOURCE_KEY
 unsigned char source;		/*Source key the priority No1 优先级最高*/
#endif/*SYSTEM_SOURCE_KEY*/

#ifdef SYSTEM_MATRIX_KEY
 unsigned char matrix;		/*Body key the priority No2 优先级其次*/
#endif/*SYSTEM_MATRIX_KEY*/

#ifdef SYSTEM_WIRED_KEY
 unsigned char wired ;		/*Wired key the priority No3 有线遥控器*/
#endif/*SYSTEM_WIRED_KEY*/

#ifdef SYSTEM_WIRELESS_KEY
 unsigned char wireless;   /*Wired key the priority No4 无线遥控器*/
#endif/*SYSTEM_WIRELESS_KEY*/

} Keycode;

static Keycode inputkey;		/*按键底层结构体变量*/

/****************************************************************
函数声明
*****************************************************************/

/**************************************
Funcname	: key_select_init
Contents 	: key select 初始化
Input		: void
Return		: void
NOTES		: 初始化时，执行变量清除
***************************************/
public void key_select_init( void )
{
	memset(&inputkey,0x0000,sizeof(inputkey));		/*初始化变量*/
}


/**************************************
Funcname	: get_precode_key
Contents 	: Precode 获取
Input		:
			  oldkey
			  newkey
Return		:
			  Precode
NOTES		: 16ms执行一次,即将进入按键解析
***************************************/
public	_UBYTE get_precode_key( void )
{
	unsigned char precode_buf = PRECODE_NULL;/*Precode暂存变量*/
	if( inputkey.matrix != 0 )
	{
		precode_buf = inputkey.matrix;		/*获得Matrix_key Precode*/
	}
	return ( precode_buf );		/*返回Precode*/
#if 0
#ifdef SYSTEM_SOURCE_KEY
	if( inputkey.source != 0 )
	{
		precode_buf = inputkey.source;	/*获得Source_key Precode*/

 #ifdef SYSTEM_MATRIX_KEY
		inputkey.matrix = 0;
 #endif/*SYSTEM_MATRIX_KEY*/

 #ifdef SYSTEM_WIRED_KEY
		inputkey.wired = 0;
 #endif/*SYSTEM_WIRED_KEY*/

 #ifdef SYSTEM_WIRELESS_KEY
		inputkey.wireless = 0;
 #endif/*SYSTEM_WIRELESS_KEY*/
	}
	
	else if( inputkey.matrix != 0 )
		
#else

 	if( inputkey.matrix != 0 )
		
#endif/*SYSTEM_SOURCE_KEY*/
	{
		precode_buf = inputkey.matrix;		/*获得Matrix_key Precode*/
		
 #ifdef SYSTEM_SOURCE_KEY
		inputkey.source = 0;
 #endif/*SYSTEM_SOURCE_KEY*/	

 #ifdef SYSTEM_WIRED_KEY
		inputkey.wired = 0;
 #endif/*SYSTEM_WIRED_KEY*/

 #ifdef SYSTEM_WIRELESS_KEY
		inputkey.wireless = 0;
 #endif/*SYSTEM_WIRELESS_KEY*/
	}
#ifdef SYSTEM_WIRED_KEY
	else if( inputkey.wired != 0 )
	{
		precode_buf = inputkey.wired;		/*获得Wired_key Precode*/

 #ifdef SYSTEM_SOURCE_KEY
		inputkey.source = 0;
 #endif/*SYSTEM_SOURCE_KEY*/	

 #ifdef SYSTEM_MATRIX_KEY
		inputkey.matrix = 0;
 #endif/*SYSTEM_MATRIX_KEY*/

 #ifdef SYSTEM_WIRELESS_KEY
		inputkey.wireless = 0;
 #endif/*SYSTEM_WIRELESS_KEY*/
	}
#endif/*SYSTEM_WIRED_KEY*/	

#ifdef SYSTEM_WIRELESS_KEY
	else if( inputkey.wireless != 0 )		/*获得Wireless_key Precode*/
	{
		precode_buf = inputkey.wireless;
		
 #ifdef SYSTEM_SOURCE_KEY
		inputkey.source = 0;
 #endif/*SYSTEM_SOURCE_KEY*/	

 #ifdef SYSTEM_MATRIX_KEY
		inputkey.matrix = 0;
 #endif/*SYSTEM_MATRIX_KEY*/

 #ifdef SYSTEM_WIRED_KEY
		inputkey.wired = 0;
 #endif/*SYSTEM_WIRED_KEY*/
	}
#endif/*SYSTEM_WIRELESS_KEY*/

#ifdef SYSTEM_SOURCE_KEY
		inputkey.source = 0;
#endif/*SYSTEM_SOURCE_KEY*/	

#ifdef SYSTEM_MATRIX_KEY
		inputkey.matrix = 0;
#endif/*SYSTEM_MATRIX_KEY*/

#ifdef SYSTEM_WIRED_KEY
		inputkey.wired = 0;
#endif/*SYSTEM_WIRED_KEY*/

#ifdef SYSTEM_WIRELESS_KEY
		inputkey.wireless = 0;
#endif/*SYSTEM_WIRELESS_KEY*/
	return ( precode_buf );		/*返回Precode*/
#endif
}


/**************************************
Funcname	: matrixkey_8ms_task
Contents 	: matrix key 8ms 函数
Input		: void
Return		:
			  Matrix Precode
NOTES		: 8ms执行一次
***************************************/
public void matrixkey_8ms_task( void )
{
#ifdef SYSTEM_MATRIX_KEY
#ifndef KYEP0_DEBUG
	unsigned char matrix_key_buf  = KEY_PORT_STATUS;	/*读按键Port的寄存器的值*/
#else	
	unsigned char matrix_key_buf =0x00;
	P1=P1;
	matrix_key_buf = (P1&0x01);
#endif
	switch( matrix_key_buf )
	{
		case KEY1_DEF:
			inputkey.matrix = PRECODE_KEY1;
			break;

		case KEY2_DEF:
			inputkey.matrix = PRECODE_KEY2;
			break;
			
		case KEY3_DEF:
			inputkey.matrix = PRECODE_KEY3;
			break;
			
		case KEY12_DEF:
			inputkey.matrix = PRECODE_DB_12;
			break;
			
		case KEY13_DEF:
			inputkey.matrix = PRECODE_DB_13;
			break;
			
		case KEY23_DEF:
			inputkey.matrix = PRECODE_DB_23;
			break;		

		default:
			inputkey.matrix = PRECODE_NULL;
			break;
	}
#endif/*SYSTEM_MATRIX_KEY*/
}

/*===================================================================
Version	|	Date		|	Author	|	Contents  					|
---------------------------------------------------------------------
V1.00		2012-08-19		ZQ			New code write.

====================================================================*/