/* mempool.c
   Memory management module for uITRON based OS.

   Notes:
   ------
   a) This module implements variable size memory management/memory pools.
   b) A memory pool is a large block of memory, from which smaller blocks
      of memory may be allocated for use & then returned once no longer
      required.
   c) The method originally supplied in with the ITIS ITRON implementation
      employed doubly linked blocks of memory, using the same machinery 
	  used to implement queues.  The implementation also made extensive
	  use of macros.  However, when the code was compiled on the ARM
	  compiler, many of the macros did not compile properly due to the
	  error checking features of the compiler.  Furthermore, the use of
	  blocks of memory doubly linked together means that two linked lists
	  must be maintained.  The first is a linked list of all the blocks
	  in the system, while the second is a subset of those blocks that 
	  are free for allocation.  The sizes of the free blocks are determined
	  by taking the difference between the address of the start of the free
	  block and the address of the next block.  When blocks are returned 
	  after use, it is necessary to determine whether the previous block
	  is being used or not so that the blocks can be coalesced, which
	  means blocks must be marked as being free or available.  In the
	  orignal implementation, the least significant bit of the link pointers
	  was used for this purpose, as in a 16 or 32 bit system, these will
	  always be zero.
   d) Point (c) indicates that the original method was quite complex.  As
      a result, the routines have been replaced with a method taken from
	  "The C Programming Language by Kernighan & Ritchie".  This method
	  is significantly simpler and easier to test, as it stores links the
	  free blocks together and uses a size field to determine the size 
	  of each block.

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

#ifdef USE_MPL

#include "task.h"
#include "wait.h"
#include "check.h"

/*---------------------------------------------------------------------------*/
/* Header block for each block of free memory.
*/
typedef struct memhdr
	{
	struct memhdr		*next;		/* Points to next memory header		*/
	INT					size;		/* Size of block in units			*/
	} MEMHDR;

/*---------------------------------------------------------------------------*/
/* This structure defines a 'memory-pool control block', which is used to
   control a large block of memory from which smaller blocks may be allocated
   and returned after use.
*/
typedef struct memorypool_control_block 
	{
	QUEUE	wait_queue;				/* Tasks waiting on memory			*/
	ID		mplid;					/* Memory pool ID					*/
	VP		exinf;					/* Extended information				*/
	ATR		mplatr;					/* Memorypool attributes			*/
	INT		mplsz;					/* Memorypool size					*/
	VP		mempool;				/* Base Pointer to memory pool		*/
	MEMHDR	*freemem;				/* Start of free memory header		*/
	} MPLCB;

/*---------------------------------------------------------------------------*/
/* Table of memory pools.  Different memory-pools may exist for different
   applications.  The static variable 'sys_memhdr' points to the initial
   pool from which all other pools are derived.
*/
static MPLCB		mplcb_table[NUM_MPLID];
static MEMHDR		*sys_memhdr;
static const INT	magic_next = 0x1234AAAA;

/*---------------------------------------------------------------------------*/
/* Defines for common-operations used throughout this source-file.
*/
#define get_mplcb(id)		(&(mplcb_table[INDEX_MPL(id)]))
#define MEMHDRSZ			(sizeof(MEMHDR))

/*---------------------------------------------------------------------------*/
/* 'free_mplcb' is a linked list of free MPLCB's ready for allocation.
   These are used in system call variants that automatically allocate and
   return a memory pool ID.
*/
#ifndef _i_vcre_mpl
QUEUE	free_mplcb;					/* Free memory pool control blocks	*/
#endif /* _i_vcre_mpl */

/*---------------------------------------------------------------------------*/
/* Given a memory-pool control block 'mplcb' for a valid memory-pool, 
   return the address of the end of memory for that memory pool.
*/
__inline VP
mempool_end(MPLCB *mplcb)
	{
	return((VP)(((VB *)(mplcb->mempool)) + mplcb->mplsz));
	}

/*---------------------------------------------------------------------------*/
/* Initialize a memory-pool with start address 'mempool' and size 'mplsz'
   bytes. 
*/
__inline static void
init_mpl(VP mempool,INT mplsz)
	{
	((MEMHDR*)mempool)->next = (MEMHDR*)mempool;
	((MEMHDR*)mempool)->size = mplsz/sizeof(MEMHDR);	/* MEMHDR units	*/
	}

/*---------------------------------------------------------------------------*/
/* Gets a block of memory of size 'blksz' from the free memory with header
   block 'ppfreemem'.  The free memory header pointer is updated by the
   function.  Returns a pointer to the allocated memory if successful, NULL 
   otherwise.
*/
static VP
_get_blk(MEMHDR **ppfreemem,INT blksz)
	{
	MEMHDR		*free, *prevfree;
	INT			units;

	units = 1 + (blksz + sizeof(MEMHDR)-1)/sizeof(MEMHDR);
	free = *ppfreemem;
	prevfree = free;
	free = prevfree->next;
	do
		{
		/* The freememory pointer is updated within this function because 
		   it is possible for the first block of memory to be allocated if 
		   it is the exact size.  It is necessary to maintain a pointer 
		   to the previous free block so that when an entire block 
		   is allocated (instead of being split), the previous block can 
		   be referenced in order to link it to the next block.
		*/
		if ( free->size>units )
			{
			/* A free block larger than the required size has been found,
			   so allocate memory at the end of the block and reduce the
			   size of the freeblock.			
			*/
			free->size -= units;		/* Adjust remaining memory size		*/
			free += free->size;			/* Allocated block start			*/
			free->size = units;			/* Allocated block size				*/
			free->next = (MEMHDR*)magic_next;
			*ppfreemem = prevfree;
			return((VP)(free+1));
			}
		else if ( free->size==units )
			{
			if ( free->next==free )
				return((VP)0);			/* Don't release all of last block	*/
			prevfree->next = free->next;
			free->next = (MEMHDR*)magic_next;			
			*ppfreemem = prevfree;
			return((VP)(free+1));		/* Allocate entire block			*/
			}
		else
			{
			/* No allocation, so try next block.
			*/
			prevfree = free;
			free = free->next;
			}
		}
	while ( free != *ppfreemem );
	
	return((VP)0);						/* Failed memory allocation			*/
	}

/*---------------------------------------------------------------------------*/
/* Release the block of memory with start address 'blk' back to the free 
   memory with header block 'ppfreemem'.
*/
static ER
_rel_blk(MEMHDR **ppfreemem,VP blk)
	{
	MEMHDR	*hdr, *newhdr;
	BOOL	surrounded;

	/* Search through the free list of memory blocks for the two memory
	   blocks that surround the new memory block to be freed.
	   This is illustrated below, where '.' indicates free memory, ':'
	   indicates the memory about to be freed & '+' indicates used
	   memory.
	   			hdr		newhdr		hdr->next
				.....	::::::		.........
	   A special case that must be dealt with is when the new block occurs
	   before the free block with the least address or after the free block
	   with the maximum address.  This case is tested at the wraparound
	   point.
	*/
	newhdr = (MEMHDR*)blk - 1;
	hdr = *ppfreemem;
	do
		{
		hdr = hdr->next;	
		surrounded = (hdr<newhdr && newhdr<hdr->next);
		if ( hdr>=hdr->next && (hdr<newhdr || newhdr<hdr->next) )
			break;
		}
	while ( !surrounded );

	/* This case tests to see if the new block can be merged with
	   the block at higher addresses (i.e to its right)
	   		hdr		newhdr		hdr->next
			....	::::::::::::.........
	   If this is not possible, then the block is simply linked in the
	   ordinary way.		
	*/	
	if ( newhdr+newhdr->size == hdr->next )
		{
		newhdr->size += hdr->next->size;
		newhdr->next = hdr->next->next;
		}
	else
		newhdr->next = hdr->next;

	/* This case tests to see if the new block can be merged with
	   the block at lower addresses (i.e to its left)
	   		hdr		newhdr		hdr->next
			........:::::::::	.........
	   If this is not possible, then the block is simply linked in the
	   ordinary way.		
	*/
	if ( hdr+hdr->size==newhdr )
		{
		hdr->size += newhdr->size;
		hdr->next = newhdr->next;
		}
	else
		hdr->next = newhdr;
	*ppfreemem = hdr;

	return(E_OK);
	}
	
/*---------------------------------------------------------------------------*/
/* Creates a SYSTEM memory pool from which all other memory pools are 
   derived.  The arguments passed by the calling function provide the
   start address 'mempool' and the size 'mplsz' of this initial block.
   These must be hardwired into the software.  As the size is the actual
   size of the memory available, an adjustment for the header block is
   initially made.
*/
ER
sys_cre_mpl(VP mempool,INT mplsz)
	{
	/* ???:	As an option, could zero all memory at this point.  This would
	   enable determination of the stack headroom by finding the lowest
	   stack address (downwards growing stack) with a non-zero value.
	*/
	mplsz -= MEMHDRSZ;
	init_mpl(mempool,mplsz);
	sys_memhdr = (MEMHDR*)mempool;
	return(E_OK);
	}

/*---------------------------------------------------------------------------*/
/* Gets a block of memory of size 'blksz'.
*/
VP
sys_get_blk(INT blksz)
	{
	assert(blksz > 0);
	return(_get_blk(&sys_memhdr,blksz));
	}

/*---------------------------------------------------------------------------*/
/* Releases a system 'blk' of memory.
*/
void
sys_rel_blk(VP blk)
	{
	ER	ercd;

 	ercd = _rel_blk(&sys_memhdr,blk);
	assert(ercd == E_OK);
 	}
	
/*---------------------------------------------------------------------------*/
/* Initialises the table of memory pool control blocks 'mplcb_table'.

   Note:
   -----
   This code contains some targer specific information, which if incorrect
   can cause an abort to occur.  The global variables listed below, namely
   'BottomOfSRAM', 'SizeOfSRAM' and 'StartOfFreeSRAM' need to be correctly
   filled in at startup for this to work correctly.

   On the GP4020, the maximum memory per chip select is limited to 512kB
   or 256kHW, although smaller amounts of memory may be installed to
   reduce cost.  Futhermore, the location of this memory will depend on
   which chip select is used to select the memory device.  On the first
   GP4020 prototype, memory starts at 0x20000000 bytes and has size 
   0x40000 bytes.
*/
extern int			StartOfFreeSRAM;
extern int			BottomOfSRAM;
extern int			SizeOfSRAM;

void
memorypool_initialize(void)
	{
	INT			i, TopOfSRAM;
	MPLCB		*mplcb;
	ID			mplid;

	#ifndef _i_vcre_mpl
	queue_initialize(&free_mplcb);
	#endif /* _i_vcre_mpl */

	for ( mplcb=mplcb_table,i = 0; i<NUM_MPLID; mplcb++,i++ )
		{
		mplid = ID_MPL(i);
		mplcb->mplid = mplid;
		mplcb->mplatr = OBJ_NONEXIST;
		#ifndef _i_vcre_mpl
		if ( !SYS_MPLID(mplid) )
			{
			queue_insert(&(mplcb->wait_queue),&free_mplcb);
			}
		#endif /* _i_vcre_mpl */
		}
		
	/* Initialise all free memory into the variable size memory pool.
	*/
	StartOfFreeSRAM += 8;
	TopOfSRAM = BottomOfSRAM + SizeOfSRAM - 8 - 0x700;
	sys_cre_mpl((VP)StartOfFreeSRAM,TopOfSRAM-StartOfFreeSRAM);
	}

/*---------------------------------------------------------------------------*/

static void
wakeup_mpl(MPLCB *mplcb)
	{
	QUEUE		*queue;
	TCB			*tcb;
	VP			blk;
	INT			blksz, maxsz;

	queue = mplcb->wait_queue.next;
	maxsz = mplcb->mplsz;
	while (queue != &(mplcb->wait_queue))
		{
		tcb = (TCB *) queue;
		queue = queue->next;
		blksz = tcb->winfo.mpl.blksz;
		if ( blksz <= maxsz )
			{
			blk = _get_blk(&(mplcb->freemem),blksz);
			if ( blk )
				{
				*(tcb->winfo.mpl.p_blk) = blk;
				wait_release_ok(tcb);
				}
			else if ( blksz < maxsz )
				{
				maxsz = blksz;
				}
			}
		}
	}

/*---------------------------------------------------------------------------*/

static WSPEC wspec_mpl_tfifo = { TTW_MPL, 0, 0 };
static WSPEC wspec_mpl_tpri = { TTW_MPL, obj_chg_pri, 0 };

/*---------------------------------------------------------------------------*/

#if !defined(_i_cre_mpl) || !defined(_i_vcre_mpl)

static ER
_cre_mpl(MPLCB *mplcb,T_CMPL *pk_cmpl,INT mplsz)
	{
	VP			mempool;

	mempool = sys_get_blk(mplsz + sizeof(MEMHDR));
	if ( mempool == 0 )
		{
		return(E_NOMEM);
		}

	#ifndef _i_vcre_mpl
	if ( !SYS_MPLID(mplcb->mplid) )
		{
		queue_delete(&(mplcb->wait_queue));
		}
	#endif /* _i_vcre_mpl */

	/* Clear the memory-pool 'wait_queue', set the attributes 'mplatr', 
	   extended information 'exinf', size 'mplsz' and starting addresss 
	   'mempool'.  Also initialise the start and end 'areas' of the 
	   memory-pool.
	*/
	queue_initialize(&(mplcb->wait_queue));
	mplcb->exinf = pk_cmpl->exinf;
	mplcb->mplatr = pk_cmpl->mplatr;
	mplcb->mplsz = mplsz;
	mplcb->mempool = mempool;
	init_mpl(mempool,mplsz);
	mplcb->freemem = (MEMHDR*)mempool;

	return(E_OK);
	}

#endif /* !defined(_i_cre_mpl) || !defined(_i_vcre_mpl) */

/*---------------------------------------------------------------------------*/
#ifndef _i_cre_mpl

SYSCALL ER
i_cre_mpl(ID mplid,T_CMPL *pk_cmpl)
	{
	MPLCB	*mplcb;
	INT		mplsz;
	ER		ercd = E_OK;

	CHECK_MPLID_OS(mplid);
	CHECK_MPLACV(mplid);
	CHECK_RSATR(pk_cmpl->mplatr,TA_FFIT|TA_TPRI);
	CHECK_PAR(pk_cmpl->mplsz > 0);
	mplsz = (pk_cmpl->mplsz/MEMHDRSZ)*MEMHDRSZ;
	mplcb = get_mplcb(mplid);

	BEGIN_CRITICAL_SECTION;
	if ( mplcb->mplatr!=OBJ_NONEXIST )
		{
		ercd = E_OBJ;
		}
	else
		{
		ercd = _cre_mpl(mplcb,pk_cmpl,mplsz);
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_cre_mpl */

/*---------------------------------------------------------------------------*/
#ifndef _i_vcre_mpl

SYSCALL ER
i_vcre_mpl(T_CMPL *pk_cmpl)
	{
	MPLCB	*mplcb;
	INT		mplsz;
	ER		ercd = E_OK;

	CHECK_RSATR(pk_cmpl->mplatr,TA_FFIT|TA_TPRI);
	CHECK_PAR(pk_cmpl->mplsz > 0);
	mplsz = (pk_cmpl->mplsz/MEMHDRSZ)*MEMHDRSZ;	

	BEGIN_CRITICAL_SECTION;
	if (queue_empty_p(&free_mplcb))
		{
		ercd = EV_FULL;
		}
	else
		{
		mplcb = (MPLCB *)(free_mplcb.next);
		ercd = _cre_mpl(mplcb,pk_cmpl,mplsz);
		if ( ercd == E_OK )
			{
			ercd = (ER)(mplcb->mplid);
			}
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_vcre_mpl */

/*---------------------------------------------------------------------------*/
#ifndef _i_del_mpl

SYSCALL ER
i_del_mpl(ID mplid)
	{
	extern void		sys_rel_blk(VP blk);
	MPLCB			*mplcb;
	ER				ercd = E_OK;

	CHECK_MPLID_OS(mplid);
	CHECK_MPLACV(mplid);
	mplcb = get_mplcb(mplid);

	BEGIN_CRITICAL_SECTION;
	if ( mplcb->mplatr == OBJ_NONEXIST )
		{
		ercd = E_NOEXS;
		}
	else
		{
		sys_rel_blk(mplcb->mempool);
		wait_delete(&(mplcb->wait_queue));
		#ifndef _i_vcre_mpl
		if (!SYS_MPLID(mplcb->mplid))
			{
			queue_insert(&(mplcb->wait_queue), &free_mplcb);
			}
		#endif /* _i_vcre_mpl */
		mplcb->mplatr = OBJ_NONEXIST;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_del_mpl */

/*---------------------------------------------------------------------------*/
/* (3.6) Get variable size memory block of size 'blksize' bytes from 
   memory-pool with ID 'mplid' and a failure timeout of 'tmout' (in same 
   timebase/units as the main clock, typically 'ms').  The start address 
   of the block is specified in 'p_blk'.
*/
#if !defined(_i_get_blk) || !defined(_i_pget_blk) || !defined(_i_tget_blk)

SYSCALL ER
i_tget_blk(VP* p_blk,ID mplid,INT blksz,TMO tmout)
	{
	MPLCB	*mplcb;
	VP		blk;
	ER		ercd = E_OK;

	CHECK_MPLID(mplid);
	CHECK_MPLACV(mplid);
	CHECK_PAR(blksz > 0);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH_POL(tmout);
	mplcb = get_mplcb(mplid);

	BEGIN_CRITICAL_SECTION;
	if ( mplcb->mplatr == OBJ_NONEXIST )
		{
		ercd = E_NOEXS;
		}
	else if ( (blk = _get_blk(&(mplcb->freemem),blksz))!=0 )
		{
		*p_blk = blk;
		}
	else
		{
		/* Code path executed if the required amount of memory cannot be
		   obtained on the initial request.  In this case, a WAIT on the
		   memory pool is initiated and necessary information is written
		   into the current tasks 'winfo' structure.  This contains the 
		   size of the required memory block and a pointer 'winfo.mpl.p_blk'
		   to a variable containing the address of the allocated block
		   This is modified when the task is woken & the memory actually 
		   allocated.
		*/
		ercd = E_TMOUT;
		if ( tmout != TMO_POL )
			{
			ctxtsk->wspec = (mplcb->mplatr & TA_TPRI) ?
				&wspec_mpl_tpri : &wspec_mpl_tfifo;
			ctxtsk->wercd = &ercd;
			ctxtsk->winfo.mpl.blksz = blksz;
			ctxtsk->winfo.mpl.p_blk = p_blk;
			gcb_make_wait((GCB *)mplcb,tmout);
			}
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_tget_blk */

/*---------------------------------------------------------------------------*/
#ifndef _i_get_blk

SYSCALL ER
i_get_blk(VP *p_blk,ID mplid,INT blksz)
	{
	return(i_tget_blk(p_blk,mplid,blksz,TMO_FEVR));
	}

#endif /* _i_get_blk */

/*---------------------------------------------------------------------------*/
/* (3.6) Poll and get variable size memory block of size 'blksz' bytes from
   memory-pool with ID 'mplid'.  The start address of the block is returned
   via 'p_blk'.
*/
#ifndef _i_pget_blk

SYSCALL ER
i_pget_blk(VP *p_blk,ID mplid,INT blksz)
	{
	return(i_tget_blk(p_blk,mplid, blksz,TMO_POL));
	}

#endif /* _i_pget_blk */

/*---------------------------------------------------------------------------*/
/* (3.6) Release a variable sized memory block with start address 'blk' 
   back to the memory-pool with ID 'mplid' (which must be the same memory-
   pool from which the block was obtained).  If any other tasks are waiting
   for memory, which comes available in the course of this call, then those
   tasks will be released from the WAIT state and may begin running.
*/
#ifndef _i_rel_blk

SYSCALL ER
i_rel_blk(ID mplid,VP blk)
	{
	MPLCB	*mplcb;
	ER		ercd = E_OK;

	CHECK_MPLID(mplid);
	CHECK_MPLACV(mplid);
	mplcb = get_mplcb(mplid);

	BEGIN_CRITICAL_SECTION;
	if (mplcb->mplatr == OBJ_NONEXIST)
		{
		ercd = E_NOEXS;
		}
	#ifdef CHK_PAR
	else if (!(mplcb->mempool <= blk && blk < mempool_end(mplcb)))
		{
		ercd = E_PAR;
		}
	#endif /* CHK_PAR */
	else if ((ercd = _rel_blk(&(mplcb->freemem),blk)) == E_OK)
		{
		wakeup_mpl(mplcb);
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_rel_blk */

/*---------------------------------------------------------------------------*/
/* (3.6) Returns information on the state of the variable size memory-pool
   with ID 'mplid', placing the results in 'pk_rmpl'.  This includes data
   such as as the total free memory size currently available, maximum
   contiguous memory size available, information on tasks waiting to be
   allocated memory and extended information.

   ??? SYSCALL ER ...
*/
#ifndef _i_ref_mpl

SYSCALL ER
i_ref_mpl(T_RMPL *pk_rmpl,ID mplid)
	{
	MPLCB	*mplcb;
	INT		frsz, maxsz, blksz;
	ER		ercd = E_OK;
	MEMHDR	*hdr, *freehdr;	

	CHECK_MPLID(mplid);
	CHECK_MPLACV(mplid);
	mplcb = get_mplcb(mplid);

	BEGIN_CRITICAL_SECTION;
	if ( mplcb->mplatr == OBJ_NONEXIST )
		{
		ercd = E_NOEXS;
		}
	else
		{
		pk_rmpl->exinf = mplcb->exinf;
		pk_rmpl->wtsk = wait_tskid(&(mplcb->wait_queue));
		frsz = maxsz = 0;
		freehdr = mplcb->freemem;
		hdr = freehdr->next;
		do
			{
			blksz = MEMHDRSZ*hdr->size;		/* Convert to bytes	*/
			frsz += blksz;
			if ( blksz>maxsz )
				maxsz = blksz;
			}
		while ( hdr!=freehdr );
		pk_rmpl->frsz = frsz;
		pk_rmpl->maxsz = maxsz;
		}
	END_CRITICAL_SECTION;
	return(ercd);
	}

#endif /* _i_ref_mpl */

/*---------------------------------------------------------------------------*/
#endif /* USE_MPL */
