/* wait.h
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

#ifndef _WAIT_
#define _WAIT_

#include "queue.h"
#include "timer.h"
#include "task.h"

/*---------------------------------------------------------------------------*/
/* Release the task with TCB 'tcb' from the WAIT state.
*/
extern void
wait_release(TCB *tcb);

/*---------------------------------------------------------------------------*/
/* Release the task with TCB 'tcb' from the WAIT state with an E_OK wait 
   error code.
*/
extern void
wait_release_ok(TCB *tcb);

/*---------------------------------------------------------------------------*/
/* This is a callback function defined to be called on expiry of a timer
   when a task is made to wait.  The callback function is called with a
   pointer to the task's TCB.
*/
extern void
wait_release_tmout(TCB *tcb);

/*---------------------------------------------------------------------------*/
/* Cancel the wait processing for the task with TCB 'tcb'.
*/
extern void
wait_cancel(TCB *tcb);

/*---------------------------------------------------------------------------*/
/* ??? - I think this function needs reordering, since 'make_non_ready'
   contains a dispatch request, which results in the process being
   pre-empted prior to the insertion of the timer.  'make_non_ready'
   should be modified to accept a parameter giving the next state of
   the process, so that the state can be changed prior to the 
   context switch.
   When 'make_wait' or 'gcb_make_wait' (which calls 'make_wait') is called, 
   it is always contained within a CRITICAL_SECTION, 
*/
extern void
make_wait(TMO tmout);

/*---------------------------------------------------------------------------*/
/* Clears the wait QUEUE 'wait_queue', releasing any tasks were refered to
   in the queue and setting the error code to E_DLT to indicate the wait
   was terminated by deletion of the object.
*/
extern void
wait_delete(QUEUE *wait_queue);

/*---------------------------------------------------------------------------*/
/* Returns the ID of any tasks that the wait QUEUE 'wait_queue' refers to,
   or FALSE if no tasks refered to in the wait queue.
*/
extern ID
wait_tskid(QUEUE *wait_queue);

/*---------------------------------------------------------------------------*/
/* Generic control block that is used for semaphores, etc.
*/
struct generic_control_block 
	{
	QUEUE		wait_queue;			/* Wait queue for object			*/
	ID			objid;				/* Object ID						*/
	VP			exinf;				/* Extended information				*/
	ATR			objatr;				/* Object attributes				*/
	};

#define OBJ_NONEXIST	((ATR)-1)

/*---------------------------------------------------------------------------*/
/* Place the item with generic control block GCB 'gcb' into the wait
   state with timeout 'tmout'.  The wait queue may be either a FIFO or a
   priority queue.
   ??? - This function may need re-ordering with queue insertions performed 
   before the 'make_wait' call, which causes a 'dispatch_request'
*/
void
gcb_make_wait(GCB *gcb,TMO tmout);

/*---------------------------------------------------------------------------*/
/* Update the wait queue of the generic control block 'gcb' in response to
   a priority change in the task 'tcb'.
*/
extern void
gcb_change_priority(GCB *gcb,TCB *tcb);

/*---------------------------------------------------------------------------*/
/* Change the priority of the task with TCB 'tcb' and old priority 
   'oldpri'.  This is a generic function which is sometimes specified as the
   'chg_pri_hook' callback field of the WSPEC structure, which is 
   automatically called when 'change_task_priority' is initiated on a waiting 
   task.
*/
extern void
obj_chg_pri(TCB *tcb,INT oldpri);

/*===========================================================================*/

#endif /* _WAIT_ */
