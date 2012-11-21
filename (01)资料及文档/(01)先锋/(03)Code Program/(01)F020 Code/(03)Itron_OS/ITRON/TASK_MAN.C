/* task_man.c
   Task manager module for uITRON based OS.
   Version 1.01

   Mods to Original File:
   ----------------------
   a) Added English & removed Japanese comments.
   b) Removed DORMANT_STACK_SIZE, CHK_CTX2 and CHK_CTX1 code from 'i_ext_tsk'
      and 'i_exd_tsk' system calls.
   c) Added in code to check for stack margin on the system stack of the task,
      affecting functions '_cre_tsk' and 'i_ref_tsk'.
   d) Added code to provide output variables for measuring processor load
      imposed by each task.
	  
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

#include "check.h"
#include "cpu.h"
#include "kernel.h"
#include "ready_q.h"
#include "task.h"
#include "wait.h"

/* Stack space is allocated using the variable size memorypool manager,
   although allocation is from the one system pool memory manager.
 */
#define sys_get_stack	sys_get_blk
#define sys_rel_stack	sys_rel_blk

#define	stack_fill		0x47

/*---------------------------------------------------------------------------*/
/* Code to create the required task.
*/
#if !defined(_i_cre_tsk) || !defined(_i_vcre_tsk)

static ER
_cre_tsk(TCB *tcb,T_CTSK *pk_ctsk,INT stksz,INT sysmode)
	{
	VP			stack;
	extern VP	sys_get_blk(INT blksz);

	stack = sys_get_stack(stksz);
	if ( stack==0 )
		return(E_NOMEM);

	#ifndef _i_vcre_tsk
	if ( !SYS_TSKID(tcb->tskid) )
		{
		queue_delete(&(tcb->tskque));
		}
	#endif /* _i_vcre_tsk */

	/* The top-of-stack is calculated as the stack base address plus the
	   size of the stack.  This results in final address that overlaps the
	   MEMHDR block of the next memory block.  In practice, this is not a
	   problem because the stack is full-descending and is decremented
	   before new values are inserted (so the MEMHDR is not clobbered).
	   This could be reduced for reasons of safety, but is not required and
	   would only serve to ensure that the 'stksz' field stored in the TCB
	   did not acually reflect the actual stack space for the task.
	*/
	tcb->exinf = pk_ctsk->exinf;
	tcb->tskatr = pk_ctsk->tskatr;
	tcb->task = pk_ctsk->task;
	tcb->ipriority = int_priority(pk_ctsk->itskpri);
	tcb->stksz = stksz;
	tcb->istack = (VP)(((VB *)stack) + stksz);

	/* This is a new attribute used to enable stack usage to be 
	   monitored.  If the TA_STKCHK attribute is set, the stack is filled
	   with a particular 'stack_fill' value during initialization of 
	   the task.  A search for the first stack location where this 
	   'stack_fill' does not occur can then be made, starting from 
	   the filled top of stack (lowest address, since stack grows downwards,
	   to determine the worst case stack usage of that task to that time.
	*/
	if ( tcb->tskatr & TA_STKCHK )
		{
		memset((VP)((UW *)stack),stack_fill,stksz);
		tcb->stkmargin = stksz;
		}
	
	#ifdef USE_QTSK_PORTION
	tcb->isysmode = sysmode;
	#else /* USE_QTSK_PORTION */
	tcb->sysmode = sysmode;
	#endif /* USE_QTSK_PORTION */

	make_dormant(tcb);
	return(E_OK);
	}

#endif

/*---------------------------------------------------------------------------*/
/* This system call creates the task specified by tskid.  Specifically, a 
   TCB (Task Control Block) is allocated for the task to be created, and 
   initialized according to accompanying parameter values of 'itskpri', 
   'task', 'stksz', etc.  A stack area is also allocated for the task based 
   on the parameter stksz.
*/
#ifndef _i_cre_tsk

SYSCALL ER
i_cre_tsk(ID tskid,T_CTSK *pk_ctsk)
	{
	TCB		*tcb;
	INT		stksz, sysmode;
	ER		ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_TSKACV(tskid);
	if ( SYS_TSKID(tskid) )
		{
		CHECK_RSATR(pk_ctsk->tskatr,TA_HLNG|TA_SSTKSZ);
		}
	else
		{
		CHECK_RSATR(pk_ctsk->tskatr,TA_HLNG|TA_SSTKSZ|TA_RNG3);
		}
	CHECK_PRI(pk_ctsk->itskpri);
	CHECK_PAR(pk_ctsk->stksz >= 0);

#if 0
	if ( pk_ctsk->tskatr & TA_SSTKSZ )
		{
		CHECK_PAR(pk_ctsk->stksz >= MIN_SYS_STACK_SIZE);
		stksz = pk_ctsk->stksz;
		}
	else
		{
		stksz = DEF_SYS_STACK_SIZE;
		}
#else
CHECK_PAR(pk_ctsk->stksz >= MIN_SYS_STACK_SIZE);
stksz = pk_ctsk->stksz;
#endif

	stksz = ((stksz>>2)<<2);			/* Word align the stack size	*/
	if ( (pk_ctsk->tskatr & TA_RNG3)==TA_RNG0 )
		{
		sysmode = 1;
		}
	else
		{
		sysmode = 0;
		}
	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	if ( tcb->state != TS_NONEXIST )
		{
		ercd = E_OBJ;
		}
	else
		{
		ercd = _cre_tsk(tcb,pk_ctsk,stksz,sysmode);
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_cre_tsk */

/*---------------------------------------------------------------------------*/
#ifndef _i_vcre_tsk

/* Implements a modified form of the 'cre_tsk' system call, which differs
   from the standard form in that the 'tskid' does not need to be 
   specified, but is calculated and returned by the function.
*/
SYSCALL ER
i_vcre_tsk(T_CTSK *pk_ctsk)
	{
	TCB		*tcb;
	INT		stksz, sysmode;
	ER		ercd = E_OK;

	CHECK_RSATR(pk_ctsk->tskatr,TA_HLNG|TA_SSTKSZ|TA_RNG3);
	CHECK_PRI(pk_ctsk->itskpri);
	CHECK_PAR(pk_ctsk->stksz >= 0);

	if (pk_ctsk->tskatr & TA_SSTKSZ)
		{
		CHECK_PAR(pk_ctsk->stksz >= MIN_SYS_STACK_SIZE);
		stksz = pk_ctsk->stksz;
		}
	else
		{
		stksz = DEF_SYS_STACK_SIZE;
		}
	if ((pk_ctsk->tskatr & TA_RNG3) == TA_RNG0)
		{
		sysmode = 1;
		}
	else
		{
		sysmode = 0;
		}

	BEGIN_CRITICAL_SECTION;
	if ( queue_empty_p(&free_tcb) )
		{
		ercd = EV_FULL;
		}
	else
		{
		tcb = (TCB *)(free_tcb.next);
		ercd = _cre_tsk(tcb,pk_ctsk,stksz,sysmode);
		if ( ercd == E_OK )
			{
			ercd = (ER)(tcb->tskid);
			}
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_vcre_tsk */

/*---------------------------------------------------------------------------*/
/* Delete the task with TCB 'tcb', releasing stack space and setting the
   task state to TS_NONEXIS.
*/
#if !defined(_i_del_tsk) || !defined(_i_exd_tsk)

static void
_del_tsk(TCB *tcb)
	{
	extern void		sys_rel_blk(VP blk);	
	
	sys_rel_stack((VP)((VB *)(tcb->istack) - tcb->stksz));
	#ifndef _i_vcre_tsk
	if ( !SYS_TSKID(tcb->tskid ))
		{
		queue_insert(&(tcb->tskque),&free_tcb);
		}
	#endif /* _i_vcre_tsk */
	tcb->state = TS_NONEXIST;
	}

#endif

/*---------------------------------------------------------------------------*/
/* This system call deletes the task specified by 'tskid'.  Specifically, it
   changes the state of the task specified by 'tskid' from DORMANT into
   NON-EXISTENT (a virtual state not existing on the system), and then 
   clears the TCB and releases stack. An E_OBJ error results if this system 
   call is used on a task which is not DORMANT.
   After deletion, another task having the same ID number can be created.
*/
#ifndef _i_del_tsk

SYSCALL ER
i_del_tsk(ID tskid)
	{
	TCB		*tcb;
	TSTAT	state;
	ER		ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_TSKACV(tskid);
	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	if ((state = tcb->state) != TS_DORMANT) 
		{
		ercd = (state == TS_NONEXIST) ? E_NOEXS : E_OBJ;
		}
	else 
		{
		_del_tsk(tcb);
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_del_tsk */

/*---------------------------------------------------------------------------*/
/* This system call starts the task specified by 'tskid'.  Specifically, it
   changes the state of the task specified by tskid from DORMANT into 
   RUN/READY.
   'stacd' can be used to specify parameters to be passed to the task when 
   it is started.  This parameter can be read by the task being started, 
   and may be used for transmitting simple messages.
*/
#ifndef _i_sta_tsk

/*---------------------------------------------------------------------------*/
/* This function ensures that the 'stacd' task argument is placed in the
   correct location to ensure that the function actually receives the
   argument.  The ARM C compiler places function arguments in registers,
   with the first argument being placed in r0.  Hence this function places
   'stacd' in 'r0'.
*/	
static void
setup_stacd(TCB *tcb,INT stacd)
	{
	tcb->tskctxb.ri[0] = stacd;
	}
/*...........................................................................*/

SYSCALL ER
i_sta_tsk(ID tskid,INT stacd)
	{
	TCB		*tcb;
	TSTAT	state;
	ER		ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_TSKACV(tskid);
	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	if ( (state=tcb->state) != TS_DORMANT )
		{
		ercd = (state == TS_NONEXIST) ? E_NOEXS : E_OBJ;
		}
	else
		{
		setup_stacd(tcb,stacd);
		make_ready(tcb);
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_sta_tsk */

/*---------------------------------------------------------------------------*/
#if !defined(_i_ext_tsk) || !defined(_i_exd_tsk) || !defined(_i_ter_tsk)

void
_ter_tsk(TCB *tcb)
	{
	TSTAT	state;

	state = tcb->state;
	if ( state == TS_READY )
		{
		make_non_ready(tcb);
		}
	else if ( state & TS_WAIT)
		{
		wait_cancel(tcb);
		#ifdef USE_REL_WAI_HOOK
		if ( tcb->wspec->rel_wai_hook )
			{
			(*(tcb->wspec->rel_wai_hook))(tcb);
			}
		#endif /* USE_REL_WAI_HOOK */
		}
	#ifdef PRISEM_SPEC1
	signal_all_prisem(tcb);
	#endif /* PRISEM_SPEC1 */
	}

#endif

/*---------------------------------------------------------------------------*/
/* This system call causes the issuing task to exit, changing the state of 
   the task into the DORMANT state.
   When a task exits due to 'ext_tsk', that task does not automatically 
   release all the resources (memory blocks, semaphores, etc.) which it had 
   obtained prior to the system call.  It is the user's responsibility that 
   all resources are released beforehand.
*/
#ifndef _i_ext_tsk

SYSCALL void
i_ext_tsk(void)
	{

	DISABLE_INTERRUPT;
	_ter_tsk(ctxtsk);
	make_dormant(ctxtsk);

	force_dispatch();
	}

#endif /* _i_ext_tsk */

/*---------------------------------------------------------------------------*/
/* This system call causes the issuing task to exit and then delete itself.  
   In other words the state of the issuing task changes into the NON-EXISTENT
   (a virtual state not existing on the system).
*/
#ifndef _i_exd_tsk

SYSCALL void
i_exd_tsk(void)
	{

	DISABLE_INTERRUPT;
	_ter_tsk(ctxtsk);

	/* Delete the current task and force a dispatch.
	*/
	_del_tsk(ctxtsk);
	force_dispatch();
	}

#endif /* _i_exd_tsk */

/*---------------------------------------------------------------------------*/
/* This system call forcibly terminates the task specified by 'tskid'.  
   That is, it changes the state of the task specified by tskid into DORMANT.
   Even if the target task is in wait state (including SUSPEND state), its 
   wait state will be released and then it will be terminated.  If the target
   task is on a queue of some sort (such as waiting for a semaphore), it will
   be removed from that queue by 'ter_tsk'.
*/
#ifndef _i_ter_tsk

SYSCALL ER
i_ter_tsk(ID tskid)
	{
	TCB		*tcb;
	TSTAT	state;
	ER		ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_TSKACV(tskid);
	CHECK_NONSELF(tskid);
	CHECK_INTSK();
	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	state = tcb->state;
	if ( !task_alive(state) )
		{
		ercd = (state == TS_NONEXIST) ? E_NOEXS : E_OBJ;
		}
	else
		{
		_ter_tsk(tcb);
		make_dormant(tcb);
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_ter_tsk */

/*---------------------------------------------------------------------------*/
/* This system call changes the current priority of the task specified by 
   'tskid' to the value specified by 'tskpri'.
   Under uITRON 3.0 specification, at least any value of 1 through 8 can be
   specified as task priority.  The smaller the value, the higher the 
   priority.
*/
#ifndef _i_chg_pri

SYSCALL ER
i_chg_pri(ID tskid, PRI tskpri)
	{
	TCB		*tcb;
	ER		ercd = E_OK;

	CHECK_TSKID_SELF(tskid);
	CHECK_TSKACV(tskid);
	CHECK_PRI_INI(tskpri);
	tcb = get_tcb_self(tskid);

	BEGIN_CRITICAL_SECTION;
	if ( tcb->state == TS_NONEXIST )
		{
		ercd = E_NOEXS;
		}
	else if ( tskpri == TPRI_INI )
		{
		change_task_priority(tcb,tcb->ipriority);
		}
	else
		{
		change_task_priority(tcb,int_priority(tskpri));
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_chg_pri */

/*---------------------------------------------------------------------------*/
/* This system call rotates tasks on the ready queue associated with the 
   priority level specified by 'tskpri'.  Specifically, the task at the head 
   of the ready queue of the priority level in question is moved to the 
   end of the ready queue, thus switching the execution of tasks having the 
   same priority.  Round robin scheduling may be implemented by periodically 
   issuing this system call in a given period of time.
   When rot_rdq is issued by task portions with tskpri = TPRI_RUN = 0, the 
   ready queue with the priority level of the issuing task is rotated.
*/
#ifndef _i_rot_rdq

SYSCALL ER
i_rot_rdq(PRI tskpri)
	{
	CHECK_PRI_RUN(tskpri);

	BEGIN_CRITICAL_SECTION;
	if ( tskpri == TPRI_RUN )
		{
		if ( in_indp() )			/* ???	*/
			{
			rotate_ready_queue_run();
			}
		else
			{
			rotate_ready_queue(ctxtsk->priority);
			}
		}
	else
		{
		rotate_ready_queue(int_priority(tskpri));
		}
	END_CRITICAL_SECTION;
	return(E_OK);
	}

#endif /* _i_rot_rdq */

/*---------------------------------------------------------------------------*/
/* This system call forcibly releases WAIT state (not including SUSPEND 
   state) of the task specified by 'tskid'.
   An E_RLWAI or EN_RLWAI error is returned to the task whose WAIT state has 
   been released using rel_wai.  If the task specified by 'tskid' is in WAIT 
   state, the WAIT state is released, otherwise an E_OBJ error will be 
   returned to the issuer.
*/
#ifndef _i_rel_wai

SYSCALL ER
i_rel_wai(ID tskid)
	{
	TCB		*tcb;
	TSTAT	state;
	ER		ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_TSKACV(tskid);
	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	if ( !((state = tcb->state) & TS_WAIT) )
		{
		ercd = (state == TS_NONEXIST) ? E_NOEXS : E_OBJ;
		}
	else
		{
		*(tcb->wercd) = E_RLWAI;
		wait_release(tcb);
		#ifdef USE_REL_WAI_HOOK
		if (tcb->wspec->rel_wai_hook)
			{
			(*(tcb->wspec->rel_wai_hook))(tcb);
			}
		#endif /* USE_REL_WAI_HOOK */
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_rel_wai */

/*---------------------------------------------------------------------------*/
/* This system call gets the ID of the issuing task, placing the result in
   the address specified by 'p_tskid'.
*/
#ifndef _i_get_tid

SYSCALL ER
i_get_tid(ID* p_tskid)
	{
	*p_tskid = in_indp() ? FALSE : ctxtsk->tskid;
	return(E_OK);
	}

#endif /* _i_get_tid */

/*---------------------------------------------------------------------------*/
/* Returns the number of bytes with the specified value 'c' , starting at 
   the specified address 'pstk_lo', the maximum top of stack down to the base of 
   the stack (low memory to high memory).  This is used to determine the 
   free stack margin.
*/
int
get_margin_ls(void *pstk_lo,unsigned char c)
	{
	register int			free_bytes = 0;
	register unsigned char 	*p0 = (unsigned char *)pstk_lo;

	while ( *p0++ == c )
		free_bytes++;
    return(free_bytes);
	}
	
/*---------------------------------------------------------------------------*/
/* Estimate the amount of spare stack capacity given the maximum address
   of the stack 'pstk_hi' and the minimum address in the stack 'pstk_lo', as
   well as the fill character 'c' used to initialize the stack.
   
   The algorithm is different from the previous function 'get_margin_ls',
   in that a binary search strategy is employed.  As a result this algorithm
   actually attempts to find the first transition between the known preloaded
   stack values and whatever values are there due to normal use.
   Although this is a 'less' accurate method of solving the problem, it has
   the important advantage of not consuming so much processor time as to
   cause a significant priority inversion, especially when a significant 
   stack margin has been included.
   The estimate of the remaining stack space is returned.
*/
int
get_margin_bs(void *pstk_lo,void *pstk_hi,unsigned char c)
	{
	register unsigned short	*phi = (unsigned short *)pstk_hi;
	register unsigned short	*plo = (unsigned short *)pstk_lo;
	register unsigned short	*p0, *p1, *p2, *p3;
	register unsigned short	init_val;
	register int			size;

	/* When differencing pointers, the compiler produces a size number that
	   is consistent with the pointer type.  Hence the value produced for
	   'size' will have units of halfwords.
	   One problem with this type of test is that is makes the assumption
	   that a particular block of memory has not been accessed based only
	   on a small number of test samples.  It has been found that using
	   only one sample (viz. p0) can result in problems.  For this reason
	   for several test locations are tested and must all be clear before
	   that block is marked as clear.
	*/	
	init_val = (c<<8)|c;
	size = (phi - plo);									/* HalfWords	*/
	p0 = plo + size;
	p1 = plo + (size>>1);
	p2 = plo + (size>>2);
	p3 = plo + (size>>3);
	while ( size )
		{
		if ( *p0==init_val && *p1==init_val && *p2==init_val && *p3==init_val )
			plo = p0;
		else
			phi = p0;
		size >>= 1;
		p0 = plo + size;
		p1 = plo + (size>>1);
		p2 = plo + (size>>2);
		p3 = plo + (size>>3);		
		}
	p0 = (unsigned short *)pstk_lo;
	size = (plo - p0)<<1;				/* Convert HalfWords to Bytes	*/
	return(size);
	}

/*---------------------------------------------------------------------------*/
/* This system call refers to the state of the task specified by tskid, and
   returns its current priority (tskpri), its task state (tskstat), and its
   extended information (exinf) via the T_RTSK structure 'pk_rtsk'.
   An error code is also returned.
*/
#ifndef _i_ref_tsk

#undef	cTIC1AReload
#define	cTIC1AReload	0x7FFFFF

SYSCALL ER 
i_ref_tsk(T_RTSK *pk_rtsk,ID tskid)
	{
	TCB		*tcb;
	TSTAT	state;
	GCB		*wgcb;
	ER		ercd = E_OK;
	INT		stkmargin;
	VP		stacklo, stackhi;

	CHECK_TSKID_SELF(tskid);
	CHECK_TSKACV(tskid);
	tcb = get_tcb_self(tskid);

	BEGIN_CRITICAL_SECTION;
	if ((state = tcb->state) == TS_NONEXIST)
		{
		ercd = E_NOEXS;
		}
	else
		{
		pk_rtsk->exinf = tcb->exinf;
		pk_rtsk->tskpri = ext_tskpri(tcb->priority);
		if ( tcb==ctxtsk )
			pk_rtsk->tskstat = TTS_RUN;
		else
			pk_rtsk->tskstat = (UW)(state << 1);
		if ( state & TS_WAIT )
			{
			pk_rtsk->tskwait = tcb->wspec->tskwait;
			wgcb = tcb->wgcb;
			if ( wgcb )
				pk_rtsk->wid = wgcb->objid;
			else
				pk_rtsk->wid = 0;
			}
		else
			{
			pk_rtsk->tskwait = 0;
			pk_rtsk->wid = 0;
			}
		pk_rtsk->wupcnt = tcb->wupcnt;
		pk_rtsk->suscnt = tcb->suscnt;

		/* Check for current stack usage if required.  Note that this 
		   call could result in a long interrupt latency, depending on the
		   amount of free stack space.  Remember that as the stack grows
		   downward, the maximum stack element is the initial stack address
		   'istack' minus the stack size 'stksz' (in bytes).
		   In order to try to speed up the search, the maximum address for
		   the stack space is read directly from the stored context for
		   that task.  i.e. 'stackhi' is initialised using 'tcb->tskctxb.sp'
		   instead of 'tcb->istack'.
		   Also copy the current stack size and start of stack addresses.
		*/
		if ( tcb->tskatr & TA_STKCHK )
			{
			stacklo = (VB *)tcb->istack - tcb->stksz;
			stackhi = (VB *)tcb->tskctxb.sp;	/* Larger than 'istack'	*/
			// stkmargin = get_margin_ls((VP)stacklo,stack_fill);
			stkmargin = get_margin_bs((VP)stacklo,(VP)stackhi,stack_fill);
			if ( stkmargin<tcb->stkmargin )
				tcb->stkmargin = stkmargin;
			}
		pk_rtsk->stkmargin = tcb->stkmargin;
		pk_rtsk->stksz = tcb->stksz;
		pk_rtsk->istack = tcb->istack;
		
		/* Update counts for measuring processor time.
		*/
		pk_rtsk->totaltime = (cTIC1AReload - tcb->cntend);
		pk_rtsk->runtime = tcb->cntaccum;
		pk_rtsk->totaltime0 = (cTIC1AReload - tcb->cntend0);
		pk_rtsk->runtime0 = tcb->cntaccum0;

		tcb->cntaccum0 = tcb->cntaccum;
		tcb->cntend0 = tcb->cntend;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_ref_tsk */

/*---------------------------------------------------------------------------*/
