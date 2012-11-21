/* cpu_arm.c
   CPU specific ARM mode module for uITRON based OS.
   Version 1.02
   Written by: Eamonn Glennon
   
   Mods:
   -----
   a) Version 1.01 modified to enable measurement of processor load imposed
      by each task.  This involves starting the TIC1A timer counter and
	  writing functions for storing counts on task entry and exit.
   b) In version 1.02 make changes so that RTOS uses a separate hardware 
      timer (Timer 1 B) for its clock.  This involves changing the
      'start_hw_timer', 'terminate_hw_timer' and 'clear_hw_timer_interrupt'
      functions.
*/
/*===========================================================================*/

#include "cpu_arm.h"
#include "kernel.h"
#include "task.h"
#include "..\dsp\gp4020.h"

/*---------------------------------------------------------------------------*/
/* These flags used to describe the current state of the processor.
   See the state transition diagram in the uITRON 3.0 specification, for 
   'loc_cpu'.  These values are used when determining the value returned
   by 'ref_sys'.  The values of these variables are determined by execution
   of the functions 'dis_dsp', 'ena_dsp', 'loc_cpu' & 'unl_cpu'.
*/
BOOL				dispatch_enabled;
BOOL				interrupts_enabled;
INT					critical_nestlevel;

/*---------------------------------------------------------------------------*/

#ifndef	_TICDEF								/* Copied from 'gp4020.h'	*/
#define	_TICDEF

/*---------------------------------------------------------------------------*/
/* These masks and defines refer to the 'Control and Status' word of the
   Firefly timer.
   To enable the counter the 'cHwEnPolBit' should be set.  The counter is
   started by setting the 'cSwCntlReq' bit and stopped by clearing this bit.
*/
#define	cHwEnPolBit		17				/* HW Enable Polarity: Enable=0	*/
#define	cHwEnPolMask	(0x1<<cHwEnPolBit)
#define	cSwCntlReqBit	18				/* SW Control Request: Halt=0	*/
#define	cSwCntlReqMask	(0x1<<cSwCntlReqBit)
#define	cTICModeMask	(0x3<<19)		/* Mode Bits Mask Value			*/
#define	cTICMode1		(0x1<<19)		/* Mode 1 Value					*/
#define	cTICMode2		(0x2<<19)		/* Mode 2 Value					*/

/* The TIC1A reload value is chosen so that the maximum output value may
   be differenced with other output values and still produce a valid (32
   bit) signed output without introducing overflow.  Using 0x3FFFFFFF
   instead of 0x7FFFFFFF produces a 1 bit margin.
*/
#define	cTIC1Prescale	0x000000FF
#define	cTIC1AReload	0x3FFFFFFF

/*---------------------------------------------------------------------------*/
/* Register definitions for the GP4020 counter/timer.  
   One of the two counter timers is used to measure each task's execution
   time during the program execution.
*/
typedef struct TIMERCOUNTER
	{
	unsigned int	ConStatA;			/* Timer A Control & Status		*/
	unsigned int	ReloadValueA;		/* Timer A Reload Value			*/
	unsigned int	CurrentValueA;		/* Timer A Current Value		*/
	unsigned int	RESERVED[0x5];		/* RESERVED area				*/
	unsigned int	ConStatB;			/* Timer B Control & Status		*/
	unsigned int	ReloadValueB;		/* Timer B Reload Value			*/
	unsigned int	CurrentValueB;		/* Timer B Current Value		*/
	} tTimerCounter;
	
/*---------------------------------------------------------------------------*/
#define	pTIC1		((volatile tTimerCounter *)0xE000E000)
#define	pTIC2		((volatile tTimerCounter *)0xE000F000)

/*---------------------------------------------------------------------------*/
#endif	/* _TICDEF	*/

/*===========================================================================*/
/* Disables interrupts, returning the value of the CPSR at the time of the
   function call.  This function must run in ARM supervisor or system mode.
   It should be noted that there are typically two ways in which interrupts
   may be disabled on a processor.  The first method, which is implemented
   here, involves setting the interrupt mask of the current program status 
   register.  However, as this register is part of the program context, 
   which is replaced during a dispatch operation, the situation may arise
   where previously disabled interrupts become enabled due to dispatching.
   The second method involves disabling interrupts by setting the interrupt
   mask at the interrupt peripheral device, which results in the avoidance
   of the above problem (although this method may not always be available).
   Furthermore, if interrupts have been disabled using the former method,
   but enabled using the latter, the end result is that interrupts are still
   disabled.

   (See uITRON spec, section 3.5, 'dis_int' description for a more detailed
   discussion).
*/
INT
disint(void)
	{
	int				Spsr;
	
	__asm																	\
		{																	\
		MRS		Spsr, CPSR				/* Save the CPSR				*/;	\
		MOV		r0,Spsr					/* Save CPSR in Spsr			*/;	\
		ORR		r0, r0, #0x80			/* Set I bit in saved CPSR		*/;	\
		MSR		CPSR, r0				/* Write new CPSR value			*/;	\
		}
	critical_nestlevel++;				/* Atomic with disabled IRQ		*/
	return(Spsr);
	}

/*---------------------------------------------------------------------------*/
/* Enables interrupts by writing the restoring the CPSR as previously saved,
   which is passed in via 'spsr'.
*/
void
enaint(INT Spsr)
	{
	if ( critical_nestlevel>0 )			/* Atomic with disabled IRQ		*/
		critical_nestlevel--;
	__asm
		{
		MOV		r0, Spsr				/* r0 set to spsr				*/;	\
		MSR		CPSR, r0				/* Restore supplied				*/	\
		}
	}

/*---------------------------------------------------------------------------*/
/* Get the current value of the CPSR.
*/
INT
get_cpsr(void)
	{
	int				cpsr;

	__asm
		{
		MRS		cpsr, CPSR				/* Save the CPSR				*/
		}
	return(cpsr);
	}

/*---------------------------------------------------------------------------*/
/* Original OS mode definitions produced below:
	#define in_qtsk()		0
	#define in_ddsp()		(current_imask() < 15 || dispatch_disabled)
	#define in_indp()		((current_psw() & 0x80000000) == 0)
	#define in_loc()		(current_imask() < 15)
	#define	in_sysmode()	(in_indp() || ctxtsk->sysmode)
*/
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
#define	cFIQMode	0x11
#define	cIRQMode	0x12
#define	cSWIMode	0x13

BOOL
in_indp(void)
	{
	INT				mode;
	
	mode = get_cpsr() & 0x1F;
	if ( cFIQMode<=mode && mode<=cSWIMode )
		return(TRUE);
	else
		return(FALSE);
	}

/*---------------------------------------------------------------------------*/
/* This 'function' returns TRUE if the processor has currently disabled
   dispatching.
*/
BOOL
in_ddsp(void)
	{
	return( !dispatch_enabled );
	}

/*---------------------------------------------------------------------------*/
/* This function returns TRUE if the processor is currently locked.  This
   means that interrupts have been disabled and dispatching disabled.
*/
BOOL
in_loc(void)
	{
	return( !interrupts_enabled && !dispatch_enabled );
	}

/*===========================================================================*/
/* The functions in this next section deal with the manipulation of the 
   hardware timer used by the OS.
*/
#define	cSwHwMask		(cSwCntlReqMask|cHwEnPolMask)
#define	cStartMode1A	(cSwHwMask|cTIC1APrescale|cTICMode1)
#define	cStartMode2A	(cSwHwMask|cTIC1APrescale|cTICMode2)

#define	cStartMode1B	(cSwHwMask|cTIC1BPrescale|cTICMode1|cIntEnMask)
#define	cStartMode2B	(cSwHwMask|cTIC1BPrescale|cTICMode2|cIntEnMask)

/*---------------------------------------------------------------------------*/
/* Starts the hardware timer.
*/
int 
start_hw_timer(void)
	{
	/* This counter is a count down counter.  The counter is placed in
	   Free Running (Mode1) in which the counter is automatically reloaded
	   when it reaches zero.  In order to avoid an excessive delay before
	   it's first reload, a reload must be forced by manipulation of the 
	   modes and registers.  This can be done with the following sequence 
	   of instructions, which sets up a particular reload value, puts the 
	   timer/counter into mode 2 and starts the clock, followed by stopping 
	   and restarting the clock in mode 1.
		pTIC1->ReloadValueA = cTIC1AReload;
		pTIC1->ConStatA = cSwCntlReqMask|cHwEnPolMask|cTIC1Prescale|cTICMode2;
		pTIC1->ConStatA &= ~cSwCntlReqMask;
		pTIC1->ConStatA = cSwCntlReqMask|cHwEnPolMask|cTIC1Prescale|cTICMode1;			
	*/
	pTIC1->ReloadValueA = cTIC1AReload;
	pTIC1->ConStatA = cStartMode2A;
	pTIC1->ConStatA &= ~cSwCntlReqMask;
	pTIC1->ConStatA = cStartMode1A;				/* Free Running Mode 1	*/

	/* Start RTOS 20 ms timer.
	*/
	pTIC1->ReloadValueB = cTIC1BReload;
	pTIC1->ConStatB = cStartMode2B;
	pTIC1->ConStatB &= ~cSwCntlReqMask;
	pTIC1->ConStatB = cStartMode1B;				/* Free Running Mode 1	*/
	
	return(0);
	}

/*---------------------------------------------------------------------------*/
/* Stops the hardware timer.
   Currently does nothing, but should mask IRQA at the interrupt peripheral.
   Alternatively, could reallocate the MEAS_INT interrupt line as a general
   purpose I/O and not an interrupt or do both.
   Masking the interrupt in the processor is dangerous, as this interupt
   is constantly masked and unmasked during critical sections, and could
   become enabled during the dispatch of a different task.
*/
int 
terminate_hw_timer(void)
	{
	pTIC1->ConStatA &= ~cSwCntlReqMask;				/* Halt Counter		*/
	pTIC1->ConStatA &= ~cHwEnPolMask;				/* De-Assert Enable	*/	

	pTIC1->ConStatB &= ~cSwCntlReqMask;				/* Halt Counter		*/
	pTIC1->ConStatB &= ~cHwEnPolMask;				/* De-Assert Enable	*/	

	return(0);
	}

/*---------------------------------------------------------------------------*/
#define	pIntCResetEdge	((volatile int *)0xE0006010)
#define	cIntCTic1TOB	0x00000080					/* TIC1B Timeout	*/

/*---------------------------------------------------------------------------*/
/* Clears the hardware timer interrupt.
   This uses the TIC1B timer, which is edge triggered, so all that needs to
   be done is to reset the timer by writing to the 'ResetEdgeTriggers'
   register.
*/
void 
clear_hw_timer_interrupt(void)
	{
	*pIntCResetEdge = cIntCTic1TOB;
	}

/*---------------------------------------------------------------------------*/
/* Returns the current value of the hardware timer register.
*/
int 
get_current_hw_time(void)
	{
	return(0);
	}
	
/*---------------------------------------------------------------------------*/
/* ???
*/
int 
fetch_hw_timer_interrupt(void)
	{
	return(0);
	}

/*---------------------------------------------------------------------------*/
/* Executed code in the event of a failure.
*/
void	
i_assert_fail(const char *expr,const char *file,unsigned int line)
	{

	}
	
/*---------------------------------------------------------------------------*/
/* This function is called on entry to a task as one of the last steps prior
   to context switching into a task.  The function reads the current count
   value from the hardware counter and stores it in the 'cntstart' field of
   the 'ctxtsk' task control block.
*/
void
task_enter_count(void)
	{
	register int	CurrentValueA;
	
	CurrentValueA = (int)pTIC1->CurrentValueA;
	if ( CurrentValueA<0 )					/* 1'st time through only	*/
		CurrentValueA = cTIC1AReload;
	ctxtsk->cntstart = CurrentValueA;
	}
	
/*---------------------------------------------------------------------------*/
/* This function is called on exit from a task as one of the last steps prior
   to entering an RTOS system call, which could result in preemption of the
   current task.  The function reads the current count value from the 
   hardware counter and stores it in the 'cntend' field of the 'ctxtsk' 
   task control block.  The accumulated count field 'cntaccum' is also 
   updated.
*/
void
task_exit_count(void)
	{
	register int	CurrentValueA;	
	register int	ExecTime;
	
	/* The counter is a count down timer that has been set with an initial 
	   value of 0x7FFFFFFF, so the accumulated count is calculated in reverse.
	   Note that the 'task_enter_count' and 'task_exit_count' code assumes
	   that the time delay between calls is generally low, which should 
	   always be the case given that tasks are being continually preempted.
	*/
	CurrentValueA = (int)pTIC1->CurrentValueA;
	if ( CurrentValueA<0 )					/* 1'st time through only	*/
		CurrentValueA = cTIC1AReload;	
	ctxtsk->cntend = CurrentValueA;
	ExecTime = (ctxtsk->cntstart - ctxtsk->cntend);
	if ( ExecTime<0 )
		ExecTime += cTIC1AReload;		/* 1 TIC error, but no overflow	*/
	ctxtsk->cntaccum += ExecTime;
	}

/*===========================================================================*/