/* timer.h
   Header file for uITRON timer module.

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

#ifndef _TIMER_
#define _TIMER_

#include "queue.h"

/*---------------------------------------------------------------------------*/

typedef void	(*CBACK)(VP);		/* Callback function 'typedef'		*/

/*---------------------------------------------------------------------------*/
/* Structure to maintain a linked list of 'timers', as well as data on the
   what is required when the timer expires as well as the timer expiry 
   'time'.
*/
typedef struct timer_event_block 
	{
	QUEUE		queue;				/* Queue of TMEB's					*/
	SYSTIME		time;				/* Timer expiry time				*/
	CBACK		callback;			/* Function to 'callback' at expiry	*/
	VP			arg;				/* Argument for 'callback' function	*/
	} TMEB;

/*===========================================================================*/

/* 'current_time' contains the current time for the OS.
*/
extern SYSTIME	current_time;

/*---------------------------------------------------------------------------*/
/* Initialise the uITRON OS hardware timer.
*/
extern void
timer_initialize(void);

/*---------------------------------------------------------------------------*/
/* Terminate the OS timer. (Pretty useless function this).
*/
void
timer_shutdown(void);

/*---------------------------------------------------------------------------*/
/* Format up a timer event block TMEB 'event' with the specified relative
   timeout 'tmout', callback function 'callback' and argument 'arg', then 
   add the TMEB to the 'timer_queue'.
 */
void
timer_insert(TMEB *event,TMO tmout,CBACK callback,VP arg);

/*---------------------------------------------------------------------------*/
/* Format up a timer event block TMEB 'event' with the specified absolute
   timeout 'tmout', callback function 'callback' and argument 'arg', then 
   add the TMEB to the 'timer_queue'.
 */
void
timer_insert_abs(TMEB *event,SYSTIME *time,CBACK callback,VP arg);

/*---------------------------------------------------------------------------*/
/* Format up a timer event block TMEB 'event' with the specified relative
   timeout 'tmout', callback function 'callback' and argument 'arg', then 
   add the TMEB to the 'timer_queue'.
 */
void
timer_insert_rel(TMEB *event,SYSTIME *time,CBACK callback,VP arg);

/*---------------------------------------------------------------------------*/
/* Remove the TMEB 'event' from the timer queue it belongs to.
*/
extern void
timer_delete(TMEB *event);

/*===========================================================================*/

#endif /* _TIMER_ */
