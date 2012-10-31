/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	模型		: 全模型
	机能		: 主函数文件，程序运行的入口
	作者		: Zq
	备注		: 主函数中，主要是管理各种状态，保证CPU有足够的空闲状态

*****************************************************************/
/*===============================================================
头文件声明
================================================================*/
#include "c8051f020.h"
#include "../CPU/Comm.h"	
#include "../Task/Remon_task.h"
#include <string.h>		/*include the memset、memcp*/
#include "./main.h"
#include "../../Manager/Display/Display.h"

/*==============================================================
Global RAM声明
===============================================================*/


/*===============================================================
Static RAM声明
===============================================================*/
private _UBYTE input_key ;			/*按键值变量*/
typedef struct
{
	_UBYTE task_2ms_count  :1;
	_UBYTE task_8ms_count  :3;
	_UBYTE task_16ms_count :4;
	_UBYTE  			   	 :1;	/*留用*/
}S_TASK_COUNT;

#define TASK_NUMBER_MAX = 1
private S_TASK_COUNT task_count;
private _UBYTE task_table[(TASK_16MS_FUNC/8)+1];

#define main_set_task_bit(c)  (task_table[c/8] |= (0x01<<(c%8)))
#define main_clear_task_bit(c)  (task_table[c/8] &= ~(0x01<<(c%8)))
#define main_check_task_bit(c)  ((task_table[c/8]&(0x01<<(c%8)))!=0?TRUE:FALSE)


/*===============================================================
函数声明
===============================================================*/
private void SysClkIn(void);		/*系统时钟初始化*/	
private void Port_Init(void);		/*端口初始化函数*/
private void Watch_dog_set( void );	/*看门狗设定函数*/
private void main_reset_init( void );	/*主函数模块初始化函数*/
private void Timer2_Init (void);

/**************************************
Funcname	: SysClkIn
Contents 	: 系统时钟初始化函数
Input		: void
Return		: void
Remart		: OSCICN配置为8M系统时钟	 

***************************************/
private void SysClkIn(void)
{
//	OSCICN=0x04 | 0x00;    //选择内部振荡器2MHZ 
//	OSCICN|=0x07;			
	OSCXCN|=0x07;	//8M
//	CKCON = 0x08;	//0x08定时器T0的时钟选择，使用系统时钟
/*	CKCON = 0x00;	//0x00定时器T0的时钟选择，使用系统时钟/12	*/	
	TCON  |= 0x10;	/*定时器TR0运行允许*/
	TMOD  |= 0x01;	/*定时器T0为16位计数器方式*/
	ET0 = 1;		/*使能定时器T0中断*/
//	IP = 0x02;		/*定时器T0中断优先级高*/
	TL0  = 0x00;	/*系统时钟8M，12分频，定时2ms*/
    TH0  = 0x00;
}

	
/**************************************
Funcname	: Watch_dog_set
Contents 	: 看门狗设定函数
Input		: void
Return		: void
Remart		: 看门狗初始化函数,默认关看门狗 

***************************************/
private void Watch_dog_set( void )
{
	EA=0;   			/*禁止所有中断*/                 
	WDTCN = 0xde;   	/*禁止WDT*/
	WDTCN = 0xAD;
	EIE2 |= 0x20;
	EA = 1;				/*开总中断*/  
}

/**************************************
Funcname	: Port_Init
Contents 	: 端口初始化函数
Input		: void
Return		: void
Remart		:  

***************************************/
private void  Port_Init(void)
{
	XBR2 = 0x40;      //使能交叉开关和弱上拉
//	XBR2 = 0x00;      //不使能交叉开关和弱上拉，P0-P3强制为输入
//	P1MDOUT = 0xFF;//推挽输出
	P1MDOUT = 0x00;//漏极开路输出
//	P1MDIN 	= 0x00;	/*输出*/ 	
	P1MDIN 	= 0xff;	/*输入*/ 
	P2MDOUT = 0xFF;	//推挽输出
//	P3MDOUT = 0xff;
	P3MDOUT = 0x00;
	P1 =	0xff;
}

/**************************************
Funcname	: main_reset_init
Contents 	: 主函数模块初始化函数
Input		: void
Return		: void
Remart		:  

***************************************/
private void main_reset_init( void )
{
	SysClkIn( );				/*系统时钟初始化*/
	Watch_dog_set( );			/*看门狗初始化*/
	Port_Init();				/*端口初始化函数*/
	Timer2_Init();				/*定时器T2初始化*/
	input_key = 0x00;			/*按键值保存函数*/
	memset(&task_table,0x00,sizeof(task_table[0]));
	memset(&task_count,0x00,sizeof(task_count));
}

/**************************************
Funcname	: main_get_key
Contents 	: 外部中断，返回当前按键值
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
Contents 	: 外部中断处理函数
Input		: void
Return		: void
Remart		: 读取按键值

***************************************/
void INT0_SVC() interrupt 19
{
//	input_key = ZLG7289_Key();
	P3IF &=0x7f;
}


/**************************************
Funcname	: TASK_TIMER
Contents 	: 定时器中断，2ms中断一次
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
Contents 	: 定时器T2初始化函数
Input		: void
Return		: void
Remart		: 

***************************************/
private void Timer2_Init (void)
{

   	T2CON|=0x00;				//
 	TR2 = 1;					// 使能 Timer2
	TH2 = 0xfa;					//设置T2 2ms定时
   	TL2 = 0x00;					
	CKCON |= 0x00;				//b'0000 0000系统时钟的12分频 
   	RCAP2L = 0x00;
	RCAP2H = 0X00;
	ET2= 1;						//使能T2中断(ET0=1)
}

/*
功能:定时器T2中断服务程序
入口参数:无
出口参数:无
备注:一秒中断一次
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
	TH2 = 0x1a;					//设置T2 2ms定时
   	TL2 = 0x00;		
	TF2 = 0;			//清除T2溢出中断标志，必须软件清零
/******************* MAIN_DEBUG ***********************/	
	if((task_table[0] ==0x07)&&(TR2==1))
	{
		TR2 = 0;
	}
/******************* MAIN_DEBUG ***********************/		
}

/**************************************
Funcname	: main
Contents 	: 主函数
Input		: void
Return		: void
Remart		: 代码从这个函数执行

***************************************/
void main(void)
{ 
	main_reset_init();			/*Main模块变量初始化*/
	remon_init_func_task();		/*初始函数集*/
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
