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
	MENU_DIRECTION_NORMAL	/*Menu�����л�*/
,	MENU_DIRECTION_REVERSE	/*Menu�����л�*/
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

/*SETB UP Menu �˵�*/
typedef enum{
 MENUST_SET_UP_START = MENUATTR_SET_UP*0x10 	/*The most menu if 16*/
,MENUST_SET_UP_RESET_DELAY_TIME					/*�ϵ���ʱ����ʱ�����*/
,MENUST_SET_UP_TWO_RET_TIME						/*����ѹ���������ʱ�����*/
,MENUST_SET_UP_PROTECT_RESET					/*�������±�����λʱ�����*/
,MENUST_SET_UP_TEMPERATURE_TIME					/*�������������뻷���������²����ʱ�����*/
,MENUST_SET_UP_MAX_DEFROST_TIME					/*���˪����ʱ�����*/
,MENUST_SET_UP_DEFROST_ADJ_TIME					/*��˪���ڵ���*/
,MENUST_SET_UP_TWO_PRESS_DELAY_TIME				/*˫ѹ����������ʱ���ʱ�����*/
,MENUST_SET_UP_END
};

/*SET DOWN Menu �˵�*/
typedef enum{
 MENUST_SET_DOWN_START = MENUATTR_SET_DOWN*0x10 /*The most menu if 16*/
,MENUST_SET_DOWN_HIGH_TEMPERATURE				/*��������¶ȵ��ڣ���ֹ�û����¶ȵ���������*/
,MENUST_SET_DOWN_TEMPERATURE_BACKLASH			/*�¶Ȼز����H55��d08��470CΪ������*/
,MENUST_SET_DOWN_STOP_TEMPERATURE				/*�������ȱ���ֹͣ�¶ȵ���*/
,MENUST_SET_DOWN_REMOVE_TEMPERATURE				/*�������ȱ�������¶ȵ���*/
,MENUST_SET_DOWN_SPRAY_START_TEMPERATURE		/*��Һ�������¶ȵ���*/
,MENUST_SET_DOWN_SPRAY_STOP_TEMPERATURE			/*��Һ���ر��¶ȵ���*/
,MENUST_SET_DOWN_EVAPORATOR_OUTSIDE_TEMP		/*�������¶ȴ������뻧���¶ȴ������²�ֵ����*/
,MENUST_SET_DOWN_DEFROST_OVER_TEMPERATURE		/*��˪�����¶ȵ���*/
,MENUST_SET_DOWN_END
};

/*UP DOWN Menu �˵�*/
typedef enum{
 MENUST_UP_DOWN_START = MENUATTR_UP_DOWN *0X10 /*The most menu if 16*/
,MENUST_UP_DOWN_REST_METHOD				/*������ʽ����*/
,MENUST_UP_DOWN_ASSISTANCE_TEMPERATURE	/*��������������¶ȵ����(��⻷���¶�)*/
,MENUST_UP_DOWN_OVERLOAD				/*ѹ�����ر�������*/
,MENUST_UP_DOWN_ANTIFREEZE_HEAT_STOP	/*����ģʽ�µļ���ֹͣ��*/
,MENUST_UP_DOWN_ELECT_HEAT				/*�ȱó��ֹ���ʱ,������Ƿ��Զ�����(0:������;1:����)*/
,MENUST_UP_DOWN_END
};



#endif/*_MENU_H_*/

