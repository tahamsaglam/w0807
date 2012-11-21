/* task_syn.c
   Task functions for uITRON OS.

   Mods to Original File:
   ----------------------
   a) Added English & removed Japanese comments.

   Notes:
   -----
   a) This module contains implementations of the following system calls:
      sus_tsk, rsm_tsk, frsm_tsk, slp_tsk, tslp_tsk, can_wup
	  
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

#include <limits.h>
#include "kernel.h"
#include "task.h"
#include "wait.h"
#include "check.h"

/*---------------------------------------------------------------------------*/
/* This system call suspends the execution of the task specified by 'tskid' 
   by putting it into SUSPEND state. SUSPEND state is released by issuing 
   the rsm_tsk or frsm_tsk system call.
*/
#ifndef _i_sus_tsk

SYSCALL ER
i_sus_tsk(ID tskid)
	{
	TCB		*tcb;
	TSTAT	state;
	ER		ercd = E_OK;

	#ifndef USE_SUSPEND_SELF
	CHECK_TSKID(tskid);
	CHECK_TSKACV(tskid);
	CHECK_NONSELF(tskid);
	tcb = get_tcb(tskid);
	#else /* USE_SUSPEND_SELF */
	CHECK_TSKID_SELF(tskid);
	CHECK_TSKACV(tskid);
	tcb = get_tcb_self(tskid);
	#endif /* USE_SUSPEND_SELF */

	BEGIN_CRITICAL_SECTION;
	if ( !task_alive(state = tcb->state) ) 
		{
		ercd = (state == TS_NONEXIST) ? E_NOEXS : E_OBJ;
		}
	else if ( tcb->suscnt == INT_MAX ) 
		{
		ercd = E_QOVR;
		}
	else 
		{
		++(tcb->suscnt);
		if ( state == TS_READY ) 
			{
			make_non_ready(tcb);
			tcb->state = TS_SUSPEND;
			}
		else if (state == TS_WAIT) 
			{
			tcb->state = TS_WAITSUS;
			}
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_sus_tsk */

/*---------------------------------------------------------------------------*/
/* Both these system calls release SUSPEND state of the task specified by
   'tskid'.  Specifically, they cause SUSPEND state to be released and the
   execution of the specified task to resume when the task has been suspended
   by the prior execution of 'sus_tsk'.
*/
#ifndef _i_rsm_tsk

SYSCALL ER
i_rsm_tsk(ID tskid)
	{
	TCB		*tcb;
	ER		ercd = E_OK;

	#ifndef USE_SUSPEND_SELF
	CHECK_TSKID(tskid);
	CHECK_TSKACV(tskid);
	CHECK_NONSELF(tskid);
	tcb = get_tcb(tskid);
	#else /* USE_SUSPEND_SELF */
	CHECK_TSKID_SELF(tskid);
	CHECK_TSKACV(tskid);
	tcb = get_tcb_self(tskid);
	#endif /* USE_SUSPEND_SELF */

	BEGIN_CRITICAL_SECTION;
	switch (tcb->state) 
		{
		case TS_NONEXIST:
			ercd = E_NOEXS;
			break;
		case TS_DORMANT:
		case TS_READY:
		case TS_WAIT:
			ercd = E_OBJ;
			break;
		case TS_SUSPEND:
			if (--(tcb->suscnt) == 0) 
				{
				make_ready(tcb);
				}
			break;
		case TS_WAITSUS:
			if ( --(tcb->suscnt) == 0 ) 
				{
				tcb->state = TS_WAIT;
				}
			break;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_rsm_tsk */

/*---------------------------------------------------------------------------*/
/* See comments for 'rsm_tsk'.
*/
#ifndef _i_frsm_tsk

SYSCALL ER
i_frsm_tsk(ID tskid)
	{
	TCB	*tcb;
	ER	ercd = E_OK;

	#ifndef USE_SUSPEND_SELF
	CHECK_TSKID(tskid);
	CHECK_TSKACV(tskid);
	CHECK_NONSELF(tskid);
	tcb = get_tcb(tskid);
	#else /* USE_SUSPEND_SELF */
	CHECK_TSKID_SELF(tskid);
	CHECK_TSKACV(tskid);
	tcb = get_tcb_self(tskid);
	#endif /* USE_SUSPEND_SELF */

	BEGIN_CRITICAL_SECTION;
	switch (tcb->state) 
		{
		case TS_NONEXIST:
			ercd = E_NOEXS;
			break;
		case TS_DORMANT:
		case TS_READY:
		case TS_WAIT:
			ercd = E_OBJ;
			break;
		case TS_SUSPEND:
			tcb->suscnt = 0;
			make_ready(tcb);
			break;
		case TS_WAITSUS:
			tcb->suscnt = 0;
			tcb->state = TS_WAIT;
			break;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_frsm_tsk */

/*---------------------------------------------------------------------------*/
/* Wait callback information for sleeping tasks
*/
#if !defined(_i_slp_tsk) || !defined(_i_tslp_tsk) || !defined(_i_wup_tsk)
static WSPEC wspec_slp = { TTW_SLP, 0, 0 };
#endif

/*---------------------------------------------------------------------------*/
/* Cause the issuing task (which is in RUN state) to sleep until 'wup_tsk'
   is invoked.  The 'tslp_tsk' system call is the same as 'slp_tsk', but with 
   an additional timeout feature.  If a 'wup_tsk' is issued before the period
   of time specified by 'tmout'	elapses, 'tslp_tsk' will complete normally.
   An E_TMOUT error will result if no 'wup_tsk' is issued before the time 
   specified by 'tmout' expires.
*/
#if !defined(_i_slp_tsk) || !defined(_i_tslp_tsk)

SYSCALL ER
i_tslp_tsk(TMO tmout)
	{
	ER	ercd = E_OK;

	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	BEGIN_CRITICAL_SECTION;
	if ( ctxtsk->wupcnt > 0 ) 
		{
		(ctxtsk->wupcnt)--;
		}
	else 
		{
		ercd = E_TMOUT;
		if (tmout != TMO_POL) 
			{
			ctxtsk->wspec = &wspec_slp;
			ctxtsk->wgcb = (GCB *) 0;
			ctxtsk->wercd = &ercd;
			make_wait(tmout);
			queue_initialize(&(ctxtsk->tskque));
			}
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_tslp_tsk */

/*---------------------------------------------------------------------------*/
/* Cause the issuing task (which is in RUN state) to sleep until 'wup_tsk'
   is invoked.
*/
#ifndef _i_slp_tsk

SYSCALL ER
i_slp_tsk(void)
	{
	return(i_tslp_tsk(TMO_FEVR));
	}

#endif /* _i_slp_tsk */

/*---------------------------------------------------------------------------*/
/* 'wup_tsk' system call releases the WAIT state of the task specified by 
   'tskid' caused by the execution of 'slp_tsk' or 'tslp_tsk'.

	Wakeup requests are queued as follows.  A wakeup request queuing count 
	'wupcnt' is kept in the TCB for each task.  Initially, when 'sta_tsk' 
	is executed, the value of 'wupcnt' is 0.  Executing 'wup_tsk' on a 
	task which is not waiting for a	wakeup increments the wakeup request 
	queuing	count by one for the specified task.  If 'slp_tsk' or 'tslp_tsk'
	is executed on that task, its wakeup request queuing count will be
	decremented by one.  If the task with wakeup request queuing count = 0
	executes 'slp_tsk' or 'tslp_tsk', that task will be put in WAIT state 
	rather than decrementing the wakeup request queuing count.
*/
#ifndef _i_wup_tsk

SYSCALL ER
i_wup_tsk(ID tskid)
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
	else if ((state & TS_WAIT) && tcb->wspec == &wspec_slp) 
		{
		wait_release_ok(tcb);
		}
	else if ( tcb->wupcnt == INT_MAX ) 
		{
		ercd = E_QOVR;
		}
	else 
		{
		++(tcb->wupcnt);
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_wup_tsk */

/*---------------------------------------------------------------------------*/
/* 'can_wup' returns the wakeup request queuing count 'wupcnt' for the
   task specified by 'tskid', while canceling all associated wakeup requests.
   Specifically, it resets the wakeup request queuing count 'wupcnt' to 0.
*/
#ifndef _i_can_wup

SYSCALL ER
i_can_wup(W *p_wupcnt, ID tskid)
	{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);
	CHECK_TSKACV(tskid);
	tcb = get_tcb_self(tskid);

	BEGIN_CRITICAL_SECTION;
	switch (tcb->state) 
		{
		case TS_NONEXIST:
			ercd = E_NOEXS;
			break;
		case TS_DORMANT:
			ercd = E_OBJ;
			break;
		default:
			*p_wupcnt = tcb->wupcnt;
			tcb->wupcnt = 0;
			break;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_can_wup */

/*---------------------------------------------------------------------------*/
