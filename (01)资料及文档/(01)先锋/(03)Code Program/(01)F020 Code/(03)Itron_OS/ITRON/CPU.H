/* cpu.h
   CPU specific header file for uITRON based OS.

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

#ifndef	_CPU_
#define	_CPU_

#include "itron.h"
#include "cpu_arm.h"

/*---------------------------------------------------------------------------*/
/* Dispatch request function.
   This function is called from 'reschedule', 'make_ready' & 'make_non_ready',
   all of which are contained in "task.c".
*/
void
dispatch_request(void);

/*---------------------------------------------------------------------------*/
/* This function attempts to perform a dispatch operation.  This implementa-
   tion has changed from the original ITIS version.  First of all, no
   dispatching is permitted within CRITICAL_SECTIONs.  The reason for
   this is that some CRITICAL_SECTIONS attempt several dispatches while
   iterating through several tasks, and enforcing this requirement ensures
   that the best task is selected at the end when all information is
   available.  Dispatching is also skipped if called within a task-
   independent portion (timer or interrupt) as required by the delayed
   dispatching rule.  In this case, the dispatching is performed by the
   'ret_int' system call made at the end of the interrupt.
   When dispatching is required, an SWI is triggered, the current context
   is saved, a switch to the new TCB is made and then that new context is
   restored.
*/
void
dispatch(void);

/*---------------------------------------------------------------------------*/
/* Force dispatch operation.  
   This function is called from 'i_exd_tsk' and	'i_ext_tsk', both of which
   are contained in "task_man.c".  One respect in which these functions 
   differ from many of the other functions is the use of the DISABLE_INTERUPT
   macro instead of a CRITICAL_SECTION pair, which has a side effect of 
   incrementing 'critical_nestlevel, thereby inhibiting further dispatch
   operations.  Hence this function resets this flag to zero.
*/
void
force_dispatch(void);

/*---------------------------------------------------------------------------*/
/* Define BEGIN_CRITICAL_SECTION, END_CRITICAL_SECTION and DISABLE_INTERRUPT
   macros.  These bracket critical code sections throughout the code
   and are used to disable interrupts.  The END_CRITICAL_SECTION macro
   enables interrupts at the end and also checks if a dispatch operation is 
   required.

   The author (hiro) of this has actually done something quite clever here,
   including a single open brace in the BEGIN_CRITICAL_SECTION definition 
   and placing the matching close brace in the END_CRITICAL_SECTION.  This 
   results in the compiler checking for matching statements, as well as 
   ensuring that the block local variable 'spsr_tmp' is maintained between 
   the two definitions.
   Some modifications made by me to ease the ARM port were to combine the
   'current_intmask()' & 'disint()' functions, by having 'disint()' 
   return the current value of the CPSR as well as disabling interrupts.
   Furthermore, the processor will usually run in Thumb mode although
   disabling interrupts requires the processor to be in ARM mode,
   which means an interworking function call is required for this
   operation.

   The CRITICAL_SECTIONs need to cope with two different occurrences.  In
   the usual situation, system calls are made by ordinary tasks.  In the
   course of making these system calls, events occur that may result in
   higher priority tasks being able to run.  However, within 'delayed
   dispatching' means that any switch to a new task waits until the last
   END_CRITICAL_SECTION (i.e the one that matches the BEGIN_CRITICAL_SECTION).
   When this is reached, an SWI is triggered within the 'dispatch()' 
   function and a context switch to the new process takes place.  In the
   second situation, a system call is made within an ISR and as before,
   dispatching waits until the end of the interrupt.  However, in this
   case an SWI does not need to be triggered and all that is required is
   to switch the current task and then to restore the new context.

   Note that DISABLE_INTERRUPT has a side-effect of incrementing 
   'critical_nestlevel', which must be cleared at a later stage for 
   dispatching to be enabled.
*/
#define BEGIN_CRITICAL_SECTION										\
		{															\
		INT spsr_tmp = disint();

#define END_CRITICAL_SECTION										\
		enaint(spsr_tmp);											\
		dispatch();													\
		}

#define DISABLE_INTERRUPT											\
		disint();

/*===========================================================================*/
#endif /* _CPU_STATUS_ */
