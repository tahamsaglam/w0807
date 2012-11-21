/* check.h
   Header file for uITRON check tests.

   Notes & Mods:
   -------------
   a) Added English & removed Japanese comments.
   b) This file contains a series of 'define' definitions that are used for
      performing various parameter tests.  The use of 'defines' (instead of
	  functions) is necessary, as failure of any of these tests is required
	  to result in exit from the system call and the return of an error code
	  to the calling function.  If these were implemented as functions,
	  the error code would be returned to the system call and no exit would
	  take place.
   c) The use of error checking can be disabled by simply redefining the 
      defines as NULL defines.
   d) This file was originally a definition of defines which used another
      set of defines generated automatically by a PERL program.  Most DOS
	  or Windows users don't have access to PERL, so the generated defines
	  have been replaced with the tests that were created by the PERL
	  program.  Configuration of the OS will thereby involve direct
	  modification of a header file, instead of a file used as in input to
	  PERL.
   e) Removed any definitions relating to PORTS & rendezous.  Merged CHK_PAR
      and CHK_CTX definitions.
   f) Many of the 'comments' are taken cut & pasted from the specification.

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

#ifndef _CHECK_
#define _CHECK_

/*===========================================================================*/
/* Check for E_ID invalid ID number errors
   Since E_ID is only detected for objects which have IDs, it is never
   detected where interrupt vector numbers or handler numbers are concerned.
   An E_PAR error will result when static error checking for out-of-range 
   or reserved numbers is made for interrupt vector and handler numbers.
*/
#ifdef CHK_ID
/*---------------------------------------------------------------------------*/
#define CHECK_TSKID(tskid)			/* Check 'tskid'			*/		\
	if ( !CHK_TSKID(tskid) )											\
		return(E_ID);
/*---------------------------------------------------------------------------*/
#define CHECK_TSKID_SELF(tskid)		/* Check 'tskid' of self	*/		\
   	if ( !(!in_indp()) && tskid==TSK_SELF )								\
   		return(E_ID);													\
   	else if ( !CHK_TSKID(tskid) )										\
		return(E_ID);
/*---------------------------------------------------------------------------*/
#define CHECK_SEMID(semid)			/* Check 'semid'			*/		\
	if ( semid<MIN_SEMID || semid>MAX_SEMID )							\
		return(E_ID);
/*---------------------------------------------------------------------------*/
#define CHECK_FLGID(flgid)			/* Check 'semid'			*/		\
	if ( flgid<MIN_FLGID || flgid>MAX_FLGID )							\
		return(E_ID);					
/*---------------------------------------------------------------------------*/
#define CHECK_MBXID(mbxid)			/* Check 'mbxid'			*/		\
	if ( mbxid<MIN_MBXID || mbxid>MAX_MBXID )							\
		return(E_ID);					
/*---------------------------------------------------------------------------*/
#define CHECK_MBFID(mbfid)			/* Check 'mbfid'			*/		\
	if ( mbfid<MIN_MBFID || mbfid>MAX_MBFID )							\
		return(E_ID);
/*---------------------------------------------------------------------------*/
#define CHECK_PORID(porid)			/* Check 'porid'			*/		\
	if ( porid<MIN_PORID || porid>MAX_PORID )							\
		return(E_ID);
/*---------------------------------------------------------------------------*/
#define CHECK_PISID(pisid)			/* Check 'pisid'			*/		\
	if ( pisid<MIN_PISID || pisid>MAX_PISID )							\
		return(E_ID);					
/*---------------------------------------------------------------------------*/
#define CHECK_MPLID(mplid)			/* Check 'mplid'			*/		\
	if ( !CHK_MPLID(mplid) )												\
		return(E_ID);					
/*---------------------------------------------------------------------------*/
#define CHECK_MPLID_OS(mplid)		/* Check 'mplid' for nom-OS	*/		\
	if ( mplid == TMPL_OS	)											\
		return(E_ID);													\
	else if ( !CHK_MPLID(mplid) )											\
		return(E_ID);
/*---------------------------------------------------------------------------*/
#define CHECK_MPFID(mpfid)			/* Check 'mpfid'			*/		\
	if ( mpfid<MIN_MFPID || mpfid>MAX_MPFID )							\
		return(E_ID);
/*---------------------------------------------------------------------------*/
#else /* CHK_ID */
/*---------------------------------------------------------------------------*/
#define CHECK_TSKID(tskid)
#define CHECK_TSKID_SELF(tskid)
#define CHECK_SEMID(semid)
#define CHECK_FLGID(flgid)
#define CHECK_MBXID(mbxid)
#define CHECK_MBFID(mbfid)
#define CHECK_PORID(porid)
#define CHECK_PISID(pisid)
#define CHECK_MPLID(mplid)
#define CHECK_MPLID_OS(mplid)
#define CHECK_MPFID(mpfid)
#endif /* CHK_ID */

/*===========================================================================*/
/* Checks 'ids' for E_OACV errors.
   An E_OACV error will result if a user task issues a system call on a 
   system object, but error detection is implementation dependent.
*/
#ifdef CHK_OACV
/*---------------------------------------------------------------------------*/
#define CHECK_TSKACV(tskid)			/* Check 'tskid'			*/		\
	if ( tskid<0 && !in_sysmode() )										\
		return(E_OACV);
/*---------------------------------------------------------------------------*/
#define CHECK_SEMACV(semid)			/* Check 'semid'			*/		\
	if ( semid<0 && !in_sysmode() )										\
		return(E_OACV);
/*---------------------------------------------------------------------------*/
#define CHECK_FLGACV(flgid)			/* Check 'flgid'			*/		\
	if ( flgid<0 && !in_sysmode() )										\
		return(E_OACV);
/*---------------------------------------------------------------------------*/
#define CHECK_MBXACV(mbxid)			/* Check 'mbxid'			*/		\
	if ( mbxid<0 && !in_sysmode() )										\
		return(E_OACV);
/*---------------------------------------------------------------------------*/
#define CHECK_MBFACV(mbfid)			/* Check 'mbfid'			*/		\
	if ( mbfid<0 && !in_sysmode() )										\
		return(E_OACV);
/*---------------------------------------------------------------------------*/
#define CHECK_PORACV(porid)			/* Check 'porid'			*/		\
	if ( porid<0 && !in_sysmode() )										\
		return(E_OACV);
/*---------------------------------------------------------------------------*/
#define CHECK_PISACV(pisid)			/* Check 'pisid'			*/		\
	if ( pisid<0 && !in_sysmode() )										\
		return(E_OACV);
/*---------------------------------------------------------------------------*/
#define CHECK_MPLACV(mplid)			/* Check 'mplid'			*/		\
	if ( mplid<0 && !in_sysmode())										\
		return(E_OACV);
/*---------------------------------------------------------------------------*/
#define CHECK_MPFACV(mpfid)			/* Check 'mpfid'			*/		\
	if ( mpfid<0 && !in_sysmode())										\
		return(E_OACV);
/*---------------------------------------------------------------------------*/
#else /* CHK_OACV */
/*---------------------------------------------------------------------------*/
#define CHECK_TSKACV(tskid)
#define CHECK_SEMACV(semid)
#define CHECK_FLGACV(flgid)
#define CHECK_MBXACV(mbxid)
#define CHECK_MBFACV(mbfid)
#define CHECK_PORACV(porid)
#define CHECK_PISACV(pisid)
#define CHECK_MPLACV(mplid)
#define CHECK_MPFACV(mpfid)
#endif /* CHK_OACV */

/*===========================================================================*/
/* Check that 'tskid' is not the current task.  
   Invalid object state error.
*/
#ifdef CHK_SELF
/*---------------------------------------------------------------------------*/
#define CHECK_NONSELF(tskid)		/* 'tskid' not current task	*/		\
	if ( !in_indp() && (tskid) == ctxtsk->tskid)						\
		return(E_OBJ);
/*---------------------------------------------------------------------------*/
#else /* CHK_SELF */
/*---------------------------------------------------------------------------*/
#define CHECK_NONSELF(tskid)
#endif /* CHK_SELF */

/*===========================================================================*/
/* Check for E_PAR parameter errors.
   Checking for this error may be omitted when making system-dependent
   adaptations.
*/
#ifdef CHK_PAR
/*---------------------------------------------------------------------------*/
#define CHECK_PRI(pri)				/* Check priority 'pri'		*/		\
	if ( pri<MIN_PRI || pri>MAX_PRI )									\
		return(E_PAR);
/*---------------------------------------------------------------------------*/
#define CHECK_PRI_INI(pri)			/* ??? - may be stuffed		*/		\
	if ( (pri) != TPRI_INI && ( pri<MIN_PRI || pri>MAX_PRI ))			\
		return(E_PAR);
/*---------------------------------------------------------------------------*/
#define CHECK_PRI_RUN(pri)			/* ??? - may be stuffed		*/		\
	if ( (pri) != TPRI_RUN && ( pri<MIN_PRI || pri>MAX_PRI ))			\
		return(E_PAR);
/*---------------------------------------------------------------------------*/
#define CHECK_TMOUT(tmout)			/* Check 'tmout'			*/		\
	if ( !((tmout) >= TMO_FEVR) )										\
		return(E_PAR);
/*---------------------------------------------------------------------------*/
#define CHECK_PAR(exp)				/* Check for non-zero 'exp'	*/		\
	if ( !(exp) )														\
		return(E_PAR);
/*---------------------------------------------------------------------------*/
#else /* CHK_PAR */
/*---------------------------------------------------------------------------*/
#define CHECK_PRI(pri)
#define CHECK_PRI_INI(pri)
#define CHECK_PRI_RUN(pri)
#define CHECK_TMOUT(tmout)
#define CHECK_PAR(exp)
#endif /* CHK_PAR */

/*===========================================================================*/
/* Check for E_RSATR reserved attribute errors.
   This error results when an object attribute which is undefined or not
   supported is specified.  Checking for this error may be omitted when making 
   system-dependent adaptations.
*/
#ifdef CHK_RSATR
/*---------------------------------------------------------------------------*/
#define CHECK_RSATR(atr,maxatr)											\
	if ( (atr) & ~(maxatr) )											\
        return(E_RSATR);
/*---------------------------------------------------------------------------*/
#else /* CHK_RSATR */
/*---------------------------------------------------------------------------*/
#define CHECK_RSATR(atr,maxatr)
#endif /* CHK_RSATR */

/*===========================================================================*/
/* Check for E_NOSPT errors.
   This error indicates that some feature of the system call (such as a
   [level X] feature) is not supported.  An E_RSATR or E_NOSPT error results 
   when this feature is specified.  An E_NOSPT error results whenever the 
   error does not correspond to E_RSATR.
*/
#ifdef CHK_NOSPT
/*---------------------------------------------------------------------------*/
#define CHECK_NOSPT(exp)												\
	if ( !(exp) )														\
		return(E_NOSPT);
/*---------------------------------------------------------------------------*/
#else /* CHK_NOSPT */
/*---------------------------------------------------------------------------*/
#define CHECK_NOSPT(exp)
#endif /* CHK_NOSPT */

/*===========================================================================*/
/* Check for E_CTX context errors.
   This error indicates that the system call in question cannot be issued
   from the current context (due to a task portion/task-independent portion 
   distinction, or handler execution state).  This error must be detected 
   whenever a semantic mistake concerning system call execution context has 
   been made, such as when a system call (such as 'wai_sem'), which causes 
   an issuing task to enter a WAIT state has been issued from a task-
   independent portion.  This error is also detected whenever a system call 
   is not allowed to be issued from a given context (such as an interrupt 
   handler) due to implementation-dependent restrictions.
*/
#ifdef CHK_CTX
/*---------------------------------------------------------------------------*/
#define CHECK_INTSK()													\
	if ( in_indp() )													\
		return(E_CTX);
/*---------------------------------------------------------------------------*/
#define CHECK_DISPATCH()												\
	if ( in_ddsp() )													\
		return(E_CTX);
/*---------------------------------------------------------------------------*/
#define CHECK_DISPATCH_POL(tmout)										\
	if ( (tmout) != TMO_POL && in_ddsp() )								\
		return(E_CTX);
/*---------------------------------------------------------------------------*/
#define CHECK_CTX(exp)													\
	if ( !(exp) )														\
		return(E_CTX);
/*---------------------------------------------------------------------------*/
#else /* CHK_CTX */
/*---------------------------------------------------------------------------*/
#define CHECK_INTSK()
#define CHECK_DISPATCH()
#define CHECK_DISPATCH_POL(tmout)
#define CHECK_CTX(exp)
#endif /* CHK_CTX */

/*===========================================================================*/

#endif /* _CHECK_ */
