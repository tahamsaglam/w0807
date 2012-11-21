/* ready_q.h
   Priority queue module header file for uITRON based OS.

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

#ifndef _READY_QUEUE_
#define _READY_QUEUE_

#include "queue.h"
#include "config.h"
#include "task.h"

/*===========================================================================*/

#define BITMAPSZ	(sizeof(UINT) * 8)
#define NUM_BITMAP	((NUM_PRI + BITMAPSZ - 1) / BITMAPSZ)

/* RDYQUE structure implements a priority queue for use by the task 
   scheduler.  The 'top_priority' field provides data on the maximum
   priority entry within the queue.  Remember low priority values
   actually are actually given a higher priority by the scheduler and
   hence run first (why do they do this?).
*/
typedef	struct ready_queue 
	{
	INT		top_priority;		/* Maximum priority in RDYQUE	*/
	QUEUE	tskque[NUM_PRI];	/* FIFO for each priority		*/
	void	*null;						/* ACTUALLY 'TCB'		*/
	UINT	bitmap[NUM_BITMAP];	/* Indicates present priorities	*/
	} RDYQUE;

/*---------------------------------------------------------------------------*/
/* This function sets a flag in the RDYQUE 'rq' indicating that an element
   with the given 'priority' is present.
*/
extern __inline void
bitmap_set(RDYQUE *rq,INT priority);

/*---------------------------------------------------------------------------*/
/* This function clears a flag in the RDYQUE 'rq' indicating that an
   element with the given 'priority' is present (i.e. element of given
   priority is not present in the RDYQUE.
*/
extern __inline void
bitmap_clear(RDYQUE *rq,INT priority);

/*---------------------------------------------------------------------------*/
/* Determines the bit location of first non-zero bit of 'i', starting 
   from the LSB and moving to the MSB.
*/
extern INT
_ffs(INT i);

/*---------------------------------------------------------------------------*/
/* Inititialise the priority queue RDYQUQE 'rq'.
*/
extern void
ready_queue_initialize(RDYQUE *rq);

/*---------------------------------------------------------------------------*/
/* Returns the (pointer to the) TCB of the highest priority task in the
   RDYQUE 'rq'.
*/
extern TCB *
ready_queue_top(RDYQUE *rq);

/*---------------------------------------------------------------------------*/
/* Returns the priority of the highest priority TCB in the RDYQUE 'rq'.
*/
extern INT
ready_queue_top_priority(RDYQUE *rq);

/*---------------------------------------------------------------------------*/
/* Inserts the TCB 'tcb' into the RDYQUE 'rq', returning 'TRUE if the new
   task if of higher priority than any other tasks or 'FALSE' if it is not.
*/
extern BOOL
ready_queue_insert(RDYQUE *rq, TCB *tcb);

/*---------------------------------------------------------------------------*/
/* Inserts the TCB 'tcb' into the RDYQUE 'rq', but in a slightly different
   way than 'ready_queue_insert'.
*/
extern void
ready_queue_insert_top(RDYQUE *rq,TCB *tcb);

/*---------------------------------------------------------------------------*/
/* Removes the tack 'tcb' from the RDYQUE 'rq'.
 */
extern void
ready_queue_delete(RDYQUE *rq,TCB *tcb);

/*---------------------------------------------------------------------------*/
/* This function rotates the FIFO in the priority queue RDYQUE 'rq' for 
   the supplied 'priority'.  This involves removing the element from the
   head of the queue and then reinserting it at the tail/end of the queue.
*/
extern void
ready_queue_rotate(RDYQUE *rq,INT priority);

/*===========================================================================*/

#endif /* _READY_QUEUE_ */
