#ifndef _OS_TRACE_H
#define _OS_TRACE_H

/*
*********************************************************************************************************
*                                                    Constants
*********************************************************************************************************
*/

enum {
    OBJ_SW,
    OBJ_COR,
    OBJ_SEM,
    OBJ_MAX
};

enum {
    PT_SW_CTX,
    PT_SW_INT,
    PT_SW_MAX
};

enum {
    PT_COR_ERDY,
    PT_COR_EWT,
    PT_COR_SH,
    PT_COR_MAX
};

enum {
    PT_SEM_PEND,
    PT_SEM_POST,
    PT_SEM_MAX
};

/*
*********************************************************************************************************
*                                                    Prototypes
*********************************************************************************************************
*/

void OSInitTrace( int Size );

void OSTrace( int Object, int Point, void *tcb, void *event, int arg0, int arg1, int arg2 );


#endif/*_OS_TRACE_H*/
