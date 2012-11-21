/* itron.h
   Header file for uITRON based OS.
   Version 1.01

   Mods to Original File:
   ----------------------
   a) Added English & removed Japanese comments.
   b) Removed T_CPOR & T_RPOR definition, i.e. ports &  rendezous.
   c) Added variables for outputing results of calls that indicate
      the amount of spare stack space per function and the processor
	  load of each function.  These variables included in the T_RTSK
	  structure.

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

#ifndef _ITRON_
#define _ITRON_

/* (6.4) General purpose data types.
*/
typedef	char			B;					/* Signed 8 bit integer		*/
typedef	short			H;					/* Signed 16 bit integer	*/
typedef	int				W;					/* Signed 32 bit integer	*/
typedef	unsigned char	UB;					/* Unsigned 8 bit integer	*/
typedef	unsigned short  UH;					/* Unsigned 16 bit integer	*/
typedef	unsigned int	UW;					/* Unsigned 32 bit integer	*/

typedef	char			VB;			/* Unpredictable 8 bit data type	*/
typedef	short			VH;			/* Unpredictable 16 bit data type	*/
typedef	int				VW;			/* Unpredictable 32 bit data type	*/
typedef	void			*VP;		/* Pointer to unpredictable type	*/

typedef	void			(*FP)();			/* Program start address	*/

/* Data Types Dependent on ITRON Specification:
   In order to clarify the meanings of certain parameters, the 
   following names are used for data types which appear frequently 
   and/or have special meanings.
*/
typedef int				INT;		/* Signed integer, processor width	*/
typedef unsigned int	UINT;		/* Unsigned integer, "        "     */
typedef INT				BOOL;		/* Boolean value: TRUE or FALSE		*/
typedef INT				FN;			/* Function code: Signed integer	*/
typedef	INT				ID;			/* Object ID number					*/
typedef	INT				BOOL_ID;	/* Boolean value or ID number		*/
typedef	INT				HNO;		/* Handler number					*/
typedef	INT				RNO;		/* Rendezvous number				*/
typedef	INT				NODE;		/* Node number						*/
typedef	UINT			ATR;		/* Object or handler attribute		*/
typedef	INT				ER;			/* Error code						*/
typedef	INT				PRI;		/* Task priority					*/
typedef	INT				TMO;		/* Timeout value					*/
typedef long long	SYSTIME;		/* System time data structure (ms)	*/
typedef SYSTIME		CYCTIME;		/* Cyclic time data structure		*/
typedef SYSTIME		ALMTIME;		/* ??? time data structure			*/
typedef INT 		DLYTIME;		/* Delay time data structure		*/
typedef long long	SYSUTIME;		/* Time data structure (us)			*/

/*---------------------------------------------------------------------------*/
/* (6.6) Common constant and data structure packet formats
 */
#define NADR		((VP) -1)		/* Invalid address or pointer		*/
#define TRUE		1				/* TRUE								*/
#define FALSE		0				/* FALSE							*/
#define TA_NULL		0				/* No particular attributes			*/

#define TA_ASM		0x00000000		/* Program written in assembly		*/
#define TA_HLNG		0x00000001		/* Program written in high level	*/

#define TA_TFIFO	0x00000000		/* Waiting tasks handled by FIFO	*/
#define TA_TPRI		0x00000001		/* Waiting tasks handled by Priority*/

#define TMO_POL		0				/* Polling							*/
#define TMO_FEVR	(-1)			/* Wait forever						*/

/*---------------------------------------------------------------------------*/
/* cre_tsk 
*/
typedef struct t_ctsk
	{
	VP				exinf;			/* Extended information				*/
	ATR				tskatr;			/* Task attributes					*/
	FP				task;			/* Task start address				*/
	PRI				itskpri;		/* Initial task priority			*/
	INT				stksz;			/* Stack size						*/
						/* Additional implementation dependend fields	*/
									/* INT	sstksz; ??? Stack size		*/
	} T_CTSK;

#define TA_COP0		0x00008000		/* ID=0‚ Coprocessor: tskatr		*/
#define TA_COP1		0x00004000		/* ID=1‚ Coprocessor: tskatr		*/
#define TA_COP2		0x00002000		/* ID=2‚ Coprocessor: tskatr		*/
#define TA_COP3		0x00001000		/* ID=3‚ Coprocessor: tskatr		*/
#define TA_COP4		0x00000800		/* ID=4‚ Coprocessor: tskatr		*/
#define TA_COP5		0x00000400		/* ID=5‚ Coprocessor: tskatr		*/
#define TA_COP6		0x00000200		/* ID=6‚ Coprocessor: tskatr		*/
#define TA_COP7		0x00000100		/* ID=7‚ Coprocessor: tskatr		*/

#define TA_SSTKSZ	0x00010000		/* ??? Stack size: ???				*/
#define	TA_STKCHK	0x00020000		/* Check stack margin enable		*/

#define TA_RNG0		0x00000000
#define TA_RNG1		0x01000000
#define TA_RNG2		0x02000000
#define TA_RNG3		0x03000000
									/* ItIs: (68040) 					*/
#define TA_SMODE	0x00000000		/* ??? System or supervisor mode	*/
#define TA_UMODE	0x01000000		/* ??? User mode					*/

#define TSK_SELF	0				/* Task specifies self				*/

#define TPRI_INI	0				/* Startup initial priority: chg_pri*/
#define TPRI_RUN	0				/* Highest execute priority: rot_rdq*/

/*---------------------------------------------------------------------------*/
/* ref_tsk 
*/
typedef struct t_rtsk
	{
	VP				exinf;			/* Extended information				*/
	PRI				tskpri;			/* Current priority					*/
	UINT			tskstat;		/* Task state						*/
												/* Extended features	*/
	UINT			tskwait;		/* Cause of wait					*/
	ID				wid;			/* ID of object being waited for	*/
	INT				wupcnt;			/* Wakeup request count				*/
	INT				suscnt;			/* SUSPEND request count			*/
	INT				stkmargin;		/* + Available stack margin (bytes)	*/
	INT				stksz;			/* + Stack size						*/
	VP				istack;			/* + Initial top of stack address	*/
	UINT			totaltime;		/* + Total Time (TICS)				*/
	UINT			runtime;		/* + Actual Running Time (TICS)		*/
	UINT			totaltime0;		/* + Previous Total Time			*/
	UINT			runtime0;		/* + Previous Actual Running Time	*/
	} T_RTSK;
													/* tskstat values	*/
#define TTS_RUN		0x00000001							/* RUN			*/
#define TTS_RDY		0x00000002							/* READY 		*/
#define TTS_WAI		0x00000004							/* WAIT 		*/
#define TTS_SUS		0x00000008							/* SUSPEND 		*/
#define TTS_WAS		0x0000000c							/* WAIT-SUSPEND */
#define TTS_DMT		0x00000010							/* DORMANT 		*/

													/* tskwait values	*/
#define TTW_SLP		0x00000001		/* Wait due to slp_tsk or tslp_tsk	*/
#define TTW_DLY		0x00000002		/* Wait due to dly_tsk 				*/
#define TTW_NOD		0x00000008		/* Connection function response wait*/
#define TTW_FLG		0x00000010		/* Wait due to wai_flg or twai_flg 	*/
#define TTW_SEM		0x00000020		/* Wait due to wai_sem or twai_sem	*/
#define TTW_MBX		0x00000040		/* Wait due to rcv_msg or trcv_msg	*/
#define TTW_SMBF	0x00000080		/* Wait due to snd_mbf or tsnd_mbf  */
#define TTW_MBF		0x00000100		/* Wait due to rcv_mbf or trcv_mbf	*/
#define TTW_CAL		0x00000200		/* Wait for rendezvous call 		*/
#define TTW_ACP		0x00000400		/* Wait for rendezvous accept 		*/
#define TTW_RDV		0x00000800		/* Wait for rendezvous completion	*/
#define TTW_MPL		0x00001000		/* Wait due to get_blk or tget_blk	*/
#define TTW_MPF		0x00002000		/* Wait due to get_blf or tget_blf	*/

													/* ItIs:???			*/
#define TTW_PIS		0x00010000		/* Wait due to 'pis'		 ???	*/
#define TTW_TMB		0x00020000		/* Wait due to 'tmb'		 ???	*/

/*---------------------------------------------------------------------------*/
/* Semaphores: cre_sem 
*/
typedef struct t_csem
	{
	VP				exinf;			/* Extended information				*/
	ATR				sematr;			/* Semaphore attributes				*/
											/* Extended functions		*/
	INT				isemcnt;		/* Initial semaphore count			*/
	INT				maxsem;			/* Maximum semaphore count			*/
	} T_CSEM;

/*---------------------------------------------------------------------------*/
/* Semaphores: ref_sem 
*/
typedef struct t_rsem
	{
	VP				exinf;			/* Extended information				*/
	BOOL_ID			wtsk;			/* Waiting task present flag		*/
	INT				semcnt;			/* Current semaphore count			*/
	} T_RSEM;

/*---------------------------------------------------------------------------*/
/* Event Flags: cre_flg 
*/
typedef struct t_cflg
	{
	VP				exinf;			/* Extended information				*/
	ATR				flgatr;			/* Event flag attributes			*/
	UINT			iflgptn;		/* Initial event flag				*/
	} T_CFLG;

#define TA_WSGL		0x00000000				/* Wait Single Task			*/
#define TA_WMUL		0x00000008				/* Wait Multiple Task		*/

#define TWF_ANDW	0x00000000						/* AND wait 		*/
#define TWF_ORW		0x00000002						/* OR wait			*/
#define TWF_CLR		0x00000001						/* Clear spec		*/

/*---------------------------------------------------------------------------*/
/* Event Flags: ref_flg 
*/
typedef struct t_rflg
	{
	VP				exinf;			/* Extended information				*/
	BOOL_ID			wtsk;			/* Waiting task present				*/
	UINT			flgptn;			/* Event flag bit pattern			*/
	} T_RFLG;

/*---------------------------------------------------------------------------*/
/* Mailbox Functions: cre_mbx 
*/
typedef struct t_cmbx
	{
	VP				exinf;			/* Extended information				*/
	ATR				mbxatr;			/* Mailbox attributes				*/
	} T_CMBX;

#define TA_MFIFO	0x00000000		/* Messages handled by FIFO			*/
#define TA_MPRI		0x00000002		/* Messages handled by Priority		*/

typedef struct t_msg
	{
	VP				msgque[1];		/* Message queue linkages			*/
	PRI				msgpri;			/* Message priority					*/
	VB				msgcont[1];		/* Message contents					*/
	} T_MSG;

/*---------------------------------------------------------------------------*/
/* Mailbox Functions: ref_mbx 
*/
typedef struct t_rmbx
	{
	VP				exinf;			/* Extended information				*/
	BOOL_ID			wtsk;			/* Waiting task present				*/	
	T_MSG			*pk_msg;		/* Message to be sent next			*/
	} T_RMBX;

/*---------------------------------------------------------------------------*/
/* Message-buffer Functions: cre_mbf
*/
typedef struct t_cmbf
	{
	VP				exinf;			/* Extended information				*/
	ATR				mbfatr;			/* Message buffer attributes		*/
	INT				bufsz;			/* Message Buffer size				*/
	INT				maxmsz;			/* Maximum size of messages			*/
	} T_CMBF;

#define TMBF_OS		(-4)			/* OS Error messages				*/
#define TMBF_DB		(-3)			/* Debugging error messages			*/

/*---------------------------------------------------------------------------*/
/* Message-buffer Functions: ref_mbf 
*/
typedef struct t_rmbf
	{
	VP				exinf;			/* Extended information				*/
	BOOL_ID			wtsk;			/* Waiting to Receive task present	*/
	BOOL_ID			stsk;			/* Waiting to Send task present		*/
	INT				msgsz;			/* Size of message to send next		*/
	INT				frbufsz;		/* Size of free buffer				*/
	} T_RMBF;

/*---------------------------------------------------------------------------*/
/* Port/Rendezous Functions: cre_por 
*/
typedef	struct t_cpor 
	{
	VP				exinf;			/* Extended information				*/	
	ATR				poratr;			/* Port Attributes					*/
	INT				maxcmsz;		/* ???								*/
	INT				maxrmsz;		/* ???								*/
	} T_CPOR;

/*---------------------------------------------------------------------------*/
/* Port/Rendezous Functions: cre_por 
*/
typedef struct t_rpor 
	{
	VP				exinf;			/* Extended information				*/		
	BOOL_ID			wtsk;			/* ??? - NFI						*/
	BOOL_ID			atsk;			/* ??? - NFI						*/
	} T_RPOR;

/*---------------------------------------------------------------------------*/
/* Priority Inheritance Semaphore: vcre_pis
*/
typedef struct t_cpis
	{
	VP				exinf;			/* Extended information				*/
	ATR				pisatr;			/* ??? Attributes					*/
	} T_CPIS;

/*---------------------------------------------------------------------------*/
/* Priority Inheritance Semaphore: vref_pis
*/
typedef struct t_rpis
	{
	VP				exinf;			/* Extended information				*/
	BOOL_ID			wtsk;			/* ???								*/
	BOOL_ID			pistsk;			/* ???								*/
	} T_RPIS;

/*---------------------------------------------------------------------------*/
/* Task Mailbox: vref_tmb 
*/
typedef struct t_rtmb
	{
	BOOL_ID			wtsk;			/* ???								*/
	T_MSG			*pk_msg;		/* ???								*/
	} T_RTMB;

/*---------------------------------------------------------------------------*/
/* Interrupt Management Functions: def_int 
*/
typedef struct t_dint
	{
	ATR				intatr;			/* Interrupt Handler attributes		*/
	FP				inthdr;			/* Interrupt Handler address		*/
											/* Extended functions		*/
	UINT			eitatr;			/* ??? attributes					*/
	} T_DINT;

#define TA_EITATR	0x00010000		/* ???								*/

/*---------------------------------------------------------------------------*/
/* Memory Pool Functions: cre_mpl 
*/
typedef struct t_cmpl
	{
	VP				exinf;			/* Extended information				*/
	ATR				mplatr;			/* Memorypool attributes			*/
	INT				mplsz;			/* Memorypool size					*/
											/* Extended functions		*/
	/* VP		mempool;				Memory pool start address		*/
	} T_CMPL;

#define TA_HFIT		0x00000000					/* Half-fit				*/
#define TA_FFIT		0x00010000					/* First-fit 			*/
#define TMPL_OS		(-4)						/* OS owned memorypool	*/

/*---------------------------------------------------------------------------*/
/* Memory Pool Functions: ref_mpl 
*/
typedef struct t_rmpl
	{
	VP				exinf;			/* Extended information				*/
	BOOL_ID			wtsk;			/* Task Waiting for memorypool		*/
	INT				frsz;			/* Total size of free memory		*/
	INT				maxsz;			/* Size of largest contiguous block	*/
	} T_RMPL;

/*---------------------------------------------------------------------------*/
/* Memory Pool Functions: cre_mpf 
*/
typedef struct t_cmpf
	{
	VP				exinf;			/* Extended information				*/
	ATR				mpfatr;			/* Memorypool attributes			*/
	INT				mpfcnt;			/* Entire memory pool block count	*/
	INT				blfsz;			/* Fixed size memory block size		*/
	} T_CMPF;

/*---------------------------------------------------------------------------*/
/* Memory Pool Functions: ref_mpf 
*/
typedef struct t_rmpf
	{
	VP				exinf;			/* Extended information				*/
	BOOL_ID			wtsk;			/* Task Waiting for memorypool		*/
	INT				frbcnt;			/* Free block count					*/
	} T_RMPF;

/*---------------------------------------------------------------------------*/
/* Timer Management Functions: def_cyc 
*/
typedef struct t_dcyc
	{
	VP				exinf;			/* Extended information				*/
	ATR				cycatr;			/* Cyclic handler attributes		*/
	FP				cychdr;			/* Cyclic handler address			*/
	UINT			cycact;			/* Cyclic handler activation		*/
	CYCTIME			cyctim;			/* Cyclic startup period			*/
	} T_DCYC;
													/* 'cycact' values	*/
#define TCY_OFF		0x00000000		/* Do not invoke cyclic handler		*/
#define TCY_ON		0x00000001		/* Invoke cyclic handler			*/
#define TCY_INI		0x00000002		/* Initialise cycle count			*/

/*---------------------------------------------------------------------------*/
/* Timer Management Functions: ref_cyc 
*/
typedef struct t_rcyc
	{
	VP				exinf;			/* Extended information				*/
	CYCTIME			lfttim;			/* Remaining time to handler start	*/
	UINT			cycact;			/* Cyclic handler activation		*/
	} T_RCYC;

/*---------------------------------------------------------------------------*/
/* Timer Management Functions: def_alm 
*/
typedef struct t_dalm
	{
	VP				exinf;			/* Extended information				*/
	ATR				almatr;			/* Alarm handler attributes			*/
	FP				almhdr;			/* Alarm handler address			*/
	UINT			tmmode;			/* Start time specification mode	*/
	ALMTIME			almtim;			/* Handler startup time				*/
	} T_DALM;
													/* 'tmmode' values	*/
#define TTM_ABS		0x00000000		/* Specified as absolute time		*/
#define TTM_REL		0x00000001		/* Specified as relative time		*/

/*---------------------------------------------------------------------------*/
/* Timer Management Functions: ref_alm 
*/
typedef struct t_ralm
	{
	VP				exinf;			/* Extended information				*/
	ALMTIME			lfttim;			/* Remaining time to handler start	*/
	} T_RALM;

/*---------------------------------------------------------------------------*/
/* System Management Functions: get_ver 
*/
typedef struct t_ver
	{
	UH				maker;			/* Vendor							*/
	UH				id;				/* Format number					*/
	UH				spver;			/* Specification version			*/
	UH				prver;			/* Product version					*/
	UH				prno[4];		/* Product control version			*/
	UH				cpu;			/* CPU variation					*/
	UH				var;			/* Variation descriptor				*/
	} T_VER;

/*---------------------------------------------------------------------------*/
/* System Management Functions: ref_sys 
*/
typedef struct t_rsys
	{
	INT				sysstat;		/* System state						*/
	ID				runtskid;		/* Run task ID	-???				*/
	ID				schedtskid;		/* Schedule task ID - ???			*/
	} T_RSYS;

/* 'sysstat' values:
    TSS_TSK is the normal state in which dispatching is enabled during
    task portion execution.  TSS_DDSP is the state after dis_dsp has been 
    executed during task portion execution (dispatch disabled).  TSS_LOC
    is the state after loc_cpu has been executed during task portion 
    execution (interrupt and dispatch disabled).  TSS_INDP is the state 
    during execution of task-independent portions (interrupt and timer 
    handlers).  TTS_QTSK is ???.
*/
#define TTS_TSK		0
#define TTS_DDSP	1
#define TTS_LOC		3
#define TTS_INDP	4
#define TTS_QTSK	8

/*---------------------------------------------------------------------------*/
/* System Management Functions: ref_cfg 
*/
typedef struct t_rcfg
	{
	ATR				dummy;		/* Not required for this implementation	*/
	} T_RCFG;

/*---------------------------------------------------------------------------*/
/* System Management Functions: def_svc 
*/
typedef struct t_dsvc
	{
	ATR				svcatr;			/* Extended SVC handler attributes	*/
	FP				svchdr;			/* Extended SVC handler address		*/
	} T_DSVC;

/*---------------------------------------------------------------------------*/
/* System Management Functions: def_exc 
*/
typedef struct t_dexc
	{
	ATR				excatr;			/* Exception handler attributes		*/
	FP				exchdr;			/* Exception handler address		*/
	} T_DEXC;

/*---------------------------------------------------------------------------*/
/* Network Management Functions:
*/
#define TND_SELF	0				/* Local node						*/
#define TND_OTHR	(-1)			/* Default remote node				*/

/*===========================================================================*/
#endif		/* _ITRON_ */

