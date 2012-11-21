/* task.c
   Task modules for uITRON based OS.

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

#include <string.h>
#include "kernel.h"
#include "task.h"
#include "wait.h"
#include "ready_q.h"
#include "cpu.h"

/*===========================================================================*/

INT		ctxboffset;						/* CTXB Offset in TCB			*/
TCB		*ctxtsk;		 				/* Currently RUNNING TCB		*/
TCB		*schedtsk;			 			/* Highest priority ready TCB	*/
TCB		tcb_table[NUM_TSKID];			/* Task control blocks			*/
RDYQUE	ready_queue;					/* Scheduler ready queue		*/

#ifndef _i_vcre_tsk
QUEUE	free_tcb;
#endif

/*---------------------------------------------------------------------------*/
/* Returns 'TRUE' if the task with state 'state' is alive.
   (i.e. not NONEXISTANT or DORMANT).
*/
BOOL
task_alive(TSTAT state)
	{
	return(state & (TS_READY|TS_WAIT|TS_SUSPEND));
	}

/*---------------------------------------------------------------------------*/
/* Function to initialise the TCB's.
*/
void
task_initialize(void)
	{
	INT		i;
	TCB		*tcb;
	ID		tskid;

	ctxtsk = schedtsk = (TCB *) 0;
	ready_queue_initialize(&ready_queue);

	#ifndef _i_vcre_tsk
	queue_initialize(&free_tcb);
	#endif

	for ( tcb=tcb_table,i = 0; i<NUM_TSKID; tcb++,i++)
		{
		tskid = ID_TSK(i);
		tcb->tskid = tskid;
		tcb->state = TS_NONEXIST;

		#ifdef USE_POR
		tcb->wrdvno = (W)(UH) tskid;
		#endif /* USE_POR */
		#ifndef _i_vcre_tsk
		if ( !SYS_TSKID(tskid) )
			{
			queue_insert(&(tcb->tskque),&free_tcb);
			}
		#endif /* _i_vcre_tsk */
		}
	dispatch_enabled = interrupts_enabled = TRUE;
	}

/*---------------------------------------------------------------------------*/
/* Given a valid TCB 'tcb', setup the context for the task.
*/
#define	cUserMode	0x10
#define	cSystemMode	0x1F
#define	cThumbMode	0x20

void
setup_context(TCB *tcb)
	{
	int		i;
	CTXB	*ptskctxb;

	ptskctxb = &tcb->tskctxb;
	memset(ptskctxb,0,sizeof(CTXB));
	ptskctxb->spsr = (cThumbMode|cSystemMode);
	
	for ( i=0; i<13; i++ )
		ptskctxb->ri[i] = 0xAAA000 + i;
	ptskctxb->sp = (VW)tcb->istack;					/* Stack pointer	*/
	ptskctxb->lr = (VW)0;							/* ???- Task return	*/
	ptskctxb->lr_xxx = (VW)tcb->task;				/* Start address	*/
	}

/*---------------------------------------------------------------------------*/
/* Makes the task with task control block 'tch' dormant.
*/
void
make_dormant(TCB *tcb)
	{
	tcb->state = TS_DORMANT;
	tcb->priority = tcb->ipriority;
	tcb->wupcnt = 0;
	tcb->suscnt = 0;
	#ifdef USE_QTSK_PORTION
	tcb->sysmode = tcb->isysmode;
	#endif /* USE_QTSK_PORTION */
	#ifdef PRISEM_SPEC1
	tcb->pislist = (PISCB *) 0;
	#endif /* PRISEM_SPEC1 */
	#ifdef USE_TASK_MAILBOX
	tcb->tmq_head = (T_MSG *) 0;
	#endif /* USE_TASK_MAILBOX */

	setup_context(tcb);
	}

/*---------------------------------------------------------------------------*/
/* Reschedules the tasks by updating the global variable 'schedtsk' to point
   to the highest priority task in the ready queue.
*/
static void
reschedule(void)
	{
	TCB		*toptsk;

	toptsk = ready_queue_top(&ready_queue);
	if ( schedtsk!=toptsk )
		{
		schedtsk = toptsk;
		dispatch_request();
		}
	}

/*---------------------------------------------------------------------------*/
/* Changes the state of task with TCB 'tcb' to READY, places the task in
   the 'ready_queue' and triggers a dispatch operation.
*/
void
make_ready(TCB *tcb)
	{

	/* 'ready_queue_insert' returns TRUE if the new 'tcb' has the highest
	   priority.
	*/
	tcb->state = TS_READY;
	if ( ready_queue_insert(&ready_queue,tcb) ) 
		{
		schedtsk = tcb;
		dispatch_request();
		}
	}

/*---------------------------------------------------------------------------*/
/* Removes task with TCB 'tcb' from the 'ready_queue'. If the deleted TCB is
   the highest priority task 'schedtsk', then 'schedtsk' is updated and a
   dispatch request initiated.
*/
void
make_non_ready(TCB *tcb)
	{
	assert(tcb->state == TS_READY);
	ready_queue_delete(&ready_queue,tcb);
	if ( schedtsk==tcb ) 
		{
		schedtsk = ready_queue_top(&ready_queue);
		dispatch_request();
		}
	}

/*---------------------------------------------------------------------------*/
/* Changes the priority of task 'tcb' to 'priority.
*/
void
change_task_priority(TCB *tcb,INT priority)
	{
	INT		oldpri;

	if ( tcb->state == TS_READY )
		{
		/* Removes 'tcb' from 'ready_queue', changes the priority and 
		   then reinserts into 'ready_queue' to ensure any affect this
		   has on the scheduling is correctly handled.  A reschedule
		   operation is performed immediately afterwards.
		 */
		ready_queue_delete(&ready_queue,tcb);
		tcb->priority = priority;
		ready_queue_insert(&ready_queue,tcb);
		reschedule();
		}
	else
		{
		oldpri = tcb->priority;
		tcb->priority = priority;
		if ((tcb->state & TS_WAIT) && tcb->wspec->chg_pri_hook)
			{
			(*(tcb->wspec->chg_pri_hook))(tcb,oldpri);
			}
		}
	}

/*---------------------------------------------------------------------------*/
/* Rotates the 'ready_queue' tasks with the given 'priority'.
*/
void
rotate_ready_queue(INT priority)
	{
	ready_queue_rotate(&ready_queue,priority);
	reschedule();
	}

/*---------------------------------------------------------------------------*/
/* ???
*/
void
rotate_ready_queue_run(void)
	{
	if ( schedtsk )
		{
		ready_queue_rotate(&ready_queue,ready_queue_top_priority(&ready_queue));
		reschedule();
		}
	}

/*---------------------------------------------------------------------------*/
