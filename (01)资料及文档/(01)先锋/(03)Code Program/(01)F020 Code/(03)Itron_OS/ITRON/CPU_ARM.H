/* cpu_arm.h
   CPU specific header file for uITRON based OS.
   Version 1.02

   Mods to Original File:
   ----------------------
   a) Added English & removed Japanese comments.
   b) Add prototypes for 'task_enter_count' and 'task_exit_count'.

   New Mods:
   ---------
   a) In version 1.02 make mods so that RTOS uses a separate hardware timer
      as the source of its clock, instead of sharing the 'MEAS_INT' interrupt.
	  This involves changing the 'TIMER_PERIOD' define to match the timer,
	  as well as changing the 'clear_hw_timer_interrupt' function prototype.
   
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

#ifndef	_CPU_ARM
#define	_CPU_ARM

#include "itron.h"

/*---------------------------------------------------------------------------*/
/* This structure stores the processor context.  See 'cpuarm.s', as this
   structure must match the processing performed the the ISR.
*/
typedef struct 
	{
	VW		spsr;		/* Saved program status register				*/	
	VW		ri[13];		/* User mode r0 to r12							*/
	VW		sp;			/* User mode 'sp' (r13)							*/
	VW		lr;			/* User mode 'lr' (r14)							*/
	VW		lr_xxx;		/* Banked 'xxx' mode 'lr', task return address	*/
	} CTXB;
	
/*---------------------------------------------------------------------------*/
extern BOOL		dispatch_enabled;
extern BOOL		interrupts_enabled;
extern INT		critical_nestlevel;

/*---------------------------------------------------------------------------*/
/* Defines for determination of the current processor mode.
   Original definitins below.  Dummies supplied for now.

	#define in_qtsk()		0
	#define in_ddsp()		(current_imask() < 15 || dispatch_disabled)
	#define in_indp()		((current_psw() & 0x80000000) == 0)
	#define in_loc()		(current_imask() < 15)
	#define	in_sysmode()	(in_indp() || ctxtsk->sysmode)
*/
#define	in_sysmode()	0

/*---------------------------------------------------------------------------*/
/* This 'function' returns TRUE if the processor is currently executing in
   a task independent portion.
   For more details regarding task-independent portions, see uITRON 3.0
   specification, section 1.6.  The end result, however, is that dispatching
   is not permitted while executing in a task independent portion, as there
   is no concept of an issuing task, and as a result it is not possible
   to execute system calls that place the current task in a WAIT state or
   implicitly specify the issuing task.  Furthermore, any dispatching must
   be delayed until the end of the task independent portion, an action 
   known as the "delayed dispatching" rule.
*/
BOOL
in_indp(void);

/*---------------------------------------------------------------------------*/
/* This 'function' returns TRUE if the processor has currently disabled
   dispatching.
*/
BOOL
in_ddsp(void);

/*---------------------------------------------------------------------------*/
/* This function returns TRUE if the processor is currently locked.  This
   means that interrupts have been disabled and dispatching disabled.
*/
BOOL
in_loc(void);

/*---------------------------------------------------------------------------*/
/* This 'function' returns TRUE if the processor is currently executing
   a quasi-task section of code.  The main feature of quasi-task sections
   is that they may specify the task (issuing task) which was executing 
   just before entering the quasi-task portion.  The task states are 
   defined here as they are in the task portions. It is even possible to 
   enter WAIT state inside a quasi-task portion.  Obviously, dispatching 
   occurs inside a quasi-task portion just as it does during execution of
   a task.  As a result, even though they are considered non-task portions,
   quasi-task portions such as OS extensions do not necessarily always
   execute with priority over task portions.  This contrasts greatly with
   the situation with interrupt handlers which always execute with priority
   over all of the tasks.
*/
#define in_qtsk()		0

/*===========================================================================*/
/* This 'function' returns TRUE if the processor is currently in a delayed
   dispatching mode of operation.  Dispatching of tasks in ITRON 
   specification may be temporarily disabled, due to delayed dispatching, 
   or when a system call is issued that disables dispatching.
   In this implementation, dispatching is disabled during CRITICAL_SECTIONS
   of code.
*/
extern BOOL
in_ddsp(void);

/*---------------------------------------------------------------------------*/
/* Disables interrupts, returning the value of the CPSR at the time of the
   function call.
*/
extern INT
disint(void);

/*---------------------------------------------------------------------------*/
/* Enables interrupts by writing the restoring the CPSR as previously saved.
*/
extern void
enaint(INT spsr);

/*---------------------------------------------------------------------------*/
/* Get the current value of the CPSR.
*/
INT
get_cpsr(void);

/*===========================================================================*/
/* TIMER_PERIOD provides the number of milliseconds per timer tick, which
   added to 'current_time' each millisecond.
   The remaining defines are present to permit the inclusion of system
   calls that allow time to be determined more accurately through the
   reading of the timer hardware & the determination of fractional
   TIMER_PERIOD's.  If these functions are chosen, then the time is
   output in units of microseconds instead of milliseconds.
*/
#define TIMER_PERIOD	20				/* Timer period (ms)			*/

#define TIMER_TICK		0
#define TO_TICK(msec)	((msec) * TIMER_TICK)
#define TO_USEC(tick)	0
#define GET_TOLERANCE	0
#define MAX_TICK		((TICK) 0xffffff)

typedef unsigned int	TICK;

/*---------------------------------------------------------------------------*/
/* Starts the hardware timer.
*/
extern int 
start_hw_timer(void);

/*---------------------------------------------------------------------------*/
/* Stops the hardware timer
*/
extern int 
terminate_hw_timer(void);

/*---------------------------------------------------------------------------*/
/* Clears the hardware timer interrupt.
   This uses the TIC1B timer, which is edge triggered, so all that needs to
   be done is to reset the timer by writing to the 'ResetEdgeTriggers'
   register.
*/
void 
clear_hw_timer_interrupt(void);

/*---------------------------------------------------------------------------*/
/* Returns the current value of the hardware timer register.
*/
extern int 
get_current_hw_time(void);

/*---------------------------------------------------------------------------*/
/* ???
*/
int 
fetch_hw_timer_interrupt(void);

/*---------------------------------------------------------------------------*/
/* This function is called on entry to a task as one of the last steps prior
   to context switching into a task.  The function reads the current count
   value from the hardware counter and stores it in the 'cntstart' field of
   the 'ctxtsk' task control block.
*/
void
task_enter_count(void);
	
/*---------------------------------------------------------------------------*/
/* This function is called on exit from a task as one of the last steps prior
   to entering an RTOS system call, which could result in preemption of the
   current task.  The function reads the current count value from the 
   hardware counter and stores it in the 'cntend' field of the 'ctxtsk' 
   task control block.  The accumulated count field 'cntaccum' is also 
   updated.
*/
void
task_exit_count(void);

/*===========================================================================*/
#endif /* _CPU_STATUS_ */
