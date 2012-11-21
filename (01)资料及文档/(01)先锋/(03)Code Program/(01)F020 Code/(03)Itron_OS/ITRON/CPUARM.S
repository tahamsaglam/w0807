; cpuarm.s
; Contains ARM code required by the OS
; Written by Eamonn Glennon
; Version 1.02
; Copyright (c) Sigtec Navigation Pty Ltd, 2000
;
; Notes:
; ------
; Assembly code for processing the timer/IRQ and SWI interrupts.  
; The following points can be made with regard to interrupts.
; a) In both the AT91 & the GP4020, multiple interrupts are multiplexed onto
;    the ARM7TDMI IRQ interrupt line.  For the GP4020, the ARM FIQ interrupt 
;    line shall be dedicated to handling the processing of the ACCUM_INT, 
;    BSIO & UART interrupts, which are completely independent of the OS.
; b) One of the most important interrupts is the timer interrupt.  For the 
;    OS running on the GP4020, this will be connected to the MEAS_INT
;    interrupt set to produce an interrupt at a period of 40ms (25Hz).
;    However, there may be other irregular interrupts that occur on the
;    IRQ line.
; c) When an interrupt occurs in such a system, the interrupt service
;    routine (ISR) can trigger changes in the program flow of control
;    by making system calls.  These system calls include sending a message,
;    setting an event flag, releasing a semaphore, etc.  At the end of 
;    such an interrupt, a call to the dispatcher should be made that will
;    usually result in a previously waiting task being READY to run, and
;    hence a context switch to this new task.
; d) To simplify the code, nested interrupts are not permitted.  This means
;    that further IRQ interrupts are masked during processing of the 
;    interrupt.  This occurs automatically with the ARM7TDMI.
; e) At the end of the interrupt, the interrupt routine should call the
;    ITRON 'ret_int' system call, which will result in a context switch
;    to a new task should this be required.  In practice, all this function
;    really does is set a flag indicating whether or not a context switch
;    is required.  Since any system calls made by the ISR are subject to
;    delayed dispatching, no actual context switches occur when these
;    system calls are made, even though pointers to the TCB that should
;    run next may be updated.  The required context switches are left to
;    the code that is run to return from an ISR.
; f) The ISR processing is performed as follows.
;	 An interrupt occurs and the code branches to the interrupt vector.
;    This in turn branches to the actual code to process the interrupt.
;    The first task of this code is to save the context of the preempted
;	 task, which is stored in a structure reserved for this purpose in 
;	 the TCB.  Since nested interrupts are not permitted, this is OK.
;	 It should also be noted that once the interrupt occurs, banked
;    stack pointer r13 and banked link register r14 are used.  This 
;    means that the original user mode r13 & r14 must be saved as 
;    part of the context, although there is no saved CPSR since the
;    interrupted task was in USER or SYSTEM mode (for which no SPSR
;    exists).  
; g) When the interrupt occurs, the processor switches from whatever mode 
;    it was in to ARM mode.  However, much of the OS code is written in 
;    Thumb code, so a switch from ARM to Thumb mode is required.  The
;    ARM processor adjusts the value stored in the 'lr' when the processor
;    is interrupted in 'Thumb' mode to ensure the same return method can
;    be used for both Thumb & ARM mode.
; h) There is a potential problem, in that while an SWI interrupt may not
;    interrupt an IRQ interrupt (since the IRQ handler does not call
;    code that results in an SWI instruction being called, as it checks the
;    mode first and does not call SWI if in IRQ mode), the converse is not 
;    necessarily true.  For this reason, it appears necessary to mask IRQ 
;    interrupts at the start of the SWI routine, which is done automatically 
;    as part of 'SaveContextMacro' macro.  Doing this should prevent any 
;    potential problems.  According to the ARM7TDMI data sheet there is no 
;    indication that entering an SWI will result in IRQ interrupts being 
;    masked.  However reading the "ARM Architectural Reference Manual" by 
;    Dave Jaggar indicates that an SWI does cause masking of IRQ instructions 
;    (see pages 3-75 & 6-81).
; i) This module also contains code that is used to measure the processing
;    time for each task.  It does this with the use of a hardware count-down
;    timer/counter.  Whenever a task is entered the 'cntstart' field of the 
;    task TCB is updated from the hardware timer.  Similarly, whenever a task 
;    is interrupted the value of the counter is stored in the 'cntend' field 
;    of the task TCB and the 'cntaccum' field is updated by accumulating the
;    difference between the above count values.
;
; The context is organised as follows:
;		Start of Context ->	SPSR_irq
;							r0
;							r1
;							...
;							r14/lr
;		End of Context ->	lr_irq (task return address)
;
; When the above context organisation was devised, it was the intention that
; restoration of the context would be possible in one instruction, using the
; "LDMIA r0, {r0-r15}^".  The problem that occurs is that the instruction 
; can either reload user mode registers or restore the CPSR from the 
; SPSR when the PC is loaded, but cannot do both simultaneously.
; This means that a two stage process is required, where the user mode
; registers are first restored, and then the CPSR and PC are restored.
; Luckily the banked r14 is available for use when actually trying to 
; do this.
;
; Mods:
; -----
; a) Version 1.01 modified to call 'task_enter_count' and 'task_exit_count'
;    on task entry and exit.  This function required for measuring processor
;    time consumed by each task. 
; b) Version 1.02 modified to permit RTOS to use TIC1B timer as its timer
;    source.  This requires that the IRQ interrupt handler actually check the
;    source of the interrupt since there are now two IRQ interrupts enabled,
;    namely 'MEAS_INT' and 'TIC1TOB'.
;
;==============================================================================

		AREA    CpuArm,CODE,READONLY,INTERWORK
		CODE32
		
		IMPORT	ctxtsk				
		IMPORT	ctxboffset
		IMPORT	dispatch_enabled
		IMPORT	schedtsk				
		IMPORT	timer_handler
		IMPORT	task_enter_count
		IMPORT	task_exit_count
		IMPORT	MeasIntIsr
		EXPORT  LoadContext
		EXPORT	IRQInterrupt
		EXPORT	SWIInterrupt
		EXPORT	ret_int
						
bCPSR_I         EQU     0x80				; CPSR I bit
bCPSR_F         EQU     0x40            	; CPSR F bit
cCPSR_USR       EQU     0x10            	; CPSR mode field - USR mode
cCPSR_FIQ       EQU     0x11            	; CPSR mode field - FIQ mode
cCPSR_IRQ       EQU     0x12            	; CPSR mode field - IRQ mode
cCPSR_SVC       EQU     0x13            	; CPSR mode field - SVC mode
cCPSR_ABT       EQU     0x17            	; CPSR mode field - ABT mode
cCPSR_UND       EQU     0x1B            	; CPSR mode field - UND mode
cCPSR_SYS       EQU     0x1F            	; CPSR mode field - SYS mode

cIntCAccumInt		EQU		0x00000002		; ACCUM_INT, Int-Channel 1
cIntCMeasInt		EQU		0x00000004		; MEAS_INT, Int-Channel 2
cIntCTic1TOB		EQU		0x00000080		; TIC 1 Time-out B, Int-Channel 7
cIntCBsio			EQU		0x00002000		; BSIO, Int-Channel 13
cIntCUart1Err		EQU		0x00004000		; UART1 Error, Int-Channel 14
cIntCUart1RxFull	EQU		0x00008000		; UART1 Rx Full, Int-Channel 15
cIntCUart1TxEmpty	EQU		0x00010000		; UART1 Tx Empty, Int-Channel 16
cIntCUart2Err		EQU		0x04000000		; UART2 Error, Int-Channel 26
cIntCUart2RxFull	EQU		0x08000000		; UART2 Rx Full, Int-Channel 27
cIntCUart2TxEmpty	EQU		0x10000000		; UART2 Tx Empty, Int-Channel 28

rIntCIrqStatus		EQU     0xE0006024		; INTC - IRQ Status Register
rIntCResetEdge		EQU		0xE0006010		; INTC - Reset edge triggers

;==============================================================================
; This macro is used to save the context of the current task into the 
; context area of the current TCB.  The macro uses stack space of the current
; processor mode for temporarily storing register values before those
; registers are saved in the context areas.  Further IRQ interrupts are also
; masked to ensure that the context save area is not overwritten by an 
; interrupt pre-emption.

        MACRO
        SaveContextMacro

		STMFD	r13!, {r0-r1}					; Temporary r0-r1 save
		MRS		r0, CPSR						; Save the CPSR in r0
		ORR		r0, r0, #0x80					; Set I bit in saved CPSR a2
		MSR		CPSR_cf, r0						; Write new CPSR value
		LDR		r0, =ctxtsk						; r0 = ctxtsk
		LDR		r0, [r0]
		LDR		r1, =ctxboffset					; r1 = ctxboffset
		LDR		r1, [r1]
		ADD		r0, r0, r1						; r0 = &ctxtsk->tskctxb
		MRS		r1, SPSR						; Save SPSR
		STMIA	r0!, {r1}
		
		STMIA	r0, {r0-r14}^					; Save user mode registers
		LDMFD	r13!, {r2-r3}					; Restore original r0,r1
		STMIA	r0, {r2-r3}						; Permanent r0-r1 save
		ADD		r0, r0, #60						; Offset to SPSR_??? & lr_???
		STMIA	r0!, {lr}						; Save lr_irq		

        MEND
				
;==============================================================================
; This macro is used for restoring the current task context from the current
; task TCB and transferring control back to that task.  The current stack is
; used for storing registers values while they are used, prior to restoration
; and return from the mode back to USER/SYSTEM mode.
; When the saved status register is restored, must use the c & f suffixes to
; ensure that both the condition code flags & the mode data is restored.
; 
        MACRO
        RestoreContextMacro

		LDR		r0, =ctxtsk						; r0 = ctxtsk
		LDR		r0, [r0]
		LDR		r1, =ctxboffset					; r1 = ctxboffset
		LDR		r1, [r1]
		ADD		r0, r0, r1						; r0 = &ctxtsk->tskctxb
		LDR		lr, [r0],#4						; lr_irq = SPSR
		MSR		SPSR_cf, lr						; Restore SPSR
		MOV		lr, r0							; lr_irq poionts to context
		LDMIA	lr, {r0-r14}^					; User mode r0-14 restored
		NOP
		ADD		lr, lr, #60						; lr_irq points to saved PC
		LDMIA	lr, {pc}^						; Return & change modes

		MEND
		
;==============================================================================
; Transfer the current task context 'ctxtsk->tskctxb' to the current 
; processor state.  This will result in the new task starting up.
; This routine is called at the start when the processor is in USER or 
; SYSTEM mode.
; As a shortcut, simply change the mode to IRQ mode, then call the same
; code used to return from an IRQ
;
LoadContext
        MOV     r0, #cCPSR_IRQ:OR:bCPSR_I:OR:bCPSR_F
        MSR     CPSR_c, r0
		B		ret_int

;------------------------------------------------------------------------------
; IRQ Interrupt routine.
;
IRQInterrupt
		SUB		lr, lr, #4						; IRQ lr return adjustment
		SaveContextMacro
;..............................................................................
; Test code to change all the register values - should get restored.  Not
; require once the code has been debugged and is working correctly.
;
;		MOV		r1,#0x0000001
;		MOV		r2,#0x0000002
;		MOV		r3,#0x0000004
;		MOV		r4,#0x0000008
;		MOV		r5,#0x0000010
;		MOV		r6,#0x0000020
;		MOV		r7,#0x0000040
;		MOV		r8,#0x0000080
;		MOV		r9,#0x0000100
;		MOV		r10,#0x0000200
;		MOV		r11,#0x0000400
;		MOV		r12,#0x0000800		
;..............................................................................		
; At this point the entire context has been stored in the TCB context
; structure.  The code can now determine the cause of the interrupt and
; branch to the appropriate routine.  The code can also switch from
; ARM mode back to Thumb mode if required.
; Since initially there is only one IRQ interrupt, simply branch to the 
; required routine.
;

TestIrqStatus
		LDR		r1,=rIntCIrqStatus				; r1 = IRQ Status register address
		LDR		r0,[r1]							; r0 = IRQ Status
		TST		r0, #cIntCMeasInt				; Test for MEAS_INT - high priorty
		BNE		IrqMeasInt
		TST		r0, #cIntCTic1TOB				; Test for TIC1TOB - low priority
		BNE		IrqTic1TOB
		
		B		ret_int							; Unknown IRQ Interrupt

;------------------------------------------------------------------------------
; Interrupt service routine to process MEAS_INT interrupt

IrqMeasInt
		BL		task_exit_count				; Update task end time
		LDR		r0, =|MeasIntIsr|
		MOV		lr, pc
		BX		r0

		B		ret_int						; 'ret_int' interrupt complete

;------------------------------------------------------------------------------
; Interrupt service routine to process TIC1TOB interrupt.  The interrupt
; is cleared within 'timer_handler'.

IrqTic1TOB
		BL		task_exit_count				; Update task end time
		LDR		r0, =|timer_handler|
		MOV		lr, pc
		BX		r0

		B		ret_int						; 'ret_int' interrupt complete
		
;------------------------------------------------------------------------------
; SWI Interrupt routine.  Note that SWI's do not require an 'lr' return
; adjustment.
;
SWIInterrupt
		SaveContextMacro					; No lr return ajustment for SWI
		
		BL		task_exit_count				; Update task end time		
		B		ret_int						; 'ret_int' interrupt complete

;------------------------------------------------------------------------------		
; Assign 'schedtsk' to 'ctxtsk'.
;
ret_int
		LDR		r0, =dispatch_enabled			; r0 = dispatch_enabled
		LDR		r0, [r0]
		TST		r0, #0x01						; if ( dispatch_enabled )
		BEQ		NoDispatch						
		LDR		r0, =schedtsk					;    r0 = schedtsk
		LDR		r0, [r0]
		LDR		r1, =ctxtsk						;    r1 = ctxtsk
		STR		r0, [r1]						;    ctxtsk = schedtsk
NoDispatch
		BL		task_enter_count				; Update task start time
		
;..............................................................................
; At this point interrupt processing is complete, so restore the context from 
; the newly updated current task.
;
 		RestoreContextMacro		
		
;==============================================================================
        END
