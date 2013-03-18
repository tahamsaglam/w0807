/*******************************************************************************

	RCS $Id: RingBuffer.h,v 1.1.1.1 2011/01/31 02:47:48 jyunichi_suzuki Exp $

	機　能：RingBufferサービス
	機　種：
	作成者：パイオニアシステムテクノロジー(株) 開発センター
		
	備　考：なし

*******************************************************************************/
#ifndef _MODULE_RINGBUFFER_ARI_
#define _MODULE_RINGBUFFER_ARI_
/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	define定義
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/


/*┌────────────┐
　│RingBuffer管理可能最大数│
　└────────────┘*/

typedef unsigned char RB_DTSIZE;

/*┌────────────┐
　│RingBuffer管理可能最大数│
　└────────────┘*/
#define	RB_NUM_MAXENTRY		10
#define	RB_VAL_ENTRY_NG		0x00

/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	typedef宣言
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/
/*┌──────────┐
　│RingBufferステータス│
　└──────────┘*/
typedef	enum{
	RB_STS_NORMAL = 0,
	RB_STS_EMPTY,
	RB_STS_FULL
}	ERBStatus;
/*┌───────┐
　│RingBuffer管理│
　└───────┘*/


/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	マクロ関数定義
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/
/* nothing */

/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	外部関数へのリンク　[External Linkage]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/
public unsigned char	RB_reqEntry		(unsigned char *, RB_DTSIZE);
public void		RB_initRingBuffer	(unsigned char);
public ERBStatus	RB_getBufferStatus	(unsigned char);
public void		RB_putRingBuffer	(unsigned char, unsigned char);
public unsigned char	RB_getRingBuffer	(unsigned char);

#endif/*_MODULE_RINGBUFFER_ARI_*/
