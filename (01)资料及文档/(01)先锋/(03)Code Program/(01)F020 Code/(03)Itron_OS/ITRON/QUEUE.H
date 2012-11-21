/* queue.h
   Header file for uITRON queue module.

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

#ifndef	_QUEUE_
#define	_QUEUE_

#include "itron.h"

/*===========================================================================*/
/*  Structure used for implemenation of a doubly linked list for a 
    queue.
*/
typedef struct queue 
	{
	struct queue 	*next;
	struct queue 	*prev;
	} QUEUE;

/*===========================================================================*/
/* Initialise the queue 'queue'.
   The initialisation is such that the element 'queue' is points to itself.
 */
extern void
queue_initialize(QUEUE *queue);

/*---------------------------------------------------------------------------*/
/* Insert the element 'entry' at the end/tail of the queue 'queue'.
*/
extern void
queue_insert(QUEUE *entry, QUEUE *queue);

/*---------------------------------------------------------------------------*/
/* Deletes/removes the 'entry' element from whatever list it is an element of.
   This implementation essentially delinks the element.
*/
extern void
queue_delete(QUEUE *entry);

/*---------------------------------------------------------------------------*/
/* Deletes/removes the next (head/first-in/oldest) element in the queue 
   'queue', returning a pointer to the delinked element 'entry'.
*/
extern QUEUE *
queue_delete_next(QUEUE *queue);

/*---------------------------------------------------------------------------*/
/* Search through the queue 'queue' for an element which has a field
   value at 'offset' greater than the value 'val'.  This routine can be
   used to find queue elements with components greater than 'val', 
   even if component occurs at different locations in different
   structures.
*/
extern QUEUE *
queue_search_gt(QUEUE *queue,INT val,INT offset);

/*---------------------------------------------------------------------------*/
/* Checks if the queue 'queue' is empty, returning 'TRUE' if it is, 'FALSE'
   if it is not.
*/
extern BOOL
queue_empty_p(QUEUE *queue);

/*===========================================================================*/

#endif		/* _QUEUE_	*/
