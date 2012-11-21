/* wait.c
   Header file for uITRON based OS.

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

#include "kernel.h"
#include "task.h"
#include "wait.h"

/*---------------------------------------------------------------------------*/
/* Cancel the wait processing for the task with TCB 'tcb'.
*/
void
wait_cancel(TCB *tcb)
	{
	timer_delete(&(tcb->wtmeb));
	queue_delete(&(tcb->tskque));
	}

/*---------------------------------------------------------------------------*/
/* Releases the task with TCB 'tcb' from the WAIT state.
*/
void
make_non_wait(TCB *tcb)
	{
	assert(tcb->state & TS_WAIT);

	if (tcb->state == TS_WAIT)
		make_ready(tcb);
	else
		tcb->state = TS_SUSPEND;
	}

/*---------------------------------------------------------------------------*/
/* Release the task with TCB 'tcb' from the WAIT state.
*/
void
wait_release(TCB *tcb)
	{
	timer_delete(&(tcb->wtmeb));
	queue_delete(&(tcb->tskque));
	make_non_wait(tcb);
	}

/*---------------------------------------------------------------------------*/
/* Release the task with TCB 'tcb' from the WAIT state with an E_OK wait 
   error code.
*/
void
wait_release_ok(TCB *tcb)
	{
	*(tcb->wercd) = E_OK;
	wait_release(tcb);
	}

/*---------------------------------------------------------------------------*/
/* This is a callback function defined to be called on expiry of a timer
   when a task is made to wait.  The callback function is called with a
   pointer to the task's TCB.
*/
void
wait_release_tmout(TCB *tcb)
	{
	queue_delete(&(tcb->tskque));
	make_non_wait(tcb);

	#ifdef USE_REL_WAI_HOOK
	if ( tcb->wspec->rel_wai_hook )
		{
		(*(tcb->wspec->rel_wai_hook))(tcb);
		}
	#endif /* USE_REL_WAI_HOOK */
	}

/*---------------------------------------------------------------------------*/
/* ??? - I think this function needs reordering, since 'make_non_ready'
   contains a dispatch request, which results in the process being
   pre-empted prior to the insertion of the timer.  'make_non_ready'
   should be modified to accept a parameter giving the next state of
   the process, so that the state can be changed prior to the 
   context switch.  Note that a another problem happens when receiving
   messages from a message buffer, which itself suggest an alternate 
   solution to the problem.  The solution is to disable any task context
   switch until the END_CRITICAL_SECTION is reached.  This ensures that
   all relavent code completes without any problem being experienced.
   The END_CRITICAL_SECTION code must then check the contents of some
   flag which will also result in a context switch, besides enabling of
   interrupts.

   When 'make_wait' or 'gcb_make_wait' (which calls 'make_wait') is called, 
   it is always contained within a CRITICAL_SECTION. 
*/
void
make_wait(TMO tmout)
	{
	assert( !(ctxtsk->state & TS_WAIT) );

	/* Question: When will 'ctxtsk' state not be READY, since this is
	   supposed to point to the currently executing task?
	*/
	switch (ctxtsk->state)
		{
		case TS_READY:
			make_non_ready(ctxtsk);
			ctxtsk->state = TS_WAIT;
			break;
		case TS_SUSPEND:
			ctxtsk->state = TS_WAITSUS;
			break;
		}

	/* Set up a timer for the task, which on expiry will result in the 
	   'wait_release_tmout' function being called.  When this triggers,
	   the task will be made READY again.
	   Note that because the task is made READY by a 'timer' when that
	   timer expires and several timers may need their callback functions 
	   run at the same time, the dispatch request that is made within the 
	   timer callback should be delayed until after the timer code has 
	   completed.
	*/
	timer_insert(&(ctxtsk->wtmeb),tmout,(CBACK)wait_release_tmout,ctxtsk);
	}

/*---------------------------------------------------------------------------*/
/* Clears the wait QUEUE 'wait_queue', releasing any tasks were refered to
   in the queue and setting the error code to E_DLT to indicate the wait
   was terminated by deletion of the object.
*/
void
wait_delete(QUEUE *wait_queue)
	{
	TCB		*tcb;

	while ( !queue_empty_p(wait_queue) )
		{
		tcb = (TCB *)(wait_queue->next);
		*(tcb->wercd) = E_DLT;				/* E_DLT Wait Error Code	*/
		wait_release(tcb);
		}
	}

/*---------------------------------------------------------------------------*/
/* Returns the ID of any tasks that the wait QUEUE 'wait_queue' refers to,
   or FALSE if no tasks refered to in the wait queue.
*/
ID
wait_tskid(QUEUE *wait_queue)
	{
	if ( queue_empty_p(wait_queue) )
		{
		return((ID) FALSE);
		}
	else
		{
		return(((TCB *)(wait_queue->next))->tskid);
		}
	}

/*---------------------------------------------------------------------------*/
/* Implement a priority queue by placing the task with TCB 'tcb' into a 
   queue location based on the task priority.  This is implemented by
   searching for the correct location in the queue prior to insertion.
*/
__inline void
queue_insert_tpri(TCB *tcb,QUEUE *queue)
	{
	QUEUE	*q;

	q = queue_search_gt(queue,tcb->priority,offsetof(TCB, priority));
	queue_insert(&(tcb->tskque),q);
	}

/*---------------------------------------------------------------------------*/
/* Place the item with generic control block GCB 'gcb' into the wait
   state with timeout 'tmout'.  The wait queue may be either a FIFO or a
   priority queue.
   Note that although 'make_wait' calls 'dispatch_request', no actual
   dispatching or task switching takes place until the function leaves
   the CRITICAL_SECTION's that enclose it.  This function must only be
   called from within a CRITICAL_SECTION.
*/
void
gcb_make_wait(GCB *gcb,TMO tmout)
	{
	ctxtsk->wgcb = gcb;
	make_wait(tmout);
	if (gcb->objatr & TA_TPRI)
		{
		queue_insert_tpri(ctxtsk,&(gcb->wait_queue));
		}
	else
		{
		queue_insert(&(ctxtsk->tskque),&(gcb->wait_queue));
		}
	}

/*---------------------------------------------------------------------------*/
/* Update the wait queue of the generic control block 'gcb' in response to
   a priority change in the task 'tcb'.
*/
void
gcb_change_priority(GCB *gcb,TCB *tcb)
	{
	assert(gcb->objatr & TA_TPRI);

	queue_delete(&(tcb->tskque));
	queue_insert_tpri(tcb,&(gcb->wait_queue));
	}

/*---------------------------------------------------------------------------*/
/* Change the priority of the task with TCB 'tcb' and old priority 
   'oldpri'.  This is a generic function which is sometimes specified as the
   'chg_pri_hook' callback field of the WSPEC structure, which is 
   automatically called when 'change_task_priority' is initiated on a waiting 
   task.
*/
void
obj_chg_pri(TCB *tcb,INT oldpri)
	{
	gcb_change_priority(tcb->wgcb,tcb);
	}

/*---------------------------------------------------------------------------*/

