/* misc_cal.c
   Miscelaneous system functions for uITRON OS, including code for handling
   extended system calls.

   Mods to Original File:
   ----------------------
   a) Added English & removed Japanese comments.

   Notes
   -----
   
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
#include "kernel.h"
#include "task.h"
#include "check.h"
#include "patchlev.h"

/*---------------------------------------------------------------------------*/
#ifndef _i_ref_sys

SYSCALL ER
i_ref_sys(T_RSYS *pk_rsys)
	{
	if ( in_indp() ) 
		pk_rsys->sysstat = TTS_INDP;
	else 
		{
		if ( in_qtsk() ) 
			pk_rsys->sysstat = TTS_QTSK;
		else 
			pk_rsys->sysstat = TTS_TSK;

		if ( in_loc() ) 
			pk_rsys->sysstat |= TTS_LOC;
		else if ( in_ddsp() ) 
			pk_rsys->sysstat |= TTS_DDSP;
		}
	pk_rsys->runtskid = (ctxtsk ? ctxtsk->tskid : FALSE);
	pk_rsys->schedtskid = (schedtsk ? schedtsk->tskid : FALSE);
	return(E_OK);
	}

#endif /* _i_ref_sys */

/*---------------------------------------------------------------------------*/
/*
*/
#ifndef _i_ref_cfg

SYSCALL ER
i_ref_cfg(T_RCFG *pk_rcfg)
	{
	return(E_OK);
	}

#endif /* _i_ref_cfg */

/*===========================================================================*/
/*
*/
#ifdef USE_SVC

/* Typedef for extended system call 
*/
typedef ER	(*SVC)();

/*---------------------------------------------------------------------------*/
/* Structure for controlling extended system calls.
*/
typedef struct extended_svc_control_block 
	{
	ATR		svcatr;
	SVC		svchdr;
	} SVCCB;

/*---------------------------------------------------------------------------*/
/* Table of extended system call control blocks.
*/
SVCCB					svccb_table[NUM_SVC];

/*---------------------------------------------------------------------------*/
/* Defines for easy indexing into the SVCCB table given the extended system
   call function codes 'fn'.
*/
#define get_svccb(fn)	(&(svccb_table[(fn)-1]))

/*---------------------------------------------------------------------------*/
/* Null handler extern definition.
*/
extern ER	no_support(void);

/*===========================================================================*/
/* Function to initialize extended system call control block table 
   'svccb_table' 
*/
void
extended_svc_initialize(void)
	{
	INT		i;

	for ( i = 0; i < NUM_SVC; i++ ) 
		{
		svccb_table[i].svchdr = no_support;
		}
	}

/*---------------------------------------------------------------------------*/
/* This system call defines an extended system call handler (called an 
   "extended SVC handler") for the function code given by 's_fncd'.  
   Specifically, the system call defines a correspondence between the 
   function code given by 's_fncd' and the extended SVC handler address 
   'svchdr' and associated attributes.  Using 'def_svc' allows execution of 
   user-defined extended SVC handlers based on the same interface with the 
   system calls of uITRON 3.0 specification.
*/
#ifndef _i_def_svc

SYSCALL ER
i_def_svc(FN s_fncd,T_DSVC *pk_dsvc)
	{
	SVCCB	*svccb;
	ER		ercd = E_OK;

	CHECK_PAR(0 < s_fncd && s_fncd <= NUM_SVC);
	if ( pk_dsvc != NADR ) 
		{
		CHECK_RSATR(pk_dsvc->svcatr,TA_HLNG);
		}
	svccb = get_svccb(s_fncd);

	BEGIN_CRITICAL_SECTION;
	if ( pk_dsvc == NADR ) 
		{
		svccb->svchdr = no_support;
		}
	else 
		{
		svccb->svcatr = pk_dsvc->svcatr;
		svccb->svchdr = (SVC) pk_dsvc->svchdr;
		}		
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_def_svc */

/*---------------------------------------------------------------------------*/
/* ???
   s_fncd, 'ientry.s', 'ctxtsk->sysmode?
*/
ER
svc_ientry(INT par1,INT par2,INT par3,INT par4,INT par5,INT par6,FN s_fncd)
	{
	ER		ercd;

	if ( s_fncd > NUM_SVC ) 
		{
		return(E_RSFN);
		}

	if ( !in_indp() ) 
		{
		ctxtsk->sysmode++;
		ercd = (*(get_svccb(s_fncd)->svchdr))(par1,par2,par3,par4,par5,par6);
		ctxtsk->sysmode--;
		}
	else 
		{
		ercd = (*(get_svccb(s_fncd)->svchdr))(par1,par2,par3,par4,par5,par6);
		}
	return(ercd);
	}

#endif /* USE_SVC */

/*===========================================================================*/
/* This system call gets informations of the maker of the ITRON or uITRON
   specification OS currently executing, the identification number of the OS,
   the ITRON or uITRON specification version number which the OS is based on,
   and the version number of the OS product
*/
#ifndef _i_get_ver

#define	FWVER		((MAJOR_REL << 12) + (MINOR_REL << 4) + PATCH_LEVEL)
#define	CPU_CODE	0

SYSCALL ER
i_get_ver(T_VER *pk_ver)
	{
	pk_ver->maker = 0x0001;		/* Arbitary manufacturer code		*/
	pk_ver->id = 0x5003;		/* Designates different build types	*/
	pk_ver->spver = 0x5302;		/* Specification Version 3.02		*/
	pk_ver->prver = FWVER;		/* Release X.YY.Z 					*/
	memset(pk_ver->prno,0,sizeof(pk_ver->prno));	/* Product data	*/
	pk_ver->cpu = CPU_CODE;		/* CPU Type							*/
	pk_ver->var = 0xc000;		/* Supplied functionality			*/
	return(E_OK);
	}

#endif /* _i_get_ver */

/*---------------------------------------------------------------------------*/
