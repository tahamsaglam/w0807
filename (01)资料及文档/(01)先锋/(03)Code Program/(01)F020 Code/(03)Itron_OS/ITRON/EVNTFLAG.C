/* evntflag.c
   Event flag functions for uITRON OS.

   Mods to Original File:
   ----------------------
   a) Added English & removed Japanese comments.

   Notes:
   ------
   a) Unlike message-buffers, where only a single task may be the 
      destination of a message transmitted by some source task, eventflags
	  have the feature whereby multiple tasks may wait on the same event.
	  This can be useful where several tasks require access to some sort
	  of data that is being output asynchronously by some other task.  In
	  this case, the source task sets an eventflag whenever new data is
	  available & the other tasks are then notified that new data is 
	  available.
   
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

#ifdef USE_FLG

#include "task.h"
#include "wait.h"
#include "check.h"

/* Structure for controlling eventflag operations.
*/
typedef struct eventflag_control_block 
	{
	QUEUE	wait_queue;				/* Eventflag wait queue				*/
	ID		flgid;					/* Eventflag ID				 		*/
	VP		exinf;					/* Extended information				*/
	ATR		flgatr;					/* Eventflag attributes				*/
	UINT	flgptn;					/* Eventflag bit pattern			*/
	} FLGCB;

/*---------------------------------------------------------------------------*/
/* Table of eventflag control blocks.
*/
static FLGCB	flgcb_table[NUM_FLGID];

/*---------------------------------------------------------------------------*/
/* Defines for easy indexing into the FLGCB table given the eventflag 'id'.
*/
#define get_flgcb(id)	(&(flgcb_table[INDEX_FLG(id)]))

/*---------------------------------------------------------------------------*/
/* 'free_flgcb' is a linked list of free FLGCB's ready for allocation.
*/   
#ifndef _i_vcre_flg
QUEUE	free_flgcb;
#endif /* _i_vcre_flg */

/*===========================================================================*/
/* Function to initialize eventflag control block table 'flgcb_table' 
*/
void
eventflag_initialize(void)
	{
	INT		i;
	FLGCB	*flgcb;
	ID		flgid;

	#ifndef _i_vcre_flg
	queue_initialize(&free_flgcb);
	#endif /* _i_vcre_flg */

	for ( flgcb = flgcb_table,i = 0; i < NUM_FLGID; flgcb++,i++) 
		{
		flgid = ID_FLG(i);
		flgcb->flgid = flgid;
		flgcb->flgatr = OBJ_NONEXIST;
		#ifndef _i_vcre_flg
		if ( !SYS_FLGID(flgid) ) 
			{
			queue_insert(&(flgcb->wait_queue), &free_flgcb);
			}
		#endif /* _i_vcre_flg */
		}
	}

/*---------------------------------------------------------------------------*/
/* This function checks if the eventflag pattern 'flgptn' contained within 
   the flag control block 'flgcb' matches the supplied pattern 'waiptn'.
   The type of comparison depends on the waitflag mode 'wfmode'.
*/
static __inline BOOL
eventflag_cond(FLGCB *flgcb,UINT waiptn,UINT wfmode)
	{
	if ( wfmode & TWF_ORW ) 
		{
		return( flgcb->flgptn & waiptn );
		}
	return( (flgcb->flgptn & waiptn)==waiptn );
	}

/*---------------------------------------------------------------------------*/
/* Wait callback information for eventflags.
*/
static WSPEC wspec_flg = { TTW_FLG, 0, 0 };

/*---------------------------------------------------------------------------*/
/* Initialize the eventflag control block 'flgcb', using the data contained 
   within 'pk_cflg'.
*/
#if !defined(_i_cre_flg) || !defined(_i_vcre_flg)

static void
_cre_flg(FLGCB *flgcb,T_CFLG *pk_cflg)
	{
	#ifndef _i_vcre_flg
	if ( !SYS_FLGID(flgcb->flgid) ) 
		{
		queue_delete(&(flgcb->wait_queue));
		}
	#endif /* _i_vcre_flg */

	queue_initialize( &(flgcb->wait_queue) );
	flgcb->exinf = pk_cflg->exinf;
	flgcb->flgatr = pk_cflg->flgatr;
	flgcb->flgptn = pk_cflg->iflgptn;
	}

#endif /* !defined(_i_cre_flg) || !defined(_i_vcre_flg) */

/*---------------------------------------------------------------------------*/
/* This system call creates the eventflag specified by 'flgid'.  Specifically, 
   a control block for the eventflag to be created is allocated and the 
   associated flag pattern is initialized using 'iflgptn'.  A single eventflag
   handles one word's worth of bits of the processor in question as a group.
   All operations are done in single word units.
*/
#ifndef _i_cre_flg

SYSCALL ER
i_cre_flg(ID flgid,T_CFLG *pk_cflg)
	{
	FLGCB	*flgcb;
	ER		ercd = E_OK;

	CHECK_FLGID(flgid);
	CHECK_FLGACV(flgid);
	CHECK_RSATR(pk_cflg->flgatr, TA_WMUL);
	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgatr!=OBJ_NONEXIST ) 
		{
		ercd =  E_OBJ;
		}
	else 
		{
		_cre_flg(flgcb, pk_cflg);
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_cre_flg */

/*---------------------------------------------------------------------------*/
/* 'vcre_flg' creates an eventflag, without having to specify an eventflag
   ID.  Specifically, a control block is allocated for the eventflag and the
   eventflag ID 'flgid' is returned.
*/
#ifndef _i_vcre_flg

SYSCALL ER
i_vcre_flg(T_CFLG *pk_cflg)
	{
	FLGCB		*flgcb;
	ER			ercd = E_OK;

	CHECK_RSATR(pk_cflg->flgatr,TA_WMUL);

	BEGIN_CRITICAL_SECTION;
	if ( queue_empty_p(&free_flgcb) ) 
		{
		ercd =  EV_FULL;
		}
	else 
		{
		flgcb = (FLGCB *)(free_flgcb.next);
		_cre_flg(flgcb, pk_cflg);
		ercd = (ER)(flgcb->flgid);
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_vcre_flg */

/*---------------------------------------------------------------------------*/
/* This system call deletes the eventflag specified by 'flgid'.
   Issuing this system call causes memory used for the control block of the
   associated eventflag to be released.  After this system call is invoked,
   another eventflag having the same ID number can be created.
   
   This system call will complete normally even if there are tasks waiting 
   for the eventflag.  In that case, an E_DLT error will be returned to each
   waiting task.
   
   When an eventflag being waited for by more than one tasks is deleted, the 
   order of tasks on the ready queue after the WAIT state is cleared is 
   implementation dependent in the case of tasks having the same priority.
*/
#ifndef _i_del_flg

SYSCALL ER
i_del_flg(ID flgid)
	{
	FLGCB	*flgcb;
	ER		ercd = E_OK;

	CHECK_FLGID(flgid);
	CHECK_FLGACV(flgid);
	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgatr==OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		}
	else 
		{
		wait_delete(&(flgcb->wait_queue));
		#ifndef _i_vcre_flg
		if ( !SYS_FLGID(flgcb->flgid) ) 
			{
			queue_insert(&(flgcb->wait_queue),&free_flgcb);
			}
		#endif /* _i_vcre_flg */
		flgcb->flgatr = OBJ_NONEXIST;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_del_flg */

/*---------------------------------------------------------------------------*/
/* The 'set_flg' system call sets the bits specified by setptn of the one 
   word eventflag specified by 'flgid'.  In other words, a logical sum is 
   taken for the values of the eventflag specified by 'flgid' with the 
   value of 'setptn'.  Nothing will happen to the target eventflag if all 
   bits of 'setptn' are specified as 0 with 'set_flg'.
   
   If the eventflag value is changed by set_flg and the new eventflag 
   value satisfies the condition to release the WAIT state of the task 
   which issued 'wai_flg' on the eventflag, the WAIT state of that task will 
   be released and the task will be put into RUN or READY state (or SUSPEND 
   state if the task was in WAIT-SUSPEND).
   
   Multiple tasks can wait for a single eventflag if that eventflags has the
   TA_WMUL attribute.  This means that even eventflags can make queues for 
   tasks to wait on.  When such eventflags are used, a single 'set_flg' call 
   may result in the release of multiple waiting tasks.  In this case, the 
   order of tasks on the ready queue after the WAIT state is cleared is 
   preserved for tasks having the same priority.  The feature represented 
   by TA_WMUL, whereby multiple tasks may wait for a single eventflag, is 
   considered an extended function [level X] for which compatibility and 
   connectivity are not guaranteed.
*/
#ifndef _i_set_flg

SYSCALL ER
i_set_flg(ID flgid,UINT setptn)
	{
	FLGCB	*flgcb;
	TCB		*tcb;
	QUEUE	*queue;
	UINT	wfmode;
	ER		ercd = E_OK;

	CHECK_FLGID(flgid);
	CHECK_FLGACV(flgid);
	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgatr==OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		goto error_exit;
		}
	flgcb->flgptn |= setptn;
    
	queue = flgcb->wait_queue.next;
	while ( queue != &(flgcb->wait_queue) ) 
		{
		tcb = (TCB *)queue;
		queue = queue->next;
		wfmode = tcb->winfo.flg.wfmode;
		if ( eventflag_cond(flgcb,tcb->winfo.flg.waiptn,wfmode) ) 
			{
			*(tcb->winfo.flg.p_flgptn) = flgcb->flgptn;
			wait_release_ok(tcb);
			if ( wfmode & TWF_CLR ) 
				{
				flgcb->flgptn = 0;
				break;
				}
			}
		}
    error_exit:
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_set_flg */

/*---------------------------------------------------------------------------*/
/* This system call is similar to 'set_flg', except that the 'clr_flg' 
   system call clears the bits of the one word eventflag based on the
   corresponding zero bits of 'clrptn'.  In other words, a logical product 
   is taken for the values of the eventflag specified by 'flgid' with the 
   value of 'clrptn'.
   Issuing clr_flg never results in wait conditions being released on a 
   task waiting for the specified eventflag.  In other words, dispatching 
   never occurs with 'clr_flg'.  Nothing will happen to the target eventflag 
   if all bits of clrptn are specified as 1 with 'clr_flg', although no 
   error will result either.
*/
#ifndef _i_clr_flg

SYSCALL ER
i_clr_flg(ID flgid,UINT clrptn)
	{
	FLGCB	*flgcb;
	ER		ercd = E_OK;

	CHECK_FLGID(flgid);
	CHECK_FLGACV(flgid);
	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgatr==OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		}
	else 
		{
		flgcb->flgptn &= clrptn; 
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_clr_flg */

/*---------------------------------------------------------------------------*/
/* The 'wai_flg' system call waits for the eventflag specified by 'flgid' 
   to be set to satisfy the wait release condition specified by 'wfmode'.
   If the eventflag specified by flgid already satisfies the wait release
   conditions given by 'wfmode', the issuing task will continue execution 
   without waiting.
   
   If TWF_ORW is specified, the issuing task will wait for any of the bits
   specified by 'waiptn' to be set for the eventflag given by 'flgid' 
   (OR wait).  If TWF_ANDW is specified, the issuing task will wait for 
   all of the bits specified by 'waiptn' to be set for the eventflag given 
   by 'flgid' (AND wait).
   If the TWF_CLR specification is not present, the eventflag value will 
   remain unchanged even after the wait conditions have been satisfied and 
   the task has been released from the WAIT state.  If TWF_CLR is specified, 
   all bits of the eventflag will be cleared to 0 once the wait conditions 
   of the waiting task have been satisfied.
   The return parameter flgptn returns the value of the eventflag after the 
   wait state of a task has been released due to this system call.  If 
   TWF_CLR was specified, the value before eventflag bits were cleared is 
   returned.  The value returned by 'flgptn' fulfills the wait release 
   conditions of this system call.
*/
#ifndef _i_wai_flg

SYSCALL ER
i_wai_flg(UINT* p_flgptn,ID flgid,UINT waiptn,UINT wfmode)
	{
	return( i_twai_flg(p_flgptn,flgid,waiptn,wfmode,TMO_FEVR) );
	}

#endif /* _i_wai_flg */

/*---------------------------------------------------------------------------*/
/* The 'pol_flg' system call has the same function as 'wai_flg' except for 
   the waiting feature.  'Pol_flg' polls whether or not the task should 
   wait if 'wai_flg' is executed
*/
#ifndef _i_pol_flg

SYSCALL ER
i_pol_flg(UINT* p_flgptn,ID flgid,UINT waiptn,UINT wfmode)
	{
	return(i_twai_flg(p_flgptn,flgid,waiptn,wfmode,TMO_POL) );
	}

#endif /* _i_pol_flg */

/*---------------------------------------------------------------------------*/
#if !defined(_i_wai_flg) || !defined(_i_pol_flg) || !defined(_i_twai_flg)

SYSCALL ER
i_twai_flg(UINT* p_flgptn,ID flgid,UINT waiptn,UINT wfmode,TMO tmout)
	{
	FLGCB	*flgcb;
	ER		ercd = E_OK;

	CHECK_FLGID(flgid);
	CHECK_FLGACV(flgid);
	CHECK_PAR(waiptn != 0);
	CHECK_PAR((wfmode & ~(TWF_ORW|TWF_CLR)) == 0);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();
	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgatr==OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		}
	else if ( !(flgcb->flgatr & TA_WMUL ) && !queue_empty_p(&(flgcb->wait_queue)) )
		{
		ercd = E_OBJ;
		}
	else if ( eventflag_cond(flgcb,waiptn,wfmode) ) 
		{
		*p_flgptn = flgcb->flgptn;
		if ( wfmode & TWF_CLR ) 
			{
			flgcb->flgptn = 0;
			}
		}
	else 
		{
		ercd = E_TMOUT;
		if ( tmout != TMO_POL ) 
			{
			ctxtsk->wspec = &wspec_flg;
			ctxtsk->wercd = &ercd;
			ctxtsk->winfo.flg.waiptn = waiptn;
			ctxtsk->winfo.flg.wfmode = wfmode;
			ctxtsk->winfo.flg.p_flgptn = p_flgptn;
			gcb_make_wait((GCB *) flgcb,tmout);
			}
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_twai_flg */

/*---------------------------------------------------------------------------*/
/* 'ref_flg' returns various parameters of interest with regard to the state 
   of the eventflag specified by 'flgid', returning current flag pattern 
   (flgptn), waiting task information (wtsk) and extended information (exinf)
   via 'pk_rflg'.
*/
#ifndef _i_ref_flg

SYSCALL ER
i_ref_flg(T_RFLG *pk_rflg,ID flgid)
	{
	FLGCB	*flgcb;
	ER		ercd = E_OK;

	CHECK_FLGID(flgid);
	CHECK_FLGACV(flgid);
	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgatr == OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		}
	else 
		{
		pk_rflg->exinf = flgcb->exinf;
		pk_rflg->wtsk = wait_tskid(&(flgcb->wait_queue));
		pk_rflg->flgptn = flgcb->flgptn;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_ref_flg */

/*---------------------------------------------------------------------------*/

#endif /* USE_FLG */
