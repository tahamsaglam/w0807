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
#include "c8051f020.h"
#include "../CPU/Comm.h"	
#include "../Task/Remon_task.h"
#include <string.h>		/*include the memset��memcp*/
#include "./main.h"
#include "../../Manager/Display/Display.h"

/*==============================================================
Global RAM����
===============================================================*/


/*===============================================================
Static RAM����
===============================================================*/
private _UBYTE input_key ;			/*����ֵ����*/
typedef struct
{
	_UBYTE task_2ms_count  :1;
	_UBYTE task_8ms_count  :3;
	_UBYTE task_16ms_count :4;
	_UBYTE  			   	 :1;	/*����*/
}S_TASK_COUNT;

#define TASK_NUMBER_MAX = 1
private S_TASK_COUNT task_count;
private _UBYTE task_table[(TASK_16MS_FUNC/8)+1];

#define main_set_task_bit(c)  (task_table[c/8] |= (0x01<<(c%8)))
#define main_clear_task_bit(c)  (task_table[c/8] &= ~(0x01<<(c%8)))
#define main_check_task_bit(c)  ((task_table[c/8]&(0x01<<(c%8)))!=0?TRUE:FALSE)


/*===============================================================
��������
===============================================================*/
private void SysClkIn(void);		/*ϵͳʱ�ӳ�ʼ��*/	
private void Port_Init(void);		/*�˿ڳ�ʼ������*/
private void Watch_dog_set( void );	/*���Ź��趨����*/
private void main_reset_init( void );	/*������ģ���ʼ������*/
private void Timer2_Init (void);

/**************************************
Funcname	: SysClkIn
Contents 	: ϵͳʱ�ӳ�ʼ������
Input		: void
Return		: void
Remart		: OSCICN����Ϊ8Mϵͳʱ��	 

***************************************/
private void SysClkIn(void)
{
//	OSCICN=0x04 | 0x00;    //ѡ���ڲ�����2MHZ 
//	OSCICN|=0x07;			
	OSCXCN|=0x07;	//8M
//	CKCON = 0x08;	//0x08��ʱ��T0��ʱ��ѡ��ʹ��ϵͳʱ��
/*	CKCON = 0x00;	//0x00��ʱ��T0��ʱ��ѡ��ʹ��ϵͳʱ��/12	*/	
	TCON  |= 0x10;	/*��ʱ��TR0��������*/
	TMOD  |= 0x01;	/*��ʱ��T0Ϊ16λ��������ʽ*/
	ET0 = 1;		/*ʹ�ܶ�ʱ��T0�ж�*/
//	IP = 0x02;		/*��ʱ��T0�ж����ȼ���*/
	TL0  = 0x00;	/*ϵͳʱ��8M��12��Ƶ����ʱ2ms*/
    TH0  = 0x00;
}

	
/**************************************
Funcname	: Watch_dog_set
Contents 	: ���Ź��趨����
Input		: void
Return		: void
Remart		: ���Ź���ʼ������,Ĭ�Ϲؿ��Ź� 

***************************************/
private void Watch_dog_set( void )
{
	EA=0;   			/*��ֹ�����ж�*/                 
	WDTCN = 0xde;   	/*��ֹWDT*/
	WDTCN = 0xAD;
	EIE2 |= 0x20;
	EA = 1;				/*�����ж�*/  
}

/**************************************
Funcname	: Port_Init
Contents 	: �˿ڳ�ʼ������
Input		: void
Return		: void
Remart		:  

***************************************/
private void  Port_Init(void)
{
	XBR2 = 0x40;      //ʹ�ܽ��濪�غ�������
//	XBR2 = 0x00;      //��ʹ�ܽ��濪�غ���������P0-P3ǿ��Ϊ����
//	P1MDOUT = 0xFF;//�������
	P1MDOUT = 0x00;//©����·���
//	P1MDIN 	= 0x00;	/*���*/ 	
	P1MDIN 	= 0xff;	/*����*/ 
	P2MDOUT = 0xFF;	//�������
//	P3MDOUT = 0xff;
	P3MDOUT = 0x00;
	P1 =	0xff;
}

/**************************************
Funcname	: main_reset_init
Contents 	: ������ģ���ʼ������
Input		: void
Return		: void
Remart		:  

***************************************/
private void main_reset_init( void )
{
	SysClkIn( );				/*ϵͳʱ�ӳ�ʼ��*/
	Watch_dog_set( );			/*���Ź���ʼ��*/
	Port_Init();				/*�˿ڳ�ʼ������*/
	Timer2_Init();				/*��ʱ��T2��ʼ��*/
	input_key = 0x00;			/*����ֵ���溯��*/
	memset(&task_table,0x00,sizeof(task_table[0]));
	memset(&task_count,0x00,sizeof(task_count));
}

/**************************************
Funcname	: main_get_key
Contents 	: �ⲿ�жϣ����ص�ǰ����ֵ
Input		: void
Return		: _UBYTE
Remart		:  

***************************************/
public _UBYTE main_get_key( void )
{
	return ( input_key );
}


/**************************************
Funcname	: INT0_SVC
Contents 	: �ⲿ�жϴ�����
Input		: void
Return		: void
Remart		: ��ȡ����ֵ

***************************************/
void INT0_SVC() interrupt 19
{
//	input_key = ZLG7289_Key();
	P3IF &=0x7f;
}


/**************************************
Funcname	: TASK_TIMER
Contents 	: ��ʱ���жϣ�2ms�ж�һ��
Input		: void
Return		: void
Remart		: 

***************************************/
void TASK_TIMER0() interrupt 1
{
#ifdef DEBUG

		task_count.task_2ms_count = 1;
		task_count.task_8ms_count+=1;
		task_count.task_16ms_count+=1;
		main_set_task_bit( TASK_2MS_FUNC );
	if( 1 == task_count.task_2ms_count  )
	{
		task_count.task_2ms_count=0;
	}
	if( 4 == task_count.task_8ms_count )
	{
		main_set_task_bit( TASK_8MS_FUNC );
	}
	if( 8 == task_count.task_16ms_count  )
	{
		main_set_task_bit( TASK_16MS_FUNC );
	}	
#endif
}

/**************************************
Funcname	: Timer2_Init
Contents 	: ��ʱ��T2��ʼ������
Input		: void
Return		: void
Remart		: 

***************************************/
private void Timer2_Init (void)
{

   	T2CON|=0x00;				//
 	TR2 = 1;					// ʹ�� Timer2
	TH2 = 0xfa;					//����T2 2ms��ʱ
   	TL2 = 0x00;					
	CKCON |= 0x00;				//b'0000 0000ϵͳʱ�ӵ�12��Ƶ 
   	RCAP2L = 0x00;
	RCAP2H = 0X00;
	ET2= 1;						//ʹ��T2�ж�(ET0=1)
}

/*
����:��ʱ��T2�жϷ������
��ڲ���:��
���ڲ���:��
��ע:һ���ж�һ��
*/			 
void Timer2_ISR (void) interrupt 5
{
	task_count.task_2ms_count +=1;
	task_count.task_8ms_count+=1;
	task_count.task_16ms_count+=1;
	if( 1 <= task_count.task_2ms_count  )
	{
		main_set_task_bit( TASK_2MS_FUNC );
		task_count.task_2ms_count = 0;
	}
	if( 4 <= task_count.task_8ms_count )
	{
		main_set_task_bit( TASK_8MS_FUNC );
		task_count.task_8ms_count = 0;
	}
	if( 8 <= task_count.task_16ms_count  )
	{
		main_set_task_bit( TASK_16MS_FUNC );
		task_count.task_16ms_count =0;
	}	
	TH2 = 0x1a;					//����T2 2ms��ʱ
   	TL2 = 0x00;		
	TF2 = 0;			//���T2����жϱ�־�������������
/******************* MAIN_DEBUG ***********************/	
	if((task_table[0] ==0x07)&&(TR2==1))
	{
		TR2 = 0;
	}
/******************* MAIN_DEBUG ***********************/		
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
	main_reset_init();			/*Mainģ�������ʼ��*/
	remon_init_func_task();		/*��ʼ������*/
	while(1)
	{	
		if( task_table[0]!=0x00 )
		{
			if( TRUE == main_check_task_bit(TASK_2MS_FUNC) )
			{
				remon_short_func_task();
				main_clear_task_bit(TASK_2MS_FUNC);
			}
			if( TRUE == main_check_task_bit(TASK_8MS_FUNC) )

			{
				remon_middle_func_task();
				main_clear_task_bit(TASK_8MS_FUNC);
			}
			if( TRUE == main_check_task_bit(TASK_16MS_FUNC) )
			{
				remon_long_func_task();	
				main_clear_task_bit(TASK_16MS_FUNC);
			}
		}	
/******************* MAIN_DEBUG ***********************/

		if((TR2 == 0)&&(task_table[0] ==0x00))
		{
			TR2 = 1;
		}
/******************* MAIN_DEBUG ***********************/		
	}
}


/*===================================================================
Version	|	Date		|	Author	|	Contents  					|
---------------------------------------------------------------------
V1.00		2012-08-19		ZQ			New code write.

====================================================================*/
