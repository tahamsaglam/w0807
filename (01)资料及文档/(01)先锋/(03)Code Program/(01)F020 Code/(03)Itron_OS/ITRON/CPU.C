/* cpu.c
   CPU specific ARM mode module for uITRON based OS.
   Version:    0.01
   Written by: Eamonn Glennon
*/
/*===========================================================================*/

#include "check.h"
#include "cpu.h"
#include "kernel.h"
#include "task.h"

/*---------------------------------------------------------------------------*/
/* Dispatch request function.
   This function is called from 'reschedule', 'make_ready' & 'make_non_ready',
   all of which are contained in "task.c".
*/
void
dispatch_request(void)
	{
	/* This function is always called from within a CRITICAL_SECTION,
	   which means that a dispatch attempt will take place when the
	   END_CRITICAL_SECTION is reached.  Furthermore, for the same reason,
	   any call to 'dispatch' made here is guaranteed to have an early
	   exit.  The original code here has 'set_dir(14)', which seems as
	   though it set a hardware line that triggered an interrupt.
	   The conditions given above mean that this function is not actually
	   required for the moment and is currently left blank.
	*/
	}

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
dispatch(void)
	{
	if ( ctxtsk==schedtsk )		/* No dispatch required					*/
		return;
	if ( critical_nestlevel )	/* No dispatch in CRITICAL_SECTION		*/
		return;
	if ( in_ddsp() )			/* Dispatching disabled					*/
		return;
		
	if ( !in_indp() )			/* Not Task-independent, so dispatch OK	*/
		{
		__asm { SWI 4 };
		}
	}

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
force_dispatch(void)
	{
	if ( critical_nestlevel>0 )
		critical_nestlevel = 0;
	dispatch_enabled = TRUE;
	dispatch();
	}

/*---------------------------------------------------------------------------*/
/* This system call disables dispatching.
*/
#ifndef _i_dis_dsp

SYSCALL ER
i_dis_dsp(void)
	{
	CHECK_CTX( !in_loc() );
	dispatch_enabled = FALSE;
	return(E_OK);
	}

#endif /* _i_dis_dsp */

/*---------------------------------------------------------------------------*/
/* This system call enables dispatching.
*/
#ifndef _i_ena_dsp

SYSCALL ER
i_ena_dsp(void)
	{
	CHECK_CTX( !in_loc() );

	BEGIN_CRITICAL_SECTION;
	dispatch_enabled = TRUE;
	END_CRITICAL_SECTION;
	return(E_OK);
	}

#endif /* _i_ena_dsp */

/*---------------------------------------------------------------------------*/	
/* This system call locks the CPU by disabling interrupts and dispatching.
*/
#ifndef _i_loc_cpu

SYSCALL ER
i_loc_cpu(void)
	{
	INT		cpsr;
	
	CHECK_INTSK();

	cpsr = get_cpsr();			/* Disable interrupts in CPSR			*/
	cpsr |= 0xC0;				/* IRQ & FIQ disabled					*/
	enaint(cpsr);
	/* ????	*/					/* Enable interrupts in peripheral 		*/
	dispatch_enabled = FALSE;
	interrupts_enabled = FALSE;
	return(E_OK);
	}

#endif /* _i_loc_cpu */

/*---------------------------------------------------------------------------*/	
/* This system call unlocks the CPU by enabling interrupts and dispatching.
   This system call enables dispatching.  Care needs to be taken with respect
   to the use of CRITICAL_SECTION's.  If CRITICAL_SECTION's are used, then 
   some problems can occur because CRITICAL_SECTIONS disable interrupts in
   the CPSR, and then restore the previously saved CPSR at the end.  Hence
   it must be ensured that the restored CPSR has interrupts enabled, 
   regardless of the saved state.
*/
#ifndef _i_unl_cpu

SYSCALL ER
i_unl_cpu(void)
	{
	CHECK_INTSK();
	
	BEGIN_CRITICAL_SECTION;
	spsr_tmp &= ~0xC0;			/* 'spsr_tmp' in CRITICAL_SECTION block	*/
	dispatch_enabled = TRUE;
	interrupts_enabled = TRUE;
	/* ????	*/					/* Enable interrupts in peripheral 		*/
	END_CRITICAL_SECTION;	
	
	return(E_OK);
	}

#endif /* _i_unl_cpu */

/*===========================================================================*/
