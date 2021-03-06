/*
************************************************************
*                         UCOS.C
*                         KERNEL
************************************************************
*/


#include    "INCLUDES.h"
/*
************************************************************
*                         TABLES
************************************************************
*/
UBYTE const OSMapTbl[]   = {0x01, 0x02, 0x04, 0x08,
                            0x10, 0x20, 0x40, 0x80};
UBYTE const OSUnMapTbl[] = {
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};
/*
************************************************************
*                GLOBAL AND LOCAL VARIABLES
************************************************************
*/
		rb_tree		*tree;
		rb_tree		tree_manager;
		
        OS_TCB     *OSTCBCur;
        OS_TCB     *OSTCBHighRdy;
        OS_TCB     *OSTCBWaitList = NULL;		//等待链表头指针
        OS_TCB     *OSTCBPrioTbl[MAX_TASK_SIZE];
        BOOLEAN     OSRunning;
        UBYTE       OSRdyGrp;
        UBYTE       OSRdyTbl[MAX_TASK_SIZE >> 3];
        UBYTE       OSLockNesting;
        UBYTE       OSIntNesting;
        OS_TCB     *OSTCBFreeList;
		
/*
************************************************************
*                LOCAL FUNCTION PROTOTYPES
************************************************************
*/

static void  far   OSTaskIdle(void *data);
/*
************************************************************
*                   uCOS INITIALIZATION
************************************************************
*/

//rb_tree * get_rb_tree();

void OSInit(void *idle_task_stk, UBYTE maxtasks)
{
    int i;
    OSTCBCur      = (OS_TCB *)0;
    OSTCBWaitList      = (OS_TCB *)0;
    OSIntNesting  = 0;
    OSLockNesting = 0;
    OSRunning     = 0;

    
    for (i = 0; i < (maxtasks - 1); i++) {
		OSTCBTbl[i].OSTCBNext = &OSTCBTbl[i+1];
    }
    OSTCBTbl[maxtasks-1].OSTCBNext = (OS_TCB *)0;
    OSTCBFreeList                  = &OSTCBTbl[0];
	tree = get_rb_tree();
    OSTaskCreate(OSTaskIdle, (void *)0, idle_task_stk, OS_LO_PRIO);
}
/*
************************************************************
*                       IDLE TASK
************************************************************
*/
static void far OSTaskIdle(void *data)
{
    data = data;
    while (1)
	;
}
/*
************************************************************
*                    START MULTITASKING
************************************************************
*/
void OSStart(void)
{
    
	OSTCBHighRdy = tree->get_min_node(tree);
    OSTCBCur     = OSTCBHighRdy;
    OSRunning    = 1;
    OSStartHighRdy();
}
/*
************************************************************
*                RUN HIGHEST PRIORITY TASK
************************************************************
*/
void OSSched(void)
{
    OS_ENTER_CRITICAL();
    if (OSLockNesting == 0 && OSIntNesting == 0) {
        
	OSTCBHighRdy = tree->get_min_node(tree);
    if (OSTCBHighRdy != OSTCBCur) {
        OS_TASK_SW();
    }
    }
    OS_EXIT_CRITICAL();
}
/*
************************************************************
*                        ENTER ISR
************************************************************
*/
void OSIntEnter(void)
{
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
}
/*
************************************************************
*                        EXIT ISR
************************************************************
*/
void OSIntExit(void)
{
    OS_ENTER_CRITICAL();
    if (--OSIntNesting == 0 && OSLockNesting == 0) {
       
		OSTCBHighRdy = tree->get_min_node(tree);
        if (OSTCBHighRdy != OSTCBCur) {
            OSIntCtxSw();
        }
    }
    OS_EXIT_CRITICAL();
}
/*
************************************************************
*          DELAY TASK 'n' TICKS   (n from 1 to 65535)
************************************************************
*/
void OSTimeDly(UWORD ticks)
{
	OS_TCB * dly_node;

    OS_ENTER_CRITICAL();
 
	dly_node = tree->rb_delete(tree, OSTCBCur->OSTCBPrio);
	
	//修改状态
	OSTCBCur->OSTCBStat &= (~OS_STAT_RDY);
	
	OSTCBCur->OSTCBStat |=  OS_STAT_WAIT;
	/*插入至等待队列*/

	OSTCBCur->OSTCBNext = OSTCBWaitList;
	OSTCBWaitList = OSTCBCur;

    OSTCBCur->OSTCBDly = ticks;
    OS_EXIT_CRITICAL();
    OSSched();
}
/*
************************************************************
*                  PROCESS SYSTEM TICK
************************************************************
*/
void OSTimeTick(void){
    OS_TCB **ptcb;
	OS_TCB * tmp = NULL;

    ptcb = &OSTCBWaitList;
    while ( *ptcb != NULL) {
        OS_ENTER_CRITICAL();
		if ((*ptcb)->OSTCBDly != 0) {
			if (--(*ptcb)->OSTCBDly == 0) {
				
				tmp = *ptcb;
				*ptcb = (*ptcb)->OSTCBNext;
				tree->rb_insert(tree, tmp);
				
				//修改状态
				tmp->OSTCBStat &= (~OS_STAT_WAIT);
	
				tmp->OSTCBStat |= OS_STAT_RDY;
				
				if (*ptcb == NULL)
					break;
            }
        }
        OS_EXIT_CRITICAL();
		ptcb = &(*ptcb)->OSTCBNext;
    }
}
/*
************************************************************
*                GET TCB FROM FREE TCB LIST
************************************************************
*/
OS_TCB *OSTCBGetFree(void)
{
    OS_TCB *ptcb;


    OS_ENTER_CRITICAL();
    ptcb          = OSTCBFreeList;
    OSTCBFreeList = ptcb->OSTCBNext;
    OS_EXIT_CRITICAL();
    return (ptcb);
}


/*
************************************************************
*                   PREVENT SCHEDULING
************************************************************
*/
void OSLock(void)
{
    OS_ENTER_CRITICAL();
    OSLockNesting++;
    OS_EXIT_CRITICAL();
}
/*
************************************************************
*                    ENABLE SCHEDULING
************************************************************
*/
void OSUnlock(void)
{
    OS_ENTER_CRITICAL();
    OSLockNesting--;
    OS_EXIT_CRITICAL();
    OSSched();
}
/*
************************************************************
*              CHANGE PRIORITY OF RUNNING TASK
************************************************************
*/

UBYTE OSChangePrio(UBYTE newp)
{
    UBYTE oldp;
	OS_TCB * ptcb = tree->get_node(tree, newp);
	OS_TCB * new_tcb = (OS_TCB *)0;
    OS_ENTER_CRITICAL();
	
	
	
    if (ptcb != tree->nil) {
        OS_EXIT_CRITICAL();
        return (OS_PRIO_EXIST);
    } else {
		//======
		oldp = OSTCBCur->OSTCBPrio;
        new_tcb = tree->rb_delete(tree, oldp);
		new_tcb->OSTCBPrio = newp;
		tree->rb_insert(tree, new_tcb);
		//======
        OS_EXIT_CRITICAL();
        OSSched();
        return (OS_NO_ERR);
    }
}
/*
************************************************************
*                    DELETE RUNNING TASK
************************************************************
*/
void OSTaskDelete(void)
{
    OS_ENTER_CRITICAL();
	
	//=====
    tree->rb_delete(tree,  OSTCBCur->OSTCBPrio);
	OSTCBCur->OSTCBPrev = OSTCBCur->OSTCBParent = NULL;
	//=====
	
	
	
    OSTCBCur->OSTCBNext = OSTCBFreeList;
    OSTCBFreeList       = OSTCBCur;
    OS_EXIT_CRITICAL();
    OSSched();
}

/*IPC部分，未修改*/
/*
************************************************************
*                   INITIALIZE SEMAPHORE
************************************************************
*/
UBYTE OSSemInit(OS_SEM *psem, WORD cnt)
{
	int i;
    OS_ENTER_CRITICAL();
    if (cnt >= 0) {
        psem->OSSemCnt    = cnt;
       
        for(i = 0; i < MAX_TASK_SIZE; i++)
			psem->OSSemTbl[i] = 0;
		
        OS_EXIT_CRITICAL();
        return (OS_NO_ERR);
    } else {
        OS_EXIT_CRITICAL();
        return (OS_SEM_ERR);
    }
}
/*
************************************************************
*                     PEND ON SEMAPHORE
************************************************************
*/
UBYTE OSSemPend(OS_SEM *psem, UWORD timeout)
{
    UBYTE x, bitx;
	int y;

    OS_ENTER_CRITICAL();
    if (psem->OSSemCnt-- > 0) {
        OS_EXIT_CRITICAL();
        return (OS_NO_ERR);
    } else {
        OSTCBCur->OSTCBStat |= OS_STAT_SEM;
        OSTCBCur->OSTCBDly   = timeout;
        y                    = OSTCBCur->OSTCBPrio >> 3;
        x                    = OSTCBCur->OSTCBPrio & 0x07;
        bitx                 = OSMapTbl[x];
		OSRdyTbl[y] &= ~bitx;
		
        psem->OSSemTbl[y] |= bitx;
  
        OS_EXIT_CRITICAL();
        OSSched();
        OS_ENTER_CRITICAL();
        if (OSTCBCur->OSTCBStat & OS_STAT_SEM) {
			psem->OSSemTbl[y] &= ~bitx;
			
            OSTCBCur->OSTCBStat = OS_STAT_RDY;
            OS_EXIT_CRITICAL();
            return (OS_TIMEOUT);
        } else {
            OS_EXIT_CRITICAL();
            return (OS_NO_ERR);
        }
    }
}
/*
************************************************************
*                    POST TO A SEMAPHORE
************************************************************
*/
UBYTE OSSemPost(OS_SEM *psem)
{
    UBYTE x,  bitx, p;
	int y;

    OS_ENTER_CRITICAL();
    if (psem->OSSemCnt < 32767) {
        if (psem->OSSemCnt++ >= 0) {
            OS_EXIT_CRITICAL();
        } else {
        	for(y = 0; y < MAX_TASK_SIZE; y++)
				if(psem->OSSemTbl[y] != 0)
					break;
            if (y != MAX_TASK_SIZE) {                          /* Rev. A, This line was missing   */

				for(y = 0; y < MAX_TASK_SIZE; y++)
					if( psem->OSSemTbl[y] )
						break;
                x    = OSUnMapTbl[psem->OSSemTbl[y]];
                bitx = OSMapTbl[x];
				
				psem->OSSemTbl[y] &= ~bitx;
                p                           = (y << 3) + x;
                OSTCBPrioTbl[p]->OSTCBDly   =  0;
                OSTCBPrioTbl[p]->OSTCBStat &= ~OS_STAT_SEM;
                OSRdyTbl[y]                |=  bitx;
                OS_EXIT_CRITICAL();
                OSSched();
            } 
			else {
                OS_EXIT_CRITICAL();
            }
        }
        return (OS_NO_ERR);
    } else {
        OS_EXIT_CRITICAL();
        return (OS_SEM_OVF);
    }
}
/*
************************************************************
*                   INITIALIZE MAILBOX
************************************************************
*/
UBYTE OSMboxInit(OS_MBOX *pmbox, void *msg)
{
	int i;
    OS_ENTER_CRITICAL();
    pmbox->OSMboxMsg    = msg;
    for(i = 0; i < MAX_TASK_SIZE; i++)
		pmbox->OSMboxTbl[i] = 0;
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
/*
************************************************************
*                     PEND ON MAILBOX
************************************************************
*/
void *OSMboxPend(OS_MBOX *pmbox, UWORD timeout, UBYTE *err)
{
    UBYTE  x, bitx;
    void  *msg;
	int y;

    OS_ENTER_CRITICAL();
    if ((msg = pmbox->OSMboxMsg) != (void *)0) {
        pmbox->OSMboxMsg = (void *)0;
        OS_EXIT_CRITICAL();
        *err = OS_NO_ERR;
    } else {
        OSTCBCur->OSTCBStat |= OS_STAT_MBOX;
        OSTCBCur->OSTCBDly   = timeout;
        y                    = OSTCBCur->OSTCBPrio >> 3;
        x                    = OSTCBCur->OSTCBPrio & 0x07;
        bitx                 = OSMapTbl[x];
		OSRdyTbl[y] &= ~bitx;
	
        pmbox->OSMboxTbl[y] |= bitx;
        OS_EXIT_CRITICAL();
        OSSched();
        OS_ENTER_CRITICAL();
        if (OSTCBCur->OSTCBStat & OS_STAT_MBOX) {
			pmbox->OSMboxTbl[y] &= ~bitx;
            OSTCBCur->OSTCBStat = OS_STAT_RDY;
            msg                 = (void *)0;
            OS_EXIT_CRITICAL();
            *err 		= OS_TIMEOUT;
        } else {
            msg                 = pmbox->OSMboxMsg;
            pmbox->OSMboxMsg    = (void *)0;
            OS_EXIT_CRITICAL();
            *err                = OS_NO_ERR;
        }
    }
    return (msg);
}
/*
************************************************************
*                    POST TO A MAILBOX
************************************************************
*/

UBYTE OSMboxPost(OS_MBOX *pmbox, void *msg)
{
    UBYTE x,  bitx,  p;
	int y;

    OS_ENTER_CRITICAL();
    if (pmbox->OSMboxMsg != (void *)0) {
        OS_EXIT_CRITICAL();
        return (OS_MBOX_FULL);
    } else {
        pmbox->OSMboxMsg = msg;
		
		for(y = 0; y < MAX_TASK_SIZE; y++)
			if(pmbox->OSMboxTbl[y] != 0)
				break;
		
	if (y != MAX_TASK_SIZE) {                            /* Rev. A, This line was missing   */
            x                = OSUnMapTbl[pmbox->OSMboxTbl[y]];
            bitx             = OSMapTbl[x];
			pmbox->OSMboxTbl[y] &= ~bitx;
			
            p                           = (y << 3) + x;
            OSTCBPrioTbl[p]->OSTCBStat &= ~OS_STAT_MBOX;
            OSTCBPrioTbl[p]->OSTCBDly   =  0;
            OSRdyTbl[y]                |= bitx;
            OS_EXIT_CRITICAL();
            OSSched();
        } else {
            OS_EXIT_CRITICAL();
        }
        return (OS_NO_ERR);
    }
}
/*
************************************************************
*                    INITIALIZE QUEUE
************************************************************
*/
UBYTE OSQInit(OS_Q *pq, void **start, UBYTE size)
{
	int i;
    OS_ENTER_CRITICAL();
    pq->OSQStart   = start;
    pq->OSQEnd     = &start[size];
    pq->OSQIn      = start;
    pq->OSQOut     = start;
    pq->OSQSize    = size;
    pq->OSQEntries = 0;
    for(i = 0; i < MAX_TASK_SIZE; i++)
		pq->OSQTbl[i] = 0;
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
/*
************************************************************
*                     PEND ON A QUEUE
************************************************************
*/
void *OSQPend(OS_Q *pq, UWORD timeout, UBYTE *err)
{
    UBYTE  x, bitx;
    void  *msg;
	int y;

    OS_ENTER_CRITICAL();
    if (pq->OSQEntries != 0) {
        msg = *pq->OSQOut++;
        pq->OSQEntries--;
        if (pq->OSQOut == pq->OSQEnd) {
            pq->OSQOut = pq->OSQStart;
        }
        OS_EXIT_CRITICAL();
        *err = OS_NO_ERR;
    } else {
        OSTCBCur->OSTCBStat |= OS_STAT_Q;
        OSTCBCur->OSTCBDly   = timeout;
        y                    = OSTCBCur->OSTCBPrio >> 3;
        x                    = OSTCBCur->OSTCBPrio & 0x07;
        bitx                 = OSMapTbl[x];
	   OSRdyTbl[y] &= ~bitx;
        pq->OSQTbl[y] |= bitx;
        OS_EXIT_CRITICAL();
        OSSched();
        OS_ENTER_CRITICAL();
        if (OSTCBCur->OSTCBStat & OS_STAT_Q) {
			pq->OSQTbl[y] &= ~bitx;
            OSTCBCur->OSTCBStat = OS_STAT_RDY;
            msg                 = (void *)0;
            OS_EXIT_CRITICAL();
            *err 		= OS_TIMEOUT;
        } else {
            msg = *pq->OSQOut++;
            pq->OSQEntries--;
            if (pq->OSQOut == pq->OSQEnd) {
                pq->OSQOut = pq->OSQStart;
            }
            OS_EXIT_CRITICAL();
            *err = OS_NO_ERR;
        }
    }
    return (msg);
}
/*
************************************************************
*                     POST TO A QUEUE
************************************************************
*/

UBYTE OSQPost(OS_Q *pq, void *msg)
{
    UBYTE x, bitx, p;
	int y;

    OS_ENTER_CRITICAL();
    if (pq->OSQEntries >= pq->OSQSize) {
        OS_EXIT_CRITICAL();
        return (OS_Q_FULL);
    } else {
        *pq->OSQIn++ = msg;
        pq->OSQEntries++;
        if (pq->OSQIn == pq->OSQEnd) {
            pq->OSQIn = pq->OSQStart;
        }

		for(y = 0; y < MAX_TASK_SIZE; y++)
			if(pq->OSQTbl[y] != 0)
				break;
		
        if (y != MAX_TASK_SIZE) {                                  /* Rev. A, This line was missing   */
            x    = OSUnMapTbl[pq->OSQTbl[y]];
            bitx = OSMapTbl[x];
			pq->OSQTbl[y] &= ~bitx;
            p                           = (y << 3) + x;
            OSTCBPrioTbl[p]->OSTCBStat &= ~OS_STAT_Q;
            OSTCBPrioTbl[p]->OSTCBDly   = 0;
            OSRdyTbl[y]                |= bitx;
            OS_EXIT_CRITICAL();
            OSSched();
        } else {
            OS_EXIT_CRITICAL();
        }
        return (OS_NO_ERR);
    }
}

