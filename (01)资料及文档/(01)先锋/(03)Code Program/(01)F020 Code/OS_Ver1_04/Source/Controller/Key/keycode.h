/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	模型		: 
	机能		: 
	作者		: 
	备注		:
			 
*****************************************************************/
#ifndef _KEYDECODE_H_
#define _KEYDECODE_H_
/***************************************
Key Globe
***************************************/
#define DB_12  0x10 /*KEY1 and KEY2*/
#define DB_13  0x11 /*KEY1 and KEY3*/
#define DB_23  0x12 /*KEY2 and KEY3*/



/********************************************************
按键键值定义
*********************************************************/

enum{
	NULL_KEY = 0x00		/* 0x00		*/
,	KEY0_KEY			/* 0x01		*/
,	KEY0_2S_KEY			/* 0x02		*/
,	KEY1_KEY			/* 0x03		*/
,	KEY1_2S_KEY			/* 0x04		*/
,	KEY2_KEY			/* 0x05		*/
,	KEY2_2S_KEY			/* 0x06		*/
,	KEY3_KEY			/* 0x07		*/
,	KEY3_2S_KEY			/* 0x08		*/
,	KEY0_KEY1_KEY		/* 0x09		*/
,	KEY0_KEY2_KEY		/* 0x0a		*/
,	KEY0_KEY3_KEY		/* 0x0b		*/
,	KEY1_KEY2_KEY		/* 0x0c		*/
,	KEY1_KEY3_KEY		/* 0x0d		*/
,	KEY2_KEY3_KEY		/* 0x0e		*/

};


#endif
/*===================================================================
Version	|	Date		|	Author	|	Contents  					|
---------------------------------------------------------------------
V1.00		2012-08-19		ZQ			New code write.

====================================================================*/