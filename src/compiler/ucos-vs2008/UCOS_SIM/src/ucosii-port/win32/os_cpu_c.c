/*
*********************************************************************************************************
*                                               uC/OS-II
*                                        The Real-Time Kernel
*
*                                         Win32 Specific code
*
* This is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
*
* File : OS_CPU_C.C
* By   : Vladimir Antonenko
*********************************************************************************************************
*/

#define  OS_CPU_GLOBALS
#include "includes.h"
#include "os_trace.h"

#if 0
#define    USE_CRITICAL_SECTION                                                /* use Win32 critical sections                                                              */
#endif

#define    ALLOW_CS_RECURSION                                                  /* allow recursion of critical sections                                                     */

#define    SET_AFFINITY_MASK                                                   /* must be used for miltiprocessor systems                                                  */

#define    OS_CPU_TRACE                                                        /* allow print trace messages                                                               */

#define     WIN_MM_TICK                                                        /* Enabling WIN_MM_TICK will cause the uC/OS-II port to use the high resolution Multimedia  */
                                                                               /* timer instead of Sleep.  The higher resolution timer has a resolution of 1 ms, which     */
                                                                               /* results in a much more "real-time" feel - jdf                                            */

#define     WIN_MM_MIN_RES (1)                                                 /* Minimum timer resolution                                                                 */


/*
*********************************************************************************************************
*                                                    Prototypes
*********************************************************************************************************
*/

DWORD WINAPI OSCtxSwW32( LPVOID lpParameter );
DWORD WINAPI OSTickW32 ( LPVOID lpParameter );
DWORD WINAPI OSTaskW32 ( LPVOID lpParameter );

/*
*********************************************************************************************************
*                                                    Globals
*********************************************************************************************************
*/

INT32U              OSTerminateTickW32     = 0;
INT32U              OSTerminateCtxSwW32    = 0;
INT32U              OSTerminateTickCtrlW32 = 0;

HANDLE              OSTick32Handle;
HANDLE              OSCtxSwW32Event;
HANDLE              OSCtxSwW32Handle;

#ifdef WIN_MM_TICK
HANDLE              OSTickEventHandle;
DWORD               OSTickTimer;
TIMECAPS            OSTimeCap;
#endif


CRITICAL_SECTION    OSCriticalSection;
HANDLE              OSSemaphore;

OS_EMU_STK          *SS_SP;

#ifdef ALLOW_CS_RECURSION
    DWORD           ThreadID = 0;
    int             Recursion = 0;
#endif


/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                            (BEGINNING)
*
* Description: This function is called by OSInit() at the beginning of OSInit().
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts should be disabled during this call.
*********************************************************************************************************
*/

void OSInitHookBegin(void)
{
#ifdef USE_CRITICAL_SECTION
    InitializeCriticalSection(&OSCriticalSection);
#else
    OSSemaphore = CreateSemaphore( NULL, 1, 1, NULL );
#endif
}

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                               (END)
*
* Description: This function is called by OSInit() at the end of OSInit().
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts should be disabled during this call.
*********************************************************************************************************
*/

void OSInitHookEnd(void) {}

/*
*********************************************************************************************************
*                                                    OS_INIT_CRITICAL()
* Initialize code necessary for WIN32 implementation 
* of ENTER and EXIT CRITICAL routines.
*********************************************************************************************************
*/

void OS_INIT_CRITICAL()
{
#ifdef USE_CRITICAL_SECTION
    InitializeCriticalSection(&OSCriticalSection);
#endif
}

/*
*********************************************************************************************************
*                                                    OS_ENTER_CRITICAL()
* Defines the beginning of a critical section of code.
*********************************************************************************************************
*/

void OS_ENTER_CRITICAL()
{
#ifdef USE_CRITICAL_SECTION
    EnterCriticalSection(&OSCriticalSection);
#else

#ifdef ALLOW_CS_RECURSION

    if( WaitForSingleObject( OSSemaphore, 0 ) == WAIT_TIMEOUT )
    {
        if( GetCurrentThreadId() != ThreadID )
            WaitForSingleObject( OSSemaphore, INFINITE );
    }
    ThreadID = GetCurrentThreadId();
    ++Recursion;

#else
    WaitForSingleObject( OSSemaphore, INFINITE );
#endif
#endif
}

/*
*********************************************************************************************************
*                                                    OS_EXIT_CRITICAL()
* Defines the end of a critical section of code.
*********************************************************************************************************
*/

void OS_EXIT_CRITICAL()
{
#ifdef USE_CRITICAL_SECTION
    LeaveCriticalSection(&OSCriticalSection);
#else
#ifdef ALLOW_CS_RECURSION

    if( Recursion > 0 ) {
        if(--Recursion == 0 ) {
            ThreadID = 0;
            ReleaseSemaphore( OSSemaphore, 1, NULL );
        }
    }
    else {
#ifdef OS_CPU_TRACE
    OS_Printf("Error: OS_EXIT_CRITICAL\n");
#endif
    }

#else
    ReleaseSemaphore( OSSemaphore, 1, NULL );
#endif
#endif
}

/*
*********************************************************************************************************
*                                        INITIALIZE A TASK'S STACK
*
* Description: This function is called by either OSTaskCreate() or OSTaskCreateExt() to initialize the
*              stack frame of the task being created.  This function is highly processor specific.
*
* Arguments  : task          is a pointer to the task code
*
*              pdata         is a pointer to a user supplied data area that will be passed to the task
*                            when the task first executes.
*
*              ptos          is a pointer to the top of stack.  It is assumed that 'ptos' points to
*                            a 'free' entry on the task stack.  If OS_STK_GROWTH is set to 1 then 
*                            'ptos' will contain the HIGHEST valid address of the stack.  Similarly, if
*                            OS_STK_GROWTH is set to 0, the 'ptos' will contains the LOWEST valid address
*                            of the stack.
*
*              opt           specifies options that can be used to alter the behavior of OSTaskStkInit().
*                            (see uCOS_II.H for OS_TASK_OPT_???).
*
* Returns    : Always returns the location of the new top-of-stack' once the processor registers have
*              been placed on the stack in the proper order.
*
* Note(s)    : Interrupts are enabled when your task starts executing. You can change this by setting the
*              PSW to 0x0002 instead.  In this case, interrupts would be disabled upon task startup.  The
*              application code would be responsible for enabling interrupts at the beginning of the task
*              code.  You will need to modify OSTaskIdle() and OSTaskStat() so that they enable 
*              interrupts.  Failure to do this will make your system crash!
*********************************************************************************************************
*/

OS_STK *OSTaskStkInit (void (*task)(void *pd), void *pdata, OS_STK *ptos, INT16U opt)
{
    OS_EMU_STK *stk;

    stk             = (OS_EMU_STK *)((char*)ptos-sizeof(OS_EMU_STK));   /* Load stack pointer          */
    stk->pData      = pdata;
    stk->Opt        = opt;
    stk->Task       = task;
    stk->Handle     = NULL;
    stk->Id         = 0;
    stk->Exit       = 0;

    return ((void *)stk);
}

/*$PAGE*/
#if OS_CPU_HOOKS_EN
/*
*********************************************************************************************************
*                                          TASK CREATION HOOK
*
* Description: This function is called when a task is created.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void OSTCBInitHook(OS_TCB *ptcb)
{
    OS_EMU_STK  *stack;
    
    stack = (OS_EMU_STK*) ptcb->OSTCBStkPtr;

    stack->Handle = CreateThread( NULL, 0, OSTaskW32, ptcb, CREATE_SUSPENDED, &stack->Id );
    
#ifdef SET_AFFINITY_MASK
    if( SetThreadAffinityMask( stack->Handle, 1 ) == 0 ) 
	{	
#ifdef OS_CPU_TRACE
        OS_Printf("Error: SetThreadAffinityMask\n");
#endif
    }
#endif
}

void OSTaskCreateHook (OS_TCB *ptcb)
{
}

/*
*********************************************************************************************************
*                                           TASK DELETION HOOK
*
* Description: This function is called when a task is deleted.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void OSTaskDelHook (OS_TCB *ptcb)
{
    OS_EMU_STK  *stack;
    
    stack = (OS_EMU_STK*) ptcb->OSTCBStkPtr;

    if(!(stack->Exit))
    {
        SuspendThread(stack->Handle);
        CloseHandle(stack->Handle);
    }
}

/*
*********************************************************************************************************
*                                           TASK SWITCH HOOK
*
* Description: This function is called when a task switch is performed.  This allows you to perform other
*              operations during a context switch.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are disabled during this call.
*              2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                 will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the 
*                 task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

void OSTaskSwHook (void)
{
}

/*
*********************************************************************************************************
*                                           STATISTIC TASK HOOK
*
* Description: This function is called every second by uC/OS-II's statistics task.  This allows your 
*              application to add functionality to the statistics task.
*
* Arguments  : none
*********************************************************************************************************
*/

void OSTaskStatHook (void)
{
}

/*
*********************************************************************************************************
*                                               TICK HOOK
*
* Description: This function is called every tick.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

void OSTimeTickHook (void)
{
}

/*
*********************************************************************************************************
*                                               TASK IDLE HOOK
*
* Description: This function is called by the idle task.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

void OSTaskIdleHook (void)
{
}
#endif

/*
;*********************************************************************************************************
;                                          START MULTITASKING
;                                       void OSStartHighRdy(void)
;
; The stack frame is assumed to look as follows:
;
; OSTCBHighRdy->OSTCBStkPtr --> DS                               (Low memory)
;                               ES
;                               DI
;                               SI
;                               BP
;                               SP
;                               BX
;                               DX
;                               CX
;                               AX
;                               OFFSET  of task code address
;                               SEGMENT of task code address
;                               Flags to load in PSW             
;                               OFFSET  of task code address
;                               SEGMENT of task code address
;                               OFFSET  of 'pdata'
;                               SEGMENT of 'pdata'               (High memory)
;
; Note : OSStartHighRdy() MUST:
;           a) Call OSTaskSwHook() then,
;           b) Set OSRunning to TRUE,
;           c) Switch to the highest priority task.
;*********************************************************************************************************
*/

void OSStartHighRdy()
{
    DWORD  dwID;

    OSInitTrace(100000);

    OS_ENTER_CRITICAL();

    OSTaskSwHook();
    ++OSRunning;

    OSCtxSwW32Event  = CreateEvent(NULL,FALSE,FALSE,NULL);
    OSCtxSwW32Handle = CreateThread( NULL, 0, OSCtxSwW32, 0, 0, &dwID );

    SetPriorityClass(OSCtxSwW32Handle,THREAD_PRIORITY_HIGHEST);

#ifdef SET_AFFINITY_MASK
    if( SetThreadAffinityMask( OSCtxSwW32Handle, 1 ) == 0 ) {
#ifdef OS_CPU_TRACE
        OS_Printf("Error: SetThreadAffinityMask\n");
#endif
       }
#endif
    
	SetThreadPriority(OSCtxSwW32Handle,THREAD_PRIORITY_TIME_CRITICAL);

    OSTick32Handle = CreateThread( NULL, 0, OSTickW32, 0, 0, &dwID );
    SetPriorityClass(OSTick32Handle,THREAD_PRIORITY_HIGHEST);

#ifdef SET_AFFINITY_MASK
    if( SetThreadAffinityMask( OSTick32Handle, 1 ) == 0 ) 
	{
#ifdef OS_CPU_TRACE
        OS_Printf("Error: SetThreadAffinityMask\n");
#endif
    }
#endif

	SetThreadPriority(OSTick32Handle,THREAD_PRIORITY_HIGHEST);

#ifdef WIN_MM_TICK
    timeGetDevCaps(&OSTimeCap, sizeof(OSTimeCap));

    if( OSTimeCap.wPeriodMin < WIN_MM_MIN_RES )
        OSTimeCap.wPeriodMin = WIN_MM_MIN_RES;

    timeBeginPeriod(OSTimeCap.wPeriodMin);

    OSTickEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
    OSTickTimer       = timeSetEvent((1000/OS_TICKS_PER_SEC),OSTimeCap.wPeriodMin,(LPTIMECALLBACK)OSTickEventHandle, dwID,TIME_PERIODIC|TIME_CALLBACK_EVENT_SET);
#endif
    
    
    SS_SP = (OS_EMU_STK*) OSTCBHighRdy->OSTCBStkPtr;                      /* OSTCBCur = OSTCBHighRdy;     */
                                                                          /* OSPrioCur = OSPrioHighRdy;   */
    ResumeThread(SS_SP->Handle);

    OS_EXIT_CRITICAL();
    
    WaitForSingleObject(OSCtxSwW32Handle,INFINITE);

#ifdef WIN_MM_TICK
    timeKillEvent(OSTickTimer);
    timeEndPeriod(OSTimeCap.wPeriodMin);
    CloseHandle(OSTickEventHandle);
#endif

	CloseHandle(OSTick32Handle);
    CloseHandle(OSCtxSwW32Event);
}

/*
;*********************************************************************************************************
;                                PERFORM A CONTEXT SWITCH (From task level)
;                                           void OSCtxSw(void)
;
; Note(s): 1) Upon entry, 
;             OSTCBCur     points to the OS_TCB of the task to suspend
;             OSTCBHighRdy points to the OS_TCB of the task to resume
;
;          2) The stack frame of the task to suspend looks as follows:
;
;                 SP -> OFFSET  of task to suspend    (Low memory)
;                       SEGMENT of task to suspend      
;                       PSW     of task to suspend    (High memory)  
;
;          3) The stack frame of the task to resume looks as follows:
; 
;                 OSTCBHighRdy->OSTCBStkPtr --> DS                               (Low memory)
;                                               ES
;                                               DI
;                                               SI
;                                               BP
;                                               SP
;                                               BX
;                                               DX
;                                               CX
;                                               AX
;                                               OFFSET  of task code address
;                                               SEGMENT of task code address
;                                               Flags to load in PSW             (High memory)
;*********************************************************************************************************
*/

void OSCtxSw()
{
    DWORD n = 0;

    if(!(SS_SP->Exit)) {
        n = SuspendThread(SS_SP->Handle);
    }
    OSTaskSwHook();

    OSTrace( OBJ_SW, PT_SW_CTX, OSTCBHighRdy, 0, OSPrioCur, OSPrioHighRdy,0 );

    OSTCBCur = OSTCBHighRdy;
    OSPrioCur = OSPrioHighRdy;
    SS_SP = (OS_EMU_STK*) OSTCBHighRdy->OSTCBStkPtr;

    ResumeThread(SS_SP->Handle);
}

/*
;*********************************************************************************************************
;                                PERFORM A CONTEXT SWITCH (From an ISR)
;                                        void OSIntCtxSw(void)
;
; Note(s): 1) Upon entry, 
;             OSTCBCur     points to the OS_TCB of the task to suspend
;             OSTCBHighRdy points to the OS_TCB of the task to resume
;
;          2) The stack frame of the task to suspend looks as follows:
;
;                                  SP+0 --> OFFSET  of return address of OSIntCtxSw()  (Low memory)
;                                    +2     SEGMENT of return address of OSIntCtxSw()
;                                    +4     PSW saved by OS_ENTER_CRITICAL() in OSIntExit()
;                                    +6     OFFSET  of return address of OSIntExit()
;                                    +8     SEGMENT of return address of OSIntExit()
;                                    +10    DS                               
;                                           ES
;                                           DI
;                                           SI
;                                           BP
;                                           SP
;                                           BX
;                                           DX
;                                           CX
;                                           AX
;                                           OFFSET  of task code address
;                                           SEGMENT of task code address
;                                           Flags to load in PSW                       (High memory)
;
;          3) The stack frame of the task to resume looks as follows:
; 
;             OSTCBHighRdy->OSTCBStkPtr --> DS                               (Low memory)
;                                           ES
;                                           DI
;                                           SI
;                                           BP
;                                           SP
;                                           BX
;                                           DX
;                                           CX
;                                           AX
;                                           OFFSET  of task code address
;                                           SEGMENT of task code address
;                                           Flags to load in PSW             (High memory)
;*********************************************************************************************************
*/

void OSIntCtxSw()
{
    DWORD n = 0;

    if(!(SS_SP->Exit)) {
        n = SuspendThread(SS_SP->Handle);
    }

    OSTaskSwHook();

    OSTrace( OBJ_SW, PT_SW_INT, OSTCBHighRdy, 0, OSPrioCur,OSPrioHighRdy,0 );

    OSTCBCur = OSTCBHighRdy;
    OSPrioCur = OSPrioHighRdy;
    SS_SP = (OS_EMU_STK*) OSTCBHighRdy->OSTCBStkPtr;

    ResumeThread(SS_SP->Handle);
}

/*
;*********************************************************************************************************
;                                            HANDLE TICK ISR
;
; Description: This function is called 199.99 times per second or, 11 times faster than the normal DOS
;              tick rate of 18.20648 Hz.  Thus every 11th time, the normal DOS tick handler is called.
;              This is called chaining.  10 times out of 11, however, the interrupt controller on the PC
;              must be cleared to allow for the next interrupt.
;
; Arguments  : none
;
; Returns    : none
;
; Note(s)    : The following C-like pseudo-code describe the operation being performed in the code below.
;
;              Save all registers on the current task's stack;
;              OSIntNesting++;
;              OSTickDOSCtr--;
;              if (OSTickDOSCtr == 0) {
;                  INT 81H;               Chain into DOS every 54.925 mS 
;                                         (Interrupt will be cleared by DOS)
;              } else {
;                  Send EOI to PIC;       Clear tick interrupt by sending an End-Of-Interrupt to the 8259
;                                         PIC (Priority Interrupt Controller)
;              }
;              OSTimeTick();              Notify uC/OS-II that a tick has occured       
;              OSIntExit();               Notify uC/OS-II about end of ISR
;              Restore all registers that were save on the current task's stack;
;              Return from Interrupt;
;*********************************************************************************************************
*/

void OSTickISR()
{
    OSIntEnter();
    OSTimeTick();
    OSIntExit();
}

/*
*********************************************************************************************************
*                                          WIN32 TASK - OSCtxSwW32()
*
* Description: These functions are body of OS multitasking in WIN32.
*
* Arguments  : lpParameter   is a pointer to special paraleter of the task.
*
* Note(s)    : 1) Priorities of these tasks are very important.
*********************************************************************************************************
*/

DWORD WINAPI OSCtxSwW32( LPVOID lpParameter )
{
    OS_INIT_CRITICAL();

    while(!OSTerminateCtxSwW32)
    {
        if( WAIT_OBJECT_0 == WaitForSingleObject(OSCtxSwW32Event,INFINITE) )
        {
            OS_ENTER_CRITICAL();
            OSCtxSw();
            OS_EXIT_CRITICAL();
        }
    }
    return 0;
}

/*
*********************************************************************************************************
*                                          WIN32 TASK - OSTickW32()
*
* Description: These functions are body of OS multitasking in WIN32.
*
* Arguments  : lpParameter   is a pointer to special paraleter of the task.
*
* Note(s)    : 1) Priorities of these tasks are very important.
*********************************************************************************************************
*/

DWORD WINAPI OSTickW32( LPVOID lpParameter )
{
    OS_INIT_CRITICAL();

    while(!OSTerminateTickW32)
    {
        OSTickISR();
#ifdef WIN_MM_TICK
        if( WaitForSingleObject(OSTickEventHandle, 5000) == WAIT_TIMEOUT)
        {
            #ifdef OS_CPU_TRACE
                OS_Printf("Error: MM OSTick Timeout!\n");
            #endif
        }

        ResetEvent(OSTickEventHandle);
#else
        Sleep(1000/OS_TICKS_PER_SEC);
#endif
    }

    return 0;
}

/*
*********************************************************************************************************
*                                          WIN32 TASK - OSTaskW32()
*
* Description: These functions are body of OS multitasking in WIN32.
*
* Arguments  : lpParameter   is a pointer to special paraleter of the task.
*
* Note(s)    : 1) Priorities of these tasks are very important.
*********************************************************************************************************
*/

DWORD WINAPI OSTaskW32( LPVOID lpParameter )
{
    OS_TCB *ptcb;
    OS_EMU_STK  *stack;

    ptcb = (OS_TCB*) lpParameter;
    stack = (OS_EMU_STK*) ptcb->OSTCBStkPtr;
    
#ifdef DISABLE_PRIORITY_BOOST
        if( SetThreadPriorityBoost( stack->Handle, TRUE ) == 0 ) {
#ifdef OS_CPU_TRACE
            OS_Printf("Error: SetThreadPriorityBoost\n");
#endif
        }
#endif

    OS_INIT_CRITICAL();

    stack->Task(stack->pData);

    stack->Exit = 1;
    OSTaskDel(ptcb->OSTCBPrio);

    return 0;
}

/*
*********************************************************************************************************
*                                          WIN32 TASK - OS_SLEEP()
*
* Description: These functions are body of OS multitasking in WIN32.
*
* Arguments  : lpParameter   is a pointer to special paraleter of the task.
*
* Note(s)    : 1) Priorities of these tasks are very important.
*********************************************************************************************************
*/

void OS_SLEEP()
{
    Sleep(1);
}

/*
*********************************************************************************************************
*                                          WIN32 TASK - OS_STOP()
*
* Description: These functions are body of OS multitasking in WIN32.
*
* Arguments  : lpParameter   is a pointer to special paraleter of the task.
*
* Note(s)    : 1) Priorities of these tasks are very important.
*********************************************************************************************************
*/

void OS_STOP()
{
    OS_ENTER_CRITICAL();
    ++OSTerminateTickW32;
    ++OSTerminateCtxSwW32;
    OS_TASK_SW();
    OS_EXIT_CRITICAL();

    Sleep(1000/OS_TICKS_PER_SEC);
}

/*
*********************************************************************************************************
*                                          OS_Printf()
*
* Description: These functions help make application more reliable.
*
* OS_Printf - analog of printf, but use critical sections
*********************************************************************************************************
*/

int OS_Printf(char *str, ...)
{
    int  ret;

    va_list marker;

    va_start( marker, str );

    OS_ENTER_CRITICAL();
    ret = vprintf( str, marker );
    OS_EXIT_CRITICAL();

    va_end( marker );

    return ret;
}
