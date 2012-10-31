/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	ģ��		: ȫģ��
	����		: �˵�����ʵ�ַ����ļ�
	����		: Zq
	��ע		: ��Ʒ���趨��menu���
*****************************************************************/
/*===============================================================
ͷ�ļ�����
================================================================*/
#include "../../Platform/CPU/Comm.h"
#include "./Setup.h"
#include "./Menu.h"
#include "../../Controller/Key/keycode.h"		/* ��������*/

#define KEY_DEBUG

#ifdef KEY_DEBUG
#include "../Display/Display.h"
#endif
/*==============================================================
Global RAM����
===============================================================*/




/*===============================================================
private RAM����
===============================================================*/
private _UINT setup_last_menu;		/*ϵͳ�趨���˵��������*/
private _UBYTE setup_key_buf;		/*Setup Menu��󰴼��洢����*/
/*===============================================================
private��������
===============================================================*/
private void sys_setup_menu_keychange( unsigned char newkey );
private unsigned char sys_system_setup_enable_check( void );
/*==============================================================
ϵͳSETUP�˵���¼�Ľṹ��
===============================================================*/
 const MenuTbl sys_system_setup_mode ={	
	 &setup_last_menu
	,&sys_system_setup_enable_check
	,MENUST_SETUP_START
	,MENUST_SETUP_END
};


/**************************************
Funcname	: set_menu_reset_init
Contents 	: ��ر����ĳ�ʼֵ����
Input		: ��
Return		: ��
Remart		: 	 

***************************************/
public void set_menu_reset_init( void )
{
	 setup_key_buf = 0x00;			/*Setup��󰴼�������ʼ��*/
	 setup_last_menu = 0x00;		/*Setup Menu���˵�������ʼ��*/
}


/**************************************
Funcname		: sys_system_setup_enable_check
Contents 	: ϵͳSETUP Menuʹ�ܺ���
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
Contents 	: Setup Menu����������
Input		: newkey
Return		: void
Remart		:	 
               ��������������
               
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
Contents 	: system setup menu�а�������
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
Remart		: ���ص�ǰ����ʾmode	 

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

