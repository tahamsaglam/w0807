/* time_cal.c
   Time management functions for uITRON OS.

   Mods to Original File:
   ----------------------
   a) Added English & removed Japanese comments.

   Notes
   -----
   a) This module implements the timer functions of the uITRON OS,
      these being both cyclic handlers and alarm handlers.  The
	  code for both of these are similar, and as a result both use code
	  that is contained within the timer module 'timer.c'.
   b) The actual checking of the timers is performed in the 'timer_handler'
      function.
	  
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
#include "wait.h"
#include "check.h"

/*---------------------------------------------------------------------------*/
/* This system call sets the system clock to the time specified by SYSTIME
   'pk_tim'.  
*/
#ifndef _i_set_tim

SYSCALL ER
i_set_tim(SYSTIME *pk_tim)
	{
	CHECK_PAR(*pk_tim >= 0);

	BEGIN_CRITICAL_SECTION;
	current_time = *pk_tim;
	END_CRITICAL_SECTION;
	return(E_OK);
	}

#endif /* _i_set_tim */

/*---------------------------------------------------------------------------*/
/* This system call gets the current value of the system clock and returns 
   it in the return parameter SYSTIME 'pk_tim'.
*/
#ifndef _i_get_tim

SYSCALL ER
i_get_tim(SYSTIME *pk_tim)
	{
	BEGIN_CRITICAL_SECTION;
	*pk_tim = current_time;
	END_CRITICAL_SECTION;
	return(E_OK);
	}

#endif /* _i_get_tim */

/*---------------------------------------------------------------------------*/
/* This system call temporarily halts the execution of the task issuing 
   the call, and makes it enter the time elapse wait state.  The task halts 
   execution for the amount of time defined by 'dlytim'.
*/
#ifndef _i_dly_tsk

static WSPEC	wspec_dly = { TTW_DLY, 0, 0 };

SYSCALL ER
i_dly_tsk(DLYTIME dlytim)
	{
	ER	ercd = E_OK;

	CHECK_PAR(dlytim >= 0);
	CHECK_DISPATCH();

	if (dlytim > 0) 
		{
		/* Setup TCB for a delay and call 'make_wait', which makes the 
		   current task 'ctxtsk' non-ready, updates 'schedtsk' and
		   initiates a dispatch request.  Following this, the current
		   task is removed from whatever queue it was in ('ready_queue').
		   Note that no actual dispatching takes place until after the
		   last END_CRITICAL_SECTION is reached.
		   
		   ('wait.c' contains the function 'make_wait', which calls the
		   function 'make_non_ready' contained in 'task.c').
		*/
		BEGIN_CRITICAL_SECTION;
		ctxtsk->wspec = &wspec_dly;
		ctxtsk->wgcb = (GCB *) 0;				/* Wait GCB				*/
		ctxtsk->wercd = &ercd;					/* Local stack copy		*/
		make_wait(dlytim);	  					/* 'dispatch_request'	*/

		queue_initialize(&(ctxtsk->tskque));	/* ???					*/
		END_CRITICAL_SECTION;
		}
	return(ercd);
	}

#endif /* _i_dly_tsk */

/*===========================================================================*/
/* This next block of code is required if cyclic handlers have been enabled
   in the OS.
*/
#ifdef USE_CYC

/* Structure for controlling cyclic handlers.
*/
typedef struct cyclic_handler_control_block 
	{
	VP		exinf;					/* Extended information				*/
	ATR		cycatr;					/* Cyclic handler attributes		*/
	FP		cychdr;					/* Cyclic handler address			*/
	UINT	cycact;					/* Cyclic handler activation		*/
	CYCTIME	cyctim;					/* Cyclic startup period			*/
	TMEB	cyctmeb;				/* Associated timer control block	*/
	} CYCCB;

/*---------------------------------------------------------------------------*/
/* Table of cyclic handler control blocks and macro to aid in getting  the
   address of the required CYCCB.
*/	
CYCCB	cyccb_table[NUM_CYC];
#define get_cyccb(no)	(&(cyccb_table[(no)-1]))

/*---------------------------------------------------------------------------*/
/* Queue of free cyclic handler CYCCB's
*/
#ifndef _i_vdef_cyc
QUEUE	free_cyccb;
#endif /* _i_vdef_cyc */

/*---------------------------------------------------------------------------*/
/* Initialise the cyclic handler.
*/
void
cyclichandler_initialize(void)
	{
	INT	i;
	CYCCB	*cyccb;

	#ifndef _i_vdef_cyc
	queue_initialize(&free_cyccb);
	#endif /* _i_vdef_cyc */

	for ( cyccb = cyccb_table,i = 0; i<NUM_CYC; cyccb++,i++ ) 
		{
		cyccb->cychdr = (FP)NADR;
		#ifndef _i_vdef_cyc
		queue_insert((QUEUE *) cyccb, &free_cyccb);
		#endif /* _i_vdef_cyc */
		}
	}

/*---------------------------------------------------------------------------*/
/* This is a generic function used to implement cyclic handlers.  The
   cyclic handler control block CYCCB 'cyccb' is specified as a parameter to
   permit calling the handler function with its required parameter.

   Detailed Explanation:
   ---------------------
   The actual checking of cyclic-handler timers is performed by the function
   'timer_handler'.  However, this code treats all timers as "one-shot"
   timers, deleting the timer from the timer queue and then calling the
   timer callback function.  Hence, this function does not result in timers 
   being re-initialized after expiry, which is required under the uITRON 
   specification.  To meet this requirement, when a cyclic timer is defined,
   the generic function 'call_cychdr' is specified as the callback function
   (defined below), which then re-inserts a new timer into the timer queue
   and then calls the actual callback function if the cyclic handler is
   active.  In this way the timer will re-trigger at the specified rate.
*/
static void
call_cychdr(CYCCB *cyccb)
	{
	timer_insert_rel(&(cyccb->cyctmeb),&(cyccb->cyctim),(CBACK)call_cychdr,cyccb);
	if (cyccb->cycact & TCY_ON) 
		{
		(*cyccb->cychdr)(cyccb->exinf);
		}
	}

/*---------------------------------------------------------------------------*/

#if !defined(_i_def_cyc) || !defined(_i_vdef_cyc)

static void
_def_cyc(CYCCB *cyccb, T_DCYC* pk_dcyc)
	{
	#ifndef _i_vdef_cyc
	queue_delete((QUEUE *) cyccb);
	#endif /* _i_vdef_cyc */

	cyccb->exinf = pk_dcyc->exinf;
	cyccb->cycatr = pk_dcyc->cycatr;
	cyccb->cychdr = pk_dcyc->cychdr;
	cyccb->cycact = pk_dcyc->cycact;
	cyccb->cyctim = pk_dcyc->cyctim;
	timer_insert_rel(&(cyccb->cyctmeb),&(cyccb->cyctim),(CBACK)call_cychdr,cyccb);
	}

#endif /* !defined(_i_def_cyc) || !defined(_i_vdef_cyc) */

/*---------------------------------------------------------------------------*/

#ifndef _i_def_cyc

SYSCALL ER
i_def_cyc(HNO cycno, T_DCYC* pk_dcyc)
	{
	CYCCB	*cyccb;

	CHECK_PAR(0 < cycno && cycno <= NUM_CYC);
	if ( pk_dcyc != NADR ) 
		{
		CHECK_RSATR(pk_dcyc->cycatr, TA_HLNG);
		CHECK_PAR((pk_dcyc->cycact & ~(TCY_ON)) == 0);
		CHECK_PAR(pk_dcyc->cyctim > 0);
		}
	cyccb = get_cyccb(cycno);

	BEGIN_CRITICAL_SECTION;
	if ( cyccb->cychdr != (FP)NADR ) 
		{
		timer_delete(&(cyccb->cyctmeb));
		#ifndef _i_vdef_cyc
		queue_insert((QUEUE *)cyccb,&free_cyccb);
		#endif /* _i_vdef_cyc */
		}

	if ( pk_dcyc == NADR ) 
		{
		cyccb->cychdr = (FP)NADR;
		}
	else 
		{
		_def_cyc(cyccb,pk_dcyc);
		}
	END_CRITICAL_SECTION;
	return(E_OK);
	}

#endif /* _i_def_cyc */

/*---------------------------------------------------------------------------*/

#ifndef _i_vdef_cyc

SYSCALL ER
i_vdef_cyc(T_DCYC* pk_dcyc)
	{
	CYCCB	*cyccb;
	ER		ercd = E_OK;

	CHECK_PAR(pk_dcyc != NADR);

	BEGIN_CRITICAL_SECTION;
	if ( queue_empty_p(&free_cyccb) ) 
		{
		ercd =  EV_FULL;
		}
	else 
		{
		cyccb = (CYCCB *)(free_cyccb.next);
		_def_cyc(cyccb, pk_dcyc);
		ercd = (cyccb - cyccb_table) + 1;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_vdef_cyc */

/*---------------------------------------------------------------------------*/

#ifndef _i_act_cyc

SYSCALL ER
i_act_cyc(HNO cycno,UINT cycact)
	{
	CYCCB	*cyccb;
	ER		ercd = E_OK;

	CHECK_PAR(0 < cycno && cycno <= NUM_CYC);
	CHECK_PAR((cycact & ~(TCY_ON|TCY_INI)) == 0);
	cyccb = get_cyccb(cycno);

	BEGIN_CRITICAL_SECTION;
	if ( cyccb->cychdr == (FP)NADR ) 
		ercd = E_NOEXS;
	else 
		{
		cyccb->cycact = (cycact & TCY_ON);
		if ( cycact & TCY_INI ) 
			{
			timer_delete(&(cyccb->cyctmeb));
			timer_insert_rel(&(cyccb->cyctmeb),&(cyccb->cyctim),(CBACK)call_cychdr,cyccb);
			}
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_act_cyc */

/*---------------------------------------------------------------------------*/

#ifndef _i_ref_cyc

SYSCALL ER
i_ref_cyc(T_RCYC* pk_rcyc,HNO cycno)
	{
	CYCCB	*cyccb;
	ER		ercd = E_OK;

	CHECK_PAR(0 < cycno && cycno <= NUM_CYC);
	cyccb = get_cyccb(cycno);

	BEGIN_CRITICAL_SECTION;
	if ( cyccb->cychdr == (FP)NADR ) 
		ercd = E_NOEXS;
	else 
		{
		pk_rcyc->exinf = cyccb->exinf;
		pk_rcyc->lfttim = cyccb->cyctmeb.time - current_time;
		pk_rcyc->cycact = cyccb->cycact;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_ref_cyc */

/*---------------------------------------------------------------------------*/
#endif /* USE_CYC */

/*===========================================================================*/
/* This next block of code is required if cyclic handlers have been enabled
   in the OS.
*/
#ifdef USE_ALM

/* Structure for controlling alarm handlers.
*/
typedef struct alarm_handler_control_block 
	{
	VP		exinf;					/* Extended information				*/
	ATR		almatr;					/* Alarm handler attributes			*/
	FP		almhdr;					/* Alarm handler address			*/
	TMEB	almtmeb;				/* Associated timer control block	*/
	} ALMCB;

/*---------------------------------------------------------------------------*/
/* Table of alarm handler control blocks and macro to aid in getting  the
   address of the required CYCCB.
*/	
ALMCB	almcb_table[NUM_ALM];
#define get_almcb(no)	(&(almcb_table[(no)-1]))

/*---------------------------------------------------------------------------*/
/* Queue of free alarm CYCCB's
*/
#ifndef _i_vdef_alm
QUEUE	free_almcb;
#endif /* _i_vdef_alm */

/*---------------------------------------------------------------------------*/
/* Initialize alarm handler.
*/
void
alarmhandler_initialize(void)
	{
	INT		i;
	ALMCB	*almcb;

	#ifndef _i_vdef_alm
	queue_initialize(&free_almcb);
	#endif /* _i_vdef_alm */

	for ( almcb = almcb_table,i = 0; i < NUM_ALM; almcb++,i++ ) 
		{
		almcb->almhdr = (FP)NADR;
		#ifndef _i_vdef_alm
		queue_insert((QUEUE *)almcb,&free_almcb);
		#endif /* _i_vdef_alm */
		}
	}

/*---------------------------------------------------------------------------*/
/* This is a generic function used to implement alarm handlers.  The
   alarm handler control block ALMCB 'almcb' is specified as a parameter to
   permit calling the handler function with its required parameter.
   The 'almcb' is returned to the free-pool of alarm control blocks after
   use.
   See 'call_cychdr' comment for more information.
*/
static void
call_almhdr(ALMCB *almcb)
	{
	FP	almhdr;
	VP	exinf;

	almhdr = almcb->almhdr;
	exinf = almcb->exinf;
	almcb->almhdr = (FP)NADR;
	#ifndef _i_vdef_alm
	queue_insert((QUEUE *)almcb,&free_almcb);
	#endif /* _i_vdef_alm */
	(*almhdr)(exinf);
	}

/*---------------------------------------------------------------------------*/

#if !defined(_i_def_alm) || !defined(_i_vdef_alm)

static void
_def_alm(ALMCB *almcb,T_DALM* pk_dalm)
	{
	#ifndef _i_vdef_alm
	queue_delete((QUEUE *) almcb);
	#endif /* _i_vdef_alm */

	almcb->exinf = pk_dalm->exinf;
	almcb->almatr = pk_dalm->almatr;
	almcb->almhdr = pk_dalm->almhdr;
	if (pk_dalm->tmmode & TTM_REL) 
		{
		timer_insert_rel(&(almcb->almtmeb),&(pk_dalm->almtim),(CBACK)call_almhdr,almcb);
		}
	else 
		{
		timer_insert_abs(&(almcb->almtmeb),&(pk_dalm->almtim),(CBACK)call_almhdr,almcb);
		}
	}

#endif /* !defined(_i_def_alm) || !defined(_i_vdef_alm) */

/*---------------------------------------------------------------------------*/

#ifndef _i_def_alm

SYSCALL ER
i_def_alm(HNO almno,T_DALM* pk_dalm)
	{
	ALMCB	*almcb;
	ER		ercd = E_OK;

	CHECK_PAR(0 < almno && almno <= NUM_ALM);
	if (pk_dalm != NADR) 
		{
		CHECK_RSATR(pk_dalm->almatr, TA_HLNG);
		CHECK_PAR((pk_dalm->tmmode & ~(TTM_REL)) == 0);
		CHECK_PAR(pk_dalm->almtim > 0);
		}
	almcb = get_almcb(almno);

	BEGIN_CRITICAL_SECTION;
	#ifdef CHK_PAR
	if (pk_dalm != NADR && !(pk_dalm->tmmode & TTM_REL)	&& pk_dalm->almtim <= current_time ) 
		{
		ercd = E_PAR;
		goto error_exit;
		}
	#endif /* CHK_PAR */

	if ( almcb->almhdr != (FP)NADR ) 
		{
		timer_delete(&(almcb->almtmeb));
		#ifndef _i_vdef_alm
		queue_insert((QUEUE *) almcb, &free_almcb);
		#endif /* _i_vdef_alm */
		}

	if (pk_dalm == NADR) 
		{
		almcb->almhdr = (FP)NADR;
		}
	else 
		{
		_def_alm(almcb, pk_dalm);
		}

    error_exit:
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_def_alm */

/*---------------------------------------------------------------------------*/

#ifndef _i_vdef_alm

SYSCALL ER
i_vdef_alm(T_DALM* pk_dalm)
	{
	ALMCB	*almcb;
	ER		ercd = E_OK;

	CHECK_PAR(pk_dalm != NADR);
	CHECK_RSATR(pk_dalm->almatr,TA_HLNG);
	CHECK_PAR((pk_dalm->tmmode & ~(TTM_REL)) == 0);
	CHECK_PAR(pk_dalm->almtim > 0);

	BEGIN_CRITICAL_SECTION;
	#ifdef CHK_PAR
	if (!(pk_dalm->tmmode & TTM_REL) && pk_dalm->almtim <= current_time) 
		{
		ercd = E_PAR;
		goto error_exit;
		}
	#endif /* CHK_PAR */

	if (queue_empty_p(&free_almcb)) 
		{
		ercd =  EV_FULL;
		}
	else 
		{
		almcb = (ALMCB *)(free_almcb.next);
		_def_alm(almcb, pk_dalm);
		ercd = (almcb - almcb_table) + 1;
		}

    error_exit:
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_vdef_alm */

/*---------------------------------------------------------------------------*/

#ifndef _i_ref_alm

SYSCALL ER
i_ref_alm(T_RALM* pk_ralm,HNO almno)
	{
	ALMCB	*almcb;
	ER		ercd = E_OK;

	CHECK_PAR(0 < almno && almno <= NUM_ALM);
	almcb = get_almcb(almno);

	BEGIN_CRITICAL_SECTION;
	if ( almcb->almhdr == (FP)NADR ) 
		{
		ercd = E_NOEXS;
		}
	else 
		{
		pk_ralm->exinf = almcb->exinf;
		pk_ralm->lfttim = almcb->almtmeb.time - current_time;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_ref_alm */

/*---------------------------------------------------------------------------*/
#endif /* USE_ALM */

/*===========================================================================*/