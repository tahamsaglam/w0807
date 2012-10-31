//========================================================================
// �ļ���: LCD_Dis.c
// ��  ��: ���нӿڰ�LCD�û��ӿڲ����
//========================================================================
#include "LCD_Driver_User.h"					//LCD�ײ���������ͷ�ļ�
#include "LCD_Config.h"							//LCD�������ļ��������������Ƿ��õȵĶ���

extern FLASH Asii0608[];			//6X8��ASII�ַ���
extern FLASH Asii0816[];			//8X16��ASII�ַ���
extern FLASH GB1616[];				//16*16�Զ���ĺ��ֿ�

DOTBYTE X_Witch;							//�ַ�д��ʱ�Ŀ��
DOTBYTE Y_Witch;							//�ַ�д��ʱ�ĸ߶�
DOTBYTE Font_Wrod;						//�����ÿ����ģռ�ö��ٸ��洢��Ԫ��
FLASH *Char_TAB;						//�ֿ�ָ��
LCDBYTE BMP_Color;
LCDBYTE Char_Color;

BYTE	Font_type=0;								//��ʶ�ַ�����

//========================================================================
// ����: void FontSet(BYTE Font_NUM,LCDBYTE Color)
// ����: �ı���������
// ����: Font_NUM ����ѡ��,�������������ֿ�Ϊ׼
//		 Color  �ı���ɫ,���������Դ��ֿ�  
// ����: ��
//========================================================================
void FontSet(BYTE Font_NUM,LCDBYTE Color)
{
	switch(Font_NUM)
	{
		case 0: Font_Wrod = 16;	//ASII�ַ�A	8*16
				X_Witch = 8;
				Y_Witch = 2;
				Char_Color = Color;
				Char_TAB = Asii0816;
				Font_type = 1;
		break;	  
		case 1: Font_Wrod = 6;	//ASII�ַ�B	 6*8
				X_Witch = 6;
				Y_Witch = 1;
				Char_Color = Color;
				Char_TAB = Asii0608;
				Font_type = 1;
		break;		
		case 2: Font_Wrod = 32;	//����A	   16*16
				X_Witch = 16;
				Y_Witch = 2;
				Char_Color = Color;
				Char_TAB = GB1616;
				Font_type = 0;
		break;
		default: break;
	}
}
//========================================================================
// ����: void PutChar(DOTBYTE x,DOTBYTE y,char a)
// ����: д��һ����׼�ַ�
// ����: x  X������     y  Y������(0~7ҳ)
//		 a  Ҫ��ʾ���ַ����ֿ��е�ƫ����  
// ����: ��
// ��ע: ASCII�ַ���ֱ������ASCII�뼴��
//========================================================================
void PutChar(DOTBYTE x,DOTBYTE y,char a)       
{
	unsigned char i,j;
	FLASH *p_data; 
	if(Font_type==1)
		p_data = Char_TAB + (a-32)*Font_Wrod;
	else
		p_data = Char_TAB + a*Font_Wrod;	//Ҫд�ַ����׵�ַ
	x = x;//+1;							//Edit by xinqiang
	for(j=0;j<Y_Witch;j++)
	{
		if((y+j) < (Dis_Y_MAX/8))
			LCD_RegWrite(y+j+0xb0);
		LCD_RegWrite(((x&0xf0) >> 4)|0x10);	//���ó�ʼ��ַ 
		LCD_RegWrite(x&0x0f);
		for(i=0;i<X_Witch;i++)
		{  
			if(((x+i) < Dis_X_MAX)&&((y+j) < (Dis_Y_MAX/8)))
				LCD_DataWrite(*p_data++);
		}
	}
}

//========================================================================
// ����: void PutString(DOTBYTE x,DOTBYTE y,char *p)
// ����: ��x��yΪ��ʼ���괦д��һ����׼�ַ�
// ����: x  X������     y  Y������(0~7ҳ)
//		 p  Ҫ��ʾ���ַ���  
// ����: ��
// ��ע: ���������Դ���ASCII�ַ�����ʾ
//========================================================================
void PutString(DOTBYTE x,DOTBYTE y,char *p)
{
	while(*p!=0)
	{
		PutChar(x,y,*p);
		x += X_Witch;
		if((x + X_Witch) > Dis_X_MAX)
		{
			x = 0;
			if((Dis_Y_MAX - y) < Y_Witch) break;
			else y += Y_Witch;
		}
		p++;
	}
}
//========================================================================
// ����: void ClrScreen(unsigned char Mode)
// ����: ����������ִ��ȫ��Ļ��������ǰ��ɫ
// ����: Mode   0:ȫ������
//				1:ȫ�����ǰ��ɫ
// ����: ��
//========================================================================
extern void ClrScreen(unsigned char Mode)
{
	if(Mode==0)
		LCD_Fill(LCD_INITIAL_COLOR);
	else
		LCD_Fill(BMP_Color);
}
