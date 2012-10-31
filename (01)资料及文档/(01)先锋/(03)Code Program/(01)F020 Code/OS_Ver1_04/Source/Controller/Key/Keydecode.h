#ifndef _keydecode_h_
#define _keydecode_h_

public void key_reset_init( void );/*按键初始化函数*/
public void keychange_16m_task( void );/*key 变化解析*/
public void key_8mtimer_task( void );		/*Key 8ms函数*/

#endif/*_keydecode_h_*/
/*===================================================================
Version	|	Date		|	Author	|	Contents  			|
---------------------------------------------------------------------
V1.00		2012-08-17		ZQ			New code write.

====================================================================*/

