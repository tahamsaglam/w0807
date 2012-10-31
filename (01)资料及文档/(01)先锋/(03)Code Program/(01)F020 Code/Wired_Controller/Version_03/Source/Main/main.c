/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	ģ��		: ȫģ��
	����		: �������ļ����������е����
	����		: Zq
	��ע		: �������У���Ҫ�ǹ������״̬����֤CPU���㹻�Ŀ���״̬

*****************************************************************/
/*===============================================================
ͷ�ļ�����
================================================================*/
#include "c8051F330.h"
#include <string.h>		/*include the memset��memcp*/
#include "./main.h"
#include "../Common/Common.h"
#include "../ADC/AD_Drive.h"


#define TIMER_DEBUG
#define TIMER3_INTERREPT  25		/*Timer3 ��ʱ25ms���ж�һ��*/
//#define TIMER_DB_COUNT (2000/TIMER3_INTERREPT)	/*��ʱ2s*/	
#define TIMER_DB_COUNT (1)	/*��ʱ2s*/	
#define	Time_2SEC_char8		0x100-0xff 	/* 2000ms(78K4: 1989.0ms �^ M16C: 2040.0ms) */
/*===============================================================
Static RAM����
===============================================================*/
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

private unsigned char matrix_key;		/*����ֵ�洢*/
sbit KeyFixTimerFlag;			/*�������������־λ��64ms����ʱ��*/

#define	KEY_IN_WAIT		0
#define	KEY_TIME_OVER_WAIT	1
#define	KEY_REMOVE_WAIT		2
#define	KEY_AFTER_REMOVE	3

static	unsigned char	kda4;				/* output keycode */
static	unsigned char	keydcd_status;
static	unsigned char	timeoverkey;
static	unsigned char	keydecode_timer;
static	unsigned char	inputkey_buf;
static	unsigned char	directcode;		

/****************************************************************
TASK���ݽṹ
*****************************************************************/
typedef struct
{
	_UINT task_short_count  	:1;		/*8ms����*/
	_UINT task_middle_count 	:3;		/*32ms����*/		
	_UINT task_long_count 		:7;		/*1s����*/
/*	_UBYTE  			   		:1;	*//*����*/
}S_TASK_COUNT;

enum{
	TASK_SHORT_FUNC	=0x00	/*2ms task bit����*/
,	TASK_MIDDLE_FUNC			/*8ms task bit����*/
,	TASK_LONG_FUNC			/*16ms task bit����*/
};
/*==============================================================
Global RAM����
===============================================================*/
private Keycode inputkey;		/*�����ײ�ṹ�����*/
private S_TASK_COUNT task_count;
private _UBYTE task_table[(TASK_LONG_FUNC/8)+1];
#define main_set_task_bit(c)  (task_table[c/8] |= (0x01<<(c%8)))
#define main_clear_task_bit(c)  (task_table[c/8] &= ~(0x01<<(c%8)))
#define main_check_task_bit(c)  ((task_table[c/8]&(0x01<<(c%8)))!=0?TRUE:FALSE)
#ifdef TIMER_DEBUG
private unsigned int timer_count;
private unsigned char buff_p1;
#endif/*TIMER_DEBUG*/
/*===============================================================
��������
===============================================================*/
private void SysReset(void);			/*ϵͳʱ�ӳ�ʼ��*/
private void PortReset(void);			/*�˿ڳ�ʼ������*/
private void DACInit( void );			/*DAC���ڻ��趨*/
private void OSC_Init( void );			/*ϵͳʱ�ӳ��ڻ��趨*/
private void TMR3_ClockSet( void );/*Timer3ʱ���趨*/
public void matrixkey_32ms_task( void );/**/
static	void	status_wait_change( unsigned char wait_timer , unsigned char nextkey );

/**************************************
Funcname	: KeyReset
Contents 	: ������ʼ������
Input		: void
Return		: void
Remart		:  

***************************************/
private void KeyReset(void)
{
	KeyFixTimerFlag = 0;
	matrix_key = PRECODE_NULL;
}

/**************************************
Funcname	: SysClkIn
Contents 	: ϵͳʱ�ӳ�ʼ������
Input		: void
Return		: void
Remart		:  

***************************************/
private void SysReset(void)
{
	OSC_Init();
	PortReset();
	DACInit();
	TMR3_ClockSet();
	ADC_InitReset();
	IE   = IE_SET;
	IP   = IP_SET;
	EIE1 = EIE1_SET;
	EIP1 = EIP1_SET;
	memset(&task_count,0x00,sizeof(task_count));
}


/**************************************
Funcname	: Port_Init
Contents 	: �˿ڳ�ʼ������
Input		: void
Return		: void
Remart		:  

***************************************/
private void  PortReset(void)
{
	P0MDIN 	= PT_P0_IN_MD;		/**/
	P0MDOUT = PT_P0_OUT_MD;		/**/
	P0SKIP	= PT_P0_SKIP_MODE;	/**/

	P1MDIN 	= PT_P1_IN_MD;		/**/
	P1MDOUT = PT_P1_OUT_MD;		/**/
	P1SKIP	= PT_P1_SKIP_MODE;	/**/

#if 1
//	P2MDIN 	= PT_P2_IN_MD;		/**/
	P2MDOUT = PT_P2_OUT_MD;		/**/
//	P2SKIP	= PT_P2_SKIP_MODE;	/**/
#else
	P2MDIN.0 	= 0;
	P2MDOUT.0 	= 0;
#endif
	P0 = 0xff;
	P1 = 0xff;
	P2 = 0xff;
#ifdef TIMER_DEBUG
	buff_p1 = 0xff;
	timer_count = 0x0000;
#endif/*TIMER_DEBUG*/
}


/**************************************
Funcname	: DACInit
Contents 	: DAC���ڻ��趨
Input		: void
Return		: void
Remart		:	 

***************************************/
private void 
DACInit( void )
{
	IDA0CN = DAC_INIT_SET;
}


/**************************************
Funcname	: OSC_Init
Contents 	: ϵͳʱ�ӳ��ڻ��趨
Input		: void
Return		: void
Remart		:	 
			  �����ڲ�H-F������ʹ�û�Ƶ24.5M��
			  ϵͳʱ�Ӳ���Ƶ(24.5M)				
***************************************/
private void 
OSC_Init( void )
{
	OSCICN = OSC_INIT_SET;
	CLKSEL = OSC_FREQ_SET;
	OSCICL = OSCICL+OSC_ICL_SET;	
}


/**************************************
Funcname	: TMR3_ClockSet
Contents 	: Timer3ʱ���趨
Input		: void
Return		: void
Remart		: 16λ�Զ����أ���ʱ

***************************************/
private void 
TMR3_ClockSet( void )
{
	TMR3RLL = TIMER3_RLL_SET;	
	TMR3RLH = TIMER3_RLH_SET;
	TMR3L	= TIMER3_RLL_SET;
	TMR3H   = TIMER3_RLH_SET;
	CKCON   = TIMER3_CKCON_SET;
	TMR3CN  = TIMER3_CLOCK_SET;
}


/**************************************
Funcname	: matrixkey_32ms_task
Contents 	: matrix key 32ms 
Input		: void
Return		:
			  Matrix Precode
NOTES		: 32Ms
***************************************/
public void matrixkey_32ms_task( void )
{
#ifdef SYSTEM_MATRIX_KEY
	unsigned char matrix_key_buf  = KEY_PORT_STATUS;	

	if(( matrix_key_buf != 0x00 ))
	{
		if( 0 == KeyFixTimerFlag )
		{
			switch( matrix_key_buf )
				{
					case KEY1_DEF:
					{
				//		inputkey.matrix = PRECODE_KEY1;
						matrix_key = PRECODE_KEY1;
					}
						break;

					case KEY2_DEF:
					{
				//		inputkey.matrix = PRECODE_KEY2;
						matrix_key = PRECODE_KEY2;
					}
						break;
						
					case KEY3_DEF:
					{
				//		inputkey.matrix = PRECODE_KEY3;
						matrix_key = PRECODE_KEY3;
					}
						break;
						
					case KEY12_DEF:
					{
				//		inputkey.matrix = PRECODE_DB_12;
						matrix_key = PRECODE_DB_12;
					}
						break;
						
					case KEY13_DEF:
					{
				//		inputkey.matrix = PRECODE_DB_13;
						matrix_key = PRECODE_DB_13;
					}
						break;
						
					case KEY23_DEF:
					{
				//		inputkey.matrix = PRECODE_DB_23;
						matrix_key = PRECODE_DB_23;
					}
						break;		

					default:
						matrix_key = PRECODE_NULL;
						break;
				}
			KeyFixTimerFlag = 1;
		}
		else
		{
			if( matrix_key != matrix_key_buf )

			{
				switch( matrix_key_buf )
					{
						case KEY1_DEF:
						{
					//		inputkey.matrix = PRECODE_KEY1;
							matrix_key = PRECODE_KEY1;
						}
							break;

						case KEY2_DEF:
						{
					//		inputkey.matrix = PRECODE_KEY2;
							matrix_key = PRECODE_KEY2;
						}
							break;
							
						case KEY3_DEF:
						{
					//		inputkey.matrix = PRECODE_KEY3;
							matrix_key = PRECODE_KEY3;
						}
							break;
							
						case KEY12_DEF:
						{
					//		inputkey.matrix = PRECODE_DB_12;
							matrix_key = PRECODE_DB_12;
						}
							break;
							
						case KEY13_DEF:
						{
					//		inputkey.matrix = PRECODE_DB_13;
							matrix_key = PRECODE_DB_13;
						}
							break;
							
						case KEY23_DEF:
						{
					//		inputkey.matrix = PRECODE_DB_23;
							matrix_key = PRECODE_DB_23;
						}
							break;		

						default:
							matrix_key = PRECODE_NULL;
							break;
					}
					KeyFixTimerFlag = 0;
				}
				else{
					inputkey.matrix = matrix_key;
				}
			
		}
	}
		else{
				
		}
	}
	else
	{
		matrix_key = PRECODE_NULL;
		KeyFixTimerFlag = 0;
	}
	
#endif/*SYSTEM_MATRIX_KEY*/
}


/**************************************
Funcname	: 
Contents 	: 
Input		: 
Return		:
NOTES		: 
***************************************/
static	unsigned char	key_remove( unsigned char oldkey )
{
	keydcd_status = KEY_AFTER_REMOVE;
	return inputkey.matrix ;
}

/**************************************
Funcname	: 
Contents 	: 
Input		: 
Return		:
NOTES		: 
***************************************/
static	unsigned char	keydcd_timer_over( unsigned char pushkey )
{
	keydcd_status = KEY_REMOVE_WAIT;
	return( timeoverkey );
}

/**************************************
Funcname	: 
Contents 	: 
Input		: 
Return		:
NOTES		: 
***************************************/
static	unsigned char	key_in( unsigned char input )
{	
	switch( input )
	{
		case PRECODE_KEY1:
		{	
			status_wait_change( Time_2SEC_char8 , Dircode2Keycode_tbl[ input ][ 1 ]);
			return( NULL_KEY);
		}
		break;
		case PRECODE_KEY2:
		{
			status_wait_change( Time_2SEC_char8 , Dircode2Keycode_tbl[ input ][ 1 ]);
			return( NULL_KEY);
		}
		break;
		case PRECODE_KEY3:
		{
			status_wait_change( Time_2SEC_char8 , Dircode2Keycode_tbl[ input ][ 1 ]);
			return( NULL_KEY);
		}
		break;	
		default:
			return( NULL_KEY );
	}
}

/**************************************
Funcname	: 
Contents 	: 
Input		: 
Return		:
NOTES		: 
***************************************/
static	void	status_wait_change( unsigned char wait_timer , unsigned char nextkey )
{
	keydcd_status	= KEY_TIME_OVER_WAIT;
	keydecode_timer	= wait_timer;
	timeoverkey	= nextkey;
}


/**************************************
Funcname	: 
Contents 	: 
Input		: 
Return		:
NOTES		: 
***************************************/
extern	void	keydcd( void )
{	

/******************************---------------*/

	unsigned char	input_precode;
	unsigned char	output_keycode;

	output_keycode = kda4;
	input_precode = inputkey.matrix ;

	if( (inputkey_buf != input_precode) || (keydcd_status == KEY_AFTER_REMOVE) ){

		if( keydcd_status == KEY_TIME_OVER_WAIT ){
			output_keycode 	= key_remove( directcode );
		}else{		
			timeoverkey 	= NULL_KEY;
			keydcd_status 	= KEY_IN_WAIT;
			inputkey_buf 	= input_precode;

			directcode 	= inputkey.matrix;
			output_keycode 	= key_in( directcode );
		}
	}else{	
		if( (keydcd_status == KEY_TIME_OVER_WAIT) && (keydecode_timer == 0) ){
			output_keycode	= keydcd_timer_over( directcode );
		}else{
			
		}
	}
	
		
		if( output_keycode != kda4 )
		{
			//appli_keychange( output_keycode , kda4 );	
			kda4 = output_keycode;
		}
/*____________________________________***********/

}

/**************************************
Funcname	: main
Contents 	: ������
Input		: void
Return		: void
Remart		: ������������ִ��

***************************************/
void main(void)
{
	SysReset();
	while(1)
	{
		if( task_table[0]!=0x00 )
		{
			if( TRUE == main_check_task_bit(TASK_SHORT_FUNC) )
			{
				//remon_short_func_task();
				main_clear_task_bit(TASK_SHORT_FUNC);
				task_count.task_short_count = 0;
			}
			if( TRUE == main_check_task_bit(TASK_MIDDLE_FUNC) )
			{
				//remon_middle_func_task();
				matrixkey_32ms_task();
				main_clear_task_bit(TASK_MIDDLE_FUNC);
				task_count.task_middle_count = 0;
			}
			if( TRUE == main_check_task_bit(TASK_LONG_FUNC) )
			{
				//remon_long_func_task();	
				#ifdef TIMER_DEBUG
				timer_count ++;
				if( timer_count %10 == 0 )
				{
					DAC_DateOutput(timer_count);
				}
				if( TIMER_DB_COUNT <= timer_count )
				{
					timer_count = 0;
					buff_p1 = ~buff_p1 ;
					P1 = buff_p1; 			/*P1ÿ��2s����תһ��*/
				}
				#endif/*TIMR_DEBUG*/
				task_count.task_long_count = 0;
				main_clear_task_bit(TASK_LONG_FUNC);
				AD_get_value();
			}
		}	
	}
}


/**************************************
Funcname	: TASK_TIMER
Contents 	: ��ʱ��Timer3�жϣ�2ms�ж�һ��
Input		: void
Return		: void
Remart		: 
			  �Զ����ط�ʽ
***************************************/
void TASK_TIMER3() interrupt 14
{
#ifdef AUTO_RL 
	TMR3L	= TIMER3_RLL_SET;
	TMR3H   = TIMER3_RLH_SET;
#endif/*AUTO_RL*/
	task_count.task_short_count += 1;
	task_count.task_middle_count += 1;
	task_count.task_long_count += 1; 

	if( 1 <= task_count.task_short_count  )
	{
		main_set_task_bit( TASK_SHORT_FUNC );
	}
	if( 4 <= task_count.task_middle_count )
	{
		main_set_task_bit( TASK_MIDDLE_FUNC );
	}
	if( 125 <= task_count.task_long_count  )
	{
		main_set_task_bit( TASK_LONG_FUNC );
	}	

#ifdef DEBUG
	main_set_task_bit( TASK_SHORT_FUNC );
	if( 1 <= task_count.task_short_count  )
	{
		task_count.task_middle_count +=1;
	}
	if( 2 <= task_count.task_middle_count )
	{
		main_set_task_bit( TASK_MIDDLE_FUNC );
		task_count.task_long_count += 1;
	}
	if( 29 <= task_count.task_long_count  )
	{
		main_set_task_bit( TASK_LONG_FUNC );
	}	
#endif
	ClearIFTMR3();/*������TMR3�жϱ�־λ*/
}

/*===================================================================
Version	|	Date		|	Author	|	Contents  					|
---------------------------------------------------------------------
V1.00		2012-10-17		ZQ			New code write.

====================================================================*/
