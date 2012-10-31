/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	模型		: 全模型
	机能		: Task组织文件
	作者		: Zq
	备注		: 各模块，需要的定周期、Task，均需要在这个文件里被管理
*****************************************************************/


/**************************************
头文件声明
***************************************/

/**************************************
Global RAM声明
***************************************/



/**************************************
Static RAM声明
***************************************/



/**************************************
Funcname		:init_func_tab
Contents 	:
Input		:
Return		:
Remart		:	 

***************************************/
static const void ( * const init_func_tab[] )( void ) =
{
#if 0
	timer_reset_init		/*Timer模块初始化函数*/
,	menu_reset_init			/*Menu模块初始化函数*/
,	set_menu_reset_init		/*Setup模块初始化函数*/
,	key_reset_init			/*Key模块初始化函数*/
,	Display_reset_init		/*显示模块初始化函数*/
/*,	ZLG7289_Init*/
#endif
};


/**************************************
Funcname		:short_2ms_task_tbl
Contents 	:
Input		:
Return		:
Remart		:	 

***************************************/
static const void ( * const short_8ms_task_tbl[] )( void ) =
{
//	disp_function
};


/**************************************
Funcname		:middle_8ms_task_tbl
Contents 	:
Input		:
Return		:
Remart		:	 

***************************************/
static const void ( * const middle_32ms_task_tbl[] ) ( void )=
{
#if 0	
	key_8mtimer_task		/*按键更新监视函数，8ms*/
,	timer_8ms_task			/*timer 8ms 监视函数*/
#endif
};


/**************************************
Funcname	:long_16ms_task_tbl
Contents 	:
Input		:
Return		:
Remart		:	 
 
***************************************/
static const void ( * const long_1s_task_tbl[] )(void) =
{
#if 0
	disp_function			//显示更新，16ms一次
,	keychange_16m_task
#endif
};


/**************************************
Funcname		: remon_init_func_task
Contents 	: 初始任务
Input		: void
Return		: void
Remart		: 	 

***************************************/
public void remon_init_func_task( void )
{
	static unsigned char init_slot_cnt;		/*计数变量*/
	void (*init_task )( void );			/*定义函数指针，指向函数名*/
	
	while( init_slot_cnt < (sizeof( init_func_tab ) / sizeof ( init_func_tab[0] )) )
	{
		init_task = init_func_tab[init_slot_cnt];
		(*init_task)(  );/*执行函数指针指向的函数*/
		init_slot_cnt++;
	}
	init_slot_cnt = 0;
#if 0 	
	for( init_slot_cnt = 0; init_slot_cnt < sizeof( init_func_tab )/sizeof( init_func_tab[0] );init_slot_cnt++	 )
	{		/*运行初期化函数Tbl中所有函数*/
		init_task = init_func_tab[init_slot_cnt];
		(*init_task)(  );/*执行函数指针指向的函数*/	
	}
#endif
}


/**************************************
Funcname	:remon_short_func_task
Contents 	:
Input		:
Return		:
Remart		:	 

***************************************/
public void remon_short_func_task( void )
{
	static unsigned char short_slot_cnt = 0;		/*计数变量*/
	void (*short_task )( void );			/*定义函数指针，指向函数名*/
	while( short_slot_cnt < (sizeof( short_8ms_task_tbl ) / sizeof ( short_8ms_task_tbl[0] )) )
	{
		short_task = short_8ms_task_tbl[short_slot_cnt];
		(*short_task)(  );/*执行函数指针指向的函数*/
		short_slot_cnt ++;
	}
	short_slot_cnt = 0;
#if 0
	for( short_slot_cnt = 0; short_slot_cnt <= sizeof( short_8ms_task_tbl )/sizeof( short_8ms_task_tbl[0] );short_slot_cnt++	 )
	{		/*运行8ms函数Tbl中所有函数*/
		short_task = short_8ms_task_tbl[short_slot_cnt];
		(*short_task)(  );/*执行函数指针指向的函数*/
		if( short_slot_cnt > sizeof( short_8ms_task_tbl ) / sizeof ( short_8ms_task_tbl[0] ) )			
		{
			short_slot_cnt = 0;
			return;
		}
	}
#endif	
}


/**************************************
Funcname		:remon_middle_func_task
Contents 	:
Input		:
Return		:
Remart		:	 

***************************************/
public void remon_middle_func_task( void )
{
	static unsigned char middle_slot_cnt;		/*计数变量*/
	void (*middle_task )( void );		/*定义函数指针，指向函数名*/
	while( middle_slot_cnt < (sizeof( middle_32ms_task_tbl ) / sizeof ( middle_32ms_task_tbl[0] ) ))
	{
		middle_task = middle_32ms_task_tbl[middle_slot_cnt];
		(*middle_task)(  );/*执行函数指针指向的函数*/
		middle_slot_cnt ++;
	}
	middle_slot_cnt=0;
#if 0	
	for( middle_slot_cnt = 0; middle_slot_cnt <= sizeof( middle_32ms_task_tbl )/sizeof( middle_32ms_task_tbl[0] );middle_slot_cnt++	 )
	{		/*运行8ms函数Tbl中所有函数*/
		middle_task = middle_32ms_task_tbl[middle_slot_cnt];
		(*middle_task)(  );/*执行函数指针指向的函数*/
		if( middle_slot_cnt > sizeof( middle_32ms_task_tbl ) / sizeof ( middle_32ms_task_tbl[0] ) )
		{
			middle_slot_cnt = 0;
			return;
		}
	}
#endif	
}


/**************************************
Funcname	:remon_long_func_task
Contents 	:
Input		:
Return		:
Remart		:	 

***************************************/
public void remon_long_func_task( void )
{
	static unsigned char long_slot_cnt;		/*计数变量*/
	void (*long_task )( void );			/*定义函数指针，指向函数名*/
	while( long_slot_cnt < (sizeof( long_1s_task_tbl ) / sizeof ( long_1s_task_tbl[0] )) )
	{
		long_task = long_1s_task_tbl[long_slot_cnt];
		(*long_task)(  );/*执行函数指针指向的函数*/
		long_slot_cnt++;
	}
	long_slot_cnt =0;
#if 0	
	for( long_slot_cnt = 0; long_slot_cnt <= sizeof( long_1s_task_tbl )/sizeof( long_1s_task_tbl[0] );long_slot_cnt++	 )
	{		/*运行16ms函数Tbl中所有函数*/
		long_task = long_1s_task_tbl[long_slot_cnt];
		(*long_task)(  );/*执行函数指针指向的函数*/
//		if( long_slot_cnt >= sizeof( long_1s_task_tbl ) / sizeof ( long_1s_task_tbl[0] ) )
		if( long_slot_cnt > sizeof( long_1s_task_tbl ) / sizeof ( long_1s_task_tbl[0] ) )
		{
			long_slot_cnt = 0;
			break;
//			return;
		}		
	}
#endif	
}


/*======================================================
Version	Date			Remart.
--------------------------------------------------------
Ver1_00	2012-07-22	新规做成 by 周前

========================================================*/
