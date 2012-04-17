/***************************************************************
Module: All
Author: ***
Time  : 2012-04-14
***************************************************************/

#ifndef _MENU_H_
#define _MENU_H_

typedef enum{
 MENUATTR_OFF = 0x00	/*Menu attribue is Off*/
,MENUATTR_SET_UP		/*if press the both of set_key and up_key,Menu enter*/
,MENUATTR_SET_DOWN		/*if press the both of set_key and down_key,Menu enter*/
,MENUATTR_UP_DOWN		/*if press the both of up_key and down_key,Menu enter*/
,MENUATTR_END		/*Menu attribut end*/
};

/*Switch Menu*/
enum{
	MENU_DIRECTION_NORMAL	/*Menu正向切换*/
,	MENU_DIRECTION_REVERSE	/*Menu方向切换*/
};

typedef union{
	struct{
		unsigned char attribute;
		unsigned char status;
	}one;
	unsigned short int all;
}MenuStatus;

typedef struct{
 unsigned short int far *last ;
 unsigned char far (*enable_check)(unsigned short int);
 unsigned short int	menu_start;
 unsigned short int	menu_end;
}MenuTbl;

extern	MenuStatus	menust;

/*SETB UP Menu 菜单*/
typedef enum{
 MENUST_SET_UP_START = MENUATTR_SET_UP*0x10 	/*The most menu if 16*/
,MENUST_SET_UP_RESET_DELAY_TIME					/*上电延时保护时间调节*/
,MENUST_SET_UP_TWO_RET_TIME						/*两次压机启动间隔时间调节*/
,MENUST_SET_UP_PROTECT_RESET					/*排气超温保护复位时间调节*/
,MENUST_SET_UP_TEMPERATURE_TIME					/*蒸发器传感器与环境传感器温差持续时间调节*/
,MENUST_SET_UP_MAX_DEFROST_TIME					/*最大化霜工作时间调节*/
,MENUST_SET_UP_DEFROST_ADJ_TIME					/*化霜周期调节*/
,MENUST_SET_UP_TWO_PRESS_DELAY_TIME				/*双压保护开关延时检测时间调节*/
,MENUST_SET_UP_END
};

/*SET DOWN Menu 菜单*/
typedef enum{
 MENUST_SET_DOWN_START = MENUATTR_SET_DOWN*0x10 /*The most menu if 16*/
,MENUST_SET_DOWN_HIGH_TEMPERATURE				/*最高上限温度调节，防止用户将温度调节至过高*/
,MENUST_SET_DOWN_TEMPERATURE_BACKLASH			/*温度回差调节H55～d08即470C为开机点*/
,MENUST_SET_DOWN_STOP_TEMPERATURE				/*排气过热保护停止温度调节*/
,MENUST_SET_DOWN_REMOVE_TEMPERATURE				/*排气过热保护解除温度调节*/
,MENUST_SET_DOWN_SPRAY_START_TEMPERATURE		/*喷液阀开启温度调节*/
,MENUST_SET_DOWN_SPRAY_STOP_TEMPERATURE			/*喷液阀关闭温度调节*/
,MENUST_SET_DOWN_EVAPORATOR_OUTSIDE_TEMP		/*蒸发器温度传感器与户外温度传感器温差值调节*/
,MENUST_SET_DOWN_DEFROST_OVER_TEMPERATURE		/*化霜结束温度调节*/
,MENUST_SET_DOWN_END
};

/*UP DOWN Menu 菜单*/
typedef enum{
 MENUST_UP_DOWN_START = MENUATTR_UP_DOWN *0X10 /*The most menu if 16*/
,MENUST_UP_DOWN_REST_METHOD				/*启动方式调节*/
,MENUST_UP_DOWN_ASSISTANCE_TEMPERATURE	/*辅助电加热切入温度点调节(检测环境温度)*/
,MENUST_UP_DOWN_OVERLOAD				/*压机过载保护调节*/
,MENUST_UP_DOWN_ANTIFREEZE_HEAT_STOP	/*防冻模式下的加热停止点*/
,MENUST_UP_DOWN_ELECT_HEAT				/*热泵出现故障时,电加热是否自动介入(0:不介入;1:介入)*/
,MENUST_UP_DOWN_END
};



#endif/*_MENU_H_*/

