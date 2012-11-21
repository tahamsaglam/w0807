/* fncode.h
   Header file providing uITRON function codes.

   Notes & Mods:
   -------------
   a) This file is generated from syscall.def by genisyscall.  However, since
      in a DOS environment the PERL script 'genisyscall' cannot be run unless 
	  PERL has been installed, the supplied file is used directly. 
   b) This file is actually pretty useless unless the function codes are 
      actually used, which means that each system calls must be implemented 
	  using software interrupts, which means each system call is allocated 
	  its own software interrupt.  However, the RTOS port may not have been 
	  implemented this way!
	  
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

#ifndef _FNCODE_
#define _FNCODE_

#define TFN_UNL_CPU		(-7)
#define TFN_LOC_CPU		(-8)
#define TFN_DEF_SVC		(-9)
#define TFN_REF_SYS		(-12)
#define TFN_REF_CFG		(-14)
#define TFN_GET_VER		(-16)
#define TFN_CRE_TSK		(-17)
#define TFN_DEL_TSK		(-18)
#define TFN_REF_TSK		(-20)
#define TFN_EXT_TSK		(-21)
#define TFN_EXD_TSK		(-22)
#define TFN_STA_TSK		(-23)
#define TFN_GET_TID		(-24)
#define TFN_TER_TSK		(-25)
#define TFN_CHG_PRI		(-27)
#define TFN_ROT_RDQ		(-28)
#define TFN_ENA_DSP		(-29)
#define TFN_DIS_DSP		(-30)
#define TFN_REL_WAI		(-31)
#define TFN_SUS_TSK		(-33)
#define TFN_RSM_TSK		(-35)
#define TFN_FRSM_TSK	(-36)
#define TFN_TSLP_TSK	(-37)
#define TFN_SLP_TSK		(-38)
#define TFN_WUP_TSK		(-39)
#define TFN_CAN_WUP		(-40)
#define TFN_CRE_FLG		(-41)
#define TFN_DEL_FLG		(-42)
#define TFN_REF_FLG		(-44)
#define TFN_WAI_FLG		(-46)
#define TFN_CLR_FLG		(-47)
#define TFN_SET_FLG		(-48)
#define TFN_CRE_SEM		(-49)
#define TFN_DEL_SEM		(-50)
#define TFN_REF_SEM		(-52)
#define TFN_WAI_SEM		(-53)
#define TFN_SIG_SEM		(-55)
#define TFN_CRE_MBX		(-57)
#define TFN_DEL_MBX		(-58)
#define TFN_REF_MBX		(-60)
#define TFN_RCV_MSG		(-61)
#define TFN_SND_MSG		(-63)
#define TFN_DEF_INT		(-65)
#define TFN_CHG_IXX		(-67)
#define TFN_REF_IXX		(-68)
#define TFN_ENA_INT		(-71)
#define TFN_DIS_INT		(-72)
#define TFN_CRE_MPF		(-73)
#define TFN_DEL_MPF		(-74)
#define TFN_REF_MPF		(-76)
#define TFN_GET_BLF		(-77)
#define TFN_REL_BLF		(-79)
#define TFN_VGET_TIM	(-82)
#define TFN_SET_TIM		(-83)
#define TFN_GET_TIM		(-84)
#define TFN_DLY_TSK		(-85)
#define TFN_DEF_ALM		(-89)
#define TFN_DEF_CYC		(-90)
#define TFN_REF_ALM		(-91)
#define TFN_REF_CYC		(-92)
#define TFN_ACT_CYC		(-94)
#define TFN_NGET_NOD	(-97)
#define TFN_NGET_VER	(-98)
#define TFN_NREA_DAT	(-99)
#define TFN_NWRI_DAT	(-100)
#define TFN_PGET_BLF	(-103)
#define TFN_PGET_BLK	(-104)
#define TFN_POL_FLG		(-106)
#define TFN_PREQ_SEM	(-107)
#define TFN_PRCV_MSG	(-108)
#define TFN_PSND_MBF	(-109)
#define TFN_PRCV_MBF	(-110)
#define TFN_PCAL_POR	(-111)
#define TFN_PACP_POR	(-112)
#define TFN_CRE_MPL		(-137)
#define TFN_DEL_MPL		(-138)
#define TFN_REF_MPL		(-140)
#define TFN_GET_BLK		(-141)
#define TFN_REL_BLK		(-143)
#define TFN_CRE_POR		(-145)
#define TFN_DEL_POR		(-146)
#define TFN_REF_POR		(-148)
#define TFN_CAL_POR		(-149)
#define TFN_ACP_POR		(-150)
#define TFN_RPL_RDV		(-151)
#define TFN_FWD_POR		(-152)
#define TFN_CRE_MBF		(-153)
#define TFN_DEL_MBF		(-154)
#define TFN_REF_MBF		(-156)
#define TFN_SND_MBF		(-157)
#define TFN_RCV_MBF		(-158)
#define TFN_TGET_BLF	(-167)
#define TFN_TGET_BLK	(-168)
#define TFN_TWAI_FLG	(-170)
#define TFN_TWAI_SEM	(-171)
#define TFN_TRCV_MSG	(-172)
#define TFN_TSND_MBF	(-173)
#define TFN_TRCV_MBF	(-174)
#define TFN_TCAL_POR	(-175)
#define TFN_TACP_POR	(-176)
#define TFN_VSET_REG	(-225)
#define TFN_VGET_REG	(-226)
#define TFN_VCRE_TSK	(-241)
#define TFN_VCRE_SEM	(-242)
#define TFN_VCRE_FLG	(-243)
#define TFN_VCRE_MBX	(-244)
#define TFN_VCRE_MBF	(-245)
#define TFN_VCRE_POR	(-246)
#define TFN_VCRE_MPL	(-247)
#define TFN_VCRE_MPF	(-248)
#define TFN_VDEF_CYC	(-255)
#define TFN_VDEF_ALM	(-256)
#define TFN_VCRE_PIS	(-257)
#define TFN_VDEL_PIS	(-258)
#define TFN_VVCRE_PIS	(-259)
#define TFN_VREF_PIS	(-260)
#define TFN_VWAI_PIS	(-261)
#define TFN_VPREQ_PIS	(-262)
#define TFN_VTWAI_PIS	(-263)
#define TFN_VSIG_PIS	(-264)
#define TFN_VREF_TMB	(-268)
#define TFN_VRCV_TMB	(-269)
#define TFN_VPRCV_TMB	(-270)
#define TFN_VTRCV_TMB	(-271)
#define TFN_VSND_TMB	(-272)

#endif /* _ITRON_FNCODE_ */

/*---------------------------------------------------------------------------*/
