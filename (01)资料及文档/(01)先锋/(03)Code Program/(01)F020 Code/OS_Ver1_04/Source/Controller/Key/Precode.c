/* copyright (c)  zhouqian 2012-07-15 All rights reserved   */
/****************************************************************
	模型		: 
	机能		: 
	作者		: 
	备注		:
			 
*****************************************************************/
/*===============================================================
头文件声明
================================================================*/
#include "./Precode.h"
#include "../../Common/comm_soft.def"		/*功能定义、ARI定义*/


/*===============================================================
PRECODE解析成DIRCODE对应表: 
	  PRECODE  ====> DIRCODE
================================================================*/
const _UBYTE dircode_tbl[] = {
	DIRCODE_NULL			/*0x00	PRCODE_NULL*/
,	DIRCODE_KEY0 			/*0x01	PRCODE_KEY0*/
,	DIRCODE_KEY1			/*0x02	PRCODE_KEY1*/
,	DIRCODE_KEY2			/*0x03	PRCODE_KEY2*/
,	DIRCODE_KEY3			/*0x04	PRCODE_KEY3*/
,	DIRCODE_DB_01			/*0x05	PRCODE_DB_01*/
,	DIRCODE_DB_02			/*0x06	PRCODE_DB_02*/
,	DIRCODE_DB_03			/*0x07	PRCODE_DB_03*/
,	DIRCODE_DB_12			/*0x08	PRCODE_DB_12*/
,	DIRCODE_DB_13			/*0x09	PRCODE_DB_13*/
,	DIRCODE_DB_23			/*0x0a	PRCODE_DB_23*/
};


/**************************************
Funcname	: precode_to_dircode
Contents 	: Precode 解析
Input		:
			  oldkey
			  newkey
Return		: Precode
NOTES		: 16ms执行一次,即将进入按键解析
***************************************/
public	_UBYTE precode_to_dircode( _UBYTE input_precode )
{
	_UBYTE output_dircode;
	output_dircode = dircode_tbl[input_precode];
	
#ifdef 	PRECODE_ARI	
	switch( input_precode ){
		case PRECODE_KEY1:
		{	
		}
		break;
			
		case PRECODE_KEY2:
		{
		}
		break;

		case PRECODE_KEY3:
		{
		}
		break;

		default:
		{
		}
		break;
	}
#endif/*DIRCODE_ARI*/

	return( output_dircode );
}






/*===================================================================
Version	|	Date		|	Author	|	Contents  			|
---------------------------------------------------------------------
V1.00		2012-08-17		ZQ			New code write.

====================================================================*/
