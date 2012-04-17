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

unsigned char set_up_reset_time;			/*�ϵ���ʱ����ʱ�����*/
unsigned char set_up_two_ret_time;			/*����ѹ���������ʱ�����*/
unsigned char set_up_protect_reset;			/*�������±�����λʱ�����*/
unsigned char set_up_temperature_time;		/*�������������뻷���������²����ʱ�����*/
unsigned char set_up_max_defrost_time ; 	/*���˪����ʱ�����*/
unsigned char set_up_defrost_adj_time;		/*��˪���ڵ���*/
unsigned char set_up_two_press_delay_tiem;	/*˫ѹ����������ʱ���ʱ�����*/


unsigned char set_down_high_temperature;			/*��������¶ȵ��ڣ���ֹ�û����¶ȵ���������*/
unsigned char set_down_temperature_backlash;		/*�¶Ȼز����H55��d08��470CΪ������*/
unsigned char set_down_stop_temperature;			/*�������ȱ���ֹͣ�¶ȵ���*/
unsigned char set_down_remove_temperature;			/*�������ȱ�������¶ȵ���*/
unsigned char set_down_spray_start_temperature;		/*��Һ�������¶ȵ���*/
unsigned char set_down_spray_stop_temperature;		/*��Һ���ر��¶ȵ���*/
unsigned char set_down_evaporator_outside_temp;		/*�������¶ȴ������뻧���¶ȴ������²�ֵ����*/
unsigned char set_down_defrost_temperature;			/*��˪�����¶ȵ���*/

unsigned char up_down_rest_method;				/*������ʽ����*/
unsigned char up_down_assistance_temperature;	/*��������������¶ȵ����(��⻷���¶�)*/
unsigned char up_down_overload;					/*ѹ�����ر�������*/
unsigned char up_down_antifreeze_heat_stop;		/*����ģʽ�µļ���ֹͣ��*/
sbit up_down_elect_heat;						/*�ȱó��ֹ���ʱ,������Ƿ��Զ�����(0:������;1:����)*/


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
contents:Menu��¼����
��ڲ���:
����ֵ:
��ע:
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
contents: �����ǰMenu
��ڲ���: void 
����ֵ  : 
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
contents: set up menu up ��������
��ڲ���: newkey
����ֵ  : void
**************************************************/
void set_up_menu_value_up_keychang( void )
{
	switch( menust.all )
	{
		case  MENUST_SET_UP_RESET_DELAY_TIME:/*�ϵ���ʱ����ʱ�����*/
			set_up_reset_time += 1 ;
			break;
			
		case MENUST_SET_UP_TWO_RET_TIME:/*����ѹ���������ʱ�����*/
			set_up_two_ret_time += 1;
			break;
			
		case MENUST_SET_UP_PROTECT_RESET:/*�������±�����λʱ�����*/
			set_up_protect_reset += 1;
			break;		
			
		case MENUST_SET_UP_TEMPERATURE_TIME:/*�������������뻷���������²����ʱ�����*/
			set_up_temperature_time += 1;
			break;
			
		case MENUST_SET_UP_MAX_DEFROST_TIME:/*���˪����ʱ�����*/
			set_up_max_defrost_time += 1;
			break;
			
		case MENUST_SET_UP_DEFROST_ADJ_TIME:/*��˪���ڵ���*/
			set_up_defrost_adj_time += 1;
			break;
			
		case MENUST_SET_UP_TWO_PRESS_DELAY_TIME:/*˫ѹ����������ʱ���ʱ�����*/
			set_up_two_press_delay_tiem += 1;
			break;	
	}
}

/*************************************************
Funcname: set_up_menu_value_down_keychang() 
contents: set up menu up ��������
��ڲ���: newkey
����ֵ  : void
**************************************************/
void set_up_menu_value_down_keychang( void )
{
	switch( menust.all )
	{
		case  MENUST_SET_UP_RESET_DELAY_TIME:/*�ϵ���ʱ����ʱ�����*/
			set_up_reset_time -= 1 ;
			break;
			
		case MENUST_SET_UP_TWO_RET_TIME:/*����ѹ���������ʱ�����*/
			set_up_two_ret_time -= 1;
			break;
			
		case MENUST_SET_UP_PROTECT_RESET:/*�������±�����λʱ�����*/
			set_up_protect_reset -= 1;
			break;		
			
		case MENUST_SET_UP_TEMPERATURE_TIME:/*�������������뻷���������²����ʱ�����*/
			set_up_temperature_time -= 1;
			break;
			
		case MENUST_SET_UP_MAX_DEFROST_TIME:/*���˪����ʱ�����*/
			set_up_max_defrost_time -= 1;
			break;
			
		case MENUST_SET_UP_DEFROST_ADJ_TIME:/*��˪���ڵ���*/
			set_up_defrost_adj_time -= 1;
			break;
			
		case MENUST_SET_UP_TWO_PRESS_DELAY_TIME:/*˫ѹ����������ʱ���ʱ�����*/
			set_up_two_press_delay_tiem -= 1;
			break;	
	}
}

/*************************************************
Funcname: set_up_menu_value_enter_keychang() 
contents: set up menu up ��������
��ڲ���: newkey
����ֵ  : void
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
contents: set up menu�а�������
��ڲ���: newkey
����ֵ  : void
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
contents: set down menu up ��������
��ڲ���: newkey
����ֵ  : void
**************************************************/
void set_down_menu_value_up_keychang( void )
{
	switch( menust.all )
	{				
		case MENUST_SET_DOWN_HIGH_TEMPERATURE:/*��������¶ȵ��ڣ���ֹ�û����¶ȵ���������*/
			set_down_high_temperature += 1;
			break;
			
		case MENUST_SET_DOWN_TEMPERATURE_BACKLASH:/*�¶Ȼز����H55��d08��470CΪ������*/
			set_down_temperature_backlash += 1;
			break;		
			
		case MENUST_SET_DOWN_STOP_TEMPERATURE:/*�������ȱ���ֹͣ�¶ȵ���*/
			set_down_spray_stop_temperature += 1;
			break;
			
		case MENUST_SET_DOWN_REMOVE_TEMPERATURE:/*�������ȱ�������¶ȵ���*/
			set_down_remove_temperature += 1;
			break;		
			
		case MENUST_SET_DOWN_SPRAY_START_TEMPERATURE:/*��Һ�������¶ȵ���*/
			set_down_spray_start_temperature += 1;
			break;
			
		case MENUST_SET_DOWN_SPRAY_STOP_TEMPERATURE:/*��Һ���ر��¶ȵ���*/
			set_down_spray_stop_temperature += 1;
			break;	
			
		case MENUST_SET_DOWN_EVAPORATOR_OUTSIDE_TEMP:/*�������¶ȴ������뻧���¶ȴ������²�ֵ����*/
			set_down_evaporator_outside_temp += 1;
			break;
			
		case MENUST_SET_DOWN_DEFROST_OVER_TEMPERATURE:/*��˪�����¶ȵ���*/
			set_down_defrost_temperature += 1;
			break;
			
		default:
			break;	
	}
}

/*************************************************
Funcname: set_down_menu_value_down_keychang() 
contents: set down menu up ��������
��ڲ���: newkey
����ֵ  : void
**************************************************/
void set_down_menu_value_down_keychang( void )
{
	switch( menust.all )
	{			
		case MENUST_SET_DOWN_HIGH_TEMPERATURE:/*��������¶ȵ��ڣ���ֹ�û����¶ȵ���������*/
			set_down_high_temperature -= 1;
			break;
			
		case MENUST_SET_DOWN_TEMPERATURE_BACKLASH:/*�¶Ȼز����H55��d08��470CΪ������*/
			set_down_temperature_backlash -= 1;
			break;		
			
		case MENUST_SET_DOWN_STOP_TEMPERATURE:/*�������ȱ���ֹͣ�¶ȵ���*/
			set_down_spray_stop_temperature -= 1;
			break;
			
		case MENUST_SET_DOWN_REMOVE_TEMPERATURE:/*�������ȱ�������¶ȵ���*/
			set_down_remove_temperature -= 1;
			break;		
			
		case MENUST_SET_DOWN_SPRAY_START_TEMPERATURE:/*��Һ�������¶ȵ���*/
			set_down_spray_start_temperature -= 1;
			break;
			
		case MENUST_SET_DOWN_SPRAY_STOP_TEMPERATURE:/*��Һ���ر��¶ȵ���*/
			set_down_spray_stop_temperature -= 1;
			break;	
			
		case MENUST_SET_DOWN_EVAPORATOR_OUTSIDE_TEMP:/*�������¶ȴ������뻧���¶ȴ������²�ֵ����*/
			set_down_evaporator_outside_temp -= 1;
			break;
			
		case MENUST_SET_DOWN_DEFROST_OVER_TEMPERATURE:/*��˪�����¶ȵ���*/
			set_down_defrost_temperature -= 1;
			break;
			
		default:	
			break;	
	}
}

/*************************************************
Funcname: set_down_menu_value_enter_keychang() 
contents: set down menu up ��������
��ڲ���: newkey
����ֵ  : void
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
contents: set down menu�а�������
��ڲ���: newkey
����ֵ  : void
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
contents: up down menu up ��������
��ڲ���: newkey
����ֵ  : void
**************************************************/
void up_down_menu_value_up_keychang( void )
{
	switch( menust.all )
	{	
		case MENUST_UP_DOWN_REST_METHOD:/*������ʽ����*/		
			up_down_rest_method += 1;
			break;
			
		case MENUST_UP_DOWN_ASSISTANCE_TEMPERATURE:/*��������������¶ȵ����(��⻷���¶�)*/
			up_down_assistance_temperature += 1;
			break;	
			
		case MENUST_UP_DOWN_OVERLOAD:/*ѹ�����ر�������*/
			up_down_overload += 1;
			break;
			
		case MENUST_UP_DOWN_ANTIFREEZE_HEAT_STOP:/*����ģʽ�µļ���ֹͣ��*/
			up_down_antifreeze_heat_stop += 1;
			break;		
			
		case MENUST_UP_DOWN_ELECT_HEAT:/*�ȱó��ֹ���ʱ,������Ƿ��Զ�����(0:������;1:����)*/
			up_down_elect_heat += 1;
			break;
			
		default:
			break;
	}
}

/*************************************************
Funcname: up_down_menu_value_down_keychang() 
contents: up down menu down ��������
��ڲ���: newkey
����ֵ  : void
**************************************************/
void up_down_menu_value_down_keychang( void )
{
	switch( menust.all )
	{	
		case MENUST_UP_DOWN_REST_METHOD:/*������ʽ����*/		
			up_down_rest_method -= 1;
			break;
			
		case MENUST_UP_DOWN_ASSISTANCE_TEMPERATURE:/*��������������¶ȵ����(��⻷���¶�)*/
			up_down_assistance_temperature -= 1;
			break;	
			
		case MENUST_UP_DOWN_OVERLOAD:/*ѹ�����ر�������*/
			up_down_overload -= 1;
			break;
			
		case MENUST_UP_DOWN_ANTIFREEZE_HEAT_STOP:/*����ģʽ�µļ���ֹͣ��*/
			up_down_antifreeze_heat_stop -= 1;
			break;		
			
		case MENUST_UP_DOWN_ELECT_HEAT:/*�ȱó��ֹ���ʱ,������Ƿ��Զ�����(0:������;1:����)*/
			up_down_elect_heat -= 1;
			break;
			
		default:
			break;
	}
}

/*************************************************
Funcname: up_down_menu_value_enter_keychang() 
contents: up down menu enter ��������
��ڲ���: newkey
����ֵ  : void
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
contents: up down menu�а�������
��ڲ���: newkey
����ֵ  : void
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
contents: set up menu �Ƿ���ЧChk
��ڲ���: menu
����ֵ  : 
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
contents: set down menu �Ƿ���ЧChk
��ڲ���: menu
����ֵ  : 
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
contents: up down menu �Ƿ���ЧChk
��ڲ���: menu
����ֵ  : 
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
contents: Menu�������ú���
��ڲ���: 
		  menu:��Ҫ�趨��Menu
		  set_value:�趨ֵ
		  set:0Ϊ��ʼ��Menu����ģʽ;1Ϊ�趨Menu����ģʽ

����ֵ  : 
		  1:true
		  0:false
**************************************************/
void menu_set_value( unsigned int menu,unsigned char set_value, unsigned char set )
{
	if( 0 == set )
	{
		set_up_reset_time = 10;				/*�ϵ���ʱ����ʱ�����*/
		set_up_two_ret_time = 10;			/*����ѹ���������ʱ�����*/
		set_up_protect_reset = 10;			/*�������±�����λʱ�����*/
		set_up_temperature_time = 5;		/*�������������뻷���������²����ʱ�����*/
		set_up_max_defrost_time = 10; 		/*���˪����ʱ�����*/
		set_up_defrost_adj_time = 45;		/*��˪���ڵ���*/
		set_up_two_press_delay_tiem = 3;	/*˫ѹ����������ʱ���ʱ�����*/


		set_down_high_temperature = 55;			/*��������¶ȵ��ڣ���ֹ�û����¶ȵ���������*/
		set_down_temperature_backlash = 8;		/*�¶Ȼز����H55��d08��470CΪ������*/
		set_down_stop_temperature = 95;			/*�������ȱ���ֹͣ�¶ȵ���*/
		set_down_remove_temperature = 70;			/*�������ȱ�������¶ȵ���*/
		set_down_spray_start_temperature = 85;		/*��Һ�������¶ȵ���*/
		set_down_spray_stop_temperature = 75;		/*��Һ���ر��¶ȵ���*/
		set_down_evaporator_outside_temp = 10;		/*�������¶ȴ������뻧���¶ȴ������²�ֵ����*/
		set_down_defrost_temperature = 8;			/*��˪�����¶ȵ���*/

		up_down_rest_method = 2;				/*������ʽ����*/
		up_down_assistance_temperature = 5;	/*��������������¶ȵ����(��⻷���¶�)*/
		up_down_overload = 25;					/*ѹ�����ر�������*/
		up_down_antifreeze_heat_stop = 9;		/*����ģʽ�µļ���ֹͣ��*/
		up_down_elect_heat = 0;				/*�ȱó��ֹ���ʱ,������Ƿ��Զ�����(0:������;1:����)*/
	}
	else
	{
		switch( menu )
		{
	/************************************************************
				*  Set up Menu  *
	************************************************************/
			case MENUST_SET_UP_RESET_DELAY_TIME:/*�ϵ���ʱ����ʱ�����*/
				set_up_reset_time = set_value;
				break;
				
			case MENUST_SET_UP_TWO_RET_TIME:/*����ѹ���������ʱ�����*/
				set_up_two_ret_time = set_value;
				break;
				
			case MENUST_SET_UP_PROTECT_RESET:/*�������±�����λʱ�����*/
				set_up_protect_reset = set_value;
				break;		
				
			case MENUST_SET_UP_TEMPERATURE_TIME:/*�������������뻷���������²����ʱ�����*/
				set_up_temperature_time = set_value;
				break;
				
			case MENUST_SET_UP_MAX_DEFROST_TIME:/*���˪����ʱ�����*/
				set_up_max_defrost_time = set_value;
				break;
				
			case MENUST_SET_UP_DEFROST_ADJ_TIME:/*��˪���ڵ���*/
				set_up_defrost_adj_time = set_value;
				break;
				
			case MENUST_SET_UP_TWO_PRESS_DELAY_TIME:/*˫ѹ����������ʱ���ʱ�����*/
				set_up_two_press_delay_tiem = set_value;
				break;	
	 /************************************************************
				*  Set down Menu  *
	 ************************************************************/				
			case MENUST_SET_DOWN_HIGH_TEMPERATURE:/*��������¶ȵ��ڣ���ֹ�û����¶ȵ���������*/
				set_down_high_temperature = set_value;
				break;
				
			case MENUST_SET_DOWN_TEMPERATURE_BACKLASH:/*�¶Ȼز����H55��d08��470CΪ������*/
				set_down_temperature_backlash = set_value;
				break;		
				
			case MENUST_SET_DOWN_STOP_TEMPERATURE:/*�������ȱ���ֹͣ�¶ȵ���*/
				set_down_spray_stop_temperature= set_value;
				break;
				
			case MENUST_SET_DOWN_REMOVE_TEMPERATURE:/*�������ȱ�������¶ȵ���*/
				set_down_remove_temperature = set_value;
				break;		
				
			case MENUST_SET_DOWN_SPRAY_START_TEMPERATURE:/*��Һ�������¶ȵ���*/
				set_down_spray_start_temperature = set_value;
				break;
				
			case MENUST_SET_DOWN_SPRAY_STOP_TEMPERATURE:/*��Һ���ر��¶ȵ���*/
				set_down_spray_stop_temperature = set_value;
				break;	
				
			case MENUST_SET_DOWN_EVAPORATOR_OUTSIDE_TEMP:/*�������¶ȴ������뻧���¶ȴ������²�ֵ����*/
				set_down_evaporator_outside_temp = set_value;
				break;
				
			case MENUST_SET_DOWN_DEFROST_OVER_TEMPERATURE:/*��˪�����¶ȵ���*/
				set_down_defrost_temperature = set_value;
				break;	
	 /************************************************************
				*  up down Menu  *
	  ************************************************************/			
			case MENUST_UP_DOWN_REST_METHOD:/*������ʽ����*/		
				up_down_rest_method = set_value;
				break;
				
			case MENUST_UP_DOWN_ASSISTANCE_TEMPERATURE:/*��������������¶ȵ����(��⻷���¶�)*/
				up_down_assistance_temperature = set_value;
				break;	
				
			case MENUST_UP_DOWN_OVERLOAD:/*ѹ�����ر�������*/
				up_down_overload = set_value;
				break;
				
			case MENUST_UP_DOWN_ANTIFREEZE_HEAT_STOP:/*����ģʽ�µļ���ֹͣ��*/
				up_down_antifreeze_heat_stop = set_value;
				break;		
				
			case MENUST_UP_DOWN_ELECT_HEAT:/*�ȱó��ֹ���ʱ,������Ƿ��Զ�����(0:������;1:����)*/
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
