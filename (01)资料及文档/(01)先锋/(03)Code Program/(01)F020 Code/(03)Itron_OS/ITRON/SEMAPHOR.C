/* semaphore.c
   Semaphore functions for uITRON OS.

   Mods to Original File:
   ----------------------
   a) Added English & removed Japanese comments.

   Notes:
   -----
   
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

#ifdef USE_SEM

#include "task.h"
#include "wait.h"
#include "check.h"

/*---------------------------------------------------------------------------*/
/* Structure for controlling semaphore operations.
*/
typedef struct semaphore_control_block 
	{
	QUEUE	wait_queue;				/* Semaphore wait queue				*/
	ID		semid;					/* Semaphore ID						*/
	VP		exinf;					/* Extended information				*/
	ATR		sematr;					/* Semaphore attributes				*/
	INT		semcnt;					/* Current semaphore count			*/
	INT		maxsem;					/* Maximum semaphore count			*/
	} SEMCB;

/*---------------------------------------------------------------------------*/
/* Table of semaphore control blocks.
*/
static SEMCB			semcb_table[NUM_SEMID];

/*---------------------------------------------------------------------------*/
/* Defines for easy indexing into the SEMCB table given the semaphore 'id'.
*/
#define get_semcb(id)	(&(semcb_table[INDEX_SEM(id)]))

/*---------------------------------------------------------------------------*/
/* 'free_semcb' is a linked list of free SEMCB's ready for allocation.
*/
#ifndef _i_vcre_sem
QUEUE	free_semcb;
#endif /* _i_vcre_sem */

/*===========================================================================*/
/* Function to initialize semaphore control block table 'semcb_table' 
*/
void
semaphore_initialize(void)
	{
	INT		i;
	SEMCB	*semcb;
	ID		semid;

	#ifndef _i_vcre_sem
	queue_initialize(&free_semcb);
	#endif /* _i_vcre_sem */

	for ( semcb = semcb_table,i = 0; i < NUM_SEMID; semcb++,i++ ) 
		{
		semid = ID_SEM(i);
		semcb->semid = semid;
		semcb->sematr = OBJ_NONEXIST;
		#ifndef _i_vcre_sem
		if ( !SYS_SEMID(semid) ) 
			{
			queue_insert(&(semcb->wait_queue), &free_semcb);
			}
		#endif /* _i_vcre_sem */
		}
	}

/*---------------------------------------------------------------------------*/
/* Wait callback information for semaphores.
*/
static WSPEC wspec_sem_tfifo = { TTW_SEM, 0, 0 };
static WSPEC wspec_sem_tpri = { TTW_SEM, obj_chg_pri, 0 };

/*---------------------------------------------------------------------------*/
/* Function to initialize the semaphore control block 'semcb' given the
   packet of semaphore initialization data 'pk_csem'.
*/
#if !defined(_i_cre_sem) || !defined(_i_vcre_sem)

static void
_cre_sem(SEMCB *semcb,T_CSEM *pk_csem)
	{
	#ifndef _i_vcre_sem
	if ( !SYS_SEMID(semcb->semid) ) 
		{
		queue_delete(&(semcb->wait_queue));
		}
	#endif /* _i_vcre_sem */

	queue_initialize(&(semcb->wait_queue));
	semcb->exinf = pk_csem->exinf;
	semcb->sematr = pk_csem->sematr;
	semcb->semcnt = pk_csem->isemcnt;
	semcb->maxsem = pk_csem->maxsem;
	}

#endif /* !defined(_i_cre_sem) || !defined(_i_vcre_sem) */

/*---------------------------------------------------------------------------*/
/* This system call creates the semaphore whose ID is specified by 'semid'.
   Specifically, a control block for the semaphore to be created is allocated,
   the associated initial semaphore count is set to 'isemcnt' and maximum 
   allowable semaphore count is set to 'maxsem', all of which are contained
   within 'pk_csem'.
*/
#ifndef _i_cre_sem

SYSCALL ER
i_cre_sem(ID semid,T_CSEM *pk_csem)
	{
	SEMCB	*semcb;
	ER		ercd = E_OK;

	CHECK_SEMID(semid);
	CHECK_SEMACV(semid);
	CHECK_RSATR(pk_csem->sematr, TA_TPRI);
	CHECK_PAR(pk_csem->isemcnt >= 0);
	CHECK_PAR(pk_csem->maxsem >= pk_csem->isemcnt);
	semcb = get_semcb(semid);

	BEGIN_CRITICAL_SECTION;
	if (semcb->sematr != OBJ_NONEXIST) 
		{
		ercd =  E_OBJ;
		}
	else 
		{
		_cre_sem(semcb,pk_csem);
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_cre_sem */

/*---------------------------------------------------------------------------*/
/* This system call creates the semaphore.  Specifically, a control block 
   for the semaphore to be created is allocated,  the associated initial 
   semaphore count is set to 'isemcnt' and maximum allowable semaphore count 
   is set to 'maxsem', all of which are contained within 'pk_csem'.
   An ID need not be specied with this variant of the system call.
*/
#ifndef _i_vcre_sem

SYSCALL ER
i_vcre_sem(T_CSEM *pk_csem)
	{
	SEMCB	*semcb;
	ER		ercd = E_OK;

	CHECK_RSATR(pk_csem->sematr, TA_TPRI);
	CHECK_PAR(pk_csem->isemcnt >= 0);
	CHECK_PAR(pk_csem->maxsem >= pk_csem->isemcnt);

	BEGIN_CRITICAL_SECTION;
	if ( queue_empty_p(&free_semcb) ) 
		{
		ercd =  EV_FULL;
		}
	else 
		{
		semcb = (SEMCB *)(free_semcb.next);
		_cre_sem(semcb,pk_csem);
		ercd = (ER)(semcb->semid);
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_vcre_sem */

/*---------------------------------------------------------------------------*/
/* This system call deletes the semaphore specified by semid.
   Issuing this system call causes memory used for the control block of the
   associated semaphore to be released.  After this system call is invoked,
   another semaphore having the same ID number can be created.
   This system call will complete normally even if there are tasks waiting 
   for the semaphore.  In that case, an E_DLT error will be returned to each
   waiting task.
*/	
#ifndef _i_del_sem

SYSCALL ER
i_del_sem(ID semid)
	{
	SEMCB	*semcb;
	ER		ercd = E_OK;
    
	CHECK_SEMID(semid);
	CHECK_SEMACV(semid);
	semcb = get_semcb(semid);

	BEGIN_CRITICAL_SECTION;
	if ( semcb->sematr == OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		}
	else 
		{
		wait_delete(&(semcb->wait_queue));

		#ifndef _i_vcre_sem
		if ( !SYS_SEMID(semcb->semid) ) 
			{
			queue_insert(&(semcb->wait_queue),&free_semcb);
			}
		#endif /* _i_vcre_sem */
		semcb->sematr = OBJ_NONEXIST;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_del_sem */

/*---------------------------------------------------------------------------*/
/* This system call returns one resource to the semaphore specified by 
   'semid'.  Specifically, if there are any tasks waiting for the specified 
   semaphore, the task at the head of the queue becomes RUN or READY state.  
   When this happens, the associated semaphore count 'semcnt' remains 
   unchanged.  On the other hand, if there are no tasks waiting for the 
   semaphore, the associated semaphore count 'semcnt' is incremented by one.
*/
#ifndef _i_sig_sem

SYSCALL ER
i_sig_sem(ID semid)
	{
	SEMCB	*semcb;
	ER		ercd = E_OK;
    
	CHECK_SEMID(semid);
	CHECK_SEMACV(semid);
	semcb = get_semcb(semid);

	BEGIN_CRITICAL_SECTION;
	if ( semcb->sematr == OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		}
	else if ( !queue_empty_p(&(semcb->wait_queue)) ) 
		{
		wait_release_ok((TCB *)(semcb->wait_queue.next));
		}
	else if ( semcb->semcnt >= semcb->maxsem ) 
		{
		ercd = E_QOVR;
		}
	else 
		{
		semcb->semcnt += 1;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_sig_sem */

/*---------------------------------------------------------------------------*/
/* The 'twai_sem' system call has the same function as 'wai_sem' with an 
   additional timeout feature.  A maximum wait time (timeout value) can be 
   specified using the parameter 'tmout'.  When a timeout is specified, a 
   timeout error, E_TMOUT, will result and the system call will finish if 
   the period specified by 'tmout' elapses without conditions for releasing 
   wait being satisfied (i.e. without sig_sem being executed).
*/
#if !defined(_i_wai_sem) || !defined(_i_preq_sem) || !defined(_i_twai_sem)

SYSCALL ER
i_twai_sem(ID semid,TMO tmout)
	{
	SEMCB	*semcb;
	ER		ercd = E_OK;

	CHECK_SEMID(semid);
	CHECK_SEMACV(semid);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();
	semcb = get_semcb(semid);

	BEGIN_CRITICAL_SECTION;
	if (semcb->sematr == OBJ_NONEXIST) 
		{
		ercd = E_NOEXS;
		}
	else if ( semcb->semcnt >= 1 ) 
		{
		semcb->semcnt -= 1;
		}
	else 
		{
		ercd = E_TMOUT;
		if ( tmout != TMO_POL ) 
			{
			ctxtsk->wspec = (semcb->sematr & TA_TPRI) ?	&wspec_sem_tpri : &wspec_sem_tfifo;
			ctxtsk->wercd = &ercd;
			gcb_make_wait((GCB *) semcb, tmout);
			}
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_twai_sem */

/*---------------------------------------------------------------------------*/
/* The 'wai_sem' system call obtains one resource from the semaphore 
   specified by 'semid'.  Specifically, if the count 'semcnt' for the 
   specified semaphore is one or greater, that semaphore's count is 
   decremented by one.  In this case, the task issuing this system call is not 
   put in WAIT state, but rather continues to execute.  If, on the other hand, 
   the semaphore's count 'semcnt' is 0, the task issuing this system call 
   will become WAIT state and be put on a queue associated with the specified 
   semaphore.  The semaphore's count 'semcnt' (0) will remain unchanged in 
   this case.
*/
#ifndef _i_wai_sem

SYSCALL ER
i_wai_sem(ID semid)
	{
	return(i_twai_sem(semid,TMO_FEVR));
	}

#endif /* _i_wai_sem */

/*---------------------------------------------------------------------------*/
/* The 'preq_sem' system call has the same function as 'wai_sem' except for
   the waiting feature.  If the count 'semcnt' of the specified semaphore is 
   0, an E_TMOUT error is returned to indicate polling failed and the system 
   call finishes.  Unlike 'wai_sem', 'preq_sem' does not wait in this case.
*/
#ifndef _i_preq_sem

SYSCALL ER
i_preq_sem(ID semid)
	{
	return(i_twai_sem(semid,TMO_POL));
	}

#endif /* _i_preq_sem */

/*---------------------------------------------------------------------------*/
/* This system call refers to the state of the semaphore specified by 'semid',
   and returns its current semaphore count 'semcnt', waiting task information
   'wtsk', and its extended information 'exinf' via the 'pk_rsem' structure.
*/
#ifndef _i_ref_sem

SYSCALL ER
i_ref_sem(T_RSEM *pk_rsem,ID semid)
	{
	SEMCB	*semcb;
	ER		ercd = E_OK;

	CHECK_SEMID(semid);
	CHECK_SEMACV(semid);
	semcb = get_semcb(semid);

	BEGIN_CRITICAL_SECTION;
	if ( semcb->sematr == OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		}
	else 
		{
		pk_rsem->exinf = semcb->exinf;
		pk_rsem->wtsk = wait_tskid(&(semcb->wait_queue));
		pk_rsem->semcnt = semcb->semcnt;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_ref_sem */

/*===========================================================================*/

#endif /* USE_SEM */

