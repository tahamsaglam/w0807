/* queue.c
   Queue module for uITRON based OS.

   Notes:
   ------
   a) This module implements FIFO based queue operations.

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
#include "queue.h"

/*===========================================================================*/
/* Initialise the queue 'queue'.
   The initialisation is such that the element 'queue' is points to itself.
 */
void
queue_initialize(QUEUE *queue)
	{
	queue->prev = queue->next = queue;
	}

/*---------------------------------------------------------------------------*/
/* Insert the element 'entry' at the end/tail of the queue 'queue'.
*/
void
queue_insert(QUEUE *entry, QUEUE *queue)
	{

	/* The element 'entry' is placed at the end of the queue 'queue'.
	   Note that 'queue->next' is not affected by this operation.
	*/
	entry->prev = queue->prev;
	entry->next = queue;
	queue->prev->next = entry;
	queue->prev = entry;
	}

/*---------------------------------------------------------------------------*/
/* Deletes/removes the 'entry' element from whatever list it is an element of.
   This implementation essentially delinks the element.
*/
void
queue_delete(QUEUE *entry)
	{
	if (entry->next != entry)
		{
		entry->prev->next = entry->next;
		entry->next->prev = entry->prev;
		}
	}

/*---------------------------------------------------------------------------*/
/* Deletes/removes the next (head/first-in/oldest) element in the queue 
   'queue', returning a pointer to the delinked element 'entry'.
*/
QUEUE *
queue_delete_next(QUEUE *queue)
	{
	QUEUE	*entry;

	/* Delinks the oldest element of the queue 'queue'.  Note that
	   'queue->prev' is unaffected by the operation.
	*/
	assert( queue->next!=queue );
	entry = queue->next;
	queue->next = entry->next;
	entry->next->prev = queue;
	return(entry);
	}

/*---------------------------------------------------------------------------*/
/* Search through the queue 'queue' for an element which has a field
   value at 'offset' greater than the value 'val'.  This routine can be
   used to find queue elements with components greater than 'val', 
   even if component occurs at different locations in different
   structures.
*/
QUEUE *
queue_search_gt(QUEUE *queue,INT val,INT offset)
	{
	QUEUE	*entry;

	for ( entry=queue->next;entry!=queue;entry=entry->next )
		{
		if (*((INT *)(((VB *) entry) + offset)) > val)
			break;
		}
	return(entry);
	}

/*---------------------------------------------------------------------------*/
/* Checks if the queue 'queue' is empty, returning 'TRUE' if it is, 'FALSE'
   if it is not.
*/
BOOL
queue_empty_p(QUEUE *queue)
	{
	if (queue->next == queue) 
		{
		assert(queue->prev == queue);
		return(TRUE);
		}
	return(FALSE);
	}

/*---------------------------------------------------------------------------*/
