/*
************************************************************
*                         TEST1.C
*                      uCOS EXAMPLE
************************************************************
*/
#include "INCLUDES.H"

#define          OS_MAX_TASKS    20
#define          STK_SIZE       500
#define 		 new_data(type,name,num, data)		type name##num = (data)
#define			 get_data_num(name, num)	name##num

OS_TCB           OSTCBTbl[OS_MAX_TASKS];
UWORD            OSIdleTaskStk[STK_SIZE];
UWORD            Stk1[STK_SIZE];
UWORD            Stk2[STK_SIZE];
UWORD            Stk3[STK_SIZE];
UWORD            Stk4[STK_SIZE];
UWORD            Stk5[STK_SIZE];

char             Data1 = '1';
char             Data2 = '2';
char             Data3 = '3';
char             Data4 = '4';
char             Data5 = '5';



void far         Task(void *data);
void interrupt (*OldTickISR)(void);
extern rb_tree * tree;



void main(void)
{
    UBYTE err;
	int i;
    clrscr();
    OldTickISR = getvect(0x08);
    setvect(UCOS, (void interrupt (*)(void))OSCtxSw);
    setvect(0xF2, OldTickISR);
    OSInit(&OSIdleTaskStk[STK_SIZE], OS_MAX_TASKS);
	
    OSTaskCreate(Task, (void *)&Data1, (void *)&Stk1[STK_SIZE], 1);
    OSTaskCreate(Task, (void *)&Data2, (void *)&Stk2[STK_SIZE], 2);
    OSTaskCreate(Task, (void *)&Data3, (void *)&Stk3[STK_SIZE], 3);
    OSTaskCreate(Task, (void *)&Data4, (void *)&Stk4[STK_SIZE], 4);
	OSTaskCreate(Task, (void *)&Data5, (void *)&Stk5[STK_SIZE], 5);	


	OSStart();
}

void far Task(void *data)
{
    setvect(0x08, (void interrupt (*)(void))OSTickISR);
    while (1) {
	OSTimeDly(18);
        gotoxy(rand() % 79 + 1, rand() % 25 + 1);
	putch(*(char *)data);
        if (kbhit()) {
            setvect(0x08, OldTickISR);
            exit(0);
        }
    }
}
