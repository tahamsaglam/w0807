#include "LCD_Config.h"
//LCD ��ʼ��������һ������LCD���ļĴ�����ʼ��
extern void LCD_Init(void);	//��LCD_Driver_User.c�ж���ĺ��������б�Ҫ�ú�����������˿ڳ�ʼ������λ�Ȳ���


//��׼�ַ����ã��������ͺ�ASCII��Ĵ�С�ߴ磬�Լ�������ɫ������
extern void FontSet(unsigned char Font_NUM,LCDBYTE Color);
//��x��y��������д��һ����׼�ַ�
extern void PutChar(DOTBYTE x,DOTBYTE y,char a);
//��x��y������Ϊ��ʼд��һ����׼�ַ���
extern void PutString(DOTBYTE x,DOTBYTE y,char *p);
//����������ִ��ȫ��Ļ��������ǰ��ɫ
extern void ClrScreen(unsigned char Mode);

//���º����Լ�����������һ�㽨���û���Ҫ���ã������߼��û������б�д������ʾЧ���Լ�����Ӧ�ó���ʱʹ��
//
//extern unsigned char code Asii16[];		//8X16��ASII�ַ���
//extern unsigned char code GB32[];		//�Զ����32X29���ֿ�
//extern unsigned char code GB48[];		//�Զ����48X55���ֿ�

extern DOTBYTE X_Witch;					//�ַ�д��ʱ�Ŀ��
extern DOTBYTE Y_Witch;					//�ַ�д��ʱ�ĸ߶�
extern DOTBYTE Font_Wrod;				//����Ĵ�
extern unsigned char *Char_TAB;		//�ֿ�ָ��
//extern unsigned char Plot_Mode;				//��ͼģʽ
extern LCDBYTE BMP_Color;
extern LCDBYTE Char_Color;

//extern void Pos_Switch(unsigned int * x,unsigned int * y);
//extern void Writ_Dot(int x,int y,unsigned int Color);
//extern void Line_f(int s_x,int s_y,int e_x);
//extern unsigned int Get_Dot(int x,int y);
//extern void Clear_Dot(int x,int y);
//extern void Set_Dot_Addr(int x,int y);
