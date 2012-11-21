/* prisem.c
   Priority inheritance semaphore functions for uITRON OS.

   Mods to Original File:
   ----------------------
   a) Added English & removed Japanese comments.

   Notes:
   -----
   a) This module implements non-specified priority inheritance semaphore
      functions in the uITRON specification.  This code is somewhat
	  different to the code contained in 'semaphor.c', which implements
	  ordinary semaphores.  Priority inheritance semaphores differ from
	  ordinary semaphores in that the priority of a task owning a
	  PI semaphore is increased when that task blocks a high priority
	  task.  This task is returned to its original priority when the 
	  semaphore is released.

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

#ifdef USE_PIS

#include "task.h"
#include "wait.h"
#include "check.h"

/* Structure for controlling priority inheritance semaphore operations.
*/
struct prisem_control_block 
	{
	QUEUE	wait_queue;				/* Priority-I semaphore  wait queue	*/
	ID		pisid;					/* Priority semaphore ID			*/
	VP		exinf;					/* Extended information				*/
	ATR		pisatr;					/* Priority semaphore attributes	*/
	TCB		*pistsk;				/* TCB of task owning the semaphore	*/
	#ifdef PRISEM_SPEC1
	PISCB	*pislist;				/* ??? -							*/
	#endif /* PRISEM_SPEC1 */
	};

/*---------------------------------------------------------------------------*/
/* Table of priority inheritance semaphore control blocks.
*/
static PISCB	piscb_table[NUM_PISID];

/*---------------------------------------------------------------------------*/
/* Defines for easy indexing into the PISCB table given the priority 
   inheritance semaphore 'id', as well as defines to simplify priority-
   inheritance-semaphore common operations.
*/
#define get_piscb(id)	(&(piscb_table[INDEX_PIS(id)]))

#ifdef PRISEM_SPEC1
#define pis_waited(piscb)	( !queue_empty_p(&(piscb->wait_queue)) )
#define	pis_head_pri(piscb)	( ((TCB *)(piscb->wait_queue.next))->priority )
#endif /* PRISEM_SPEC1 */

/*---------------------------------------------------------------------------*/
/* 'free_piscb' is a linked list of free PISCB's ready for allocation.
*/
#ifndef _i_vvcre_pis
QUEUE	free_piscb;
#endif /* _i_vvcre_pis */

/*===========================================================================*/
/* Function to initialize priority semaphore control block table 'piscb_table' 
*/
void
prisem_initialize(void)
	{
	INT		i;
	PISCB	*piscb;
	ID		pisid;

	#ifndef _i_vvcre_pis
	queue_initialize(&free_piscb);
	#endif /* _i_vvcre_pis */

	for ( piscb=piscb_table,i = 0; i<NUM_PISID; piscb++,i++ ) 
		{
		pisid = ID_PIS(i);
		piscb->pisid = pisid;
		piscb->pisatr = OBJ_NONEXIST;
		#ifndef _i_vvcre_pis
		if ( !SYS_PISID(pisid) ) 
			{
			queue_insert(&(piscb->wait_queue),&free_piscb);
			}
		#endif /* _i_vvcre_pis */
		}
	}

/*---------------------------------------------------------------------------*/
#ifdef PRISEM_SPEC1

/* ???
*/
reset_priority(TCB *tcb)
	{
	INT		newpri = tcb->ipriority;
	PISCB	*piscb;

	for ( piscb=tcb->pislist; piscb; piscb=piscb->pislist ) 
		{
		if ( pis_waited(piscb) && newpri > pis_head_pri(piscb) )
			{
			newpri = pis_head_pri(piscb);
			}
		}
	if ( newpri != tcb->priority ) 
		{
		change_task_priority(tcb,newpri);
		}
	}

/*---------------------------------------------------------------------------*/
/* ???
*/
static void
release_prisem(TCB *tcb,PISCB *relpiscb)
	{
	PISCB	*piscb, **last_piscb;

	last_piscb = &tcb->pislist;
	if ( pis_waited(relpiscb) && tcb->priority == pis_head_pri(relpiscb)) 
		{
		/* ???
		*/
		INT	newpri = tcb->ipriority;

		while ( piscb = *last_piscb ) 
			{
			if ( piscb == relpiscb ) 
				{
				*last_piscb = piscb->pislist;
				break;
				}
			if ( pis_waited(piscb) && newpri > pis_head_pri(piscb) ) 
				{
				newpri = pis_head_pri(piscb);
				}
			last_piscb = &piscb->pislist;
			}
		assert(piscb);

		while (	piscb = piscb->pislist	) 
			{
			if ( pis_waited(piscb) && newpri > pis_head_pri(piscb) ) 
				{
				newpri = pis_head_pri(piscb);
				}
			}
		if ( tcb->priority < newpri	) 
			{
			change_task_priority(tcb, newpri);
			}
		}
	else 
		{
		/* ???
		*/
		while ( piscb = *last_piscb ) 
			{
			if ( piscb == relpiscb ) 
				{
				*last_piscb = piscb->pislist;
				break;
				}
			last_piscb = &piscb->pislist;
			}
		assert(piscb);
		}
	}

/*---------------------------------------------------------------------------*/
/* ???
*/
void
signal_all_prisem(TCB *tcb)
	{
	PISCB	*piscb, *next_piscb;
	TCB		*next_tcb;

	next_piscb = tcb->pislist;
	while ( piscb = next_piscb ) 
		{
		next_piscb = piscb->pislist;
		if ( !queue_empty_p(&(piscb->wait_queue)) ) 
			{
			next_tcb = (TCB *)(piscb->wait_queue.next);
			wait_release_ok(next_tcb);
			piscb->pistsk = next_tcb;
			piscb->pislist = next_tcb->pislist;
			next_tcb->pislist = piscb;
			}
		else 
			{
			piscb->pistsk = (TCB *) 0;
			}
		}
	}

#endif /* PRISEM_SPEC1 */

/*---------------------------------------------------------------------------*/
/* ???
*/
void
pis_chg_pri(TCB *tcb,INT oldpri)
	{
	PISCB	*piscb;
	TCB		*pistsk;

	piscb = (PISCB *) tcb->wgcb;
	gcb_change_priority((GCB *)piscb,tcb);

	pistsk = piscb->pistsk;
	assert(pistsk);
	if ( pistsk->priority > tcb->priority ) 
		{
		change_task_priority(pistsk,tcb->priority);
		}
	#ifdef PRISEM_SPEC1
	else if ( pistsk->priority == oldpri ) 
		{
		reset_priority(pistsk);
		}
	#endif /* PRISEM_SPEC1 */
	}

/*---------------------------------------------------------------------------*/
/* 'pis_rel_wai' is the callback function triggered when a timeout on a 
   priority inheritance semaphore occurs and 'PRISEM_SPEC1' has been selected.
   When this is called, the priority of the task currently holding the 
   semphore is reduced to a previous level.
*/
#ifdef PRISEM_SPEC1

void
pis_rel_wai(TCB *tcb)
	{
	PISCB	*piscb;
	TCB		*pistsk;

	piscb = (PISCB *) tcb->wgcb;
	pistsk = piscb->pistsk;
	assert(pistsk);
	if ( pistsk->priority == tcb->priority	) 
		{
		reset_priority(pistsk);
		}
	}

#endif /* PRISEM_SPEC1 */

/*---------------------------------------------------------------------------*/
/* Wait callback information for priority inheritance semaphores.
*/
#ifdef PRISEM_SPEC1
static WSPEC wspec_pis = { TTW_PIS, pis_chg_pri, pis_rel_wai };
#else /* PRISEM_SPEC1 */
static WSPEC wspec_pis = { TTW_PIS, pis_chg_pri, 0 };
#endif /* PRISEM_SPEC1 */

/*---------------------------------------------------------------------------*/
/* Function to initialize the priority inheritance semaphore control block 
   'piscb' given the packet of priority semaphore semaphore initialization 
   data 'pk_cpis'.
   The code is similar to '_cre_sem'.
*/
#if !defined(_i_vcre_pis) || !defined(_i_vvcre_pis)

static void
_vcre_pis(PISCB *piscb,T_CPIS *pk_cpis)
	{
	#ifndef _i_vvcre_pis
	if ( !SYS_PISID(piscb->pisid) )
		{
		queue_delete(&(piscb->wait_queue));
		}
	#endif /* _i_vvcre_pis */

	queue_initialize(&(piscb->wait_queue));
	piscb->exinf = pk_cpis->exinf;
	piscb->pisatr = (pk_cpis->pisatr | TA_TPRI);
	piscb->pistsk = (TCB *) 0;
	}

#endif /* !defined(_i_vcre_pis) || !defined(_i_vvcre_pis) */

/*---------------------------------------------------------------------------*/
/* This system call creates the priority inheritance semaphore whose ID is 
   specified by 'pisid'.  Specifically, a control block for the priority
   inheritance semaphore to be created is allocated and initialized with
   the data contained within 'pk_cpis'.  
   The code is similar to 'i_cre_sem'.
*/
#ifndef _i_vcre_pis

SYSCALL ER
i_vcre_pis(ID pisid,T_CPIS *pk_cpis)
	{
	PISCB	*piscb;
	ER		ercd = E_OK;

	CHECK_PISID(pisid);
	CHECK_PISACV(pisid);
	CHECK_RSATR(pk_cpis->pisatr, TA_TPRI);
	piscb = get_piscb(pisid);

	BEGIN_CRITICAL_SECTION;
	if ( piscb->pisatr != OBJ_NONEXIST ) 
		{
		ercd =  E_OBJ;
		}
	else 
		{
		_vcre_pis(piscb,pk_cpis);
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_vcre_pis */

/*---------------------------------------------------------------------------*/
/* This system call creates the priority inheritance semaphore.  Specifically,
   a control block for the priority inheritance semaphore to be created is 
   allocated and initialized with the data contained within 'pk_cpis'.  
   The code is similar to 'i_cre_sem'.
   An ID need not be specied with this variant of the system call.
   The code is similar to 'i_vcre_sem'.
*/
#ifndef _i_vvcre_pis

SYSCALL ER
i_vvcre_pis(T_CPIS *pk_cpis)
	{
	PISCB	*piscb;
	ER		ercd = E_OK;

	CHECK_RSATR(pk_cpis->pisatr,TA_TPRI);

	BEGIN_CRITICAL_SECTION;
	if ( queue_empty_p(&free_piscb) ) 
		{
		ercd =  EV_FULL;
		}
	else 
		{
		piscb = (PISCB *)(free_piscb.next);
		_vcre_pis(piscb, pk_cpis);
		ercd = (ER)(piscb->pisid);
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_vvcre_pis */

/*---------------------------------------------------------------------------*/
/* This system call deletes the priority inheritance semaphore specified by 
   'pisid'.
   The code is similar to 'i_del_sem'.
*/	
#ifndef _i_vdel_pis

SYSCALL ER
i_vdel_pis(ID pisid)
	{
	PISCB	*piscb;
	ER		ercd = E_OK;
    
	CHECK_PISID(pisid);
	CHECK_PISACV(pisid);
	piscb = get_piscb(pisid);

	BEGIN_CRITICAL_SECTION;
	if ( piscb->pisatr == OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		}
	else 
		{
		#ifdef PRISEM_SPEC1
		/* This section is the portion of the code that differs from
		   the standard 'semaphore' processing (as found in 'i_del_sem').
		*/
		if ( piscb->pistsk ) 
			{
			release_prisem(piscb->pistsk,piscb);
			}
		#endif /* PRISEM_SPEC1 */

		wait_delete(&(piscb->wait_queue));
		#ifndef _i_vvcre_pis
		if ( !SYS_PISID(piscb->pisid) ) 
			{
			queue_insert(&(piscb->wait_queue),&free_piscb);
			}
		#endif /* _i_vvcre_pis */
		piscb->pisatr = OBJ_NONEXIST;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_vdel_pis */

/*---------------------------------------------------------------------------*/
/* This system call returns the priority inheritance semaphore specified 
   by 'pisid'.  If the priority of the current task was raised to allow rapid
   release of the semaphore, then the current task priority is reduced back
   to its initial priority.  Then, if there are any tasks waiting for the 
   specified semaphore, the task at the head of the queue changes to the 
   RUN or READY state and the semaphore is assigned to this new task.  If no
   other tasks require the semaphore, then the 'pistsk' pointer is cleared,
   indicating availability of the semaphore.
*/
#ifndef _i_vsig_pis

SYSCALL ER
i_vsig_pis(ID pisid)
	{
	PISCB	*piscb;	
	TCB		*tcb;
	ER		ercd = E_OK;
    
	CHECK_PISID(pisid);
	CHECK_PISACV(pisid);
	CHECK_INTSK();
	piscb = get_piscb(pisid);

	BEGIN_CRITICAL_SECTION;
	if ( piscb->pisatr == OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		goto error_exit;
		}
	if ( piscb->pistsk != ctxtsk ) 
		{
		ercd = E_OBJ;
		goto error_exit;
		}

	#ifdef PRISEM_SPEC1
	release_prisem(ctxtsk,piscb);
	#else /* PRISEM_SPEC1 */
	if ( ctxtsk->priority < ctxtsk->ipriority ) 
		{
		change_task_priority(ctxtsk,ctxtsk->ipriority);
		}
	#endif /* PRISEM_SPEC1 */
	if ( !queue_empty_p(&(piscb->wait_queue)) )
		{
		tcb = (TCB *)(piscb->wait_queue.next);
		wait_release_ok(tcb);
		piscb->pistsk = tcb;

		#ifdef PRISEM_SPEC1
		piscb->pislist = tcb->pislist;
		tcb->pislist = piscb;
		#endif /* PRISEM_SPEC1 */
		}
	else 
		{
		piscb->pistsk = (TCB *) 0;
		}

    error_exit:
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_vsig_pis */

/*---------------------------------------------------------------------------*/
/* The 'vtwai_sem' system call has the same function as 'vwai_sem' with an 
   additional timeout feature.  A maximum wait time (timeout value) can be 
   specified using the parameter 'tmout'.  When a timeout is specified, a 
   timeout error, E_TMOUT, will result and the system call will finish if 
   the period specified by 'tmout' elapses without conditions for releasing 
   wait being satisfied (i.e. without sig_sem being executed).
*/
#if !defined(_i_vwai_pis) || !defined(_i_vpreq_pis) || !defined(_i_vtwai_pis)

SYSCALL ER
i_vtwai_pis(ID pisid,TMO tmout)
	{
	PISCB	*piscb;
	TCB		*pistsk;
	ER		ercd = E_OK;

	CHECK_PISID(pisid);
	CHECK_PISACV(pisid);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();
	piscb = get_piscb(pisid);

	BEGIN_CRITICAL_SECTION;
	if ( piscb->pisatr == OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		}
	else if ( (pistsk = piscb->pistsk)==0 )
		{
		piscb->pistsk = ctxtsk;
		#ifdef PRISEM_SPEC1
		piscb->pislist = ctxtsk->pislist;
		ctxtsk->pislist = piscb;
		#endif /* PRISEM_SPEC1 */
		}
	else 
		{
		ercd = E_TMOUT;
		if ( tmout != TMO_POL ) 
			{
			if ( pistsk->priority > ctxtsk->priority ) 
				{
				change_task_priority(pistsk,ctxtsk->priority);
				}
			ctxtsk->wspec = &wspec_pis;
			ctxtsk->wercd = &ercd;
			gcb_make_wait((GCB *)piscb,tmout);
			}
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_vtwai_pis */

/*---------------------------------------------------------------------------*/
/* The 'vwai_sem' system call obtains the priority inheritance semaphore 
   specified by 'pisid'.  If the semaphore is successfully obtained, the 
   task issuing this system call is not put in WAIT state, but continues 
   to execute.  If, on the other hand, the semaphore is currently owned by 
   another task, the task issuing this system call will be placed in the 
   become WAIT state and be put on a queue associated with the specified 
   semaphore.  The priority of the task that currenly owns the semaphore 
   is raised to the same priority as the current task thereby providing 
   processing time to ensure quick release of the semaphore. That task is 
   returned to its initial priority when the semaphore is released.
*/
#ifndef _i_vwai_pis

SYSCALL ER
i_vwai_pis(ID pisid)
	{
	return(i_vtwai_pis(pisid,TMO_FEVR));
	}

#endif /* _i_vwai_pis */

/*---------------------------------------------------------------------------*/
#ifndef _i_vpreq_pis

SYSCALL ER
i_vpreq_pis(ID pisid)
	{
	return(i_vtwai_pis(pisid,TMO_POL));
	}

#endif /* _i_vpreq_pis */

/*---------------------------------------------------------------------------*/
#ifndef _i_vref_pis

SYSCALL ER
i_vref_pis(T_RPIS *pk_rpis,ID pisid)
	{
	PISCB	*piscb;
	ER		ercd = E_OK;

	CHECK_PISID(pisid);
	CHECK_PISACV(pisid);
	piscb = get_piscb(pisid);

	BEGIN_CRITICAL_SECTION;
	if ( piscb->pisatr == OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		}
	else 
		{
		pk_rpis->exinf = piscb->exinf;
		pk_rpis->wtsk = wait_tskid(&(piscb->wait_queue));
		pk_rpis->pistsk = piscb->pistsk ? piscb->pistsk->tskid	: (ID) FALSE;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_vref_pis */
/*===========================================================================*/

#endif /* USE_PIS */
