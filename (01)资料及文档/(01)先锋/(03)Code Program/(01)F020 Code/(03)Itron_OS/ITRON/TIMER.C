/* timer.c
   Version 1.01.
   Timer module for uITRON OS.

   Notes:
   ------
   a) This module treats all timers as one-shot timers.  Where automatically
      retriggering timers are required, this is implemented by specifying a
      generic callback function for the oneshot timer, which then inserts
      a new oneshot timer and calls the actual callback function.
   b) 'timer_queue' contains the list of timers waiting to expire, which
      is ordered by expiry time.  This requires searching through the list
	  when new elements are inserted, so that the correct insertion location
	  can be determined, which means the insertion-time is proportional to
	  the number timers currently in the queue.  
   c) The timer-insertion time could be a problem if the number of active 
      timers grows large, as the 'timer_handler' runs as a high priority 
      interrupt and disables interrupts when executing!

   Mods to Original File:
   ----------------------
   a) Added English & removed Japanese comments.

   Additional Mods:
   ----------------
   a) In version 1.01 modify 'timer_handler' to cope with dedicated RTOS
      timer interrupt obtained from 'TIC1TOB'.  This has been set to a 20
	  ms interval.  The 'RTOS' no longer shares the 'MEAS_INT' interrupt,
	  whose processing is now handled separately.

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
#include "timer.h"
#include "task.h"

/*---------------------------------------------------------------------------*/
/* 'current_time' contains the current time for the OS.
*/
SYSTIME				current_time;

/*---------------------------------------------------------------------------*/
/* 'timer_queue' points to the queue of 'timer_event_blocks' TEBS
*/
static QUEUE		timer_queue;

/*---------------------------------------------------------------------------*/
/* Remove the TMEB 'event' from the timer queue it belongs to.
*/
void
timer_delete(TMEB *event)
	{
	queue_delete(&(event->queue));
	}

/*---------------------------------------------------------------------------*/
/* Initialise the uITRON OS hardware timer.
*/
void
timer_initialize(void)
	{
	current_time = 0;
	queue_initialize(&timer_queue);

	/* The function used to contain a call to 'start_hw_timer', but this 
	   has been moved so that it takes place after the initial task has been
	   properly started.
	*/
	/* start_hw_timer()'	*/
	}

/*---------------------------------------------------------------------------*/
/* Terminate the OS timer. (Pretty useless function this).
*/
void
timer_shutdown(void)
	{
	terminate_hw_timer();
	}

/*---------------------------------------------------------------------------*/
/* Add the timer 'event' to the 'timer_queue', placing it in the correct
   location in relation to other timer expiry times (the queue is ordered
   by expiry time).
*/
static void
enqueue_tmeb(TMEB *event)
	{
	QUEUE			*q;

	for (q = timer_queue.next; q != &timer_queue; q = q->next) 
		{
		/* The test below needs to be < and not <=, as contained in the 
		   original version, if the order of timer expiry is to be
		   maintained.
		*/
		if ( event->time < ((TMEB *)q)->time )
			break;
		}
	queue_insert(&(event->queue),q);
	}

/*---------------------------------------------------------------------------*/
/* Format up a timer event block TMEB 'event' with the specified relative
   timeout 'tmout', callback function 'callback' and argument 'arg', then 
   add the TMEB to the 'timer_queue'.
   NOTE:
   A close examination will show significant similarity amongst the following
   next three functions.  'timer_insert_abs' and 'timer_insert_rel' are
   identical, apart from the way in which the time is specified.  These
   two functions are the most used of the three.  'timer_insert' is a
   variant on 'timer_insert_rel', which is required only by the 'make_wait'
   function contained in 'wait.c'.  The variation provides special treatment
   when given an infinite timeout period (i.e no timeout, TMO_FEVR mean
   forever).
 */
void
timer_insert(TMEB *event,TMO tmout,CBACK callback,VP arg)
	{
	event->callback = callback;
	event->arg = arg;
	if ( tmout==TMO_FEVR ) 
		queue_initialize(&(event->queue));
	else 
		{
		assert(tmout > 0);
		event->time = current_time + tmout;
		enqueue_tmeb(event);
		}
	}

/*---------------------------------------------------------------------------*/
/* Format up a timer event block TMEB 'event' with the specified absolute
   timeout 'tmout', callback function 'callback' and argument 'arg', then 
   add the TMEB to the 'timer_queue'.
 */
void
timer_insert_abs(TMEB *event,SYSTIME *time,CBACK callback,VP arg)
	{
	event->callback = callback;
	event->arg = arg;
	event->time = *time;
	enqueue_tmeb(event);
	}

/*---------------------------------------------------------------------------*/
/* Format up a timer event block TMEB 'event' with the specified relative
   timeout 'tmout', callback function 'callback' and argument 'arg', then 
   add the TMEB to the 'timer_queue'.
 */
void
timer_insert_rel(TMEB *event,SYSTIME *time,CBACK callback,VP arg)
	{
	event->callback = callback;
	event->arg = arg;
	event->time = current_time + *time;
	enqueue_tmeb(event);
	}

/*---------------------------------------------------------------------------*/
/* 'timer_handler' searches through the list of (one-shot) timers that are
   in the 'timer_queue' and removes and executes the callback functions for
   those timers that have expired.
   The code should be called from the timer interrupt.
*/
void
timer_handler(void)
	{
	TMEB			*event;

	/* Clear the TIC1TOB interrupt, then continue the timer processing.
	*/
	clear_hw_timer_interrupt();
	
	/* This function seems to disable interrupts for a significant
	   and possibly indeterminate period of time, depending on what
	   the callback functions actually do and the number of timers
	   that require execution.
	   This seems somewhat dangerous, although may be OK if the 
	   timer callback code is fairly simple and several timers
	   do not expire simultaneously.  However, if simultaneous
	   expiry does occur, additional code could be added to limit
	   the number of timers processed at this timer-tick.
	*/
	BEGIN_CRITICAL_SECTION;
	current_time += TIMER_PERIOD;

	while ( !queue_empty_p(&timer_queue) ) 
		{
		event = (TMEB *)(timer_queue.next);
		if ( event->time <= current_time ) 
			{
			queue_delete(&(event->queue));
			if ( event->callback ) 
				{
				(*(event->callback))(event->arg);
				}
			}
		else 
			{
			break;
			}
		}
	END_CRITICAL_SECTION;
	}

/*---------------------------------------------------------------------------*/
/* Implements SYSTEM-CALL to obtain an accurate version of the current time.
   This differs from the i_get_tim' system call, which outputs the current
   time in units of 'milliseconds' and is also limited to the system clock
   resolution.  In most implementations, including this one, the system 
   clock will typically run with a period of greater than 10 ms.
   Here the time is output in units of microseconds (us).
*/
#ifndef _i_vget_tim

SYSCALL ER
i_vget_tim(SYSUTIME *pk_utim)
	{
	SYSTIME			ctime;
	TICK			tick;
	BOOL			ireq;

	BEGIN_CRITICAL_SECTION;
	ctime = current_time;
	tick = get_current_hw_time();
	ireq = fetch_hw_timer_interrupt();
	END_CRITICAL_SECTION;

	if (ireq && tick < (TO_TICK(TIMER_PERIOD) - GET_TOLERANCE)) 
		{
		ctime += TIMER_PERIOD;
		}
	*pk_utim = ctime * 1000 + TO_USEC(tick);
	return(E_OK);
	}

#endif /* _i_vget_tim */

/*===========================================================================*/

