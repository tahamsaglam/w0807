/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	ģ��		: 
	����		: 
	����		: 
	��ע		:
			 
*****************************************************************/
#ifndef MENU_H_INCLUDE
#define MENU_H_INCLUDE
/*===============================================================
ͷ�ļ�����
================================================================*/
#include "../../Platform/CPU/Comm.h"		/*��������ͷ�ļ�*/
/******************************************************
Menu ����������
*****************************************************/
typedef union{
	struct {
		_UBYTE status;		/*Menu״̬*/
		_UBYTE attribute;	/*Menu����*/
	}one;
	_UINT all;	
}MenuStatus;


/******************************************************
Menu �ṹ������
*****************************************************/
typedef struct{
	_UINT *last_menu;
	_UBYTE ( * enable_chk )( _UINT );
	_UINT menu_start;
	_UINT menu_end;
}MenuTbl;


/******************************************************
Menu attribute enum ����
*****************************************************/
enum {
	MENUATTR_OFF			= 0x00
,	MENUATTR_SYSTEM_SETUP
,	MENUATTR_FUNCTION
,	MENUATTR_MENU1
,	MENUATTR_MENU2
};

/******************************************************
Menu ��ת������
*****************************************************/
enum{
	MENU_DIRECTION_NORMAL = 0x00		/*������תMenu*/
,	MENU_DIRECTION_REVERSE			/*������תMenu*/
};


/******************************************************
Menu �궨��
*****************************************************/
#define MENUST_MENU_OFF  0x00 			/*Menu.all���*/
#define MENU_EXIT_TIME_30S  0xf0		/*Menu 30s�Զ��˳�ʱ������*/
#define MENU_EXIT_TIME_CLEAR 0x00		/*Menu �Զ��˳�ʱ�����*/


/******************************************************
Menu �ײ㶨��
*****************************************************/
enum{
	MENU_LAYER_UNKNOW	= 0x00	/*Menuδ֪��*/
,	MENU_LAYER_FIRST			/*Menu��һ�ײ�*/
,	MENU_LAYER_SECOND			/*Menu�ڶ��ײ�*/
,	MENU_LAYER_THREE			/*Menu�����ײ�*/
,	MENU_RQ_LAYER_INC	= 0xf0	/*Menu�ײ�������*/
,	MENU_RQ_LAYER_DEC			/*Menu�ײ�������*/
};


/******************************************************
Menu �ⲿ��������
*****************************************************/
public MenuStatus menust ;			/*menuȫ�ֱ���*/


/******************************************************
Menu �ⲿ��������
*****************************************************/

/**************************************
Funcname		: menu_reset_init
Contents 	: Menu��ʼ��
Input		: void
Return		: void
Remart		: menu��ʼ������	 

***************************************/
public void menu_reset_init( void );


/**************************************
Funcname		: menu_exit_requst
Contents 	: �˵����
Input		: void
Return		: void
Remart		: ���øú��������menu

***************************************/
public void menu_exit_requst( void );


/**************************************
Funcname		: menu_get_layer
Contents		: ��õ�ǰ�Ľײ���
Input		: void
Return		:
			  MENU_LAYER_FIRST  ��һ��
			  MENU_LAYER_SECOND �ڶ���
			  MENU_LAYER_THREE  ������
Remart		: ��ȡ��ǰ�ײ�
***************************************/
public _UBYTE menu_get_layer( void );


/*************************************
Funcname		: menu_set_layer
Contents		: Menu�ײ������
Input		: request_layer��Χ
			  ��1�㵽�Ѷ��������
Return		: void
Remart		:
			  ����Ĳ���:
			  MENU_RQ_LAYER_INC ��ǰ������1
			  MENU_RQ_LAYER_DEC ��ǰ������1
*************************************/
public void menu_set_layer( _UBYTE requst_layer );

/********************************************************
Funcname		: menu_exit_timer_reload
Contents		: 30SMenu�Զ��˳�ʱ������
Input		: void
Return		: void
Remart		: menu��30s�Զ��˳�ʱ������

********************************************************/
public void menu_exit_timer_reload( void );


/**************************************
Funcname		: menu_128mtimer_task
Contents 	: �˵���¼����
Input		: MenuTbl
Return		: void
Remart		: 	 

***************************************/
public void menu_128mtimer_task( void );


/**************************************
Funcname		: menu_method_function
Contents 	: �˵���¼����
Input		: MenuTbl
Return		: void
Remart		: 	 
			  Menu��¼������ÿ��Menu������
			  ����Ҫͨ���ú���ע�ᡣ
***************************************/
public void menu_method(MenuTbl *unit_tbl,_UBYTE menu_direct );

#endif /*MENU_H_INCLUDE*/
/*======================================================
Version	Date			Remart.
--------------------------------------------------------
Ver1_00	2012-07-22	�¹����� by ��ǰ




========================================================*/

