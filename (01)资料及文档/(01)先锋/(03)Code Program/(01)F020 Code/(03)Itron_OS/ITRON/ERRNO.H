/* errno.h
   Header file for uITRON based OS error codes.

   Mods to Original File:
   ----------------------
   a) Added English & removed Japanese comments.
   
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

#ifndef _ERRNO_
#define _ERRNO_

/* Error codes for different types of uITRON errors.
   Notice that all error codes are negative, permitting functions to
   return results that may be interpreted as error codes if negative
   or something else if positive.
   More detailed explanations can be found in the specifications, 
   which specifies values for each error as well as a very detailed
   description.
*/
#define	E_OK		0			/* No Error							*/

#define E_SYS		(-5)		/* System error: Unknown cause		*/
#define	E_NOMEM		(-10)		/* Insufficient Memory 				*/
#define	E_NOSPT		(-17)		/* Unsupported Feature				*/
#define	E_INOSPT	(-18)		/* ITRON/FILE ‚Unsupported Feature	*/
#define	E_RSFN		(-20)		/* Reserved Function Code Number	*/
#define	E_RSATR		(-24)		/* Reserved Attribute				*/
#define	E_PAR		(-33)		/* Parameter error					*/
#define	E_ID		(-35)		/* Invalid ID number				*/
#define	E_NOEXS		(-52)		/* Object does not exist			*/
#define	E_OBJ		(-63)		/* Invalid object state				*/
#define	E_MACV		(-65)		/* Memory access disable/violation	*/
#define	E_OACV		(-66)		/* Object access violation			*/
#define	E_CTX		(-69)		/* Context error					*/
#define	E_QOVR		(-73)		/* Queuing or nesting overflow		*/
#define	E_DLT		(-81)		/* Waiting object deleted			*/
#define	E_TMOUT		(-85)		/* Polling failure or timeout		*/
#define	E_RLWAI		(-86)		/* ‘WAIT state forcibly released	*/

#define	EN_NOND		(-113)		/* Target node does not exist		*/
#define	EN_OBJNO	(-114)		/* Object could not be accessed		*/
#define	EN_PROTO	(-115)		/* No target support of Protocol	*/
#define	EN_RSFN		(-116)		/* No target support of System-call	*/
#define	EN_COMM		(-117)		/* No response from target			*/
#define	EN_RLWAI	(-118)		/* Connection WAIT forced release	*/
#define	EN_PAR		(-119)		/* Out of range paramter			*/
#define	EN_RPAR		(-120)		/* Out of range return parameter	*/
#define	EN_CTXID	(-121)		/* SEE SPECIFICATION				*/
#define	EN_EXEC		(-122)		/* Insuf. resources for System-call */
#define	EN_NOSPT	(-123)		/* Connection fn not supported		*/

#define EV_FULL		(-225)		/* ??? - NOT IN SPECIFICATION		*/

/*===========================================================================*/

#endif /* _ITRON_ERRNO_ */
