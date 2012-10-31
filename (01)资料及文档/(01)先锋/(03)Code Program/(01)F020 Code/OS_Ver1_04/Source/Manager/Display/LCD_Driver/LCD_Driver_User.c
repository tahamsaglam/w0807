//========================================================================
// �ļ���: LCD_Driver_User.c
// ��  ��: �ײ�ӿ�������չ����
//========================================================================
#include "LCD_Config.h"
#include "LCD_PortConfig.h"

//========================================================================
// ����: void LCD_DataWrite(unsigned int Data)
// ����: дһ���ֽڵ���ʾ������LCD�е���ʾ����RAM����
// ����: Data д������� 
// ����: ��
//========================================================================
void LCD_DataWrite(LCDBYTE Dat)
{
	unsigned char Num;
	LCD_CS_CLR();// = 0;
	LCD_A0_SET();// = 1;
	for(Num=0;Num<8;Num++)
	{
		if((Dat&0x80) == 0)	LCD_SDI_CLR();// = 0;
		else LCD_SDI_SET();// = 1;
		Dat = Dat << 1;
		LCD_CLK_CLR();// = 0;
		LCD_CLK_SET();// = 1;
	}
	LCD_CS_SET();// = 1;
}
//========================================================================
// ����: void LCD_RegWrite(unsigned char Command)
// ����: дһ���ֽڵ�������LCD�еĿ��ƼĴ�������
// ����: Command		д������ݣ��Ͱ�λ��Ч��byte�� 
// ����: ��
//========================================================================
void LCD_RegWrite(LCDBYTE Command)
{
	unsigned char Num;
	LCD_CS_CLR();// = 0;
	LCD_A0_CLR();// = 0;
	for(Num=0;Num<8;Num++)
	{
		if((Command&0x80) == 0)	LCD_SDI_CLR();// = 0;
		else LCD_SDI_SET();// = 1;
		Command = Command << 1;
		LCD_CLK_CLR();// = 0;
		LCD_CLK_SET();// = 1;
	}
	LCD_CS_SET();// = 1;
}
//========================================================================
// ����: void LCD_Fill(unsigned int Data)
// ����: ���������Data��������������
// ����: Data   Ҫ������ɫ����
// ����: ��
// ��ע: ����LCD��ʼ�������е���
//========================================================================
void LCD_Fill(LCDBYTE Data)
{
	unsigned char i,j;
	unsigned char uiTemp;
	uiTemp = Dis_Y_MAX;
	uiTemp = uiTemp>>3;
	for(i=0;i<=uiTemp;i++)								//��LCD������ʼ������ʾ����
	{
		LCD_RegWrite(0xb0+i);
		LCD_RegWrite(0x00);//1);
		LCD_RegWrite(0x10);
		for(j=0;j<=Dis_X_MAX;j++)
		{
			LCD_DataWrite(Data);
		}
	}
}
//========================================================================
// ����:TimeDelay
// ����: LCD��ʼ����������������LCD��ʼ����Ҫ���õ����Ĵ������������
//		 �û����˽⣬����鿴DataSheet���и����Ĵ���������
// ����: �� 
// ����: ��
// ��ע:
//========================================================================
//��ʱ����

void TimeDelay(int Time)
{
	int i;
	while(Time > 0)
	{
		for(i = 0;i < 8000;i++)
		{
			;//_nop_();
		}
		Time --;
	}
}
//========================================================================
// ����: LCD_Init
// ����: LCD��ʼ��
// ����: void
// ����: void
// ��ע: 
//========================================================================
extern void LCD_Init(void)
{
	//LCD������ʹ�õ��Ķ˿ڵĳ�ʼ��������б�Ҫ�Ļ���
	LCD_Ctrl_GPIO();
	LCD_Ctrl_Out();

	TimeDelay(200);
	LCD_RE_CLR();// = 0;
	TimeDelay(200);
	LCD_RE_SET();// = 1;
	TimeDelay(20);
		
	LCD_RegWrite(M_LCD_ON);							//LCD On
	LCD_RegWrite(M_LCD_POWER_ALL);					//�����ϵ����ģʽ
	
	LCD_RegWrite(M_LCD_ELE_VOL);					//��������ģʽ����ʾ���ȣ�
	LCD_RegWrite(0x1f);								//ָ������0x0000~0x003f
	
	LCD_RegWrite(M_LCD_VDD_SET);					//V5�ڲ���ѹ���ڵ�������
	LCD_RegWrite(M_LCD_VDD);						//LCDƫѹ���ã�V3ʱѡ
	
	LCD_RegWrite(M_LCD_COM_REV);					//Com ɨ�跽ʽ����
	LCD_RegWrite(M_LCD_SEG_NOR);					//Segment����ѡ��
	LCD_RegWrite(M_LCD_ALL_LOW);					//ȫ������/�䰵ָ��
	LCD_RegWrite(M_LCD_ALL_NOR);					//��������ʾ����ָ��
	
	LCD_RegWrite(M_LCD_STATIC_OFF);					//�رվ�ָ̬ʾ��
	LCD_RegWrite(0x00);								//ָ������
	
	LCD_RegWrite(M_LCD_BEGIN_LINE);					//������ʾ��ʼ�ж�ӦRAM
	LCD_Fill(LCD_INITIAL_COLOR);
//	ClrScreen(0);									/*����*/
}

