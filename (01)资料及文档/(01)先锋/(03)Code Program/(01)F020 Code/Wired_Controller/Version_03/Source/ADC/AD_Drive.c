/********************
������ֲ��Ҫע�������:
1.AD ͨ�������
2.ADת����õ���ֵ��USHRT�ͣ�����ADC_execConvertSequence��AD_get_result ���ز���
3.���Ĵ���������:����AD������������־��ADת�����ֵ�Ĵ�����

************************/


/**************************************
ͷ�ļ�����
***************************************/
#include"../Common/common.h"

#define AD_DEBUG
#ifdef AD_DEBUG
#define ADA0CE 0x00;
#define ADIF 0x00;
#define ADA0M0 0x00;
#define ADA0M1 0x00;
#define ADA0M2 0x00;
#define ADA0PFM 0x00;
#define ADA0PFT 0x00;
#define ADA0S 0x00;

#define ADA0CR0 0x00;
#define ADA0CR1 0x00;
#define ADA0CR2 0x00;
#define ADA0CR3 0x00;
#define ADA0CR4 0x00;
#define ADA0CR5 0x00;
#define ADA0CR6 0x00;
#define ADA0CR7 0x00;
#define ADA0CR8 0x00;
#define ADA0CR9 0x00;
#define ADA0CR10 0x00;
#define ADA0CR11 0x00;
#define ADA0CR12 0x00;
#define ADA0CR13 0x00;
#define ADA0CR14 0x00;
#define ADA0CR15 0x00;
#endif/*AD_DEBUG*/

/**************************************
define ����
***************************************/
#define AD_MODE_CH0 	TRUE		/**/
#define AD_MODE_CH1 	TRUE
#define AD_MODE_CH2 	TRUE
#define AD_MODE_CH3 	TRUE
#define AD_MODE_CH4 	TRUE
#define AD_MODE_CH5 	TRUE
#define AD_MODE_CH6 	TRUE
#define AD_MODE_CH7 	TRUE
#define AD_MODE_CH8 	TRUE
#define AD_MODE_CH9 	TRUE
#define AD_MODE_CH10 	TRUE
#define AD_MODE_CH11 	TRUE
#define AD_MODE_CH12 	TRUE
#define AD_MODE_CH13 	TRUE
#define AD_MODE_CH14 	TRUE
#define AD_MODE_CH15 	TRUE

#define	AD_VAL_CH0		0x00
#define	AD_VAL_CH1		0x01
#define	AD_VAL_CH2		0x02
#define	AD_VAL_CH3		0x03
#define	AD_VAL_CH4		0x04
#define	AD_VAL_CH5		0x05
#define	AD_VAL_CH6		0x06
#define	AD_VAL_CH7		0x07
#define	AD_VAL_CH8		0x08
#define	AD_VAL_CH9		0x09
#define	AD_VAL_CH10	0x0a
#define	AD_VAL_CH11	0x0b
#define	AD_VAL_CH12	0x0c
#define	AD_VAL_CH13	0x0d
#define	AD_VAL_CH14	0x0e
#define	AD_VAL_CH15	0x0f
#define AD_SAMPLE_MAX 	0x03			/*AD��������*/

#define AD_Ch_Check_enable( ch )  	( ( ch > AD_VAL_CH15)? FALSE:TRUE)	/*�������ĺ궨��/�ж�ADͨ�����Ƿ񳬷�Χ*/
#define AD_stopADConverter() 		ADA0CE = 0			/*CPU�ڲ�������Ĵ���������ADC��ת����ʹ��*/
#define AD_startADConverter()		ADA0CE = 1			/*ADCת��ʹ��,����ʼת��*/
#define AD_stop_interrupt()  				/*��ֹ�ж�����*/
#define AD_checkConvertEnd()		((ADIF == 1)?TRUE : FALSE) /*ADCת���������1:������0:����ת��*/
#define AD_CNT_WAIT_MAX	 		0x80		/*ADCת�������ȴ�ʱ�����ֵ:(ת��ʱ��=104/fxp=3.31us,�ȶ�ʱ�� =50/fxp=1.59us)*/

private const unsigned char ADC_ChannelCheckTable[]= { /*ADCͨ����Ч��Ч��*/
	AD_MODE_CH0,
	AD_MODE_CH1,
	AD_MODE_CH2,
	AD_MODE_CH3,
	AD_MODE_CH4,
	AD_MODE_CH5,
	AD_MODE_CH6,
	AD_MODE_CH7,
	AD_MODE_CH8,
	AD_MODE_CH9,
	AD_MODE_CH10,
	AD_MODE_CH11,
	AD_MODE_CH12,
	AD_MODE_CH13,
	AD_MODE_CH14,
	AD_MODE_CH15,
};


/**************************************
Static RAM����
***************************************/
private unsigned char ad_buff[ AD_SAMPLE_MAX ]={0x00,0x00,0x00};

/**************************************
Funcname	: ADC_offADPower
Contents 	: ����ʱ�Ĵ���ֹͣADת��
Input		: void
Return		: void
Remart		: void	 

***************************************/
#define	ADC_offADPower()		ADA0CE = 0


/**************************************
Funcname	: ADC_onADPower
Contents 	: ��Դ�ϵ紦��
Input		: void
Return		: void
Remart		: void	 

***************************************/
#define	ADC_onADPower()		


/**************************************
Funcname	: ADC_InitReset
Contents 	: �ϵ��ʼ������
Input		: void
Return		: void
Remart		: void	 

***************************************/
public void 
ADC_InitReset( void )
{
	ADA0CE = 0;
	ADA0M0 = 0x20;	
	ADA0M1 = 0x83;		/* A/D: fxp=31.457MHz�ł̍ő���ݒ�	*/
	ADA0M2 = 0x00;		
	ADA0PFM = 0x00;
	ADA0PFT = 0x00;
}

/**************************************
Funcname	: ADC_check_Channel
Contents 	: ��֤ADͨ���Ƿ���Ч
Input		: ch ��Ҫת����ADͨ����
Return		: ��ת����ͨ���Ƿ���Ч
 			  TRUE:  1
			  FALSE: 0
Remart		: void 
***************************************/
private unsigned char 
ADC_Check_Channel( unsigned char ch )
{
	unsigned char result = 0;
	if( FALSE == AD_Ch_Check_enable( ch ) ){	/*�ж�ADͨ�����Ƿ񳬷�Χ*/
/*	if( FALSE == ( sizeof( ADC_ChannelCheckTable )/sizeof(ADC_ChannelCheckTable[0]) )	*//*�ж�ADͨ�����Ƿ񳬷�Χ*/
		result = FALSE;
	}
	else{
		result = ADC_ChannelCheckTable[ch];/*ͨ��û�г���Channel��Χ����֤�Ƿ���Ч*/
	}
	return( result );
}

/**************************************
Funcname	: ADC_setADConverter
Contents 	: ADת��������趨
Input		: ch
Return		: void
Remart		:	 

***************************************/
package void
ADC_setADConverter(unsigned char ch)
{
	if ( FALSE == AD_Ch_Check_enable( ch ) ){
		return ;
	}
	ADA0S = ch;		/*CPU��AD�ڲ��Ĵ���,ͨ��ѡ��Ĵ���*/
	ADA0M0 = 0x20;
	ADA0M1 = 0x83;  /*(ת��ʱ��=104/fxp=3.31us,�ȶ�ʱ�� =50/fxp=1.59us)*/
}

/**************************************
Funcname	: AD_get_result
Contents 	: ���ADת�����ADֵ
Input		: chͨ����
Return		: ADֵ
Remart		:	 
			  ��Ч��ת�����Ϊ0
***************************************/
package unsigned int
ADC_get_result( unsigned char ch)
{
	
	unsigned short	ad_buff = 0x00;	/*ADֵ����*/

	switch ( ch ){		/*ADͨ���ţ���Ӧ��ADת�����ֵ*/
		case AD_VAL_CH0:
			ad_buff = ADA0CR0;	
			break;
		case AD_VAL_CH1:
			ad_buff = ADA0CR1;
			break;
		case AD_VAL_CH2:
			ad_buff = ADA0CR2;
			break;
		case AD_VAL_CH3:
			ad_buff = ADA0CR3;
			break;
		case AD_VAL_CH4:
			ad_buff = ADA0CR4;
			break;
		case AD_VAL_CH5:
			ad_buff = ADA0CR5;
			break;
		case AD_VAL_CH6:
			ad_buff = ADA0CR6;
			break;
		case AD_VAL_CH7:
			ad_buff = ADA0CR7;
			break;
		case AD_VAL_CH8:
			ad_buff = ADA0CR8;
			break;
		case AD_VAL_CH9:
			ad_buff = ADA0CR9;
			break;
		case AD_VAL_CH10:
			ad_buff = ADA0CR10;
			break;
		case AD_VAL_CH11:
			ad_buff = ADA0CR11;
			break;
		case AD_VAL_CH12:
			ad_buff = ADA0CR12;
			break;
		case AD_VAL_CH13:
			ad_buff = ADA0CR13;
			break;
		case AD_VAL_CH14:
			ad_buff = ADA0CR14;
			break;
		case AD_VAL_CH15:
			ad_buff = ADA0CR15;
			break;
		default:
			break;
	}
	return( ad_buff );
}


/**************************************
Funcname	: ADC_execConvertSequence
Contents 	: ���ADת��ֵ
Input		: chͨ����
Return		: ����ADת�������ֵ
Remart		: ����ֵ��unsigned int	 

***************************************/
public unsigned int
ADC_execConvert( unsigned char ch)
{
	unsigned int value = 0x0000;
	unsigned char cnt_time;
//	UCHAR ad_buf = 0x00;	
	if ( FALSE == AD_Ch_Check_enable( ch ) ){
		return ;	/*ת��ͨ����Ч��ֱ�ӷ���*/
	}
	AD_stopADConverter();		/*ת��ǰ��ֹͣADת��*/
	ADC_setADConverter(ch);	/*����ADת���Ĳ���*/
	AD_stop_interrupt();		/*��ֹ�ж�����*/		
	AD_startADConverter();	/*ADCת����ʼ*/

	for ( cnt_time = 0; cnt_time < AD_CNT_WAIT_MAX; cnt_time ++ ){	/*�ȴ�ADת������*/
		if ( AD_checkConvertEnd() == TRUE ) {		/*��ѯADת���Ƿ����*/
			value = ADC_get_result( ch );		/*��ö�Ӧͨ����ADת��ֵ*/	
			break;							/*ADת������,����ѭ��*/
		}
	}
	AD_stopADConverter();	/*ת����ֹͣADת��*/
	return ( value );		/*����ADֵ*/
//	ad_buf = (UCHAR)( value >> 8 );	/**/
//	return ( ad_buf );
}

 /**************************************
 Funcname	: AD_get_value
 Contents	: AD ����ȡƽ��ֵ
 Input		: chͨ����
 Return		: ����ƽ��ֵ
 Remart		:	  
 
 ***************************************/
 public unsigned char 
 AD_get_value( unsigned char ch )
 {
	unsigned int ad_value = 0x00;
	unsigned char loop = 0;
	for( loop=( AD_SAMPLE_MAX - 1 );loop > 0; loop ++){
		ad_buff[loop] = ad_buff[loop-1];
	}
	ad_buff[0] =( unsigned char )(ADC_execConvert[ch]>>8);
//	ad_value = ADC_execConvertSequence[ch];
	for( loop=0;loop<AD_SAMPLE_MAX;loop++){
		ad_value += ad_buff[loop];
	}
	ad_value /= AD_SAMPLE_MAX;
	return( ad_value );
 }


 /***********************************************************************************
 Ver		date		name		reviewed	 information	



***********************************************************************************/


