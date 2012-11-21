/* Various constants for operating system.  Based on 'config.def' file and
   'config.v' perl program.  Have also included most of  'isysconfig.h' within 
   this file

   Notes:
   ------
   a) The INDEX_??? defines are not all 100% compliant with the specification,
      as they do not take into account the reserved nature of ID's -4 to 0.
	  Some however, do.

   Mods:
   -----
   a) Increase NUM_CYC from 20 to 40 to permit more output reports.
*/
/*===========================================================================*/

#ifndef _CONFIG_
#define _CONFIG_

/* Define constants for operating system resources.
*/
#define	_no_support			0

/*---------------------------------------------------------------------------*/
/* Task configurations.
*/
#define MIN_TSKID			(-5)
#define MAX_TSKID			(20)
#define NUM_TSKID			(21)
#define INDEX_TSK(id)		(id<0 ? (id-MIN_TSKID) : (id-MIN_TSKID-5) )
#define CHK_TSKID(id)		((MIN_TSKID<=id && id<-4) || (0<id && id<=MAX_TSKID))
#define ID_TSK(index)		(index<=(-MIN_TSKID-5) ? (index+MIN_TSKID) : (index+MIN_TSKID+5))
#define SYS_TSKID(id)		(id<0)
#define TSK_INIT			(-5)

#define	MAX_PRI				256
#define	MIN_PRI				1
#define	NUM_PRI				(MAX_PRI-MIN_PRI+1)
#define	MIN_SYS_STACK_SIZE	0x100					/* ??? - 256 bytes	*/
#define	DEF_SYS_STACK_SIZE	0x100					/* ??? - 256 bytes	*/

/*---------------------------------------------------------------------------*/
/* Semaphore configurations.
*/
#define	MAX_SEMID			8
#define	MIN_SEMID			1
#define	NUM_SEMID			(MAX_SEMID-MIN_SEMID+1)
#define CHK_SEMID(id)		(MIN_SEMID<=id && id<=MAX_SEMID)
#define	INDEX_SEM(id)		(id-MIN_SEMID)
#define	ID_SEM(index)		(index+MIN_SEMID)
#define	SYS_SEMID(id)		(id<0)

/*---------------------------------------------------------------------------*/
/* Eventflag configurations.
*/
#define	MAX_FLGID			4
#define	MIN_FLGID			1
#define	NUM_FLGID			(MAX_FLGID-MIN_FLGID+1)
#define CHK_FLGID(id)		(MIN_FLGID<=id && id<=MAX_FLGID)
#define	INDEX_FLG(id)		(id-MIN_FLGID)
#define ID_FLG(index)		(index+MIN_FLGID)
#define SYS_FLGID(id)		0

/*---------------------------------------------------------------------------*/
/* Mailbox configurations
*/
#define	MAX_MBXID			4
#define	MIN_MBXID			1
#define	NUM_MBXID			(MAX_MBXID-MIN_MBXID+1)
#define	INDEX_MBX(id)		(id-MIN_MBXID)
#define	ID_MBX(index)		(index+MIN_MBXID)
#define	SYS_MBXID(id)		(id<0)

/*---------------------------------------------------------------------------*/
/* Message buffer configurations
*/
#define	NUM_KERNEL_MBF		0
#define	MAX_MBFID			20
#define	MIN_MBFID			1
#define	NUM_MBFID			(MAX_MBFID-MIN_MBFID+1)
#define	INDEX_MBF(id)		(id-MIN_MBFID)
#define	ID_MBF(index)		(index+MIN_MBFID)
#define	SYS_MBFID(id)		(id<0)

/*---------------------------------------------------------------------------*/
/* Port/Rendezous configurations
*/
#define	MAX_PORID			8
#define	MIN_PORID			1
#define	NUM_PORID			0
#define	INDEX_POR(id)		(id-MIN_PISID)

/*---------------------------------------------------------------------------*/
/* Priority inheritance semaphore configurations
*/
#define	MAX_PISID			8
#define	MIN_PISID			1
#define	NUM_PISID			0
#define	INDEX_PIS(id)		(id-MIN_PISID)
#define	ID_PIS(index)		((index)+MIN_PISID)
#define	SYS_PISID(id)		(id<0)

/*---------------------------------------------------------------------------*/
/* Variable size memory pool configurations.
   Here there is a system memory pool, with ID -5, while the ordinary
   memory pools have IDs of 1 to 4. 
*/
#define	NUM_KERNEL_MPL		0
#define MIN_MPLID			-5
#define MAX_MPLID			7
#define NUM_MPLID			8
#define CHK_MPLID(id)		((MIN_MPLID<=id && id<-3)||(0<id && id<=MAX_MPLID))
#define SYS_MPLID(id)		(id < 0)
#define INDEX_MPL(id)		(id < 0 ? (id-MIN_MPLID) : (id-MIN_MPLID-4))
#define ID_MPL(index)		(index <= (-MIN_MPLID-4) ? (index+MIN_MPLID) : (index+MIN_MPLID+4))

/*---------------------------------------------------------------------------*/
/* Fixed size memory pool configurations.
*/
#define	MAX_MPFID			8
#define	MIN_MPFID			1
#define	NUM_MPFID			0
#define	INDEX_MPF(mpfid)	((mpfid)-MIN_MPFID)

/*---------------------------------------------------------------------------*/
/* Number of cyclic handlers.
*/
#define	NUM_CYC				40
#define	NUM_ALM				0
#define	NUM_SVC				0

/*---------------------------------------------------------------------------*/

#define	CHKEN				1		/* Enable/disable CHK's	*/

#if CHKEN
#define	CHK_NOSPT						/* E_NOSPT				*/
#define	CHK_RSATR						/* E_RSATR				*/
#define	CHK_PAR							/* E_PAR				*/
#define	CHK_ID							/* E_ID					*/
#define	CHK_OACV						/* E_OACV				*/
#define	CHK_CTX					
#define	CHK_CTX1				
#define	CHK_CTX2				
#define	CHK_SELF				
#endif

/*---------------------------------------------------------------------------*/

#define	PRISEM_SPEC			1

#define	USE_SUSPEND_SELF
#undef	USE_QTSK_PORTION
#define	USE_AUTO_ID
#undef	USE_DEBUG_SUPPORT
#undef	USE_VGET_TIM
#undef	USE_TASK_MAILBOX

/*---------------------------------------------------------------------------*/
/* Macros, that when defined prevent compilation of the associated system
   calls.
*/
#if NUM_SEMID
#define USE_SEM
#endif

#if NUM_FLGID
#define USE_FLG
#endif

#if NUM_MBXID
#define USE_MBX
#endif

#if NUM_MBFID
#define USE_MBF
#endif

#if NUM_PORID
#define USE_POR
#endif

#if NUM_PISID
#define USE_PIS
#endif

#if NUM_MPLID
#define USE_MPL
#endif

#if NUM_MPFID
#define USE_MPF
#endif

#if NUM_CYC
#define USE_CYC
#endif

#if NUM_ALM
#define USE_ALM
#endif

#if NUM_SVC
#define USE_SVC
#endif

/*---------------------------------------------------------------------------*/

#ifndef USE_SEM
#define _i_cre_sem		_no_support
#define _i_vcre_sem		_no_support
#define _i_del_sem		_no_support
#define _i_sig_sem		_no_support
#define _i_wai_sem		_no_support
#define _i_preq_sem		_no_support
#define _i_twai_sem		_no_support
#define _i_ref_sem		_no_support
#endif /* USE_SEM */

#ifndef USE_FLG
#define _i_cre_flg		_no_support
#define _i_vcre_flg		_no_support
#define _i_del_flg		_no_support
#define _i_set_flg		_no_support
#define _i_clr_flg		_no_support
#define _i_wai_flg		_no_support
#define _i_pol_flg		_no_support
#define _i_twai_flg		_no_support
#define _i_ref_flg		_no_support
#endif /* USE_FLG */

#ifndef USE_MBX
#define _i_cre_mbx		_no_support
#define _i_vcre_mbx		_no_support
#define _i_del_mbx		_no_support
#define _i_snd_msg		_no_support
#define _i_rcv_msg		_no_support
#define _i_prcv_msg		_no_support
#define _i_trcv_msg		_no_support
#define _i_ref_mbx		_no_support
#endif /* USE_MBX */

#ifndef USE_MBF
#define _i_cre_mbf		_no_support
#define _i_vcre_mbf		_no_support
#define _i_del_mbf		_no_support
#define _i_snd_mbf		_no_support
#define _i_psnd_mbf		_no_support
#define _i_tsnd_mbf		_no_support
#define _i_rcv_mbf		_no_support
#define _i_prcv_mbf		_no_support
#define _i_trcv_mbf		_no_support
#define _i_ref_mbf		_no_support
#endif /* USE_MBF */

#ifndef USE_POR
#define _i_cre_por		_no_support
#define _i_vcre_por		_no_support
#define _i_del_por		_no_support
#define _i_cal_por		_no_support
#define _i_pcal_por		_no_support
#define _i_tcal_por		_no_support
#define _i_acp_por		_no_support
#define _i_pacp_por		_no_support
#define _i_tacp_por		_no_support
#define _i_fwd_por		_no_support
#define _i_rpl_rdv		_no_support
#define _i_ref_por		_no_support
#endif /* USE_POR */

#ifndef USE_PIS
#define _i_vcre_pis		_no_support
#define _i_vvcre_pis	_no_support
#define _i_vdel_pis		_no_support
#define _i_vsig_pis		_no_support
#define _i_vwai_pis		_no_support
#define _i_vpreq_pis	_no_support
#define _i_vtwai_pis	_no_support
#define _i_vref_pis		_no_support
#endif /* USE_PIS */

#ifndef USE_TASK_MAILBOX
#define _i_vsnd_tmb		_no_support
#define _i_vtrcv_tmb	_no_support
#define _i_vprcv_tmb	_no_support
#define _i_vrcv_tmb		_no_support
#define _i_vref_tmb		_no_support
#endif /* USE_TASK_MAILBOX */

#ifndef USE_MPL
#define _i_cre_mpl		_no_support
#define _i_vcre_mpl		_no_support
#define _i_del_mpl		_no_support
#define _i_get_blk		_no_support
#define _i_pget_blk		_no_support
#define _i_tget_blk		_no_support
#define _i_rel_blk		_no_support
#define _i_ref_mpl		_no_support
#endif /* USE_MPL */

#ifndef USE_MPF
#define _i_cre_mpf		_no_support
#define _i_vcre_mpf		_no_support
#define _i_del_mpf		_no_support
#define _i_get_blf		_no_support
#define _i_pget_blf		_no_support
#define _i_tget_blf		_no_support
#define _i_rel_blf		_no_support
#define _i_ref_mpf		_no_support
#endif /* USE_MPF */

#ifndef USE_CYC
#define _i_def_cyc		_no_support
#define _i_vdef_cyc		_no_support
#define _i_act_cyc		_no_support
#define _i_ref_cyc		_no_support
#endif /* USE_CYC */

#ifndef USE_ALM
#define _i_def_alm		_no_support
#define _i_vdef_alm		_no_support
#define _i_ref_alm		_no_support
#endif /* USE_ALM */

#ifndef USE_SVC
#define _i_def_svc		_no_support
#endif /* USE_SVC */

#ifndef USE_AUTO_ID
#define _i_vcre_tsk		_no_support
#define _i_vcre_sem		_no_support
#define _i_vcre_flg		_no_support
#define _i_vcre_mbx		_no_support
#define _i_vcre_mbf		_no_support
#define _i_vcre_por		_no_support
#define _i_vvcre_pis	_no_support
#define _i_vcre_mpl		_no_support
#define _i_vcre_mpf		_no_support
#define _i_vdef_cyc		_no_support
#define _i_vdef_alm		_no_support
#endif /* USE_AUTO_ID */

#ifndef USE_DEBUG_SUPPORT
#define _i_vget_reg		_no_support
#define _i_vset_reg		_no_support
#endif /* USE_DEBUG_SUPPORT */

#ifndef USE_VGET_TIM
#define _i_vget_tim		_no_support
#endif /* USE_VGET_TIM */

/*===========================================================================*/
#endif	/* _CONFIG_	*/
