/*
*********************************************************************************************************
*                                               uC/OS-II
*                                        The Real-Time Kernel
*
*                                        Win32 Specific code
*
* This is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
*
* File : OS_CPU.H
* By   : Vladimir Antonenko
*
* Rules of use of the given emulation uCOS in Win32:
*
*   1. All Win32-threads which interact with uCOS-tasks should initialize
*      critical section by call OS_INIT_CRITICAL() (if defined USE_CRITICAL_SECTION
*      in os_cpu_c.c). They can be considered as handlers of interrupts.
*   2. Function printf() and it similar should be made in critical section as
*      the console is shared resource and can cause deadlock of tasks ( see osprintf()
*      in the end of file ).
*   3. It is necessary very closely use shared resources of Windows - they can cause
*      blocking of tasks.
*
* The given emulation starts each uCOS-task in separate thread. At any moment of time
* only 0 or one from these threads can be active. (remaining in SUSPEND state, not considering
* two service threads (have priorities 10 and 15). Each thread in SUSPEND state
* must have SUSPEND counter = 1).
*
*********************************************************************************************************
*/

#ifndef _OS_CPU_H
#define _OS_CPU_H

#include <stdio.h>
#include <windows.h>

/*
*********************************************************************************************************
*                                              Defines
*********************************************************************************************************
*/
#ifndef _WIN32_WINNT
#define _WIN32_WINNT          0x0400 
#endif

#define  OS_CPU_EXT           extern
#define  OS_TASK_SW()         SetEvent(OSCtxSwW32Event)
#define  OS_STK_GROWTH        1                             /* Stack grows from HIGH to LOW memory on 80x86  */

/*
*********************************************************************************************************
*                                              GLOBALS
*********************************************************************************************************
*/

OS_CPU_EXT  CRITICAL_SECTION  OSCriticalSection;
OS_CPU_EXT  HANDLE            OSCtxSwW32Event;

/*
*********************************************************************************************************
*                                              Prototypes
*********************************************************************************************************
*/

void     OS_SLEEP();                             /* Recommend to insert call of this function in Idle task                     */
void     OS_STOP();
int      OS_Printf( char* str, ... );            /* analog of printf, but use critical sections                                */

void     OS_INIT_CRITICAL();
void     OS_ENTER_CRITICAL();
void     OS_EXIT_CRITICAL();

void     OSStartHighRdy();
void     OSIntCtxSw();

/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned long  INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   long  INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef unsigned short OS_STK;                   /* Each stack entry is 16-bit wide                    */

#define BYTE           INT8S                     /* Define data types for backward compatibility ...   */
#define UBYTE          INT8U                     /* ... to uC/OS V1.xx.  Not actually needed for ...   */
#define WORD           INT16S                    /* ... uC/OS-II.                                      */
#define UWORD          INT16U
#define LONG           INT32S
#define ULONG          INT32U

typedef struct {
    void    *pData;
    INT16U  Opt;
    void    (*Task)(void*);
    HANDLE  Handle;
    INT32U  Id;
    INT32   Exit;
} OS_EMU_STK;

#endif                                           /*_OS_CPU_H                                                                   */
