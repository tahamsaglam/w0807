/* kernel.h
   Header file for uITRON based OS.

   Mods to Original File:
   ----------------------
   a) Added English & removed Japanese comments.
   b) Removed system memory allocation prototypes.
   
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

#ifndef _KERNEL_
#define _KERNEL_

#include "itron.h"
#include "errno.h"
#include "fncode.h"

/*---------------------------------------------------------------------------*/
/* Defines to allow compilation of 'assert()' statements.
*/
#ifndef NDEBUG

extern void	
i_assert_fail(const char *expr,const char *file,unsigned int line);

#define assert(exp)				/* Assert defined code	*/					\
	if ( !(exp) )															\
		i_assert_fail(#exp, __MODULE__, __LINE__);

#else /* NDEBUG */

#define assert(exp)

#endif /* NDEBUG */

/*---------------------------------------------------------------------------*/
/* Defines for the 'offsetof' macro and the SYSCALL qualifier.
*/
#define	offsetof(structure,field)	((INT) &(((structure *) 0)->field))
#define SYSCALL

/*---------------------------------------------------------------------------*/

#include "config.h"
#include "cpu.h"
#include "isyscall.h"

#if 0
#include "cpu_conf.h"
#include "sys_conf.h"
#include "isysconf.h"
#include "cpu_status.h"
#endif

/*---------------------------------------------------------------------------*/
/* Initialization functions for various modules.
*/
extern void	task_initialize(void);
extern void	semaphore_initialize(void);
extern void	eventflag_initialize(void);
extern void	mailbox_initialize(void);
extern void	messagebuffer_initialize(void);
extern void	rendezvous_initialize(void);
extern void	prisem_initialize(void);
extern void	memorypool_initialize(void);
extern void	fix_memorypool_initialize(void);
extern void	cyclichandler_initialize(void);
extern void	alarmhandler_initialize(void);
extern void	extended_svc_initialize(void);

/*---------------------------------------------------------------------------*/
/* RTOS Exit - 'startup.c'.
*/
extern void	itis_exit(void);

/*===========================================================================*/

#endif /* _ITIS_KERNEL_ */
