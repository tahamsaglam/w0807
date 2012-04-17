/***************************************************************
Module: All
Author: ***
Data  : 2012-04-14
***************************************************************/
/**********************
include define
***********************/
#include"menu.h"
#include"key.h"

/**********************
Global define
***********************/
#define	MENUST_MENU_OFF		0x0000

MenuStatus	menust;

static short int set_up_last_menu;	
static short int set_down_last_menu;
static short int up_down_last_menu;

unsigned char set_up_reset_time;			/*上电延时保护时间调节*/
unsigned char set_up_two_ret_time;			/*两次压机启动间隔时间调节*/
unsigned char set_up_protect_reset;			/*排气超温保护复位时间调节*/
unsigned char set_up_temperature_time;		/*蒸发器传感器与环境传感器温差持续时间调节*/
unsigned char set_up_max_defrost_time ; 	/*最大化霜工作时间调节*/
unsigned char set_up_defrost_adj_time;		/*化霜周期调节*/
unsigned char set_up_two_press_delay_tiem;	/*双压保护开关延时检测时间调节*/


unsigned char set_down_high_temperature;			/*最高上限温度调节，防止用户将温度调节至过高*/
unsigned char set_down_temperature_backlash;		/*温度回差调节H55～d08即470C为开机点*/
unsigned char set_down_stop_temperature;			/*排气过热保护停止温度调节*/
unsigned char set_down_remove_temperature;			/*排气过热保护解除温度调节*/
unsigned char set_down_spray_start_temperature;		/*喷液阀开启温度调节*/
unsigned char set_down_spray_stop_temperature;		/*喷液阀关闭温度调节*/
unsigned char set_down_evaporator_outside_temp;		/*蒸发器温度传感器与户外温度传感器温差值调节*/
unsigned char set_down_defrost_temperature;			/*化霜结束温度调节*/

unsigned char up_down_rest_method;				/*启动方式调节*/
unsigned char up_down_assistance_temperature;	/*辅助电加热切入温度点调节(检测环境温度)*/
unsigned char up_down_overload;					/*压机过载保护调节*/
unsigned char up_down_antifreeze_heat_stop;		/*防冻模式下的加热停止点*/
sbit up_down_elect_heat;						/*热泵出现故障时,电加热是否自动介入(0:不介入;1:介入)*/


const MenuTbl set_up_menu = 
{
	&set_up_last_menu;
	&set_up_menu_enable_chk;
	MENUST_SET_UP_START;
	MENUST_SET_UP_END;
}

const MenuTbl set_down_menu = 
{
	&set_down_last_menu;
	&set_down_menu_enable_chk;
	MENUST_SET_DOWN_START;
	MENUST_SET_DOWN_END;
}

const MenuTbl up_down_menu =
{
	&up_down_last_menu;
	&up_down_menu_enable_chk;
	MENUST_UP_DOWN_START;
	MENUST_UP_DOWN_END;
}

	
/*****************************************************************
funcname:menu_funckey_method
contents:Menu登录函数
入口参数:
返回值:
备注:
*****************************************************************/
extern	unsigned char	menu_funckey_method( MenuTbl far *unit_tbl ,unsigned char direction )
{
	unsigned char		countover_check = 0x00;
	unsigned short int	last_menu = 0x0000;
	unsigned char		last_menu_fukki = 0;

		if( unit_tbl->last == (void *)0x00000 )
		{
			return( 0 );
		}
		last_menu = *(unit_tbl->last);
		if( last_menu == MENUST_MENU_OFF )
		{
			last_menu_fukki = 1;
		}
		if( menust.one.attribute != ((MenuStatus far *)&(unit_tbl->menu_start))->one.attribute )
		{
			last_menu_fukki = 1;
		}
	

	if( last_menu_fukki )
	{
		if( last_menu == MENUST_MENU_OFF )
		{
			if( direction == MENU_DIRECTION_NORMAL )
			{
				last_menu = unit_tbl->menu_start;	
			}
			else
			{
				last_menu = unit_tbl->menu_end;		
			}
		}
	}

	do
	{
		if( last_menu_fukki )
		{
			menust.all = last_menu;
			last_menu_fukki = 0;
		}
		else
		{
			if( direction == MENU_DIRECTION_NORMAL )
			{
				(menust.all) ++;				
				if( menust.all >= unit_tbl->menu_end )
				{
					menust.all = unit_tbl->menu_start;
					countover_check += 1;
				}
			}
			else
			{
				(menust.all) --;			
				if( menust.all == unit_tbl->menu_start )
				{
					menust.all = unit_tbl->menu_end;
					countover_check += 1;
				}
			}
		}
	} while( ( ( *unit_tbl->enable_check)( menust.all ) == MENUST_MENU_OFF ) && (countover_check < 2) );

	if( countover_check >= 2 )
	{
		menust.all = MENUST_MENU_OFF;
		return 0;
	}
	else
	{
		*unit_tbl->last = menust.all;	
		return 1;
	}
}

/*************************************************
Funcname: menu_exit_set
contents: 解除当前Menu
入口参数: void 
返回值  : 
		  1:ok
		  0:NG
**************************************************/

extern unsigned char menu_exit_set( void )
{
	menust.all = MENUST_MENU_OFF;
	return 1;
}

/*************************************************
Funcname: set_up_menu_value_up_keychang() 
contents: set up menu up 按键动作
入口参数: newkey
返回值  : void
**************************************************/
void set_up_menu_value_up_keychang( void )
{
	switch( menust.all )
	{
		case  MENUST_SET_UP_RESET_DELAY_TIME:/*上电延时保护时间调节*/
			set_up_reset_time += 1 ;
			break;
			
		case MENUST_SET_UP_TWO_RET_TIME:/*两次压机启动间隔时间调节*/
			set_up_two_ret_time += 1;
			break;
			
		case MENUST_SET_UP_PROTECT_RESET:/*排气超温保护复位时间调节*/
			set_up_protect_reset += 1;
			break;		
			
		case MENUST_SET_UP_TEMPERATURE_TIME:/*蒸发器传感器与环境传感器温差持续时间调节*/
			set_up_temperature_time += 1;
			break;
			
		case MENUST_SET_UP_MAX_DEFROST_TIME:/*最大化霜工作时间调节*/
			set_up_max_defrost_time += 1;
			break;
			
		case MENUST_SET_UP_DEFROST_ADJ_TIME:/*化霜周期调节*/
			set_up_defrost_adj_time += 1;
			break;
			
		case MENUST_SET_UP_TWO_PRESS_DELAY_TIME:/*双压保护开关延时检测时间调节*/
			set_up_two_press_delay_tiem += 1;
			break;	
	}
}

/*************************************************
Funcname: set_up_menu_value_down_keychang() 
contents: set up menu up 按键动作
入口参数: newkey
返回值  : void
**************************************************/
void set_up_menu_value_down_keychang( void )
{
	switch( menust.all )
	{
		case  MENUST_SET_UP_RESET_DELAY_TIME:/*上电延时保护时间调节*/
			set_up_reset_time -= 1 ;
			break;
			
		case MENUST_SET_UP_TWO_RET_TIME:/*两次压机启动间隔时间调节*/
			set_up_two_ret_time -= 1;
			break;
			
		case MENUST_SET_UP_PROTECT_RESET:/*排气超温保护复位时间调节*/
			set_up_protect_reset -= 1;
			break;		
			
		case MENUST_SET_UP_TEMPERATURE_TIME:/*蒸发器传感器与环境传感器温差持续时间调节*/
			set_up_temperature_time -= 1;
			break;
			
		case MENUST_SET_UP_MAX_DEFROST_TIME:/*最大化霜工作时间调节*/
			set_up_max_defrost_time -= 1;
			break;
			
		case MENUST_SET_UP_DEFROST_ADJ_TIME:/*化霜周期调节*/
			set_up_defrost_adj_time -= 1;
			break;
			
		case MENUST_SET_UP_TWO_PRESS_DELAY_TIME:/*双压保护开关延时检测时间调节*/
			set_up_two_press_delay_tiem -= 1;
			break;	
	}
}

/*************************************************
Funcname: set_up_menu_value_enter_keychang() 
contents: set up menu up 按键动作
入口参数: newkey
返回值  : void
**************************************************/
void set_up_menu_value_enter_keychang( void )
{
	if( MENUATTR_OFF != menust.all )
	{
		menu_funckey_method(&set_up_menu, MENU_DIRECTION_NORMAL);
	}
}

/*************************************************
Funcname: set_up_menu_keychang() 
contents: set up menu中按键动作
入口参数: newkey
返回值  : void
**************************************************/
extern void set_up_menu_keychang( newkey )
{
	if( MENUATTR_SET_UP == menust.one.attribute )
	{
		switch( newkey )
		{
			case UP_KEY:
				set_up_menu_value_up_keychang( void );
				break;
				
			case DOWN_KEY:
				set_up_menu_value_down_keychang( void );
				break;
				
			case SET_KEY:
				set_up_menu_value_enter_keychang( void );
				break;
				
			case SET_3S_KEY:
				menu_exit_set();
				break;
				
			default:
				break;
		}
	}
}

/*************************************************
Funcname: set_down_menu_value_up_keychang() 
contents: set down menu up 按键动作
入口参数: newkey
返回值  : void
**************************************************/
void set_down_menu_value_up_keychang( void )
{
	switch( menust.all )
	{				
		case MENUST_SET_DOWN_HIGH_TEMPERATURE:/*最高上限温度调节，防止用户将温度调节至过高*/
			set_down_high_temperature += 1;
			break;
			
		case MENUST_SET_DOWN_TEMPERATURE_BACKLASH:/*温度回差调节H55～d08即470C为开机点*/
			set_down_temperature_backlash += 1;
			break;		
			
		case MENUST_SET_DOWN_STOP_TEMPERATURE:/*排气过热保护停止温度调节*/
			set_down_spray_stop_temperature += 1;
			break;
			
		case MENUST_SET_DOWN_REMOVE_TEMPERATURE:/*排气过热保护解除温度调节*/
			set_down_remove_temperature += 1;
			break;		
			
		case MENUST_SET_DOWN_SPRAY_START_TEMPERATURE:/*喷液阀开启温度调节*/
			set_down_spray_start_temperature += 1;
			break;
			
		case MENUST_SET_DOWN_SPRAY_STOP_TEMPERATURE:/*喷液阀关闭温度调节*/
			set_down_spray_stop_temperature += 1;
			break;	
			
		case MENUST_SET_DOWN_EVAPORATOR_OUTSIDE_TEMP:/*蒸发器温度传感器与户外温度传感器温差值调节*/
			set_down_evaporator_outside_temp += 1;
			break;
			
		case MENUST_SET_DOWN_DEFROST_OVER_TEMPERATURE:/*化霜结束温度调节*/
			set_down_defrost_temperature += 1;
			break;
			
		default:
			break;	
	}
}

/*************************************************
Funcname: set_down_menu_value_down_keychang() 
contents: set down menu up 按键动作
入口参数: newkey
返回值  : void
**************************************************/
void set_down_menu_value_down_keychang( void )
{
	switch( menust.all )
	{			
		case MENUST_SET_DOWN_HIGH_TEMPERATURE:/*最高上限温度调节，防止用户将温度调节至过高*/
			set_down_high_temperature -= 1;
			break;
			
		case MENUST_SET_DOWN_TEMPERATURE_BACKLASH:/*温度回差调节H55～d08即470C为开机点*/
			set_down_temperature_backlash -= 1;
			break;		
			
		case MENUST_SET_DOWN_STOP_TEMPERATURE:/*排气过热保护停止温度调节*/
			set_down_spray_stop_temperature -= 1;
			break;
			
		case MENUST_SET_DOWN_REMOVE_TEMPERATURE:/*排气过热保护解除温度调节*/
			set_down_remove_temperature -= 1;
			break;		
			
		case MENUST_SET_DOWN_SPRAY_START_TEMPERATURE:/*喷液阀开启温度调节*/
			set_down_spray_start_temperature -= 1;
			break;
			
		case MENUST_SET_DOWN_SPRAY_STOP_TEMPERATURE:/*喷液阀关闭温度调节*/
			set_down_spray_stop_temperature -= 1;
			break;	
			
		case MENUST_SET_DOWN_EVAPORATOR_OUTSIDE_TEMP:/*蒸发器温度传感器与户外温度传感器温差值调节*/
			set_down_evaporator_outside_temp -= 1;
			break;
			
		case MENUST_SET_DOWN_DEFROST_OVER_TEMPERATURE:/*化霜结束温度调节*/
			set_down_defrost_temperature -= 1;
			break;
			
		default:	
			break;	
	}
}

/*************************************************
Funcname: set_down_menu_value_enter_keychang() 
contents: set down menu up 按键动作
入口参数: newkey
返回值  : void
**************************************************/
void set_down_menu_value_enter_keychang( void )
{
	if( MENUATTR_OFF != menust.all )
	{
		menu_funckey_method(&set_down_menu, MENU_DIRECTION_NORMAL);
	}
}

/*************************************************
Funcname: set_down_menu_keychang() 
contents: set down menu中按键动作
入口参数: newkey
返回值  : void
**************************************************/
extern void set_down_menu_keychang( newkey )
{
	if( MENUATTR_SET_DOWN == menust.one.attribute )
	{
		switch( newkey )
		{
			case UP_KEY:
				set_down_menu_value_up_keychang( void );
				break;
				
			case DOWN_KEY:
				set_down_menu_value_down_keychang( void );
				break;
				
			case SET_KEY:
				set_down_menu_value_enter_keychang( void );
				break;
				
			case SET_3S_KEY:
				menu_exit_set();
				break;
				
			default:
				break;
		}
	}
}

/*************************************************
Funcname: up_down_menu_value_up_keychang() 
contents: up down menu up 按键动作
入口参数: newkey
返回值  : void
**************************************************/
void up_down_menu_value_up_keychang( void )
{
	switch( menust.all )
	{	
		case MENUST_UP_DOWN_REST_METHOD:/*启动方式调节*/		
			up_down_rest_method += 1;
			break;
			
		case MENUST_UP_DOWN_ASSISTANCE_TEMPERATURE:/*辅助电加热切入温度点调节(检测环境温度)*/
			up_down_assistance_temperature += 1;
			break;	
			
		case MENUST_UP_DOWN_OVERLOAD:/*压机过载保护调节*/
			up_down_overload += 1;
			break;
			
		case MENUST_UP_DOWN_ANTIFREEZE_HEAT_STOP:/*防冻模式下的加热停止点*/
			up_down_antifreeze_heat_stop += 1;
			break;		
			
		case MENUST_UP_DOWN_ELECT_HEAT:/*热泵出现故障时,电加热是否自动介入(0:不介入;1:介入)*/
			up_down_elect_heat += 1;
			break;
			
		default:
			break;
	}
}

/*************************************************
Funcname: up_down_menu_value_down_keychang() 
contents: up down menu down 按键动作
入口参数: newkey
返回值  : void
**************************************************/
void up_down_menu_value_down_keychang( void )
{
	switch( menust.all )
	{	
		case MENUST_UP_DOWN_REST_METHOD:/*启动方式调节*/		
			up_down_rest_method -= 1;
			break;
			
		case MENUST_UP_DOWN_ASSISTANCE_TEMPERATURE:/*辅助电加热切入温度点调节(检测环境温度)*/
			up_down_assistance_temperature -= 1;
			break;	
			
		case MENUST_UP_DOWN_OVERLOAD:/*压机过载保护调节*/
			up_down_overload -= 1;
			break;
			
		case MENUST_UP_DOWN_ANTIFREEZE_HEAT_STOP:/*防冻模式下的加热停止点*/
			up_down_antifreeze_heat_stop -= 1;
			break;		
			
		case MENUST_UP_DOWN_ELECT_HEAT:/*热泵出现故障时,电加热是否自动介入(0:不介入;1:介入)*/
			up_down_elect_heat -= 1;
			break;
			
		default:
			break;
	}
}

/*************************************************
Funcname: up_down_menu_value_enter_keychang() 
contents: up down menu enter 按键动作
入口参数: newkey
返回值  : void
**************************************************/
void up_down_menu_value_enter_keychang( void )
{
	if( MENUATTR_OFF != menust.all )
	{
		menu_funckey_method(&up_down_menu, MENU_DIRECTION_NORMAL);
	}
}

/*************************************************
Funcname: up_down_menu_keychang() 
contents: up down menu中按键动作
入口参数: newkey
返回值  : void
**************************************************/
extern void up_down_menu_keychang( newkey )
{
	if( MENUATTR_SET_UP == menust.one.attribute )
	{
		switch( newkey )
		{
			case UP_KEY:
				up_down_menu_value_up_keychang( void );
				break;
				
			case DOWN_KEY:
				up_down_menu_value_down_keychang( void );
				break;
				
			case SET_KEY:
				up_down_menu_value_enter_keychang( void );
				break;
				
			case SET_3S_KEY:
				menu_exit_set();
				break;
				
			default:
				break;
		}
	}
}


/*************************************************
Funcname: set_up_menu_enable_chk() 
contents: set up menu 是否有效Chk
入口参数: menu
返回值  : 
		  1:true
		  0:false
**************************************************/
static unsigned char set_up_menu_enable_chk( unsigned short menu )
{
	unsigned char output_flag = 0;
	switch( menu )
	{
		case MENUST_SET_UP_RESET_DELAY_TIME:
			output_flag = 1;
			break;
			
		case MENUST_SET_UP_TWO_RET_TIME:
			output_flag = 1;
			break;

		case MENUST_SET_UP_PROTECT_RESET:
			output_flag = 1;
			break;
			
		case MENUST_SET_UP_TEMPERATURE_TIME:
			output_flag = 1;
			break;
			
		case MENUST_SET_UP_MAX_DEFROST_TIME:
			output_flag = 1;
			break;
			
		case MENUST_SET_UP_DEFROST_ADJ_TIME:
			output_flag = 1;
			break;
			
		case MENUST_SET_UP_TWO_PRESS_DELAY_TIME:
			output_flag = 1;
			break;
			
		default:
			break;
	}
	return( output_flag );
}

/*************************************************
Funcname: set_down_menu_enable_chk() 
contents: set down menu 是否有效Chk
入口参数: menu
返回值  : 
		  1:true
		  0:false
**************************************************/
static unsigned char set_down_menu_enable_chk( unsigned short menu )
{
	unsigned char output_flag = 0;
	switch( menu )
	{
		case MENUST_SET_DOWN_HIGH_TEMPERATURE:
			output_flag = 1;
			break;
			
		case MENUST_SET_DOWN_TEMPERATURE_BACKLASH:
			output_flag = 1;
			break;

		case MENUST_SET_DOWN_STOP_TEMPERATURE:
			output_flag = 1;
			break;
			
		case MENUST_SET_DOWN_REMOVE_TEMPERATURE:
			output_flag = 1;
			break;
			
		case MENUST_SET_DOWN_SPRAY_START_TEMPERATURE:
			output_flag = 1;
			break;
			
		case MENUST_SET_DOWN_SPRAY_STOP_TEMPERATURE:
			output_flag = 1;
			break;
			
		case MENUST_SET_DOWN_EVAPORATOR_OUTSIDE_TEMP:
			output_flag = 1;
			break;
			
		case MENUST_SET_DOWN_DEFROST_OVER_TEMPERATURE:
			output_flag = 1;
			break;break;			
			
		default:
			break;
	}
	return( output_flag );
}

/*************************************************
Funcname: up_down_menu_enable_chk() 
contents: up down menu 是否有效Chk
入口参数: menu
返回值  : 
		  1:true
		  0:false
**************************************************/
static unsigned char up_down_menu_enable_chk( unsigned short menu )
{
	unsigned char output_flag = 0;
	switch( menu )
	{
		case MENUST_SET_DOWN_HIGH_TEMPERATURE:
			output_flag = 1;
			break;
			
		case MENUST_SET_DOWN_TEMPERATURE_BACKLASH:
			output_flag = 1;
			break;

		case MENUST_SET_DOWN_STOP_TEMPERATURE:
			output_flag = 1;
			break;
			
		case MENUST_SET_DOWN_REMOVE_TEMPERATURE:
			output_flag = 1;
			break;
			
		case MENUST_SET_DOWN_SPRAY_START_TEMPERATURE:
			output_flag = 1;
			break;
			
		case MENUST_SET_DOWN_SPRAY_STOP_TEMPERATURE:
			output_flag = 1;
			break;
			
		case MENUST_SET_DOWN_EVAPORATOR_OUTSIDE_TEMP:
			output_flag = 1;
			break;
			
		case MENUST_SET_DOWN_DEFROST_OVER_TEMPERATURE:
			output_flag = 1;
			break;			
			
		default:
			break;
	}
	return( output_flag );
}
/*************************************************
Funcname: menu_set_value
contents: Menu参数设置函数
入口参数: 
		  menu:需要设定的Menu
		  set_value:设定值
		  set:0为初始化Menu参数模式;1为设定Menu参数模式

返回值  : 
		  1:true
		  0:false
**************************************************/
void menu_set_value( unsigned int menu,unsigned char set_value, unsigned char set )
{
	if( 0 == set )
	{
		set_up_reset_time = 10;				/*上电延时保护时间调节*/
		set_up_two_ret_time = 10;			/*两次压机启动间隔时间调节*/
		set_up_protect_reset = 10;			/*排气超温保护复位时间调节*/
		set_up_temperature_time = 5;		/*蒸发器传感器与环境传感器温差持续时间调节*/
		set_up_max_defrost_time = 10; 		/*最大化霜工作时间调节*/
		set_up_defrost_adj_time = 45;		/*化霜周期调节*/
		set_up_two_press_delay_tiem = 3;	/*双压保护开关延时检测时间调节*/


		set_down_high_temperature = 55;			/*最高上限温度调节，防止用户将温度调节至过高*/
		set_down_temperature_backlash = 8;		/*温度回差调节H55～d08即470C为开机点*/
		set_down_stop_temperature = 95;			/*排气过热保护停止温度调节*/
		set_down_remove_temperature = 70;			/*排气过热保护解除温度调节*/
		set_down_spray_start_temperature = 85;		/*喷液阀开启温度调节*/
		set_down_spray_stop_temperature = 75;		/*喷液阀关闭温度调节*/
		set_down_evaporator_outside_temp = 10;		/*蒸发器温度传感器与户外温度传感器温差值调节*/
		set_down_defrost_temperature = 8;			/*化霜结束温度调节*/

		up_down_rest_method = 2;				/*启动方式调节*/
		up_down_assistance_temperature = 5;	/*辅助电加热切入温度点调节(检测环境温度)*/
		up_down_overload = 25;					/*压机过载保护调节*/
		up_down_antifreeze_heat_stop = 9;		/*防冻模式下的加热停止点*/
		up_down_elect_heat = 0;				/*热泵出现故障时,电加热是否自动介入(0:不介入;1:介入)*/
	}
	else
	{
		switch( menu )
		{
	/************************************************************
				*  Set up Menu  *
	************************************************************/
			case MENUST_SET_UP_RESET_DELAY_TIME:/*上电延时保护时间调节*/
				set_up_reset_time = set_value;
				break;
				
			case MENUST_SET_UP_TWO_RET_TIME:/*两次压机启动间隔时间调节*/
				set_up_two_ret_time = set_value;
				break;
				
			case MENUST_SET_UP_PROTECT_RESET:/*排气超温保护复位时间调节*/
				set_up_protect_reset = set_value;
				break;		
				
			case MENUST_SET_UP_TEMPERATURE_TIME:/*蒸发器传感器与环境传感器温差持续时间调节*/
				set_up_temperature_time = set_value;
				break;
				
			case MENUST_SET_UP_MAX_DEFROST_TIME:/*最大化霜工作时间调节*/
				set_up_max_defrost_time = set_value;
				break;
				
			case MENUST_SET_UP_DEFROST_ADJ_TIME:/*化霜周期调节*/
				set_up_defrost_adj_time = set_value;
				break;
				
			case MENUST_SET_UP_TWO_PRESS_DELAY_TIME:/*双压保护开关延时检测时间调节*/
				set_up_two_press_delay_tiem = set_value;
				break;	
	 /************************************************************
				*  Set down Menu  *
	 ************************************************************/				
			case MENUST_SET_DOWN_HIGH_TEMPERATURE:/*最高上限温度调节，防止用户将温度调节至过高*/
				set_down_high_temperature = set_value;
				break;
				
			case MENUST_SET_DOWN_TEMPERATURE_BACKLASH:/*温度回差调节H55～d08即470C为开机点*/
				set_down_temperature_backlash = set_value;
				break;		
				
			case MENUST_SET_DOWN_STOP_TEMPERATURE:/*排气过热保护停止温度调节*/
				set_down_spray_stop_temperature= set_value;
				break;
				
			case MENUST_SET_DOWN_REMOVE_TEMPERATURE:/*排气过热保护解除温度调节*/
				set_down_remove_temperature = set_value;
				break;		
				
			case MENUST_SET_DOWN_SPRAY_START_TEMPERATURE:/*喷液阀开启温度调节*/
				set_down_spray_start_temperature = set_value;
				break;
				
			case MENUST_SET_DOWN_SPRAY_STOP_TEMPERATURE:/*喷液阀关闭温度调节*/
				set_down_spray_stop_temperature = set_value;
				break;	
				
			case MENUST_SET_DOWN_EVAPORATOR_OUTSIDE_TEMP:/*蒸发器温度传感器与户外温度传感器温差值调节*/
				set_down_evaporator_outside_temp = set_value;
				break;
				
			case MENUST_SET_DOWN_DEFROST_OVER_TEMPERATURE:/*化霜结束温度调节*/
				set_down_defrost_temperature = set_value;
				break;	
	 /************************************************************
				*  up down Menu  *
	  ************************************************************/			
			case MENUST_UP_DOWN_REST_METHOD:/*启动方式调节*/		
				up_down_rest_method = set_value;
				break;
				
			case MENUST_UP_DOWN_ASSISTANCE_TEMPERATURE:/*辅助电加热切入温度点调节(检测环境温度)*/
				up_down_assistance_temperature = set_value;
				break;	
				
			case MENUST_UP_DOWN_OVERLOAD:/*压机过载保护调节*/
				up_down_overload = set_value;
				break;
				
			case MENUST_UP_DOWN_ANTIFREEZE_HEAT_STOP:/*防冻模式下的加热停止点*/
				up_down_antifreeze_heat_stop = set_value;
				break;		
				
			case MENUST_UP_DOWN_ELECT_HEAT:/*热泵出现故障时,电加热是否自动介入(0:不介入;1:介入)*/
				up_down_elect_heat = set_value;
				break;
				
			default:
				break;
		}

	}
}

void applihand_menu_keychang(newkey)
{
	switch( newkey )
	{
		case SET_UP_6S_KEY:
			if( MENUATTR_OFF == menust.all )
			{
				menu_funckey_method( &set_up_menu,MENU_DIRECTION_NORMAL );
			}
			break;
			
		case SET_DOWN_6S_KEY:
			if( MENUATTR_OFF == menust.all )
			{
				menu_funckey_method( &set_down_menu,MENU_DIRECTION_NORMAL );
			}
			break;
			
		case UP_DOWN_6S_KEY:
			if( MENUATTR_OFF == menust.all )
			{
				menu_funckey_method( &up_down_menu,MENU_DIRECTION_NORMAL );
			}
			break;
			
		default:
			set_up_menu_keychang(newkey);
			set_down_menu_keychang(newkey);
			up_down_menu_keychang(newkey);
			break;
	}
}

/*************************************************
--------------------------------------------------
Verup
--------------------------------------------------
 1.01	Reviewed by zhouqian .
 
 1.02	2012-04-15	zhouqian
 Add the function: menu_set_value for set value .
 Reviewed by zhouqian.
 

*************************************************/
