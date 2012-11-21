/* ready_q.c
   Priority queue module for uITRON based OS.

   Notes:
   ------
   a) This module implements a priority queue.

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
#include "ready_q.h"

/*---------------------------------------------------------------------------*/
/* This function sets a flag in the RDYQUE 'rq' indicating that an element
   with the given 'priority' is present.
*/
__inline void
bitmap_set(RDYQUE *rq,INT priority)
	{
	rq->bitmap[ priority/BITMAPSZ ] |= (1 << (priority % BITMAPSZ));
	}

/*---------------------------------------------------------------------------*/
/* This function clears a flag in the RDYQUE 'rq' indicating that an
   element with the given 'priority' is present (i.e. element of given
   priority is not present in the RDYQUE.
*/
__inline void
bitmap_clear(RDYQUE *rq,INT priority)
	{
	rq->bitmap[ priority/BITMAPSZ ] &= ~(1 << (priority % BITMAPSZ));
	}

/*---------------------------------------------------------------------------*/
/* Determines the bit location of first non-zero bit of 'i', starting 
   from the LSB and moving to the MSB.
*/
INT
_ffs(INT i)
	{
	INT		j = 0;

	assert(i!=0);
	while( (i&1)==0 )
		{
		i >>= 1;
		j++;
		}
	return(j);
	}

/*---------------------------------------------------------------------------*/
/* Inititialise the priority queue RDYQUQE 'rq'.
*/
void
ready_queue_initialize(RDYQUE *rq)
	{
	INT		i;

	/* Remember that low values for priority actually indicate higher
	   priority tasks. 
	*/
	rq->top_priority = NUM_PRI;				/* Lowest possible priority	*/
	for ( i=0;i<NUM_PRI;i++ )
		{
		queue_initialize(&rq->tskque[i]);
		}
	rq->null = (TCB *) 0;
	memset(rq->bitmap,0,sizeof(rq->bitmap));
	}

/*---------------------------------------------------------------------------*/
/* Returns the (pointer to the) TCB of the highest priority task in the
   RDYQUE 'rq'.
*/
TCB *
ready_queue_top(RDYQUE *rq)
	{
	return( (TCB *)(rq->tskque[rq->top_priority].next) );
	}

/*---------------------------------------------------------------------------*/
/* Returns the priority of the highest priority TCB in the RDYQUE 'rq'.
*/
INT
ready_queue_top_priority(RDYQUE *rq)
	{
	return(rq->top_priority);
	}

/*---------------------------------------------------------------------------*/
/* Inserts the TCB 'tcb' into the RDYQUE 'rq', returning 'TRUE if the new
   task if of higher priority than any other tasks or 'FALSE' if it is not.
*/
BOOL
ready_queue_insert(RDYQUE *rq, TCB *tcb)
	{
	INT		priority;

	/* Get the task priority, then insert it into the appropriate 'FIFO'
	   of the priority queue 'rq' and set the flag indicating presence of
	   the element with said 'priority'.
	*/
	priority = tcb->priority;
	queue_insert(&(tcb->tskque), &(rq->tskque[priority]));
	bitmap_set(rq,priority);
	if ( priority<rq->top_priority ) 
		{
		rq->top_priority = priority;
		return(TRUE);
		}
	return(FALSE);
	}

/*---------------------------------------------------------------------------*/
/* Inserts the TCB 'tcb' into the RDYQUE 'rq', but in a slightly different
   way than 'ready_queue_insert'.
*/
void
ready_queue_insert_top(RDYQUE *rq,TCB *tcb)
	{
	INT		priority;

	/* Get the task priority, then insert it into the appropriate 'FIFO'
	   of the priority queue 'rq' and set the flag indicating presence of
	   the element with said 'priority'.
	   This function differs from 'ready_queue_insert' in that the item
	   is inserted at the head of the associated 'FIFO' instead at the
	   end.  Furthermore, there is no return indication provided.
	*/
	priority = tcb->priority;
	queue_insert(&(tcb->tskque),rq->tskque[priority].next);
	bitmap_set(rq,priority);
	if ( priority<rq->top_priority ) 
		{
		rq->top_priority = priority;
		}
	}

/*---------------------------------------------------------------------------*/
/* Removes the tack 'tcb' from the RDYQUE 'rq'.
 */
void
ready_queue_delete(RDYQUE *rq,TCB *tcb)
	{
	INT		i, priority;

	/* Remove the task 'tcb' from the associated FIFO (for that priority).
	   However, it that task is the only element at that priority then
	   the 'bitmap' field must be adjusted.
	*/
	priority = tcb->priority;
	queue_delete(&(tcb->tskque));
	if ( !queue_empty_p(&(rq->tskque[priority])) )
		return;
	bitmap_clear(rq, priority);
	if ( priority!=rq->top_priority )
		return;

	/* This section of the code deals with the case where the highest
	   priority task was removed and as a result, the 'top_priority'
	   field must be updated.
	*/
	for ( i=priority/BITMAPSZ;i<NUM_BITMAP;i++ )
		{
		if ( rq->bitmap[i] )
			{
			rq->top_priority = i * BITMAPSZ + _ffs(rq->bitmap[i]);
			return;
			}
		}
	rq->top_priority = NUM_PRI;
	}

/*---------------------------------------------------------------------------*/
/* This function rotates the FIFO in the priority queue RDYQUE 'rq' for 
   the supplied 'priority'.  This involves removing the element from the
   head of the queue and then reinserting it at the tail/end of the queue.
*/
void
ready_queue_rotate(RDYQUE *rq,INT priority)
	{
	QUEUE	*entry, *queue;

	if ( !queue_empty_p(queue=&(rq->tskque[priority])) )
		{
		entry = queue_delete_next(queue);
		queue_insert(entry, queue);
		}
	}

/*---------------------------------------------------------------------------*/
