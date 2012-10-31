/******************************************************
文件完成AD通道的选择，AD_MODE_CH* 为TRUE时，表明通道有效

******************************************************/
#ifndef _AD_DRIVE_H_
#define _AD_DRIVE_H_

public void ADC_InitReset( void );
public unsigned int ADC_execConvert( unsigned char ch);
public unsigned char  AD_get_value( unsigned char ch );



#endif
