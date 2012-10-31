/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	模型		: 全模型
	机能		: 菜单功能实现方法文件
	作者		: Zq
	备注		: Menu是菜单模块，是实现所有
			  相关设定的基础文件,故作为一个单独模块
*****************************************************************/




/**************************************
Global RAM声明
***************************************/



/**************************************
private RAM声明
***************************************/



/*==============================================================
头文件声明
===============================================================*/
#include "./menu.h"
#include "../../Platform/CPU/Comm.h"
#include "../../Common/Timer/Timer.h"

/*==============================================================
Global RAM声明
==============================================================*/
MenuStatus menust;

/*==============================================================
private RAM声明
==============================================================*/
private _UBYTE menu_layer;		/*Menu 阶层记忆变量*/
private _UINT menu_exit_timer;	/*Menu 自动退出时间计数*/
private _UINT menu_dirction;		/*Menu 旋转方向*/
enum{
	MENU_TYPE_MEMORY   =0x00 
,	MENU_TYPE_DISMEMORY		
};


/**************************************
Funcname		:menu_reset_init
Contents 	:Menu初始化
Input		:void
Return		:void
Remart		:	 

***************************************/
public void menu_reset_init( void )
{
	menust.all = MENUST_MENU_OFF;			/*无Menu*/
	menu_layer = MENU_LAYER_UNKNOW;		/*阶层退出*/
	menu_exit_timer = MENU_EXIT_TIME_CLEAR;	/*倒计时清除*/
}

/**************************************
Funcname		:menu_exit_requst
Contents 	:菜单解除
Input		:void
Return		:void
Remart		:

***************************************/
public void menu_exit_requst( void )
{
	menust.all = MENUST_MENU_OFF;				/*Menu退出*/
	menu_layer = MENU_LAYER_UNKNOW;			/*阶层退出*/
	menu_exit_timer = MENU_EXIT_TIME_CLEAR;	/*倒计时清除*/
}


/**************************************
Funcname		:menu_get_layer
Contents		:获得当前的阶层数
Input		:void
Return		:
			 MENU_LAYER_FIRST  第一层
			 MENU_LAYER_SECOND 第二层
			 MENU_LAYER_THREE  第三层
Remart
***************************************/
public _UBYTE menu_get_layer( void )
{
	return ( menu_layer );
}


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
public void menu_set_layer( _UBYTE requst_layer )
{
	switch( requst_layer )
	{
		case MENU_RQ_LAYER_INC:
			if(( MENU_LAYER_UNKNOW != menu_layer )		/*Menu登录后，在Menu中才可增加阶层数*/
				&&(MENU_LAYER_THREE > menu_layer ))		/*Menu阶层数，小于最大层才可进层*/
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
Contents		: 30SMenu自动退出时间重载
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
Contents 	: 菜单登录函数
Input		: MenuTbl
Return		: void
Remart		: 	 
			  Menu登录函数，每个Menu进场，
			  都需要通过该函数注册。
***************************************/
public void menu_method(MenuTbl *unit_tbl,_UBYTE menu_direct )
{

#if 0
	_UBYTE		countover_check = 0x00;
	unsigned short int	last_menu = 0x0000;
	_UBYTE		last_menu_fukki = 0;

		if( unit_tbl->last == (void *)0x00000 ){
			return( 0 );	/* 僼僃乕儖僙乕僼 */
		}
		last_menu = *unit_tbl->last;
		/* 弶傔偰儊僯儏乕偵擖傞偲偒偼(儔僗僩傪愝掕偟偰)儔僗僩儊僯儏乕暅婣 */
		if( last_menu == MENUST_MENU_OFF ){
			last_menu_fukki = 1;
		}
		/* 堎側傞儊僯儏乕懏惈娫慗堏偑偁偭偨応崌偼儔僗僩儊僯儏乕暅婣 */
		if( menust.one.attribute != ((MenuStatus far *)&(unit_tbl->menu_start))->one.attribute ){
			last_menu_fukki = 1;
		}
	

	if( last_menu_fukki ){
		/* 弶傔偰儊僯儏乕偵擖傞偲偒偺儔僗僩傪擖傟傞張棟 */
		if( last_menu == MENUST_MENU_OFF ){
			if( direction == MENU_DIRECTION_NORMAL ){
				last_menu = unit_tbl->menu_start;	/* 僩僌儖偺嵟弶偐傜 */
			}else{
				last_menu = unit_tbl->menu_end;		/* 僩僌儖偺嵟屻偐傜 */
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
				(menust.all) ++;				/* 弴曽岦menust峏怴 */
				if( menust.all >= unit_tbl->menu_end )
				{
					menust.all = unit_tbl->menu_start;
					countover_check += 1;
				}
			}
			else
			{
				(menust.all) --;				/* 媡曽岦menust峏怴 */
				if( menust.all == unit_tbl->menu_start )
				{
					menust.all = unit_tbl->menu_end;
					countover_check += 1;
				}
			}
		}
	/* 愝掕偟偨儊僯儏乕偑婲摦偱偒側偔丄妿偮儊僯儏乕僩僌儖傪1廃偟偰偄側偄応崌偼張棟孞傝曉偟 */
	} while( ( ( *unit_tbl->enable_check)( menust.all ) == MENUST_MENU_OFF ) && (countover_check < 2) );

	/* 儊僯儏乕僩僌儖傪侾廃埲忋偟偰偄傞応崌 */
	if( countover_check >= 2 )
	{
		menust.all = MENUST_MENU_OFF;
		menu_exit_request();
		return 0;
	}
	else
	{
		/* 儊僯儏乕僩僌儖侾廃枹枮 -> 婲摦壜擻儊僯儏乕桳傝 */
		*unit_tbl->last = menust.all;	/* last偵曐懚偡傞 */
#ifdef BEEPOUT
		/* 偙偙偱儊僯儏乕夝彍僞僀儅偼愝掕偟側偄 奺傾僾儕偱愝掕偡傞嶌傝偲偡傞 */
		/* 廬棃捠傝偺僐乕僨傿儞僌偱栤戣側偔摦偔 by wata */
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
//	last_menu = *unit_tbl->last_menu;						/*读取last_menu*/

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
			 *(unit_tbl->last_menu) = menust.all;		/*记忆last_menu*/
		}
		menu_layer = MENU_LAYER_FIRST;
	}
	#endif
}


/**************************************
Funcname	: menu_128mtimer_task
Contents 	: 菜单登录函数
Input		: MenuTbl
Return		: void
Remart		: 	 

***************************************/
public void menu_128mtimer_task( void )
{

	if( 1 == tcount_int( &menu_exit_timer ))
	{
		menu_exit_requst( );		/*30S定时到，自动退Menu*/
	}
}





/*===================================================================
Version	|	Date		|	Author	|	Contents  					|
---------------------------------------------------------------------
V1.00		2012-08-19		ZQ			New code write.

====================================================================*/

