/*
************************************************************
*                      UCOS186C.H
*               80186/80188 Specific Code
*                  SMALL MEMORY MODEL
************************************************************
*                      CONSTANTS
************************************************************
*/

#ifndef _UCOS186C_H
#define _UCOS186C_H

#define  UCOS  0xF1     /* Context switch vector number */
/*
************************************************************
*                        MACROS
************************************************************
*/
#define  OS_ENTER_CRITICAL()  asm  CLI
#define  OS_EXIT_CRITICAL()   asm  STI
#define  OS_TASK_SW()         asm  INT   UCOS
/*
************************************************************
*                       DATA TYPES
************************************************************
*/
typedef unsigned char  BOOLEAN;
typedef unsigned char  UBYTE;
typedef signed   char  BYTE;
typedef unsigned int   UWORD;
typedef signed   int   WORD;

#endif