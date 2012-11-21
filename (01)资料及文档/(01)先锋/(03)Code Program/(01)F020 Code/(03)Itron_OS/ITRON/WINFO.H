/* winfo.h
   Header file for uITRON wait information.

   Notes:
   ------
   a) When a task is required to WAIT for some resource or other reason, it 
      is often necessary that information be provided so that when the
	  resource becomes available, the original request for the resource
	  can continue and be completed.  This header file contains structures
	  which contain whatever data is required for each type of WAIT that
	  may occur.

   Mods to Original File:
   ----------------------
   a) Added English & removed Japanese comments.
   
   Copyright Notice:
   -----------------
   Original copyright notice & disclaimer included below, thereby complying 
   with conditions 1 & 2 of the terms of use.
   
   ............................................................................
 	    ItIs - ITRON Implementation by Sakamura Lab
  
   Copyright (C) 1989-1996 by Sakamura Lab, the University of Tokyo, JAPAN
  
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
   3. Neither the name of the University nor the names of the laboratory
      may be used to endorse or promote products derived from this software
      without specific prior written permission.
  
   THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL THE UNIVERSITY OR THE LABORATORY BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
   ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*===========================================================================*/

#ifndef _WINFO_
#define _WINFO_

/*---------------------------------------------------------------------------*/
/* Wait information when wait due to 'wai_flg' or 'twai_flg'
  ('tskwait' is TTW_FLG)
*/
typedef struct 
	{
	UINT	waiptn;
	UINT	wfmode;
	UINT	*p_flgptn;
	} WINFO_FLG;

/*---------------------------------------------------------------------------*/
/* Wait information when wait due to 'rcv_msg' or 'trcv_msg'
  ('tskwait' is TTW_MBX)
*/
typedef struct 
	{
	T_MSG	**ppk_msg;
	} WINFO_MBX;

/*---------------------------------------------------------------------------*/
/* Wait information when wait due to 'rcv_mbf', 'trcv_mbf', 'snd_mbf' or
   'tsnd_mbf'.  ('tskwait' is TTW_MBF or TTW_SMBF )
*/
typedef struct 
	{
	VP		msg;
	INT		*p_msgsz;
	} WINFO_MBF;

typedef struct 
	{
	VP		msg;
	INT		msgsz;
	} WINFO_SMBF;

/*---------------------------------------------------------------------------*/
/* ??? - TTW_TMB
*/
typedef struct 
	{
	T_MSG	**ppk_msg;
	} WINFO_TMB;

/*---------------------------------------------------------------------------*/
/* Wait information when wait due to 'get_blk' or 'tget_blk'
  ('tskwait' is TTW_MPL)
  Note double indirection of 'p_blk', so '*p_blk' is the address of the 
  allocated memory block.
*/
typedef struct 
	{
	INT		blksz;					/* Size of required memory block	*/
	VP		*p_blk;					/* Allocated memory block address	*/
	} WINFO_MPL;

/*---------------------------------------------------------------------------*/
/* Wait information when wait due to 'get_blf' or 'tget_blf'
  ('tskwait' is TTW_MPF)
*/
typedef struct 
	{
	VP	*p_blf;
	} WINFO_MPF;

/*---------------------------------------------------------------------------*/
/* 'WINFO' provides data required for wait processing
*/
typedef union 
	{
	#ifdef USE_FLG
	WINFO_FLG	flg;
	#endif /* USE_FLG */
	#ifdef USE_MBX
	WINFO_MBX	mbx;
	#endif /* USE_MBX */
	#ifdef USE_MBF
	WINFO_MBF	mbf;
	WINFO_SMBF	smbf;
	#endif /* USE_MBF */
	#ifdef USE_TASK_MAILBOX
	WINFO_TMB	tmb;
	#endif /* USE_TASK_MAILBOX */
	#ifdef USE_MPL
	WINFO_MPL	mpl;
	#endif /* USE_MPL */
	#ifdef USE_MPF
	WINFO_MPF	mpf;
	#endif /* USE_MPF */
	} WINFO;

/*---------------------------------------------------------------------------*/
/* This structure is used for specifying call-back hooks that need to be
   called in the event that a waiting task is subject to a change in priority 
   or is released.
   'chg_pri_hook' is called in 'change_task_priority'.
   'rel_wai_hook' is called in 'wait_release_tmout', '_ter_tsk' & 'i_rel_wai'.
*/
typedef const struct 
	{
	UINT	tskwait;						/* Cause of wait 			*/
	void	(*chg_pri_hook)(TCB *,INT);		/* Change priority callback	*/
	void	(*rel_wai_hook)(TCB *);			/* Wait release callback	*/
	} WSPEC;

/*===========================================================================*/

#endif /* _WINFO_ */
