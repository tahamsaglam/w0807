/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	ģ��		: 
	����		: 
	����		: 
	��ע		:
			 
*****************************************************************/
/*===============================================================
ͷ�ļ�����
================================================================*/
#include <string.h>			/* memcpy(),memset()etc */
#include "./Keyselect.h"
#include "./Precode.h"
#include "../../Common/Comm_soft.def"
/****************************************************************
Key�ײ����ݽṹ
*****************************************************************/
typedef struct{
#ifdef SYSTEM_SOURCE_KEY
 unsigned char source;		/*Source key the priority No1 ���ȼ����*/
#endif/*SYSTEM_SOURCE_KEY*/

#ifdef SYSTEM_MATRIX_KEY
 unsigned char matrix;		/*Body key the priority No2 ���ȼ����*/
#endif/*SYSTEM_MATRIX_KEY*/

#ifdef SYSTEM_WIRED_KEY
 unsigned char wired ;		/*Wired key the priority No3 ����ң����*/
#endif/*SYSTEM_WIRED_KEY*/

#ifdef SYSTEM_WIRELESS_KEY
 unsigned char wireless;   /*Wired key the priority No4 ����ң����*/
#endif/*SYSTEM_WIRELESS_KEY*/

} Keycode;

static Keycode inputkey;		/*�����ײ�ṹ�����*/

/****************************************************************
��������
*****************************************************************/

/**************************************
Funcname	: key_select_init
Contents 	: key select ��ʼ��
Input		: void
Return		: void
NOTES		: ��ʼ��ʱ��ִ�б������
***************************************/
public void key_select_init( void )
{
	memset(&inputkey,0x0000,sizeof(inputkey));		/*��ʼ������*/
}


/**************************************
Funcname	: get_precode_key
Contents 	: Precode ��ȡ
Input		:
			  oldkey
			  newkey
Return		:
			  Precode
NOTES		: 16msִ��һ��,�������밴������
***************************************/
public	_UBYTE get_precode_key( void )
{
	unsigned char precode_buf = PRECODE_NULL;/*Precode�ݴ����*/
	if( inputkey.matrix != 0 )
	{
		precode_buf = inputkey.matrix;		/*���Matrix_key Precode*/
	}
	return ( precode_buf );		/*����Precode*/
#if 0
#ifdef SYSTEM_SOURCE_KEY
	if( inputkey.source != 0 )
	{
		precode_buf = inputkey.source;	/*���Source_key Precode*/

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
		precode_buf = inputkey.matrix;		/*���Matrix_key Precode*/
		
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
		precode_buf = inputkey.wired;		/*���Wired_key Precode*/

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
	else if( inputkey.wireless != 0 )		/*���Wireless_key Precode*/
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
	return ( precode_buf );		/*����Precode*/
#endif
}


/**************************************
Funcname	: matrixkey_8ms_task
Contents 	: matrix key 8ms ����
Input		: void
Return		:
			  Matrix Precode
NOTES		: 8msִ��һ��
***************************************/
public void matrixkey_8ms_task( void )
{
#ifdef SYSTEM_MATRIX_KEY
#ifndef KYEP0_DEBUG
	unsigned char matrix_key_buf  = KEY_PORT_STATUS;	/*������Port�ļĴ�����ֵ*/
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