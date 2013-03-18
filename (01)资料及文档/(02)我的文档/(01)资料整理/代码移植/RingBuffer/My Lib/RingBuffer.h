/*******************************************************************************

	RCS $Id: RingBuffer.h,v 1.1.1.1 2011/01/31 02:47:48 jyunichi_suzuki Exp $

	�@�@�\�FRingBuffer�T�[�r�X
	�@�@��F
	�쐬�ҁF�p�C�I�j�A�V�X�e���e�N�m���W�[(��) �J���Z���^�[
		
	���@�l�F�Ȃ�

*******************************************************************************/
#ifndef _MODULE_RINGBUFFER_ARI_
#define _MODULE_RINGBUFFER_ARI_
/*������������������������������������������������������������������������������
	define��`
������������������������������������������������������������������������������*/


/*����������������������������
�@��RingBuffer�Ǘ��\�ő吔��
�@����������������������������*/

typedef unsigned char RB_DTSIZE;

/*����������������������������
�@��RingBuffer�Ǘ��\�ő吔��
�@����������������������������*/
#define	RB_NUM_MAXENTRY		10
#define	RB_VAL_ENTRY_NG		0x00

/*������������������������������������������������������������������������������
	typedef�錾
������������������������������������������������������������������������������*/
/*������������������������
�@��RingBuffer�X�e�[�^�X��
�@������������������������*/
typedef	enum{
	RB_STS_NORMAL = 0,
	RB_STS_EMPTY,
	RB_STS_FULL
}	ERBStatus;
/*������������������
�@��RingBuffer�Ǘ���
�@������������������*/


/*������������������������������������������������������������������������������
	�}�N���֐���`
������������������������������������������������������������������������������*/
/* nothing */

/*������������������������������������������������������������������������������
	�O���֐��ւ̃����N�@[External Linkage]
������������������������������������������������������������������������������*/
public unsigned char	RB_reqEntry		(unsigned char *, RB_DTSIZE);
public void		RB_initRingBuffer	(unsigned char);
public ERBStatus	RB_getBufferStatus	(unsigned char);
public void		RB_putRingBuffer	(unsigned char, unsigned char);
public unsigned char	RB_getRingBuffer	(unsigned char);

#endif/*_MODULE_RINGBUFFER_ARI_*/
