/* task.h
   Header file for uITRON based OS - task module.
   Version 1.01

   Mods to Original File:
   ----------------------
   a) Added English & removed Japanese comments.
   b) Remove USE_SEPARATE_SSTACK stuff.
   c) Add variables for measuring processor load of each task and 
      measuring free stack space.
   
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

#ifndef _TASK_
#define _TASK_

/*---------------------------------------------------------------------------*/
typedef struct task_control_block		TCB;
typedef struct generic_control_block	GCB;
typedef struct prisem_control_block		PISCB;

/*---------------------------------------------------------------------------*/
#include "itron.h"
#include "queue.h"
#include "timer.h"
#include "winfo.h"

/*---------------------------------------------------------------------------*/
/* (1.6) Enumerated type for task/process states.
   Note that there does not appear to be an explicit mention of the RUN
   state as contained in the 'itron' specification.
*/
typedef enum 
	{
	TS_NONEXIST	= 0,					/* Non-created/deleted task		*/
	TS_READY	= 1,					/* Task READY to execute		*/
	TS_WAIT		= 2,					/* Waiting due to system call	*/
	TS_SUSPEND	= 4,					/* Forced to wait by other task	*/
	TS_WAITSUS	= 6,					/* Both WAIT & SUSPEND			*/
	TS_DORMANT	= 8						/* Task not executing or exited	*/
	} TSTAT;

/*---------------------------------------------------------------------------*/
/* Returns 'TRUE' if the task with state 'state' is alive.
   (i.e. not NONEXISTANT or DORMANT).
*/
extern BOOL
task_alive(TSTAT state);

/*---------------------------------------------------------------------------*/
/* Defines used to convert between the types INT and PRI through a removal
   or addition of an offset and application of the required cast.
   In the 'task_control_block' structure, items 'ipriority' and 'priority'
   are declared to be of type 'INT' instead of type 'PRI'.
*/
#define int_priority(x)		((INT)((x) - MIN_PRI))
#define ext_tskpri(x)		((PRI)((x) + MIN_PRI))

/*---------------------------------------------------------------------------*/
/* 'Task control block' for ITRON tasks.
   The 'tskque' structure is used to link the TCB into whatever queue is
   appropriate at the time.  For example, when the TCB is READY, it is 
   placed in the 'ready_queue', while if the task is waiting on some
   event it will be removed from the 'ready_queue' and placed in some
   other waiting queue.
*/
struct task_control_block 
	{
	QUEUE			tskque;			/* Task Queue structure				*/
	ID				tskid;			/* Task ID of the task				*/
	VP				exinf;			/* Extended information				*/
	ATR				tskatr;			/* Task attributes					*/
	FP				task;			/* Task start address				*/
	INT				ipriority;		/* Initial task priority			*/
	INT				stksz;			/* Stack size 						*/
	INT				priority;		/* Task priority					*/
	TSTAT			state;			/* Task state						*/
	WSPEC			*wspec;			/* Wait callback hooks				*/
	GCB				*wgcb;			/* Wait generic control block		*/
	INT				wupcnt;			/* Number of queued wakeup requests	*/
	INT				suscnt;			/* ??? - SUSPEND					*/
	INT				sysmode;		/* ???								*/
	
	#ifdef USE_POR
	RNO				wrdvno;			/* ???								*/
	#endif /* USE_POR */

	#ifdef USE_QTSK_PORTION
	INT				isysmode;		/* ???								*/
	#endif /* USE_QTSK_PORTION */

	#ifdef PRISEM_SPEC1
	PISCB			*pislist;		/* Priority Inheritance Semaphores	*/
	#endif /* PRISEM_SPEC1 */

	#ifdef USE_TASK_MAILBOX
	T_MSG			*tmq_head;		/* ???								*/
	T_MSG			*tmq_tail;		/* ???								*/
	#endif /* USE_TASK_MAILBOX */

	ER				*wercd;			/* Wait Error Code 					*/
	WINFO			winfo;			/* Wait information data			*/
	TMEB			wtmeb;			/* Wait timer event block			*/
	VP				istack;			/* Initial top of stack address		*/
	CTXB			tskctxb;		/* Task context						*/
	INT				stkmargin;		/* Task Stack-Margin				*/

									/* Task Processing Time Variables	*/
	UINT			cntstart;		/* HW Counter Start Value			*/
	UINT			cntend;			/* HW Counter End Value				*/
	UINT			cntend0;		/* Previous HW Counter End Value	*/
	UINT			cntaccum;		/* HW Counter Accumulated			*/
	UINT			cntaccum0;		/* Previous HW Counter End Value	*/
	};

/*---------------------------------------------------------------------------*/
/* 'ctxtsk' points the the task that is currently RUNNING.
   'ctxboffset' is an offset that must be added to the start address of the
   CTXB structure, such that LDMFD & STMFD instructions may be used to save
   and restore contexts to and from the CTXB structure.  Care needs to be
   taken with these because LDM & STM instructions always operate on the 
   lowest numbered register first and the result will therefor depend on
   the addressing mode selected.
*/
extern INT			ctxboffset;
extern TCB			*ctxtsk;

/*---------------------------------------------------------------------------*/
/* 'schedtsk' points to the highest priority READY task.
*/
extern TCB			*schedtsk;

/*---------------------------------------------------------------------------*/
/* 'tcb_table' contains the table of task control blocks for the OS.
*/
extern TCB			tcb_table[];

/*---------------------------------------------------------------------------*/
/* 'free_tcb' is a linked list of free TCB's ready for allocation.
*/
#ifndef _i_vcre_tsk
extern QUEUE		free_tcb;
#endif /* _i_vcre_tsk */

/*---------------------------------------------------------------------------*/
/* Defines for easy indexing into the TCB table given the task 'id'.
*/
#define get_tcb(id)	 		(&tcb_table[INDEX_TSK(id)])
#define get_tcb_self(id)	((id)==TSK_SELF ? ctxtsk : &tcb_table[INDEX_TSK(id)])

/*===========================================================================*/
/* Makes the task with task control block 'tch' dormant.
*/
extern void
make_dormant(TCB *tcb);

/*---------------------------------------------------------------------------*/
/* Changes the state of task with TCB 'tcb' to READY, places the task in
   the 'ready_queue' and triggers a dispatch operation.
*/
extern void
make_ready(TCB *tcb);

/*---------------------------------------------------------------------------*/
/* Removes task with TCB 'tcb' from the 'ready_queue'. If the deleted TCB is
   the highest priority task 'schedtsk', then 'schedtsk' is updated and a
   dispatch request initiated.
*/
extern void
make_non_ready(TCB *tcb);

/*---------------------------------------------------------------------------*/
/* Changes the priority of task 'tcb' to 'priority.
*/
extern void
change_task_priority(TCB *tcb,INT priority);

/*---------------------------------------------------------------------------*/
/* Rotates the 'ready_queue' tasks with the given 'priority'.
*/
extern void
rotate_ready_queue(INT priority);

/*---------------------------------------------------------------------------*/
/* ???
*/
extern void
rotate_ready_queue_run(void);

/*===========================================================================*/
#endif /* _TASK_ */
