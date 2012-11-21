/* taskmbox.c
   Task-mailbox functions for uITRON OS.

   Mods to Original File:
   ----------------------
   a) Added English & removed Japanese comments.

   Notes:
   ------
   a) These task-mailboxes seem to be an extension to the uITRON
      specification, since no mention of task mailboxes occurs in the
	  specification.

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

#include "kernel.h"

#ifdef USE_TASK_MAILBOX

#include "task.h"
#include "wait.h"
#include "check.h"

/*---------------------------------------------------------------------------*/
/* Define to allow 'easy' access to the 'msgque' component of 'msg'.
*/
#define nextmsg(msg)	*((T_MSG **)&((msg)->msgque[0]))

/*---------------------------------------------------------------------------*/
/* Wait callback information for mailboxes.
*/
static WSPEC wspec_tmb_tfifo = { TTW_TMB, 0, 0 };

/*---------------------------------------------------------------------------*/
/* 
*/
#ifndef _i_vsnd_tmb

SYSCALL ER
i_vsnd_tmb(ID tskid,T_MSG *pk_msg)
	{
	TCB		*tcb;
	TSTAT	state;
	ER		ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_TSKACV(tskid);
	CHECK_NONSELF(tskid);
	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	if ( !task_alive(state = tcb->state) ) 
		{
		ercd = (state == TS_NONEXIST) ? E_NOEXS : E_OBJ;
		}
	else if ((state & TS_WAIT) && tcb->wspec == &wspec_tmb_tfifo) 
		{
		*(tcb->winfo.tmb.ppk_msg) = pk_msg;
		wait_release_ok(tcb);
		}
	else 
		{
		nextmsg(pk_msg) = (T_MSG *) 0;
		if (tcb->tmq_head) 
			{
			nextmsg(tcb->tmq_tail) = pk_msg;
			}
		else 
			{
			tcb->tmq_head = pk_msg;
			}
		tcb->tmq_tail = pk_msg;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_vsnd_tmb */

/*---------------------------------------------------------------------------*/
#if !defined(_i_vrcv_tmb) || !defined(_i_vprcv_tmb) || !defined(_i_vtrcv_tmb)

SYSCALL ER
i_vtrcv_tmb(T_MSG **ppk_msg, TMO tmout)
	{
	ER	ercd = E_OK;
    
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	BEGIN_CRITICAL_SECTION;
	if ( ctxtsk->tmq_head ) 
		{
		*ppk_msg = ctxtsk->tmq_head;
		ctxtsk->tmq_head = nextmsg(*ppk_msg);
		}
	else 
		{
		ercd = E_TMOUT;
		if ( tmout != TMO_POL ) 
			{
			ctxtsk->wspec = &wspec_tmb_tfifo;
			ctxtsk->wgcb = (GCB *) 0;
			ctxtsk->wercd = &ercd;
			ctxtsk->winfo.tmb.ppk_msg = ppk_msg;
			make_wait(tmout);
			queue_initialize(&(ctxtsk->tskque));
			}
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_vtrcv_tmb */

/*---------------------------------------------------------------------------*/
#ifndef _i_vrcv_tmb

SYSCALL ER
i_vrcv_tmb(T_MSG **ppk_msg)
	{
	return( i_vtrcv_tmb(ppk_msg,TMO_FEVR) );
	}

#endif /* _i_vrcv_tmb */

/*---------------------------------------------------------------------------*/
#ifndef _i_vprcv_tmb

SYSCALL ER
i_vprcv_tmb(T_MSG **ppk_msg)
	{
	return( i_vtrcv_tmb(ppk_msg, TMO_POL) );
	}

#endif /* _i_vprcv_tmb */

/*---------------------------------------------------------------------------*/
#ifndef _i_vref_tmb

SYSCALL ER
i_vref_tmb(T_RTMB *pk_rtmb,ID tskid)
	{
	TCB		*tcb;
	TSTAT	state;
	ER		ercd = E_OK;

	CHECK_TSKID_SELF(tskid);
	CHECK_TSKACV(tskid);
	tcb = get_tcb_self(tskid);

	BEGIN_CRITICAL_SECTION;
	if ( !task_alive(state = tcb->state) ) 
		{
		ercd = (state == TS_NONEXIST) ? E_NOEXS : E_OBJ;
		}
	else 
		{
		if ( (state & TS_WAIT) && tcb->wspec == &wspec_tmb_tfifo ) 
			{
			pk_rtmb->wtsk = tcb->tskid;
			}		
		else 
			{
			pk_rtmb->wtsk = 0;
			}
		if ( tcb->tmq_head ) 
			{
			pk_rtmb->pk_msg = tcb->tmq_head;
			}
		else 
			{
			pk_rtmb->pk_msg = (T_MSG *)NADR;
			}
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_vref_tmb */

/*===========================================================================*/

#endif /* USE_TASK_MAILBOX */
