#include "cpu.h"

void LoadContext(CTXB *ctxb)
	{
	__asm																\
		{																\
		MOV		r0,ctxb				/* Load r1						*/;	\
		MOV		r1, r0				/* Save r1						*/;	\
		MOV		r0,ctxb->ri[2]		/* Load r2						*/;	\
		MOV		r2, r0				/* Save r2						*/;	\
		MOV		r0,ctxb->ri[3]		/* Load r3						*/;	\
		MOV		r3, r0				/* Save r3						*/;	\
		MOV		r0,ctxb->ri[4]		/* Load r4						*/;	\
		MOV		r4, r0				/* Save r4						*/;	\
		MOV		r0,ctxb->ri[5]		/* Load r5						*/;	\
		MOV		r5, r0				/* Save r5						*/;	\
		MOV		r0,ctxb->ri[6]		/* Load r6						*/;	\
		MOV		r6, r0				/* Save r6						*/;	\
		MOV		r0,ctxb->ri[7]		/* Load r7						*/;	\
		MOV		r7, r0				/* Save r7						*/;	\
		MOV		r0,ctxb->ri[8]		/* Load r8						*/;	\
		MOV		r8, r0				/* Save r8						*/;	\
		MOV		r0,ctxb->ri[9]		/* Load r9						*/;	\
		MOV		r9, r0				/* Save r9						*/;	\
		MOV		r0,ctxb->ri[10]		/* Load r10						*/;	\
		MOV		r10, r0				/* Save r10						*/;	\
		MOV		r0,ctxb->ri[11]		/* Load r11						*/;	\
		MOV		r11, r0				/* Save r11						*/;	\
		MOV		r0,ctxb->ri[2]		/* Load r12						*/;	\
		MOV		r12, r0				/* Save r12						*/
		}
	}