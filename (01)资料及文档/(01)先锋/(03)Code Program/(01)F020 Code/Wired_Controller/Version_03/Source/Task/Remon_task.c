/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	ģ��		: ȫģ��
	����		: Task��֯�ļ�
	����		: Zq
	��ע		: ��ģ�飬��Ҫ�Ķ����ڡ�Task������Ҫ������ļ��ﱻ����
*****************************************************************/


/**************************************
ͷ�ļ�����
***************************************/

/**************************************
Global RAM����
***************************************/



/**************************************
Static RAM����
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
	timer_reset_init		/*Timerģ���ʼ������*/
,	menu_reset_init			/*Menuģ���ʼ������*/
,	set_menu_reset_init		/*Setupģ���ʼ������*/
,	key_reset_init			/*Keyģ���ʼ������*/
,	Display_reset_init		/*��ʾģ���ʼ������*/
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
	key_8mtimer_task		/*�������¼��Ӻ�����8ms*/
,	timer_8ms_task			/*timer 8ms ���Ӻ���*/
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
	disp_function			//��ʾ���£�16msһ��
,	keychange_16m_task
#endif
};


/**************************************
Funcname		: remon_init_func_task
Contents 	: ��ʼ����
Input		: void
Return		: void
Remart		: 	 

***************************************/
public void remon_init_func_task( void )
{
	static unsigned char init_slot_cnt;		/*��������*/
	void (*init_task )( void );			/*���庯��ָ�룬ָ������*/
	
	while( init_slot_cnt < (sizeof( init_func_tab ) / sizeof ( init_func_tab[0] )) )
	{
		init_task = init_func_tab[init_slot_cnt];
		(*init_task)(  );/*ִ�к���ָ��ָ��ĺ���*/
		init_slot_cnt++;
	}
	init_slot_cnt = 0;
#if 0 	
	for( init_slot_cnt = 0; init_slot_cnt < sizeof( init_func_tab )/sizeof( init_func_tab[0] );init_slot_cnt++	 )
	{		/*���г��ڻ�����Tbl�����к���*/
		init_task = init_func_tab[init_slot_cnt];
		(*init_task)(  );/*ִ�к���ָ��ָ��ĺ���*/	
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
	static unsigned char short_slot_cnt = 0;		/*��������*/
	void (*short_task )( void );			/*���庯��ָ�룬ָ������*/
	while( short_slot_cnt < (sizeof( short_8ms_task_tbl ) / sizeof ( short_8ms_task_tbl[0] )) )
	{
		short_task = short_8ms_task_tbl[short_slot_cnt];
		(*short_task)(  );/*ִ�к���ָ��ָ��ĺ���*/
		short_slot_cnt ++;
	}
	short_slot_cnt = 0;
#if 0
	for( short_slot_cnt = 0; short_slot_cnt <= sizeof( short_8ms_task_tbl )/sizeof( short_8ms_task_tbl[0] );short_slot_cnt++	 )
	{		/*����8ms����Tbl�����к���*/
		short_task = short_8ms_task_tbl[short_slot_cnt];
		(*short_task)(  );/*ִ�к���ָ��ָ��ĺ���*/
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
	static unsigned char middle_slot_cnt;		/*��������*/
	void (*middle_task )( void );		/*���庯��ָ�룬ָ������*/
	while( middle_slot_cnt < (sizeof( middle_32ms_task_tbl ) / sizeof ( middle_32ms_task_tbl[0] ) ))
	{
		middle_task = middle_32ms_task_tbl[middle_slot_cnt];
		(*middle_task)(  );/*ִ�к���ָ��ָ��ĺ���*/
		middle_slot_cnt ++;
	}
	middle_slot_cnt=0;
#if 0	
	for( middle_slot_cnt = 0; middle_slot_cnt <= sizeof( middle_32ms_task_tbl )/sizeof( middle_32ms_task_tbl[0] );middle_slot_cnt++	 )
	{		/*����8ms����Tbl�����к���*/
		middle_task = middle_32ms_task_tbl[middle_slot_cnt];
		(*middle_task)(  );/*ִ�к���ָ��ָ��ĺ���*/
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
	static unsigned char long_slot_cnt;		/*��������*/
	void (*long_task )( void );			/*���庯��ָ�룬ָ������*/
	while( long_slot_cnt < (sizeof( long_1s_task_tbl ) / sizeof ( long_1s_task_tbl[0] )) )
	{
		long_task = long_1s_task_tbl[long_slot_cnt];
		(*long_task)(  );/*ִ�к���ָ��ָ��ĺ���*/
		long_slot_cnt++;
	}
	long_slot_cnt =0;
#if 0	
	for( long_slot_cnt = 0; long_slot_cnt <= sizeof( long_1s_task_tbl )/sizeof( long_1s_task_tbl[0] );long_slot_cnt++	 )
	{		/*����16ms����Tbl�����к���*/
		long_task = long_1s_task_tbl[long_slot_cnt];
		(*long_task)(  );/*ִ�к���ָ��ָ��ĺ���*/
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
Ver1_00	2012-07-22	�¹����� by ��ǰ

========================================================*/
