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
* File : OS_TRACE.C
* By   : Vladimir Antonenko
*
*********************************************************************************************************
*/

#include "includes.h"
#include "os_trace.h"

/*
*********************************************************************************************************
*                                              GLOBALS
*********************************************************************************************************
*/

typedef struct {
    INT32U        Time;
    INT32U        TimeW32;
    INT8U         Object;
    INT8U         Point;
    INT8U         RdyGrp;
    OS_TCB        *TcbCur;
    INT8U         PrioCur;
    OS_TCB        *Tcb;
    OS_EVENT      *Event;
    INT8U         Arg0;
    INT8U         Arg1;
    INT8U         Arg2;
} OS_TRACE;

int        trEnable   = -1;
OS_TRACE   *trData    = NULL;
int        trSlots    = 0;
int        trCount    = 0;
int        trLast     = 0;

/*
*********************************************************************************************************
*                                              OSInitTrace()
* Arguments: Size: Defines the size of the trace buffer.
*********************************************************************************************************
*/

void OSInitTrace(int Size)
{
    OS_ENTER_CRITICAL();
    trData  = (OS_TRACE*) malloc( Size * sizeof(OS_TRACE) );
    trSlots = Size;
    trCount = 0;
    trLast  = 0;
    OS_EXIT_CRITICAL();
}

/*
*********************************************************************************************************
*                                                    OSTrace()
*********************************************************************************************************
*/

void OSTrace(int object, int point, void *tcb, void *event, int arg0, int arg1, int arg2)
{
    if( trEnable & (1 << object) )
    {
        OS_ENTER_CRITICAL();

        trData[trLast].Time    =  OSTime;
        trData[trLast].TimeW32 =  GetTickCount();
        trData[trLast].Object  =  object;
        trData[trLast].Point   =  point;
        trData[trLast].RdyGrp  =  OSRdyGrp;
        trData[trLast].TcbCur  =  OSTCBCur;
        trData[trLast].PrioCur =  OSPrioCur;
        trData[trLast].Tcb     =  tcb;
        trData[trLast].Event   =  event;
        trData[trLast].Arg0    =  arg0;
        trData[trLast].Arg1    =  arg1;
        trData[trLast].Arg2    =  arg2;
        
        if( ++trLast == trSlots )
            trLast = 0;

        ++trCount;
        OS_EXIT_CRITICAL();
    }
}
