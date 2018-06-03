/*
***********************************************************
*                         UCOS.H
*                   SYSTEM DECLARATIONS
***********************************************************
*/
#ifndef _UCOS_H
#define _UCOS_H

#include "UCOS186C.h"
#include "RBTREE.H"
#define  MAX_TASK_SIZE		64
#define OS_LO_PRIO          MAX_TASK_SIZE - 1 /*IDLE task priority      */

                               /*TASK STATUS             */
#define OS_STAT_RDY       0x00 /*Ready to run            */

#define OS_STAT_MBOX      0x01 /*Pending on mailbox      */
#define OS_STAT_SEM       0x02 /*Pending on semaphore    */
#define OS_STAT_Q         0x04 /*Pending on queue        */
#define OS_STAT_RES1      0x08 /*Reserved                */
#define OS_STAT_RES2      0x10 /*Reserved                */
#define OS_STAT_RES3      0x20 /*Reserved                */
#define OS_STAT_RES4      0x40 /*Reserved                */

/*修改部分，添加了新状态 OS_STAT_WAIT*/
#define	 OS_STAT_WAIT	  0x80

#define  OS_NO_ERR           0
#define  OS_TIMEOUT         10
#define  OS_MBOX_FULL       20
#define  OS_Q_FULL	    	30
#define  OS_PRIO_EXIST      40
#define  OS_SEM_ERR         50
#define  OS_SEM_OVF         51
		


/*
***********************************************************
*         uCOS TASK CONTROL BLOCK DATA STRUCTURE
***********************************************************
*/
typedef struct os_tcb {
    void          *OSTCBStkPtr;
    UBYTE          OSTCBStat;
    UBYTE          OSTCBPrio;
    UWORD          OSTCBDly;
	UBYTE color;
    struct os_tcb *OSTCBNext;
    struct os_tcb *OSTCBPrev;
    struct os_tcb *OSTCBParent;
} OS_TCB;

/*
***********************************************************
*                SEMAPHORE DATA STRUCTURE
***********************************************************
*/
typedef struct os_sem {
    WORD   OSSemCnt;
    //UBYTE  OSSemGrp;
    UBYTE  OSSemTbl[MAX_TASK_SIZE];
} OS_SEM;

/*
***********************************************************
*                    MAILBOX DATA STRUCTURE
***********************************************************
*/
typedef struct os_mbox {
    void  *OSMboxMsg;
    //UBYTE  OSMboxGrp;
    UBYTE  OSMboxTbl[MAX_TASK_SIZE];
} OS_MBOX;

/*
***********************************************************
*                      QUEUE DATA STRUCTURE
***********************************************************
*/
typedef struct os_q {
    void **OSQStart;
    void **OSQEnd;
    void **OSQIn;
    void **OSQOut;
    UBYTE  OSQSize;
    UBYTE  OSQEntries;
   // UBYTE  OSQGrp;
    UBYTE  OSQTbl[MAX_TASK_SIZE];
} OS_Q;



/*
***********************************************************
*                 uCOS GLOBAL VARIABLES
***********************************************************
*/
extern BOOLEAN  OSRunning;
extern OS_TCB  *OSTCBCur;
extern OS_TCB  *OSTCBHighRdy;
extern UBYTE    OSRdyGrp;
extern UBYTE    OSRdyTbl[];
extern OS_TCB  *OSTCBPrioTbl[];
extern OS_TCB   OSTCBTbl[];
extern OS_TCB  *OSTCBWaitList;
extern UBYTE    OSLockNesting;
extern UBYTE    OSIntNesting;
extern OS_TCB  *OSTCBFreeList;

/*
***********************************************************
*                 uCOS FUNCTION PROTOTYPES
***********************************************************
*/
void     OSInit(void *idle_stk, UBYTE maxtasks);
void     OSStart(void);
UBYTE    OSTaskCreate(void (far *task)(void *dptr), void  *data, void  *pstk, UBYTE  prio);
void     OSTimeDly(UWORD ticks);
void     OSTimeTick(void);
void     OSIntEnter(void);
void     OSIntExit(void);
void far OSCtxSw(void);
void     OSIntCtxSw(void);
void far OSTickISR(void);
void     OSStartHighRdy(void);
void     OSSched(void);
OS_TCB  *OSTCBGetFree(void);
UBYTE    OSChangePrio(UBYTE newp);
void     OSTaskDelete(void);
void     OSLock(void);
void     OSUnlock(void);
UBYTE    OSSemInit(OS_SEM *psem, WORD value);
UBYTE    OSSemPost(OS_SEM *psem);
UBYTE    OSSemPend(OS_SEM *psem, UWORD timeout);
UBYTE    OSMboxInit(OS_MBOX *pmbox, void *msg);
UBYTE    OSMboxPost(OS_MBOX *pmbox, void *msg);
void    *OSMboxPend(OS_MBOX *pmbox, UWORD timeout, UBYTE *err);
UBYTE    OSQInit(OS_Q *pq, void **start, UBYTE size);
UBYTE    OSQPost(OS_Q *pq, void *msg);
void    *OSQPend(OS_Q *pq, UWORD timeout, UBYTE *err);
void     OSTCBPutFree(OS_TCB *ptcb);

#endif