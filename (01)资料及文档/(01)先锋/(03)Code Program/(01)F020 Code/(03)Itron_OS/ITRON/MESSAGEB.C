/* messageb.c
   Message-buffer module for uITRON based OS.

   Notes:
   ------

   Mods to Original File:
   ----------------------
   a) Added English & removed Japanese comments.
   b) Quite a few changes to remove compile errors picked up by ARM
      compiler.
   c) Note that this requires the 'sys_get_blk' and 'sys_rel_blk' function
      calls, which are defined in 'mempool.c'.  This means that message
	  buffers cannot be chosen unless variable size memory management has
	  been selected.

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

#ifdef USE_MBF

#include <string.h>
#include "task.h"
#include "wait.h"
#include "check.h"

/*---------------------------------------------------------------------------*/
/* Structure for controlling message-buffer operations.
   TTW_MBF, TTW_SMBF
*/
typedef struct messagebuffer_control_block 
	{
	QUEUE	wait_queue;				/* Message-buffer wait queue		*/
	ID		mbfid;					/* Message-buffer ID				*/
	VP		exinf;					/* Extended information				*/
	ATR		mbfatr;					/* Message-buffer attributes		*/
	QUEUE	send_queue;				/* Messages to send					*/
	INT		bufsz;					/* Buffer-size						*/
	INT		maxmsz;					/* Maximum message size				*/
	INT		frbufsz;				/* Free-buffer size					*/
	INT		head;					/* Head of queue					*/
	INT		tail;					/* Tail of queue					*/
	VB		*buffer;				/* Message buffer-buffer			*/
	} MBFCB;

/*---------------------------------------------------------------------------*/
/* Table of message buffer control blocks.
*/
static MBFCB			mbfcb_table[NUM_MBFID];

/*---------------------------------------------------------------------------*/
/* Defines for easy indexing into the MBFCB table given the message 
   buffer 'id'.
*/
#define get_mbfcb(id)	(&(mbfcb_table[INDEX_MBF(id)]))

/*---------------------------------------------------------------------------*/
/* 'free_mbf is a linked list of free MBFCB's ready for allocation.
*/
#ifndef _i_vcre_mbf
QUEUE	free_mbfcb;
#endif /* _i_vcre_mbf */

/*===========================================================================*/
/* Function to initialize message buffer control block table 'mbfcb_table' 
*/
void
messagebuffer_initialize()
	{
	INT		i;
	MBFCB	*mbfcb;
	ID		mbfid;

	#ifndef _i_vcre_mbf
	queue_initialize(&free_mbfcb);
	#endif /* _i_vcre_mbf */

	for ( mbfcb = mbfcb_table,i = 0; i < NUM_MBFID; mbfcb++,i++ ) 
		{
		mbfid = ID_MBF(i);
		mbfcb->mbfid = mbfid;
		mbfcb->mbfatr = OBJ_NONEXIST;
		#ifndef _i_vcre_mbf
		if ( !SYS_MBFID(mbfid) ) 
			{
			queue_insert(&(mbfcb->wait_queue),&free_mbfcb);
			}
		#endif /* _i_vcre_mbf */
		}
	}

/*---------------------------------------------------------------------------*/
/* 'typedef's and defines to assist in allocation of memory from the message
   buffer memory pool for messages.
*/
typedef INT			HEADER;
#define HEADERSZ	sizeof(HEADER)

#define ROUNDSIZE	sizeof(HEADER)
#define	ROUNDSZ(sz)	(((sz) + (ROUNDSIZE-1)) & ~(ROUNDSIZE-1))

/*---------------------------------------------------------------------------*/
/* Returns a boolean indicating if enough memory is available for a message
   of size 'msgsz' from the message buffer controlled by 'mbfcb'
*/
__inline BOOL
mbf_free(MBFCB* mbfcb,INT msgsz)
	{
	return(HEADERSZ + msgsz <= mbfcb->frbufsz);
	}

/*---------------------------------------------------------------------------*/
/* Returns a boolean indicating if the message buffer pool 'mbfcb' is 
   empty.
*/
__inline BOOL
mbf_empty(MBFCB* mbfcb)
	{
	return(mbfcb->frbufsz == mbfcb->bufsz);
	}

/*---------------------------------------------------------------------------*/
/* Place of message of size 'msgsz' and starting address 'msg' in the 
   message buffer pool 'mpbcb'.  Assumes that a check for sufficient
   room in the message buffer has been performed.
*/
static void
msg_to_mbf(MBFCB* mbfcb,VP msg,INT msgsz)
	{
	INT		tail = mbfcb->tail;
	VB		*buffer = mbfcb->buffer;
	INT		remsz;

	/* First reduce the free buffer size 'frbufsz' by the required amount
	   and fill in the message size 'msgsz' in the header.  Then copy the
	   message into the message buffer, ensuring the the message is
	   correctly split between the end and start of the message buffer
	   if required.
	   Q) What if the size of the header is greater than the size of the
	      buffer space remaining at the end of the buffer before a wrap 
	      around occurs? ???
	*/
	mbfcb->frbufsz -= HEADERSZ + ROUNDSZ(msgsz);
	*((HEADER *) &(buffer[tail])) = msgsz;
	tail += HEADERSZ;
	if ( tail >= mbfcb->bufsz ) 
		{
		tail = 0;
		}

	if ((remsz = mbfcb->bufsz - tail) < msgsz) 
		{
		memcpy(&(buffer[tail]),msg,remsz);
		msg = (VP) ((VB*)msg + remsz);
		msgsz -= remsz;
		tail = 0;
		}
	memcpy(&(buffer[tail]),msg,msgsz);
	tail += ROUNDSZ(msgsz);
	if ( tail >= mbfcb->bufsz ) 
		{
		tail = 0;
		}
	mbfcb->tail = tail;
	}

/*---------------------------------------------------------------------------*/
/* Extract a message from the message buffer pool 'mbfcb', placing it in the
   address 'msg'.
*/
static INT
mbf_to_msg(MBFCB* mbfcb,VP msg)
	{
	INT		head = mbfcb->head;
	VB		*buffer = mbfcb->buffer;
	INT		msgsz, copysz;
	INT		remsz;

	/* Extract the message from the message buffer, ensuring that the
	   message is correctly extracted in the event of a split between
	   the end & start of the circular buffer.
	*/
	msgsz = *((HEADER *) &(buffer[head]));
	head += HEADERSZ;
	if (head >= mbfcb->bufsz) 
		{
		head = 0;
		}
	mbfcb->frbufsz += HEADERSZ + ROUNDSZ(msgsz);

	copysz = msgsz;
	if ((remsz = mbfcb->bufsz - head) < copysz) 
		{
		memcpy(msg,&(buffer[head]),remsz);
		msg = (VP) ((VB*)msg + remsz);
		copysz -= remsz;
		head = 0;
		}
	memcpy(msg,&(buffer[head]),copysz);
	head += ROUNDSZ(copysz);
	if (head >= mbfcb->bufsz) 
		{
		head = 0;
		}
	mbfcb->head = head;
	return(msgsz);
	}

/*---------------------------------------------------------------------------*/
/* Wait callback information for message buffers.
*/
static WSPEC wspec_mbf_tfifo = { TTW_MBF, 0, 0 };
static WSPEC wspec_mbf_tpri = { TTW_MBF, obj_chg_pri, 0 };
static WSPEC wspec_smbf = { TTW_SMBF, 0, 0 };

/*---------------------------------------------------------------------------*/
/* Function to initialize the message buffere control block 'mbfcb' given the
   packet of message buffer initialization data 'pk_cmbf'.
*/
#if !defined(_i_cre_mbf) || !defined(_i_vcre_mbf)

static ER
_cre_mbf(MBFCB *mbfcb,T_CMBF *pk_cmbf,INT bufsz)
	{
	extern VP	sys_get_blk(INT blksz);

	if ( bufsz > 0 && (mbfcb->buffer = (VB*)sys_get_blk(bufsz)) == 0 ) 
		{
		return(E_NOMEM);
		}

	#ifndef _i_vcre_mbf
	if ( !SYS_MBFID(mbfcb->mbfid) ) 
		{
		queue_delete(&(mbfcb->wait_queue));
		}
	#endif /* _i_vcre_mbf */

	queue_initialize(&mbfcb->wait_queue);
	mbfcb->exinf = pk_cmbf->exinf;
	mbfcb->mbfatr = pk_cmbf->mbfatr;
	queue_initialize(&mbfcb->send_queue);
	mbfcb->bufsz = mbfcb->frbufsz = bufsz;
	mbfcb->maxmsz = pk_cmbf->maxmsz;
	mbfcb->head = mbfcb->tail = 0;
	return(E_OK);
	}

#endif /* !defined(_i_cre_mbf) || !defined(_i_vcre_mbf) */

/*---------------------------------------------------------------------------*/
/* 'cre_mbf' creates a messagebuffer having the ID number specified by 
   'mbfid'.  Specifically, a control block is allocated for the messagebuffer.
   A ring buffer area is also allocated for use as a message queue (for 
   waiting for messages to be received) based on the value of 'bufsz'.

   Messagebuffers are objects used for managing the transfer of variable 
   length messages.  The main difference from a mailbox 'mbx' is that 
   variable length message contents are copied during sending and receiving 
   in the messagebuffer.  This means that this feature may be used for 
   transferring messages between nodes on a loosely-coupled network which have 
   no shared memory.  Also, a task which sends message may enter a WAIT state 
   when the buffer is full.  Lastly, when a mailbox message is sent, the
   mailbox packet is linked into the list of packets, which means that this
   section of memory is required to remain unchanged until the message is
   received.
*/
#ifndef _i_cre_mbf

SYSCALL ER
i_cre_mbf(ID mbfid,T_CMBF *pk_cmbf)
	{
	MBFCB	*mbfcb;
	INT		bufsz;
	ER		ercd = E_OK;

	CHECK_MBFID(mbfid);
	CHECK_MBFACV(mbfid);
	CHECK_RSATR(pk_cmbf->mbfatr,TA_TPRI);
	CHECK_PAR(pk_cmbf->bufsz >= 0);
	CHECK_PAR(pk_cmbf->maxmsz > 0);
	bufsz = ROUNDSZ(pk_cmbf->bufsz);
	mbfcb = get_mbfcb(mbfid);

	BEGIN_CRITICAL_SECTION;
	if ( mbfcb->mbfatr != OBJ_NONEXIST ) 
		{
		ercd = E_OBJ;
		}
	else 
		{
		ercd = _cre_mbf(mbfcb,pk_cmbf,bufsz);
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_cre_mbf */

/*---------------------------------------------------------------------------*/
/* 'vcre_mbf' creates a messagebuffer, without having to specify a message
   buffer ID.  Specifically, a control block is allocated for the 
   messagebuffer.  The message buffer ID 'mbfid' is returned.
*/
#ifndef _i_vcre_mbf

SYSCALL ER
i_vcre_mbf(T_CMBF *pk_cmbf)
	{
	MBFCB	*mbfcb;
	INT		bufsz;
	ER		ercd = E_OK;

	CHECK_RSATR(pk_cmbf->mbfatr,TA_TPRI);
	CHECK_PAR(pk_cmbf->bufsz >= 0);
	CHECK_PAR(pk_cmbf->maxmsz > 0);
	bufsz = ROUNDSZ(pk_cmbf->bufsz);

	BEGIN_CRITICAL_SECTION;
	if (queue_empty_p(&free_mbfcb)) 
		{
		ercd =  EV_FULL;
		}
	else 
		{
		mbfcb = (MBFCB *)(free_mbfcb.next);
		ercd = _cre_mbf(mbfcb,pk_cmbf,bufsz);
		if (ercd == E_OK) 
			{
			ercd = (ER)(mbfcb->mbfid);
			}
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_vcre_mbf */

/*---------------------------------------------------------------------------*/
/* This system call deletes the messagebuffer specified by 'mbfid'.
   Issuing this system call releases memory area used for the control block 
   of the associated messagebuffer and the buffer area used for storing 
   messages.  After this system call is invoked, another messagebuffer having 
   the same ID number can be created.
   This system call will complete normally even if there are tasks waiting 
   to send or receive messages at the messagebuffer.  In that case, an E_DLT 
   error will be returned to each waiting task.  If there are messages still 
   in the messagebuffer, they will be deleted along with the messagebuffer 
   and no error will result.
*/
#ifndef _i_del_mbf

SYSCALL ER
i_del_mbf(ID mbfid)
	{
	extern void		sys_rel_blk(VP blk);
	MBFCB			*mbfcb;
	ER				ercd = E_OK;

	CHECK_MBFID(mbfid);
	CHECK_MBFACV(mbfid);
	mbfcb = get_mbfcb(mbfid);

	BEGIN_CRITICAL_SECTION;
	if ( mbfcb->mbfatr == OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		}
	else 
		{
		if ( mbfcb->bufsz > 0 ) 
			{
			sys_rel_blk(mbfcb->buffer);
			}
		wait_delete(&(mbfcb->wait_queue));
		wait_delete(&(mbfcb->send_queue));
		#ifndef _i_vcre_mbf
		if ( !SYS_MBFID(mbfcb->mbfid) ) 
			{
			queue_insert(&(mbfcb->wait_queue),&free_mbfcb);
			}
		#endif /* _i_vcre_mbf */
		mbfcb->mbfatr = OBJ_NONEXIST;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_del_mbf */

/*---------------------------------------------------------------------------*/
/* The 'tsnd_mbf' system call has the same function as snd_mbf with an 
   additional timeout feature.  A maximum wait time (timeout value) can 
   be specified using the parameter 'tmout'.  When a timeout is specified, 
   a timeout error, E_TMOUT, will result and the system call will finish 
   if the period specified by 'tmout' elapses without conditions for 
   releasing wait being satisfied (i.e. without sufficient buffer space 
   becoming available).
*/
#if !defined(_i_snd_mbf) || !defined(_i_psnd_mbf) || !defined(_i_tsnd_mbf)

SYSCALL ER
i_tsnd_mbf(ID mbfid,VP msg,INT msgsz,TMO tmout)
	{
	MBFCB	*mbfcb;
	TCB		*tcb;
	ER		ercd = E_OK;

	CHECK_MBFID(mbfid);
	CHECK_MBFACV(mbfid);
	CHECK_PAR(msgsz > 0);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH_POL(tmout);
	mbfcb = get_mbfcb(mbfid);

	BEGIN_CRITICAL_SECTION;
	if ( mbfcb->mbfatr == OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		}
	#ifdef CHK_PAR
	else if ( msgsz > mbfcb->maxmsz ) 
		{
		ercd = E_PAR;
		}
	#endif /* CHK_PAR */
	else if ( !queue_empty_p(&(mbfcb->wait_queue)) ) 
		{
		/* In this case, a 'task' is waiting on the message queue, so the
		   data is not copied to the message buffer circular-buffer,
		   but is copied to the output buffer specified by the waiting
		   process.
		*/
		tcb = (TCB *)(mbfcb->wait_queue.next);
		memcpy(tcb->winfo.mbf.msg,msg,msgsz);
		*(tcb->winfo.mbf.p_msgsz) = msgsz;
		wait_release_ok(tcb);
		}
	else if ( mbf_free(mbfcb,msgsz) ) 
		{
		/* No tasks waiting on the message buffer and sufficient space
		   in the circular buffer exists for the message, so copy the 
		   message into the message buffer circular-buffer.
		*/
		msg_to_mbf(mbfcb,msg,msgsz);
		}
	else 
		{
		/* Insufficient room in the message buffer for the message.
		   The system call is made to wait on the release of memory,
		   which occurs during the read of the message-buffer, after 
		   which the data can be dealt with.
		   ??? - Reorder 'make_wait' & 'queue_insert'.
		*/
		ercd = E_TMOUT;
		if ( tmout != TMO_POL ) 
			{
			ctxtsk->wspec = &wspec_smbf;
			ctxtsk->wgcb = (GCB *) mbfcb;
			ctxtsk->wercd = &ercd;
			ctxtsk->winfo.smbf.msg = msg;
			ctxtsk->winfo.smbf.msgsz = msgsz;
			make_wait(tmout);
			queue_insert(&(ctxtsk->tskque),&(mbfcb->send_queue));
			}
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_tsnd_mbf */

/*---------------------------------------------------------------------------*/
/* 'snd_mbf' sends the message stored at the address given by msg to the 
   message buffer specified by mbfid.  The size of the message is specified 
   by 'msgsz'; that is, 'msgsz' number of bytes beginning from 'msg' are 
   copied to the message queue of the messagebuffer specified by 'mbfid'.
   The message queue is implemented by the ring buffer.
*/
#ifndef _i_snd_mbf

SYSCALL ER
i_snd_mbf(ID mbfid,VP msg,INT msgsz)
	{
	return(i_tsnd_mbf(mbfid,msg,msgsz,TMO_FEVR));
	}

#endif /* _i_snd_mbf */

/*---------------------------------------------------------------------------*/
/* The 'psnd_mbf' system call has the same function as 'snd_mbf' except for 
   the waiting feature.  'psnd_mbf' polls whether or not the task should 
   wait if 'snd_mbf' is executed.  If there is not enough space in the 
   buffer, an E_TMOUT error is returned to indicate polling failed and the 
   system call finishes.
*/
#ifndef _i_psnd_mbf

SYSCALL ER
i_psnd_mbf(ID mbfid,VP msg,INT msgsz)
	{
	return(i_tsnd_mbf(mbfid,msg,msgsz,TMO_POL));
	}

#endif /* _i_psnd_mbf */

/*---------------------------------------------------------------------------*/
/* The 'trcv_mbf' system call has the same function as 'rcv_mbf' with an 
   additional timeout feature.  A maximum wait time (timeout value) can be 
   specified using the parameter 'tmout'.  When a timeout is specified, a 
   timeout error, E_TMOUT, will result and the system call will finish if 
   the period specified by 'tmout' elapses without conditions for releasing 
   wait being satisfied (i.e. without a message arriving).
*/
#if !defined(_i_rcv_mbf) || !defined(_i_prcv_mbf) || !defined(_i_trcv_mbf)

SYSCALL ER
i_trcv_mbf(VP msg,INT *p_msgsz,ID mbfid,TMO tmout)
	{
	MBFCB	*mbfcb;
	QUEUE	*queue;
	TCB		*tcb;
	INT		msgsz;
	ER		ercd = E_OK;

	CHECK_MBFID(mbfid);
	CHECK_MBFACV(mbfid);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();
	mbfcb = get_mbfcb(mbfid);
    
	BEGIN_CRITICAL_SECTION;
	if ( mbfcb->mbfatr == OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		}
	else if ( !mbf_empty(mbfcb) ) 
		{
		/* In this case, the message buffer is not empty, so remove the
		   message from the queue and then check if other tasks were waiting
		   on the memory release so that more message could be inserted.
		   If so, copy these messages to the message buffer circular buffer.
		   Those tasks that are waiting are then released as the data is
		   copied to the message buffer.

		   ??? - Need to be careful here.  Interrupts are disabled, and the
		   data in the queue is removed.  Data waiting to be placed in the
		   queue is then placed in the queue and those tasks that are waiting
		   are released as the data is copied.  If these tasks are high 
		   priority tasks, then they will run immediately on completion.
		   However, perhaps there are still other messages to be copied.  
		   Given that this is the case, what is the status of the 
		   CRITICAL_SECTION on return.  Really need to disable dispatching 
		   until after the END_CRITICAL_SECTION is reached.
		*/
		*p_msgsz = mbf_to_msg(mbfcb,msg);
		queue = mbfcb->send_queue.next;
		while ( queue != &(mbfcb->send_queue) ) 
			{
			tcb = (TCB *) queue;
			queue = queue->next;
			msgsz = tcb->winfo.smbf.msgsz;
			if ( mbf_free(mbfcb,msgsz) ) 
				{
				msg_to_mbf(mbfcb,tcb->winfo.smbf.msg,msgsz);
				wait_release_ok(tcb);
				}
			}
		}
	else if ( !queue_empty_p(&(mbfcb->send_queue)) ) 
		{
		tcb = (TCB *)(mbfcb->send_queue.next);
		*p_msgsz = tcb->winfo.smbf.msgsz;
		memcpy(msg,tcb->winfo.smbf.msg,*p_msgsz);
		wait_release_ok(tcb);
		}
	else 
		{
		ercd = E_TMOUT;
		if ( tmout != TMO_POL ) 
			{
			ctxtsk->wspec = (mbfcb->mbfatr & TA_TPRI) ?	&wspec_mbf_tpri : &wspec_mbf_tfifo;
			ctxtsk->wercd = &ercd;
			ctxtsk->winfo.mbf.msg = msg;
			ctxtsk->winfo.mbf.p_msgsz = p_msgsz;
			gcb_make_wait((GCB *)mbfcb,tmout);
			}
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_trcv_mbf */

/*---------------------------------------------------------------------------*/
/* 'rcv_mbf' receives the message from the messagebuffer specified by 
   'mbfid', and stores it at the memory location given by 'msg'.  In other 
   words, the content of the message at the head of the message queue of 
   the messagebuffer specified by 'mbfid' is copied into an area which 
   begins from 'msg' and whose size is 'msgsz'.
   If the message has not yet been sent to the messagebuffer specified by 
   'mbfid' (if the message queue is empty), the task issuing this system 
   call will wait on a receive message wait queue until a message arrives.
   Q) What if the amount of room available at 'msg' is less than 'msgsz'?
   A) You can use 'ref_mbf' to determine the size of the next message
      before actually receiving it.
*/
#ifndef _i_rcv_mbf

SYSCALL ER
i_rcv_mbf(VP msg,INT *p_msgsz,ID mbfid)
	{
	return(i_trcv_mbf(msg,p_msgsz,mbfid,TMO_FEVR));
	}

#endif /* _i_rcv_mbf */

/*---------------------------------------------------------------------------*/
/* The 'prcv_mbf' system call has the same function as rcv_mbf except for 
   the waiting feature.  'prcv_mbf' polls whether or not the task should 
   wait if 'rcv_mbf' is executed.  If there is no message in the specified 
   messagebuffer, an E_TMOUT error is returned to indicate polling failed 
   and the system call finishes.
*/
#ifndef _i_prcv_mbf

SYSCALL ER
i_prcv_mbf(VP msg,INT *p_msgsz,ID mbfid)
	{
	return(i_trcv_mbf(msg,p_msgsz,mbfid,TMO_POL));
	}

#endif /* _i_prcv_mbf */

/*---------------------------------------------------------------------------*/
/* This system call refers to the state of the messagebuffer specified 
   by 'mbfid',  and returns information of a task waiting to send a message
   'stsk', the size of the next message to be received 'msgsz', the free 
   buffer size 'frbufsz', information of a task waiting to receive a message
   'wtsk', and its extended information 'exinf', all contained in the 
   structure 'pk_rmbf'.
*/
#ifndef _i_ref_mbf

SYSCALL ER
i_ref_mbf(T_RMBF *pk_rmbf, ID mbfid)
	{
	MBFCB	*mbfcb;
	ER		ercd = E_OK;

	CHECK_MBFID(mbfid);
	CHECK_MBFACV(mbfid);
	mbfcb = get_mbfcb(mbfid);

	BEGIN_CRITICAL_SECTION;
	if ( mbfcb->mbfatr == OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		}
	else 
		{
		pk_rmbf->exinf = mbfcb->exinf;
		pk_rmbf->wtsk = wait_tskid(&(mbfcb->wait_queue));
		pk_rmbf->stsk = wait_tskid(&(mbfcb->send_queue));
		pk_rmbf->msgsz = mbf_empty(mbfcb) ? FALSE :	*((HEADER *) &(mbfcb->buffer[mbfcb->head]));
		pk_rmbf->frbufsz = mbfcb->frbufsz;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_ref_mbf */

/*---------------------------------------------------------------------------*/
#ifdef USE_TMBF_OS

#define log_mbfcb	get_mbfcb(TMBF_OS)

ER
log_snd_mbf(VP msg,INT msgsz)
	{
	TCB		*tcb;
	ER		ercd = E_OK;

	BEGIN_CRITICAL_SECTION;
	if ( log_mbfcb->mbfatr == OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		}
	else if ( !queue_empty_p(&(log_mbfcb->wait_queue)) ) 
		{
		tcb = (TCB *)(log_mbfcb->wait_queue.next);
		memcpy(tcb->winfo.mbf.msg,msg, msgsz);
		*(tcb->winfo.mbf.p_msgsz) = msgsz;
		wait_release_ok(tcb);
		}
	else if ( mbf_free(log_mbfcb,msgsz) ) 
		{
		msg_to_mbf(log_mbfcb,msg,msgsz);
		}
	else 
		{
		ercd = E_TMOUT;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* USE_TMBF_OS */
/*===========================================================================*/
#endif /* USE_MBF */
