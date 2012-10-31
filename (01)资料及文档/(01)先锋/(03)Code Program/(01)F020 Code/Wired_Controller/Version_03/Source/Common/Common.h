/* copyright (c)  周前 2012-08-19All rights reserved   */
/****************************************************************
	模型		: All
	机能		: 公共声明文件
	作者		: Zq
	备注		: 声明文件

*****************************************************************/
#ifndef _COMMON_H
#define _COMMON_H
/*===============================================================
一般的共同定义
================================================================*/
#define	TRUE	1			/*真*/
#define	FALSE	0			/*假*/

/*===============================================================
系统共同定义
================================================================*/
#define	ON		1
#define OFF		0

#define	HIGH	1
#define LOW		0

#define	NONE	0

/*===============================================================
数据类型定义
================================================================*/
typedef signed char _SBYTE;
typedef unsigned char _UBYTE;
typedef signed short _SWORD;
typedef unsigned short _UWORD;
typedef signed int _SINT;
typedef unsigned int _UINT;
typedef signed long _SDWORD;
typedef unsigned long _UDWORD;
/*
typedef signed long long _SQWORD;
typedef unsigned long long _UQWORD;
*/

/*===============================================================
可见属性定义
================================================================*/
#define	public		extern		/* UML的" + "	*/
#define	protected	extern		/* UML的" # "	*/
#define	package		extern		/* UML的" ~ "	*/
#define	private		static		/* UML的" - "	*/

#endif/*_COMMON_H*/
/*===================================================================
Version	|	Date		|	Author	|	Contents  					|
---------------------------------------------------------------------
V1.00		2012-08-19		ZQ			New code write.

====================================================================*/

