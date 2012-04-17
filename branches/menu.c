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
#include"stdio.h"

/**********************
Global define
***********************/
#define	MENUST_MENU_OFF		0x0000

MenuStatus	menust;

static short int set_up_last_menu;	
static short int set_down_last_menu;
static short int up_down_last_menu;

unsigned char set_up_reset_time;			/*ÉÏµçÑÓÊ±±£»¤Ê±¼äµ÷½Ú*/
unsigned char set_up_two_ret_time;			/*Á½´ÎÑ¹»úÆô¶¯¼ä¸ôÊ±¼äµ÷½Ú*/
unsigned char set_up_protect_reset;			/*ÅÅÆø³¬ÎÂ±£»¤¸´Î»Ê±¼äµ÷½Ú*/
unsigned char set_up_temperature_time;		/*Õô·¢Æ÷´«¸ÐÆ÷Óë»·¾³´«¸ÐÆ÷ÎÂ²î³ÖÐøÊ±¼äµ÷½Ú*/
unsigned char set_up_max_defrost_time ; 	/*×î´ó»¯Ëª¹¤×÷Ê±¼äµ÷½Ú*/
unsigned char set_up_defrost_adj_time;		/*»¯ËªÖÜÆÚµ÷½Ú*/
unsigned char set_up_two_press_delay_tiem;	/*Ë«Ñ¹±£»¤¿ª¹ØÑÓÊ±¼ì²âÊ±¼äµ÷½Ú*/


unsigned char set_down_high_temperature;			/*×î¸ßÉÏÏÞÎÂ¶Èµ÷½Ú£¬·ÀÖ¹ÓÃ»§½«ÎÂ¶Èµ÷½ÚÖÁ¹ý¸ß*/
unsigned char set_down_temperature_backlash;		/*ÎÂ¶È»Ø²îµ÷½ÚH55¡«d08¼´470CÎª¿ª»úµã*/
unsigned char set_down_stop_temperature;			/*ÅÅÆø¹ýÈÈ±£»¤Í£Ö¹ÎÂ¶Èµ÷½Ú*/
unsigned char set_down_remove_temperature;			/*ÅÅÆø¹ýÈÈ±£»¤½â³ýÎÂ¶Èµ÷½Ú*/
unsigned char set_down_spray_start_temperature;		/*ÅçÒº·§¿ªÆôÎÂ¶Èµ÷½Ú*/
unsigned char set_down_spray_stop_temperature;		/*ÅçÒº·§¹Ø±ÕÎÂ¶Èµ÷½Ú*/
unsigned char set_down_evaporator_outside_temp;		/*Õô·¢Æ÷ÎÂ¶È´«¸ÐÆ÷Óë»§ÍâÎÂ¶È´«¸ÐÆ÷ÎÂ²îÖµµ÷½Ú*/
unsigned char set_down_defrost_temperature;			/*»¯Ëª½áÊøÎÂ¶Èµ÷½Ú*/

unsigned char up_down_rest_method;				/*Æô¶¯·½Ê½µ÷½Ú*/
unsigned char up_down_assistance_temperature;	/*¸¨Öúµç¼ÓÈÈÇÐÈëÎÂ¶Èµãµ÷½Ú(¼ì²â»·¾³ÎÂ¶È)*/
unsigned char up_down_overload;					/*Ñ¹»ú¹ýÔØ±£»¤µ÷½Ú*/
unsigned char up_down_antifreeze_heat_stop;		/*·À¶³Ä£Ê½ÏÂµÄ¼ÓÈÈÍ£Ö¹µã*/
sbit up_down_elect_heat;						/*ÈÈ±Ã³öÏÖ¹ÊÕÏÊ±,µç¼ÓÈÈÊÇ·ñ×Ô¶¯½éÈë(0:²»½éÈë;1:½éÈë)*/


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
contents:MenuµÇÂ¼º¯Êý
Èë¿Ú²ÎÊý:
·µ»ØÖµ:
±¸×¢:
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
contents: ½â³ýµ±Ç°Menu
Èë¿Ú²ÎÊý: void 
·µ»ØÖµ  : 
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
contents: set up menu up °´¼ü¶¯×÷
Èë¿Ú²ÎÊý: newkey
·µ»ØÖµ  : void
**************************************************/
void set_up_menu_value_up_keychang( void )
{
	switch( menust.all )
	{
		case  MENUST_SET_UP_RESET_DELAY_TIME:/*ÉÏµçÑÓÊ±±£»¤Ê±¼äµ÷½Ú*/
			set_up_reset_time += 1 ;
			break;
			
		case MENUST_SET_UP_TWO_RET_TIME:/*Á½´ÎÑ¹»úÆô¶¯¼ä¸ôÊ±¼äµ÷½Ú*/
			set_up_two_ret_time += 1;
			break;
			
		case MENUST_SET_UP_PROTECT_RESET:/*ÅÅÆø³¬ÎÂ±£»¤¸´Î»Ê±¼äµ÷½Ú*/
			set_up_protect_reset += 1;
			break;		
			
		case MENUST_SET_UP_TEMPERATURE_TIME:/*Õô·¢Æ÷´«¸ÐÆ÷Óë»·¾³´«¸ÐÆ÷ÎÂ²î³ÖÐøÊ±¼äµ÷½Ú*/
			set_up_temperature_time += 1;
			break;
			
		case MENUST_SET_UP_MAX_DEFROST_TIME:/*×î´ó»¯Ëª¹¤×÷Ê±¼äµ÷½Ú*/
			set_up_max_defrost_time += 1;
			break;
			
		case MENUST_SET_UP_DEFROST_ADJ_TIME:/*»¯ËªÖÜÆÚµ÷½Ú*/
			set_up_defrost_adj_time += 1;
			break;
			
		case MENUST_SET_UP_TWO_PRESS_DELAY_TIME:/*Ë«Ñ¹±£»¤¿ª¹ØÑÓÊ±¼ì²âÊ±¼äµ÷½Ú*/
			set_up_two_press_delay_tiem += 1;
			break;	
	}
}

/*************************************************
Funcname: set_up_menu_value_down_keychang() 
contents: set up menu up °´¼ü¶¯×÷
Èë¿Ú²ÎÊý: newkey
·µ»ØÖµ  : void
**************************************************/
void set_up_menu_value_down_keychang( void )
{
	switch( menust.all )
	{
		case  MENUST_SET_UP_RESET_DELAY_TIME:/*ÉÏµçÑÓÊ±±£»¤Ê±¼äµ÷½Ú*/
			set_up_reset_time -= 1 ;
			break;
			
		case MENUST_SET_UP_TWO_RET_TIME:/*Á½´ÎÑ¹»úÆô¶¯¼ä¸ôÊ±¼äµ÷½Ú*/
			set_up_two_ret_time -= 1;
			break;
			
		case MENUST_SET_UP_PROTECT_RESET:/*ÅÅÆø³¬ÎÂ±£»¤¸´Î»Ê±¼äµ÷½Ú*/
			set_up_protect_reset -= 1;
			break;		
			
		case MENUST_SET_UP_TEMPERATURE_TIME:/*Õô·¢Æ÷´«¸ÐÆ÷Óë»·¾³´«¸ÐÆ÷ÎÂ²î³ÖÐøÊ±¼äµ÷½Ú*/
			set_up_temperature_time -= 1;
			break;
			
		case MENUST_SET_UP_MAX_DEFROST_TIME:/*×î´ó»¯Ëª¹¤×÷Ê±¼äµ÷½Ú*/
			set_up_max_defrost_time -= 1;
			break;
			
		case MENUST_SET_UP_DEFROST_ADJ_TIME:/*»¯ËªÖÜÆÚµ÷½Ú*/
			set_up_defrost_adj_time -= 1;
			break;
			
		case MENUST_SET_UP_TWO_PRESS_DELAY_TIME:/*Ë«Ñ¹±£»¤¿ª¹ØÑÓÊ±¼ì²âÊ±¼äµ÷½Ú*/
			set_up_two_press_delay_tiem -= 1;
			break;	
	}
}

/*************************************************
Funcname: set_up_menu_value_enter_keychang() 
contents: set up menu up °´¼ü¶¯×÷
Èë¿Ú²ÎÊý: newkey
·µ»ØÖµ  : void
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
contents: set up menuÖÐ°´¼ü¶¯×÷
Èë¿Ú²ÎÊý: newkey
·µ»ØÖµ  : void
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
contents: set down menu up °´¼ü¶¯×÷
Èë¿Ú²ÎÊý: newkey
·µ»ØÖµ  : void
**************************************************/
void set_down_menu_value_up_keychang( void )
{
	switch( menust.all )
	{				
		case MENUST_SET_DOWN_HIGH_TEMPERATURE:/*×î¸ßÉÏÏÞÎÂ¶Èµ÷½Ú£¬·ÀÖ¹ÓÃ»§½«ÎÂ¶Èµ÷½ÚÖÁ¹ý¸ß*/
			set_down_high_temperature += 1;
			break;
			
		case MENUST_SET_DOWN_TEMPERATURE_BACKLASH:/*ÎÂ¶È»Ø²îµ÷½ÚH55¡«d08¼´470CÎª¿ª»úµã*/
			set_down_temperature_backlash += 1;
			break;		
			
		case MENUST_SET_DOWN_STOP_TEMPERATURE:/*ÅÅÆø¹ýÈÈ±£»¤Í£Ö¹ÎÂ¶Èµ÷½Ú*/
			set_down_spray_stop_temperature += 1;
			break;
			
		case MENUST_SET_DOWN_REMOVE_TEMPERATURE:/*ÅÅÆø¹ýÈÈ±£»¤½â³ýÎÂ¶Èµ÷½Ú*/
			set_down_remove_temperature += 1;
			break;		
			
		case MENUST_SET_DOWN_SPRAY_START_TEMPERATURE:/*ÅçÒº·§¿ªÆôÎÂ¶Èµ÷½Ú*/
			set_down_spray_start_temperature += 1;
			break;
			
		case MENUST_SET_DOWN_SPRAY_STOP_TEMPERATURE:/*ÅçÒº·§¹Ø±ÕÎÂ¶Èµ÷½Ú*/
			set_down_spray_stop_temperature += 1;
			break;	
			
		case MENUST_SET_DOWN_EVAPORATOR_OUTSIDE_TEMP:/*Õô·¢Æ÷ÎÂ¶È´«¸ÐÆ÷Óë»§ÍâÎÂ¶È´«¸ÐÆ÷ÎÂ²îÖµµ÷½Ú*/
			set_down_evaporator_outside_temp += 1;
			break;
			
		case MENUST_SET_DOWN_DEFROST_OVER_TEMPERATURE:/*»¯Ëª½áÊøÎÂ¶Èµ÷½Ú*/
			set_down_defrost_temperature += 1;
			break;
			
		default:
			break;	
	}
}

/*************************************************
Funcname: set_down_menu_value_down_keychang() 
contents: set down menu up °´¼ü¶¯×÷
Èë¿Ú²ÎÊý: newkey
·µ»ØÖµ  : void
**************************************************/
void set_down_menu_value_down_keychang( void )
{
	switch( menust.all )
	{			
		case MENUST_SET_DOWN_HIGH_TEMPERATURE:/*×î¸ßÉÏÏÞÎÂ¶Èµ÷½Ú£¬·ÀÖ¹ÓÃ»§½«ÎÂ¶Èµ÷½ÚÖÁ¹ý¸ß*/
			set_down_high_temperature -= 1;
			break;
			
		case MENUST_SET_DOWN_TEMPERATURE_BACKLASH:/*ÎÂ¶È»Ø²îµ÷½ÚH55¡«d08¼´470CÎª¿ª»úµã*/
			set_down_temperature_backlash -= 1;
			break;		
			
		case MENUST_SET_DOWN_STOP_TEMPERATURE:/*ÅÅÆø¹ýÈÈ±£»¤Í£Ö¹ÎÂ¶Èµ÷½Ú*/
			set_down_spray_stop_temperature -= 1;
			break;
			
		case MENUST_SET_DOWN_REMOVE_TEMPERATURE:/*ÅÅÆø¹ýÈÈ±£»¤½â³ýÎÂ¶Èµ÷½Ú*/
			set_down_remove_temperature -= 1;
			break;		
			
		case MENUST_SET_DOWN_SPRAY_START_TEMPERATURE:/*ÅçÒº·§¿ªÆôÎÂ¶Èµ÷½Ú*/
			set_down_spray_start_temperature -= 1;
			break;
			
		case MENUST_SET_DOWN_SPRAY_STOP_TEMPERATURE:/*ÅçÒº·§¹Ø±ÕÎÂ¶Èµ÷½Ú*/
			set_down_spray_stop_temperature -= 1;
			break;	
			
		case MENUST_SET_DOWN_EVAPORATOR_OUTSIDE_TEMP:/*Õô·¢Æ÷ÎÂ¶È´«¸ÐÆ÷Óë»§ÍâÎÂ¶È´«¸ÐÆ÷ÎÂ²îÖµµ÷½Ú*/
			set_down_evaporator_outside_temp -= 1;
			break;
			
		case MENUST_SET_DOWN_DEFROST_OVER_TEMPERATURE:/*»¯Ëª½áÊøÎÂ¶Èµ÷½Ú*/
			set_down_defrost_temperature -= 1;
			break;
			
		default:	
			break;	
	}
}

/*************************************************
Funcname: set_down_menu_value_enter_keychang() 
contents: set down menu up °´¼ü¶¯×÷
Èë¿Ú²ÎÊý: newkey
·µ»ØÖµ  : void
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
contents: set down menuÖÐ°´¼ü¶¯×÷
Èë¿Ú²ÎÊý: newkey
·µ»ØÖµ  : void
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
contents: up down menu up °´¼ü¶¯×÷
Èë¿Ú²ÎÊý: newkey
·µ»ØÖµ  : void
**************************************************/
void up_down_menu_value_up_keychang( void )
{
	switch( menust.all )
	{	
		case MENUST_UP_DOWN_REST_METHOD:/*Æô¶¯·½Ê½µ÷½Ú*/		
			up_down_rest_method += 1;
			break;
			
		case MENUST_UP_DOWN_ASSISTANCE_TEMPERATURE:/*¸¨Öúµç¼ÓÈÈÇÐÈëÎÂ¶Èµãµ÷½Ú(¼ì²â»·¾³ÎÂ¶È)*/
			up_down_assistance_temperature += 1;
			break;	
			
		case MENUST_UP_DOWN_OVERLOAD:/*Ñ¹»ú¹ýÔØ±£»¤µ÷½Ú*/
			up_down_overload += 1;
			break;
			
		case MENUST_UP_DOWN_ANTIFREEZE_HEAT_STOP:/*·À¶³Ä£Ê½ÏÂµÄ¼ÓÈÈÍ£Ö¹µã*/
			up_down_antifreeze_heat_stop += 1;
			break;		
			
		case MENUST_UP_DOWN_ELECT_HEAT:/*ÈÈ±Ã³öÏÖ¹ÊÕÏÊ±,µç¼ÓÈÈÊÇ·ñ×Ô¶¯½éÈë(0:²»½éÈë;1:½éÈë)*/
			up_down_elect_heat += 1;
			break;
			
		default:
			break;
	}
}

/*************************************************
Funcname: up_down_menu_value_down_keychang() 
contents: up down menu down °´¼ü¶¯×÷
Èë¿Ú²ÎÊý: newkey
·µ»ØÖµ  : void
**************************************************/
void up_down_menu_value_down_keychang( void )
{
	switch( menust.all )
	{	
		case MENUST_UP_DOWN_REST_METHOD:/*Æô¶¯·½Ê½µ÷½Ú*/		
			up_down_rest_method -= 1;
			break;
			
		case MENUST_UP_DOWN_ASSISTANCE_TEMPERATURE:/*¸¨Öúµç¼ÓÈÈÇÐÈëÎÂ¶Èµãµ÷½Ú(¼ì²â»·¾³ÎÂ¶È)*/
			up_down_assistance_temperature -= 1;
			break;	
			
		case MENUST_UP_DOWN_OVERLOAD:/*Ñ¹»ú¹ýÔØ±£»¤µ÷½Ú*/
			up_down_overload -= 1;
			break;
			
		case MENUST_UP_DOWN_ANTIFREEZE_HEAT_STOP:/*·À¶³Ä£Ê½ÏÂµÄ¼ÓÈÈÍ£Ö¹µã*/
			up_down_antifreeze_heat_stop -= 1;
			break;		
			
		case MENUST_UP_DOWN_ELECT_HEAT:/*ÈÈ±Ã³öÏÖ¹ÊÕÏÊ±,µç¼ÓÈÈÊÇ·ñ×Ô¶¯½éÈë(0:²»½éÈë;1:½éÈë)*/
			up_down_elect_heat -= 1;
			break;
			
		default:
			break;
	}
}

/*************************************************
Funcname: up_down_menu_value_enter_keychang() 
contents: up down menu enter °´¼ü¶¯×÷
Èë¿Ú²ÎÊý: newkey
·µ»ØÖµ  : void
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
contents: up down menuÖÐ°´¼ü¶¯×÷
Èë¿Ú²ÎÊý: newkey
·µ»ØÖµ  : void
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
contents: set up menu ÊÇ·ñÓÐÐ§Chk
Èë¿Ú²ÎÊý: menu
·µ»ØÖµ  : 
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
contents: set down menu ÊÇ·ñÓÐÐ§Chk
Èë¿Ú²ÎÊý: menu
·µ»ØÖµ  : 
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
contents: up down menu ÊÇ·ñÓÐÐ§Chk
Èë¿Ú²ÎÊý: menu
·µ»ØÖµ  : 
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
contents: Menu²ÎÊýÉèÖÃº¯Êý
Èë¿Ú²ÎÊý: 
		  menu:ÐèÒªÉè¶¨µÄMenu
		  set_value:Éè¶¨Öµ
		  set:0Îª³õÊ¼»¯Menu²ÎÊýÄ£Ê½;1ÎªÉè¶¨Menu²ÎÊýÄ£Ê½

·µ»ØÖµ  : 
		  1:true
		  0:false
**************************************************/
void menu_set_value( unsigned int menu,unsigned char set_value, unsigned char set )
{
	if( 0 == set )
	{
		set_up_reset_time = 10;				/*ÉÏµçÑÓÊ±±£»¤Ê±¼äµ÷½Ú*/
		set_up_two_ret_time = 10;			/*Á½´ÎÑ¹»úÆô¶¯¼ä¸ôÊ±¼äµ÷½Ú*/
		set_up_protect_reset = 10;			/*ÅÅÆø³¬ÎÂ±£»¤¸´Î»Ê±¼äµ÷½Ú*/
		set_up_temperature_time = 5;		/*Õô·¢Æ÷´«¸ÐÆ÷Óë»·¾³´«¸ÐÆ÷ÎÂ²î³ÖÐøÊ±¼äµ÷½Ú*/
		set_up_max_defrost_time = 10; 		/*×î´ó»¯Ëª¹¤×÷Ê±¼äµ÷½Ú*/
		set_up_defrost_adj_time = 45;		/*»¯ËªÖÜÆÚµ÷½Ú*/
		set_up_two_press_delay_tiem = 3;	/*Ë«Ñ¹±£»¤¿ª¹ØÑÓÊ±¼ì²âÊ±¼äµ÷½Ú*/


		set_down_high_temperature = 55;			/*×î¸ßÉÏÏÞÎÂ¶Èµ÷½Ú£¬·ÀÖ¹ÓÃ»§½«ÎÂ¶Èµ÷½ÚÖÁ¹ý¸ß*/
		set_down_temperature_backlash = 8;		/*ÎÂ¶È»Ø²îµ÷½ÚH55¡«d08¼´470CÎª¿ª»úµã*/
		set_down_stop_temperature = 95;			/*ÅÅÆø¹ýÈÈ±£»¤Í£Ö¹ÎÂ¶Èµ÷½Ú*/
		set_down_remove_temperature = 70;			/*ÅÅÆø¹ýÈÈ±£»¤½â³ýÎÂ¶Èµ÷½Ú*/
		set_down_spray_start_temperature = 85;		/*ÅçÒº·§¿ªÆôÎÂ¶Èµ÷½Ú*/
		set_down_spray_stop_temperature = 75;		/*ÅçÒº·§¹Ø±ÕÎÂ¶Èµ÷½Ú*/
		set_down_evaporator_outside_temp = 10;		/*Õô·¢Æ÷ÎÂ¶È´«¸ÐÆ÷Óë»§ÍâÎÂ¶È´«¸ÐÆ÷ÎÂ²îÖµµ÷½Ú*/
		set_down_defrost_temperature = 8;			/*»¯Ëª½áÊøÎÂ¶Èµ÷½Ú*/

		up_down_rest_method = 2;				/*Æô¶¯·½Ê½µ÷½Ú*/
		up_down_assistance_temperature = 5;	/*¸¨Öúµç¼ÓÈÈÇÐÈëÎÂ¶Èµãµ÷½Ú(¼ì²â»·¾³ÎÂ¶È)*/
		up_down_overload = 25;					/*Ñ¹»ú¹ýÔØ±£»¤µ÷½Ú*/
		up_down_antifreeze_heat_stop = 9;		/*·À¶³Ä£Ê½ÏÂµÄ¼ÓÈÈÍ£Ö¹µã*/
		up_down_elect_heat = 0;				/*ÈÈ±Ã³öÏÖ¹ÊÕÏÊ±,µç¼ÓÈÈÊÇ·ñ×Ô¶¯½éÈë(0:²»½éÈë;1:½éÈë)*/
	}
	else
	{
		switch( menu )
		{
	/************************************************************
				*  Set up Menu  *
	************************************************************/
			case MENUST_SET_UP_RESET_DELAY_TIME:/*ÉÏµçÑÓÊ±±£»¤Ê±¼äµ÷½Ú*/
				set_up_reset_time = set_value;
				break;
				
			case MENUST_SET_UP_TWO_RET_TIME:/*Á½´ÎÑ¹»úÆô¶¯¼ä¸ôÊ±¼äµ÷½Ú*/
				set_up_two_ret_time = set_value;
				break;
				
			case MENUST_SET_UP_PROTECT_RESET:/*ÅÅÆø³¬ÎÂ±£»¤¸´Î»Ê±¼äµ÷½Ú*/
				set_up_protect_reset = set_value;
				break;		
				
			case MENUST_SET_UP_TEMPERATURE_TIME:/*Õô·¢Æ÷´«¸ÐÆ÷Óë»·¾³´«¸ÐÆ÷ÎÂ²î³ÖÐøÊ±¼äµ÷½Ú*/
				set_up_temperature_time = set_value;
				break;
				
			case MENUST_SET_UP_MAX_DEFROST_TIME:/*×î´ó»¯Ëª¹¤×÷Ê±¼äµ÷½Ú*/
				set_up_max_defrost_time = set_value;
				break;
				
			case MENUST_SET_UP_DEFROST_ADJ_TIME:/*»¯ËªÖÜÆÚµ÷½Ú*/
				set_up_defrost_adj_time = set_value;
				break;
				
			case MENUST_SET_UP_TWO_PRESS_DELAY_TIME:/*Ë«Ñ¹±£»¤¿ª¹ØÑÓÊ±¼ì²âÊ±¼äµ÷½Ú*/
				set_up_two_press_delay_tiem = set_value;
				break;	
	 /************************************************************
				*  Set down Menu  *
	 ************************************************************/				
			case MENUST_SET_DOWN_HIGH_TEMPERATURE:/*×î¸ßÉÏÏÞÎÂ¶Èµ÷½Ú£¬·ÀÖ¹ÓÃ»§½«ÎÂ¶Èµ÷½ÚÖÁ¹ý¸ß*/
				set_down_high_temperature = set_value;
				break;
				
			case MENUST_SET_DOWN_TEMPERATURE_BACKLASH:/*ÎÂ¶È»Ø²îµ÷½ÚH55¡«d08¼´470CÎª¿ª»úµã*/
				set_down_temperature_backlash = set_value;
				break;		
				
			case MENUST_SET_DOWN_STOP_TEMPERATURE:/*ÅÅÆø¹ýÈÈ±£»¤Í£Ö¹ÎÂ¶Èµ÷½Ú*/
				set_down_spray_stop_temperature= set_value;
				break;
				
			case MENUST_SET_DOWN_REMOVE_TEMPERATURE:/*ÅÅÆø¹ýÈÈ±£»¤½â³ýÎÂ¶Èµ÷½Ú*/
				set_down_remove_temperature = set_value;
				break;		
				
			case MENUST_SET_DOWN_SPRAY_START_TEMPERATURE:/*ÅçÒº·§¿ªÆôÎÂ¶Èµ÷½Ú*/
				set_down_spray_start_temperature = set_value;
				break;
				
			case MENUST_SET_DOWN_SPRAY_STOP_TEMPERATURE:/*ÅçÒº·§¹Ø±ÕÎÂ¶Èµ÷½Ú*/
				set_down_spray_stop_temperature = set_value;
				break;	
				
			case MENUST_SET_DOWN_EVAPORATOR_OUTSIDE_TEMP:/*Õô·¢Æ÷ÎÂ¶È´«¸ÐÆ÷Óë»§ÍâÎÂ¶È´«¸ÐÆ÷ÎÂ²îÖµµ÷½Ú*/
				set_down_evaporator_outside_temp = set_value;
				break;
				
			case MENUST_SET_DOWN_DEFROST_OVER_TEMPERATURE:/*»¯Ëª½áÊøÎÂ¶Èµ÷½Ú*/
				set_down_defrost_temperature = set_value;
				break;	
	 /************************************************************
				*  up down Menu  *
	  ************************************************************/			
			case MENUST_UP_DOWN_REST_METHOD:/*Æô¶¯·½Ê½µ÷½Ú*/		
				up_down_rest_method = set_value;
				break;
				
			case MENUST_UP_DOWN_ASSISTANCE_TEMPERATURE:/*¸¨Öúµç¼ÓÈÈÇÐÈëÎÂ¶Èµãµ÷½Ú(¼ì²â»·¾³ÎÂ¶È)*/
				up_down_assistance_temperature = set_value;
				break;	
				
			case MENUST_UP_DOWN_OVERLOAD:/*Ñ¹»ú¹ýÔØ±£»¤µ÷½Ú*/
				up_down_overload = set_value;
				break;
				
			case MENUST_UP_DOWN_ANTIFREEZE_HEAT_STOP:/*·À¶³Ä£Ê½ÏÂµÄ¼ÓÈÈÍ£Ö¹µã*/
				up_down_antifreeze_heat_stop = set_value;
				break;		
				
			case MENUST_UP_DOWN_ELECT_HEAT:/*ÈÈ±Ã³öÏÖ¹ÊÕÏÊ±,µç¼ÓÈÈÊÇ·ñ×Ô¶¯½éÈë(0:²»½éÈë;1:½éÈë)*/
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
