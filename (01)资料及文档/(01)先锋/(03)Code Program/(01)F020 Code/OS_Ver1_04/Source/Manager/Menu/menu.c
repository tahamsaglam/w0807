/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	ģ��		: ȫģ��
	����		: �˵�����ʵ�ַ����ļ�
	����		: Zq
	��ע		: Menu�ǲ˵�ģ�飬��ʵ������
			  ����趨�Ļ����ļ�,����Ϊһ������ģ��
*****************************************************************/




/**************************************
Global RAM����
***************************************/



/**************************************
private RAM����
***************************************/



/*==============================================================
ͷ�ļ�����
===============================================================*/
#include "./menu.h"
#include "../../Platform/CPU/Comm.h"
#include "../../Common/Timer/Timer.h"

/*==============================================================
Global RAM����
==============================================================*/
MenuStatus menust;

/*==============================================================
private RAM����
==============================================================*/
private _UBYTE menu_layer;		/*Menu �ײ�������*/
private _UINT menu_exit_timer;	/*Menu �Զ��˳�ʱ�����*/
private _UINT menu_dirction;		/*Menu ��ת����*/
enum{
	MENU_TYPE_MEMORY   =0x00 
,	MENU_TYPE_DISMEMORY		
};


/**************************************
Funcname		:menu_reset_init
Contents 	:Menu��ʼ��
Input		:void
Return		:void
Remart		:	 

***************************************/
public void menu_reset_init( void )
{
	menust.all = MENUST_MENU_OFF;			/*��Menu*/
	menu_layer = MENU_LAYER_UNKNOW;		/*�ײ��˳�*/
	menu_exit_timer = MENU_EXIT_TIME_CLEAR;	/*����ʱ���*/
}

/**************************************
Funcname		:menu_exit_requst
Contents 	:�˵����
Input		:void
Return		:void
Remart		:

***************************************/
public void menu_exit_requst( void )
{
	menust.all = MENUST_MENU_OFF;				/*Menu�˳�*/
	menu_layer = MENU_LAYER_UNKNOW;			/*�ײ��˳�*/
	menu_exit_timer = MENU_EXIT_TIME_CLEAR;	/*����ʱ���*/
}


/**************************************
Funcname		:menu_get_layer
Contents		:��õ�ǰ�Ľײ���
Input		:void
Return		:
			 MENU_LAYER_FIRST  ��һ��
			 MENU_LAYER_SECOND �ڶ���
			 MENU_LAYER_THREE  ������
Remart
***************************************/
public _UBYTE menu_get_layer( void )
{
	return ( menu_layer );
}


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
public void menu_set_layer( _UBYTE requst_layer )
{
	switch( requst_layer )
	{
		case MENU_RQ_LAYER_INC:
			if(( MENU_LAYER_UNKNOW != menu_layer )		/*Menu��¼����Menu�вſ����ӽײ���*/
				&&(MENU_LAYER_THREE > menu_layer ))		/*Menu�ײ�����С������ſɽ���*/
			{
				menu_layer ++;
			}
			break;
			
		case MENU_RQ_LAYER_DEC:
			if(( MENU_LAYER_UNKNOW != menu_layer )
				&&( MENU_LAYER_FIRST < menu_layer ))
			{
				menu_layer --;
			}
			break;

		default:
			if(( MENU_LAYER_UNKNOW != menu_layer )
				&&( MENU_LAYER_THREE >= menu_layer ))
			{
				menu_layer = requst_layer;
			}
			break;
	}
}


/********************************************************
Funcname		: menu_exit_timer_reload
Contents		: 30SMenu�Զ��˳�ʱ������
Input		: void
Return		: void
Remart		:
********************************************************/
public void menu_exit_timer_reload( void )
{
	menu_exit_timer = MENU_EXIT_TIME_30S ;
}


/**************************************
Funcname		: menu_method_function
Contents 	: �˵���¼����
Input		: MenuTbl
Return		: void
Remart		: 	 
			  Menu��¼������ÿ��Menu������
			  ����Ҫͨ���ú���ע�ᡣ
***************************************/
public void menu_method(MenuTbl *unit_tbl,_UBYTE menu_direct )
{

#if 0
	_UBYTE		countover_check = 0x00;
	unsigned short int	last_menu = 0x0000;
	_UBYTE		last_menu_fukki = 0;

		if( unit_tbl->last == (void *)0x00000 ){
			return( 0 );	/* �t�F�[���Z�[�t */
		}
		last_menu = *unit_tbl->last;
		/* ���߂ă��j���[�ɓ���Ƃ���(���X�g��ݒ肵��)���X�g���j���[���A */
		if( last_menu == MENUST_MENU_OFF ){
			last_menu_fukki = 1;
		}
		/* �قȂ郁�j���[�����ԑJ�ڂ��������ꍇ�̓��X�g���j���[���A */
		if( menust.one.attribute != ((MenuStatus far *)&(unit_tbl->menu_start))->one.attribute ){
			last_menu_fukki = 1;
		}
	

	if( last_menu_fukki ){
		/* ���߂ă��j���[�ɓ���Ƃ��̃��X�g�����鏈�� */
		if( last_menu == MENUST_MENU_OFF ){
			if( direction == MENU_DIRECTION_NORMAL ){
				last_menu = unit_tbl->menu_start;	/* �g�O���̍ŏ����� */
			}else{
				last_menu = unit_tbl->menu_end;		/* �g�O���̍Ōォ�� */
			}
		}
	}

	do{
		if( last_menu_fukki )
		{
			menust.all = last_menu;
			last_menu_fukki = 0;
		}
		else
		{
			if( direction == MENU_DIRECTION_NORMAL )
			{
				(menust.all) ++;				/* ������menust�X�V */
				if( menust.all >= unit_tbl->menu_end )
				{
					menust.all = unit_tbl->menu_start;
					countover_check += 1;
				}
			}
			else
			{
				(menust.all) --;				/* �t����menust�X�V */
				if( menust.all == unit_tbl->menu_start )
				{
					menust.all = unit_tbl->menu_end;
					countover_check += 1;
				}
			}
		}
	/* �ݒ肵�����j���[���N���ł��Ȃ��A�����j���[�g�O����1�����Ă��Ȃ��ꍇ�͏����J��Ԃ� */
	} while( ( ( *unit_tbl->enable_check)( menust.all ) == MENUST_MENU_OFF ) && (countover_check < 2) );

	/* ���j���[�g�O�����P���ȏサ�Ă���ꍇ */
	if( countover_check >= 2 )
	{
		menust.all = MENUST_MENU_OFF;
		menu_exit_request();
		return 0;
	}
	else
	{
		/* ���j���[�g�O���P������ -> �N���\���j���[�L�� */
		*unit_tbl->last = menust.all;	/* last�ɕۑ����� */
#ifdef BEEPOUT
		/* �����Ń��j���[�����^�C�}�͐ݒ肵�Ȃ� �e�A�v���Őݒ肷����Ƃ��� */
		/* �]���ʂ�̃R�[�f�B���O�Ŗ��Ȃ����� by wata */
		if( direction == MENU_DIRECTION_NORMAL )
		{
			BeepOutput( BeepOutON );
		}
		else
		{
			BeepOutput( BeepOutOFF );
		}
#endif /* BEEPOUT */	
		menu_layer = MENU_LAYER_TOP;
		return 1;
	}
}






	_UBYTE contover_check = 0;
	_UINT last_menu = MENUST_MENU_OFF;
	_UBYTE menu_type = MENU_TYPE_MEMORY;
	_UBYTE menu_change_flag = 0;
/*************************************
	if(  MENUATTR_TOP == (((MenuStatus *)&(unit_tbl->menu_start))->one.attribute ) )
	{
		menu_type = MENU_TYPE_DISMEMORY;
	}
*************************************/	
//	last_menu = *unit_tbl->last_menu;						/*��ȡlast_menu*/

	if( MENUST_MENU_OFF == last_menu )
	{
		if( MENU_DIRECTION_NORMAL == menu_direct )
		{
			last_menu = unit_tbl->menu_start;
		}
		else
		{
			last_menu = unit_tbl->menu_end;					
		}
		if( MENU_TYPE_MEMORY == menu_type )
		{
			menu_change_flag = 1;
		}
		menust.all = last_menu;
	}
	if( menust.one.attribute != (( (MenuStatus *)&( unit_tbl->menu_start ) )->one.attribute))
	{
		if( last_menu == MENUST_MENU_OFF )
		{
			if( MENU_DIRECTION_NORMAL == menu_direct )
			{
				last_menu = unit_tbl->menu_start;
			}
			else
			{
				last_menu = unit_tbl->menu_end;					
			}
		}			
		menust.all = last_menu;
		menu_change_flag = 1;
	}

	do
	{
//		if( 1 == menu_change_flag )
//		{
//			menu_change_flag = 0;
//		}
//		else
		{
			if( MENU_DIRECTION_NORMAL == menu_direct )
			{
				( menust.all )++;
				if( menust.all >= unit_tbl->menu_end )
				{
					menust.all = unit_tbl->menu_start;
					contover_check += 1;
				}
			}
			else
			{
				( menust.all )--;
				if( menust.all == unit_tbl->menu_start )
				{
					menust.all = unit_tbl->menu_end;
					contover_check += 1;
				}
			}
		}
	}
while( ( ( *unit_tbl->enable_chk)( menust.all ) == MENUST_MENU_OFF ) && (contover_check < 2) );

	if( contover_check >= 2 )
	{
		menu_exit_requst( );
	}
	else
	{
		if( MENU_TYPE_MEMORY == menu_type )
		{
			 *(unit_tbl->last_menu) = menust.all;		/*����last_menu*/
		}
		menu_layer = MENU_LAYER_FIRST;
	}
	#endif
}


/**************************************
Funcname	: menu_128mtimer_task
Contents 	: �˵���¼����
Input		: MenuTbl
Return		: void
Remart		: 	 

***************************************/
public void menu_128mtimer_task( void )
{

	if( 1 == tcount_int( &menu_exit_timer ))
	{
		menu_exit_requst( );		/*30S��ʱ�����Զ���Menu*/
	}
}





/*===================================================================
Version	|	Date		|	Author	|	Contents  					|
---------------------------------------------------------------------
V1.00		2012-08-19		ZQ			New code write.

====================================================================*/

