/*
***********************************************************
*                      UCOS186C.C
*               80186/80188 Specific Code
*                  SMALL MEMORY MODEL
***********************************************************
*                     INCLUDE FILES
***********************************************************
*/
#include "INCLUDES.H"

extern UBYTE    OSMapTbl[];
extern UBYTE    OSUnMapTbl[];
extern rb_tree  * tree;
/*
***********************************************************
*                    CREATE A TASK
***********************************************************
*/

UBYTE OSTaskCreate(void (far *task)(void *dptr), void  *data, void  *pstk, UBYTE  p)
{
    OS_TCB *ptr;
    UWORD  *stk;


    if (OSTCBPrioTbl[p] == (OS_TCB *)0) {                       /* Avoid creating task if already exist */
        ptr              = OSTCBGetFree();
        ptr->OSTCBPrio   = (UBYTE)p;
        ptr->OSTCBStat   = OS_STAT_RDY;
        ptr->OSTCBDly    = 0;

        stk              = (UWORD *)pstk;                       /* 80186/80188 Small Model              */
        *--stk           = (UWORD)FP_OFF(data);
        *--stk           = (UWORD)FP_SEG(task);
        *--stk           = (UWORD)FP_OFF(task);
        *--stk           = (UWORD)0x0200;                       /* PSW = Int. En.                       */
        *--stk           = (UWORD)FP_SEG(task);
        *--stk           = (UWORD)FP_OFF(task);
        *--stk           = (UWORD)0x0000;                       /* AX = 0                               */
        *--stk           = (UWORD)0x0000;                       /* CX = 0                               */
        *--stk           = (UWORD)0x0000;                       /* DX = 0                               */
        *--stk           = (UWORD)0x0000;                       /* BX = 0                               */
        *--stk           = (UWORD)0x0000;                       /* SP = 0                               */
        *--stk           = (UWORD)0x0000;                       /* BP = 0                               */
        *--stk           = (UWORD)0x0000;                       /* SI = 0                               */
        *--stk           = (UWORD)0x0000;                       /* DI = 0                               */
        *--stk           = (UWORD)0x0000;                       /* ES = 0                               */
        ptr->OSTCBStkPtr = (void *)stk;                         /* Load SP in TCB                       */
        OS_ENTER_CRITICAL();
		tree->rb_insert(tree, ptr);

		OS_EXIT_CRITICAL();
        if (OSRunning) {
            OSSched();
        }
        return (OS_NO_ERR);
    } else {
        return (OS_PRIO_EXIST);
    }
}
