/**************************************
头文件声明
***************************************/
#include "../../Platform/CPU/Comm.h"	/*公共定义声明头文件*/
#include "./Keyselect.h"		/*包含get_precode_key函数*/
#include "./keycode.h"			/*按键值声明*/
#include "./Precode.h"
#include "../../Common/Timer/TIMER.H"
#include "./keyapplihand.h"		/*应用层按键处理函数*/

/**************************************
Global RAM声明
***************************************/
private	_UBYTE	last_key_buffer;				/*last按键存储*/
private	_UBYTE	precode_buf;					/*Precode暂存变量*/
private	_UBYTE	timeoverkey;					/*定时到按键变量*/
private	_UBYTE	directcode;						/*Dircode暂存变量*/
private	_UBYTE	keydecode_timer;

typedef enum{
	KEY_IN_WAIT = 0x00
,	KEY_TIME_OVER_WAIT
,	KEY_REMOVE_WAIT
,	KEY_AFTER_REMOVE
}E_KEY_STATUS;
private E_KEY_STATUS key_status;		/*按键状态enum定义*/

private void keydecode_reset_init( void );

/*===============================================================
函数声明
===============================================================*/
private	void	status_wait_change( _UBYTE wait_timer , _UBYTE nextkey );
/**************************************
按键表: 
	   第一列为按键的短压键值
	   第二列为按键的长压键值
***************************************/
const _UBYTE Dircode2Keycode_tbl[][2] = {
/*    短压			长压			*/
	{ NULL_KEY	,	0 				}	/* 0x00	PRECODE_NULL	*/
,	{ KEY0_KEY	,	KEY0_2S_KEY		}	/* 0x00 PRECODE_KEY0	*/
,	{ KEY1_KEY	,	KEY1_2S_KEY		}	/* 0x01	PRECODE_KEY1 	*/
,	{ KEY2_KEY	,	KEY2_2S_KEY		}	/* 0x02	PRECODE_KEY2 	*/
,	{ KEY3_KEY	,	KEY3_2S_KEY		}	/* 0x03	PRECODE_KEY3 	*/
,	{ 0			,	KEY1_KEY2_KEY	}	/* 0x04	PRECODE_DB_12 	*/
,	{ 0			,	KEY1_KEY3_KEY	}	/* 0x05	PRECODE_DB_13 	*/
,	{ 0			,	KEY2_KEY3_KEY	}	/* 0x06	PRECODE_DB_23 	*/
,	{ 0			,	0				}	/* 0x07 PRECODE_EERO	*/
};

/**************************************
Funcname	: key_reset_init
Contents 	: 按键初始化函数
Input		: void
Return		: void
NOTES		:

***************************************/
public void key_reset_init( void )
{
	key_select_init();
	keydecode_reset_init();
}

/**************************************
Funcname	: keydecode_reset_init
Contents 	: key select初始化函数
Input		: void
Return		: void
NOTES		:

***************************************/
private void keydecode_reset_init( void )
{
	last_key_buffer = NULL_KEY;
	key_status = KEY_IN_WAIT;
	directcode = 0x00;
	precode_buf = 0x00;
	timeoverkey = 0x00;
	keydecode_timer = 0x00;
}


/**************************************
Funcname	:key_in
Contents 	:按键长短压解析
Input		:
			 input_directcode
Return		:
			 key value
NOTES		:
***************************************/
private	_UBYTE key_in( input_directcode )
{
	_UBYTE output_key;
#if 1	
		switch( input_directcode )
		{
			case DIRCODE_KEY1:
			{
				status_wait_change(Time_2SEC_char8, Dircode2Keycode_tbl[input_directcode][1]);/*发长压按键*/
				output_key = NULL_KEY;
			}
			break;

			case DIRCODE_KEY2:
			{
				status_wait_change(Time_2SEC_char8, Dircode2Keycode_tbl[input_directcode][1]);/*发长压按键*/
				output_key = NULL_KEY;		
			}
			break;

			case DIRCODE_KEY3:
			{
				status_wait_change(Time_2SEC_char8, Dircode2Keycode_tbl[input_directcode][1]);/*发长压按键*/
				output_key = NULL_KEY;		
			}
			break;

			default:
			{
				if ( Dircode2Keycode_tbl[ input_directcode ][ 1 ] != NULL_KEY ){
					status_wait_change( Time_2SEC_char8 , Dircode2Keycode_tbl[ input_directcode ][ 1 ] );
				}else{
					output_key = Dircode2Keycode_tbl[ input_directcode ][ 0 ];
				}
				return( NULL_KEY );
			}
			break;
		}
#endif		
	return( output_key );
}

/**************************************
Funcname	:keydcd_timer_over
Contents 	:
Input		:
			 dircode_key
Return		:
			 key value
NOTES		:
***************************************/
private	_UBYTE	keydcd_timer_over( _UBYTE pushkey )
{
	key_status = KEY_REMOVE_WAIT;
	pushkey = pushkey;		/*消除警告*/
	return( timeoverkey );
}

/**************************************
Funcname	: key_remove
Contents 	:
Input		:
			  dircode_key
Return		:
			  key value
NOTES		:
***************************************/
private	_UBYTE	key_remove( _UBYTE oldkey )
{
	key_status = KEY_AFTER_REMOVE;
	
	return(Dircode2Keycode_tbl[ oldkey ][ 0 ]);
}

/**************************************
Funcname	: key_8mtimer_task
Contents 	: Key 8ms函数
Input		:
			  dircode_key
Return		:
			  key value
NOTES		:
***************************************/
public void key_8mtimer_task( void )
{
	tcount( &keydecode_timer );
	matrixkey_8ms_task();
}

/**************************************
Funcname	:status_wait_change
Contents 	:
Input		:
			 
Return		:
			  
NOTES		:
***************************************/
private	void	status_wait_change( _UBYTE wait_timer , _UBYTE nextkey )
{
	key_status	= KEY_TIME_OVER_WAIT;
	keydecode_timer	= wait_timer;
	timeoverkey	= nextkey;
}

/**************************************
Funcname	: keychange_16m_task
Contents 	: key 变化解析
Input		: void
Return		: void
NOTES		: 16ms执行一次,即将进入按键解析
***************************************/
public	void keychange_16m_task( void )
{
	_UBYTE	input_precode;
	_UBYTE	output_keycode;
	
	output_keycode = last_key_buffer;		/*取出上次按键值*/

	input_precode = get_precode_key();	/*读出Precode_key*/

	if( (precode_buf != input_precode) || ( key_status == KEY_AFTER_REMOVE) ){

		if( key_status == KEY_TIME_OVER_WAIT ){
			output_keycode 	= key_remove( directcode );
		}
		else{		
			timeoverkey 	= NULL_KEY;
			key_status 	= KEY_IN_WAIT;
			precode_buf 	= input_precode;
			directcode 	= precode_to_dircode( precode_buf );
			output_keycode 	= key_in( directcode );
		}
	}
	else{	
		if( (key_status == KEY_TIME_OVER_WAIT) && (keydecode_timer == 0) ){
			output_keycode	= keydcd_timer_over( directcode );
		}
		else{
			/*Do nothing*/
		}
	}

	if( output_keycode != last_key_buffer )
	{
		applihand_keychange( output_keycode );	/*更新所有新按键*/
		last_key_buffer = output_keycode;
	}
}


