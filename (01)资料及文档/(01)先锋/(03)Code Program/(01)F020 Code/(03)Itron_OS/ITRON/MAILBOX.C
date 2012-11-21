/* mailbox.c
   Mailbox functions for uITRON OS.

   Mods to Original File:
   ----------------------
   a) Added English & removed Japanese comments.

   Notes:
   -----
   a) This mailbox implementation creates a linked list of mail packets that
      are eventually delivered to the waiting function.  A consequence of this 
      is that when mail packets are transmitted, it must be realised that the 
      mail packet passed to tbe OS will be linked into a list, which means that 
      the memory used by that packet structure must be 'permanent' (not changed)
      until the message has been delivered.  Hence that structure cannot be an 
      automatic (stack) variable, or cannot have its contents changed when a 
      new message must be sent and then resent again.
   b) To achieve this, the packet structure could be allocated from a variable
      sized memory pool and then deallocated by the receiving function when
	  the message has been received.  Alternatively, a message could be 
	  transmitted using a message buffer, which actually copies the contents
	  of the message into its own internal buffer.
   
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

#ifdef USE_MBX

#include "task.h"
#include "wait.h"
#include "check.h"

/* Structure for controlling mailbox operations.
   ??? - mq_head & mq_tail had specific comments re FIFO & TA_MFIFO
*/
typedef struct mailbox_control_block 
	{
	QUEUE	wait_queue;				/* Mailbox wait queue				*/
	ID		mbxid;					/* Mailbox ID						*/
	VP		exinf;					/* Extended information				*/
	ATR		mbxatr;					/* Mailbox attributes				*/

	T_MSG	*mq_head;				/* Mailbox queue head				*/
	T_MSG	*mq_tail;				/* Mailbox queue tail				*/
	} MBXCB;

/*---------------------------------------------------------------------------*/
/* Table of mailbox control blocks.
*/
static MBXCB			mbxcb_table[NUM_MBXID];

/*---------------------------------------------------------------------------*/
/* Defines for easy indexing into the MBXCB table given the mailbox 'id'.
*/
#define get_mbxcb(id)	(&(mbxcb_table[INDEX_MBX(id)]))

/*---------------------------------------------------------------------------*/
/* 'free_mbxcb' is a linked list of free MBXCB's ready for allocation.
*/
#ifndef _i_vcre_mbx
QUEUE	free_mbxcb;
#endif /* _i_vcre_mbx */

/*===========================================================================*/
/* Function to initialize mailbox control block table 'mbxcb_table' 
*/
void 
mailbox_initialize()
	{
	INT		i;
	MBXCB	*mbxcb;
	ID		mbxid;

	#ifndef _i_vcre_mbx
	queue_initialize(&free_mbxcb);
	#endif /* _i_vcre_mbx */

	for ( mbxcb = mbxcb_table,i = 0; i < NUM_MBXID; mbxcb++,i++ ) 
		{
		mbxid = ID_MBX(i);
		mbxcb->mbxid = mbxid;
		mbxcb->mbxatr = OBJ_NONEXIST;
		#ifndef _i_vcre_mbx
		if ( !SYS_MBXID(mbxid) ) 
			{
			queue_insert(&(mbxcb->wait_queue),&free_mbxcb);
			}
		#endif /* _i_vcre_mbx */
		}
	}

/*---------------------------------------------------------------------------*/
/* Define to allow easy access to the next message.
*/
#define nextmsg(msg)	*((T_MSG **) &((msg)->msgque[0]))

/*---------------------------------------------------------------------------*/
/* Inserts the message 'pk_msg' into the sorted message queue 'head'.
*/
__inline void
queue_insert_mpri(T_MSG *pk_msg,T_MSG **head)
	{
	T_MSG	*msg;
	T_MSG	*prevmsg = (T_MSG *) head;

	/* Search for the first location in the (sorted) queue where message
	   priority is lower (numerically greater) then the new message to
	   be inserted.
	*/
	while ( (msg = nextmsg(prevmsg))!=0 )
		{
		if ( msg->msgpri>=pk_msg->msgpri ) 
			break;
		prevmsg = msg;
		}

	nextmsg(pk_msg) = msg;
	nextmsg(prevmsg) = pk_msg;
	}

/*---------------------------------------------------------------------------*/
/* Wait callback information for mailboxes.
*/
static WSPEC wspec_mbx_tfifo = { TTW_MBX, 0, 0 };
static WSPEC wspec_mbx_tpri = { TTW_MBX, obj_chg_pri, 0 };

/*---------------------------------------------------------------------------*/
/* Initialize the mailbox control block 'mbxcb', using the data contained 
   within 'pk_cmbx'.
*/
#if !defined(_i_cre_mbx) || !defined(_i_vcre_mbx)

static void
_cre_mbx(MBXCB *mbxcb,T_CMBX *pk_cmbx)
	{
	#ifndef _i_vcre_mbx
	if ( !SYS_MBXID(mbxcb->mbxid) ) 
		{
		queue_delete(&(mbxcb->wait_queue));
		}
	#endif /* _i_vcre_mbx */

	queue_initialize(&(mbxcb->wait_queue));
	mbxcb->exinf = pk_cmbx->exinf;
	mbxcb->mbxatr = pk_cmbx->mbxatr;
	mbxcb->mq_head = (T_MSG *) 0;
	}

#endif /* !defined(_i_cre_mbx) || !defined(_i_vcre_mbx) */

/*---------------------------------------------------------------------------*/
/* This system call creates the mailbox specified by 'mbxid'.  Specifically, 
   a control block and buffer area are allocated for the mailbox to be 
   created.
   TA_TFIFO and TA_TPRI are used to specify the manner in which tasks 
   receiving messages are put on the mailbox's queue.  If TA_TFIFO is 
   specified, tasks will be put on the queue on a "First In, First Out" 
   basis.  If TA_TPRI is specified, tasks will be placed on the queue 
   according to their priority level.  The TA_TPRI specification is considered 
   an extended function [level X] for which compatibility and connectivity are 
   not guaranteed.
   Similarly, TA_MFIFO and TA_MPRI are used to specify the manner in which
   messages are put on the message queue (the queue for message waiting to 
   be received).  If TA_MFIFO is specified, messages will be put on the 
   message queue on a "First In, First Out" basis.  If TA_MPRI is specified, 
   messages will be placed on the message queue according to their priority 
   level.  Message priority may be specified inside the associated message 
   packet.
*/
#ifndef _i_cre_mbx

SYSCALL ER
i_cre_mbx(ID mbxid,T_CMBX *pk_cmbx)
	{
	MBXCB	*mbxcb;
	ER		ercd = E_OK;

	CHECK_MBXID(mbxid);
	CHECK_MBXACV(mbxid);
	CHECK_RSATR(pk_cmbx->mbxatr,TA_MPRI|TA_TPRI);
	mbxcb = get_mbxcb(mbxid);

	BEGIN_CRITICAL_SECTION;
	if ( mbxcb->mbxatr != OBJ_NONEXIST ) 
		{
		ercd =  E_OBJ;
		}
	else 
		{
		_cre_mbx(mbxcb,pk_cmbx);
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_cre_mbx */

/*---------------------------------------------------------------------------*/
/* This system call creates the mailbox without specification of an ID.  
   Specifically, a control block and buffer area are allocated for the 
   mailbox to be created.  The ID number of the mailbox is returned.
*/
#ifndef _i_vcre_mbx

SYSCALL ER
i_vcre_mbx(T_CMBX *pk_cmbx)
	{
	MBXCB	*mbxcb;
	ER		ercd = E_OK;

	CHECK_RSATR(pk_cmbx->mbxatr,TA_MPRI|TA_TPRI);

	BEGIN_CRITICAL_SECTION;
	if ( queue_empty_p(&free_mbxcb) ) 
		{
		ercd =  EV_FULL;
		}
	else 
		{
		mbxcb = (MBXCB *)(free_mbxcb.next);
		_cre_mbx(mbxcb, pk_cmbx);
		ercd = (ER)(mbxcb->mbxid);
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_vcre_mbx */

/*---------------------------------------------------------------------------*/
/* This system call deletes the mailbox specified by 'mbxid'.
   Issuing this system call causes memory used for the control block and 
   buffer area of the associated mailbox to be released.  After this system 
   call is invoked, another mailbox having the same ID number can be created.
   This system call will complete normally even if there are tasks waiting 
   for a message at the mailbox.  In that case, an E_DLT error will be 
   returned to each waiting task.  If there are messages still in the 
   mailbox, they will be deleted along with the mailbox and no error will 
   result.
*/
#ifndef _i_del_mbx

SYSCALL ER
i_del_mbx(ID mbxid)
	{
	MBXCB	*mbxcb;
	ER		ercd = E_OK;

	CHECK_MBXID(mbxid);
	CHECK_MBXACV(mbxid);
	mbxcb = get_mbxcb(mbxid);
    
	BEGIN_CRITICAL_SECTION;
	if ( mbxcb->mbxatr == OBJ_NONEXIST ) 
		{
		ercd = E_NOEXS;
		}
	else 
		{
		wait_delete(&(mbxcb->wait_queue));
		#ifndef _i_vcre_mbx
		if ( !SYS_MBXID(mbxcb->mbxid) ) 
			{
			queue_insert(&(mbxcb->wait_queue),&free_mbxcb);
			}
		#endif /* _i_vcre_mbx */
		mbxcb->mbxatr = OBJ_NONEXIST;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_del_mbx */

/*---------------------------------------------------------------------------*/
/* This system call sends the message packet whose start address is given 
   by 'pk_msg' to the mailbox specified by 'mbxid'.  The start address 
   'pk_msg' is simply passed during receive without any message packet 
   contents being copied.
   If there are tasks already waiting for a message at the specified mailbox, 
   the task at the head of the queue will be released from waiting and the 
   start address of the message 'pk_msg' specified with 'snd_msg' will be 
   transferred to that task as one of the return parameters of 'rcv_msg'.
   If, on the other hand, there are no tasks waiting for a message at the 
   specified mailbox, the message sent will be placed on that mailbox's 
   message queue.  In neither case will the task issuing the 'snd_msg' call 
   be made to wait.
*/
#ifndef _i_snd_msg

SYSCALL ER
i_snd_msg(ID mbxid,T_MSG *pk_msg)
	{
	MBXCB	*mbxcb;
	TCB		*tcb;
	ER		ercd = E_OK;

	CHECK_MBXID(mbxid);
	CHECK_MBXACV(mbxid);
	mbxcb = get_mbxcb(mbxid);
    
	BEGIN_CRITICAL_SECTION;
	if (mbxcb->mbxatr == OBJ_NONEXIST) 
		{
		ercd = E_NOEXS;
		}
	else if ( !queue_empty_p(&(mbxcb->wait_queue)) )
		{
		/* Task already waiting on the message.
		*/
		tcb = (TCB *)(mbxcb->wait_queue.next);
		*(tcb->winfo.mbx.ppk_msg) = pk_msg;
		wait_release_ok(tcb);
		}
	else 
		{
		/* No task waiting on the message, so place the message into the 
		   queue.  The insertion is either based on priority or received
		   order (FIFO).
		*/
		if ( mbxcb->mbxatr & TA_MPRI ) 
			{
			queue_insert_mpri(pk_msg,&(mbxcb->mq_head));
			}
		else 
			{
			nextmsg(pk_msg) = (T_MSG *) 0;
			if ( mbxcb->mq_head ) 
				{
				nextmsg(mbxcb->mq_tail) = pk_msg;
				}
			else 
				{
				mbxcb->mq_head = pk_msg;
				}
			mbxcb->mq_tail = pk_msg;
			}
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_snd_msg */

/*---------------------------------------------------------------------------*/

#if !defined(_i_rcv_msg) || !defined(_i_prcv_msg) || !defined(_i_trcv_msg)

SYSCALL ER
i_trcv_msg(T_MSG **ppk_msg,ID mbxid,TMO tmout)
	{
	MBXCB	*mbxcb;
	ER		ercd = E_OK;
    
	CHECK_MBXID(mbxid);
	CHECK_MBXACV(mbxid);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();
	mbxcb = get_mbxcb(mbxid);
    
	BEGIN_CRITICAL_SECTION;
	if (mbxcb->mbxatr == OBJ_NONEXIST) 
		{
		ercd = E_NOEXS;
		}
	else if ( mbxcb->mq_head ) 
		{
		*ppk_msg = mbxcb->mq_head;
		mbxcb->mq_head = nextmsg(*ppk_msg);
		}
	else 
		{
		ercd = E_TMOUT;
		if ( tmout != TMO_POL ) 
			{
			ctxtsk->wspec = (mbxcb->mbxatr & TA_TPRI) ?	&wspec_mbx_tpri : &wspec_mbx_tfifo;
			ctxtsk->wercd = &ercd;
			ctxtsk->winfo.mbx.ppk_msg = ppk_msg;
			gcb_make_wait((GCB *)mbxcb,tmout);
			}
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_trcv_msg */

/*---------------------------------------------------------------------------*/
/* The 'rcv_msg' system call receives a message from the mailbox specified 
   by 'mbxid'.  If there is no message in the specified mailbox (if the 
   message queue is empty), the task issuing this system call will enter the 
   WAIT state, and be put on the queue for waiting for arriving messages.  
   If the message queue of the mailbox is not empty, the first message on 
   the message queue will be retrieved and returned to the issuing task as 
   the return parameter 'pk_msg'.
*/
#ifndef _i_rcv_msg

SYSCALL ER
i_rcv_msg(T_MSG **ppk_msg,ID mbxid)
	{
	return(i_trcv_msg(ppk_msg,mbxid,TMO_FEVR));
	}

#endif /* _i_rcv_msg */

/*---------------------------------------------------------------------------*/
/* The 'prcv_msg' system call is the same as 'rcv_msg' without the waiting 
   feature.  If there are no messages in the specified mailbox, an E_TMOUT 
   error is returned to indicate polling failed and the system call finishes.
   Unlike 'rcv_msg', the issuing task does not wait in this case and the 
   status of the mailbox and the message queue remain unchanged.
*/
#ifndef _i_prcv_msg

SYSCALL ER
i_prcv_msg(T_MSG **ppk_msg,ID mbxid)
	{
	return(i_trcv_msg(ppk_msg,mbxid,TMO_POL));
	}

#endif /* _i_prcv_msg */

/*---------------------------------------------------------------------------*/

#ifndef _i_ref_mbx

SYSCALL ER
i_ref_mbx(T_RMBX *pk_rmbx,ID mbxid)
	{
	MBXCB	*mbxcb;
	ER		ercd = E_OK;

	CHECK_MBXID(mbxid);
	CHECK_MBXACV(mbxid);
	mbxcb = get_mbxcb(mbxid);
    
	BEGIN_CRITICAL_SECTION;
	if (mbxcb->mbxatr == OBJ_NONEXIST) 
		{
		ercd = E_NOEXS;
		}
	else 
		{
		pk_rmbx->exinf = mbxcb->exinf;
		pk_rmbx->wtsk = wait_tskid(&(mbxcb->wait_queue));
		if (mbxcb->mq_head) 
			{
			pk_rmbx->pk_msg = (T_MSG *)NADR;
			}
		else 
			{
			pk_rmbx->pk_msg = mbxcb->mq_head;
			}
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_ref_mbx */

/*---------------------------------------------------------------------------*/

#endif /* USE_MBX */
