/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	ģ��		: 
	����		: 
	����		: 
	��ע		:
			 
*****************************************************************/
#ifndef SET_MENU_H_INCLUDE
#define SET_MENU_H_INCLUDE

/*===============================================================
ͷ�ļ�����
================================================================*/
#include "../../Platform/CPU/Comm.h"
#include "./menu.h"
/**************************************
Funcname	: setup_menu_keychange
Contents 	: �°�������
Input		: newkey
Return		: void
Remart		:	 
               ��������������
               
***************************************/
public void setup_menu_keychange( unsigned char newkey );


/**************************************
Funcname	: set_menu_reset_init
Contents 	: ��ر����ĳ�ʼֵ����
Input		: ��
Return		: ��
Remart		: 	 

***************************************/
public void set_menu_reset_init( void );


/**************************************
Funcname		: menu_disp_mode_set
Contents 	: disp mode set
Input		: newkey
Return		: void
Remart		: ���ص�ǰ����ʾmode	 

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
Ver1_00	2012-07-22	�¹����� by ��ǰ


========================================================*/


