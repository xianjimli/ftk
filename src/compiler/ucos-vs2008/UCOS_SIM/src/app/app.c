/*
*********************************************************************************************************
*                                               uC/OS-II
*                                         The Real-Time Kernel
*
*                             (c) Copyright 1998-2004, Micrium, Weston, FL
*                                          All Rights Reserved
*
*
*                                            WIN32 Sample Code
*
* File : APP.C
* By   : Eric Shufro
*********************************************************************************************************
*/

#include <includes.h>

/*
*********************************************************************************************************
*                                                CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE    1024
#define  APP_TASK_PRIO    5

/*
*********************************************************************************************************
*                                                VARIABLES
*********************************************************************************************************
*/

static OS_STK AppTaskStk[TASK_STK_SIZE];

/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void  AppTask(void *p_arg);

#if OS_VIEW_MODULE > 0
static  void  AppTerminalRx(INT8U rx_data);
#endif

#define _log(msg)	do{   \
	OS_ENTER_CRITICAL();	\
	printf("%s",msg);		\
	OS_EXIT_CRITICAL();	\
}while(0)

/*
*********************************************************************************************************
*                                                _tmain()
*
* Description : This is the standard entry point for C++ WIN32 code.  
* Arguments   : none
*********************************************************************************************************
*/

void C_AppMain(void)
{
	INT8U  err;

#if 0
    BSP_IntDisAll();                       /* For an embedded target, disable all interrupts until we are ready to accept them */
#endif
    OSInit();                              /* Initialize "uC/OS-II, The Real-Time Kernel"                                      */

	OSTaskCreate(AppTask,
		(void *)0,
		(OS_STK *)&AppTaskStk[TASK_STK_SIZE-1],
		APP_TASK_PRIO);

#if OS_TASK_NAME_SIZE > 11
    OSTaskNameSet(APP_TASK_START_PRIO, (INT8U *)"Start Task", &err);
#endif

#if OS_TASK_NAME_SIZE > 14
    OSTaskNameSet(OS_IDLE_PRIO, (INT8U *)"uC/OS-II Idle", &err);
#endif

#if (OS_TASK_NAME_SIZE > 14) && (OS_TASK_STAT_EN > 0)
    OSTaskNameSet(OS_STAT_PRIO, "uC/OS-II Stat", &err);
#endif

    OSStart();                             /* Start multitasking (i.e. give control to uC/OS-II)                               */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
* Arguments   : p_arg   is the argument passed to 'AppStartTask()' by 'OSTaskCreate()'.
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*               2) Interrupts are enabled once the task start because the I-bit of the CCR register was
*                  set to 0 by 'OSTaskCreate()'.
*********************************************************************************************************
*/
static int argc = 1;
static char* argv[] = {"ftk", NULL};
extern int ftk_main(int argc, char* argv[]);
extern HANDLE ucoshandle;

static void AppTask (void *p_arg)
{
    p_arg = p_arg;
#if 0
    BSP_Init();                                  /* For embedded targets, initialize BSP functions                             */
#endif

#if OS_TASK_STAT_EN > 0
    OSStatInit();                                /* Determine CPU capacity                                                     */
#endif
	OSTimeDly(10);

	ftk_main(argc, argv); /* Task never return,so ftk_deinit() will be not called*/
	_log("ftk end\n");
#ifdef UCOS_SIM
	ReleaseSemaphore(ucoshandle, 1, NULL); // increase count by one
	OSTaskDel(OS_PRIO_SELF);
#else
	while(1)
	{
		_log(".");
		OSTimeDly(200);
	}
#endif
}

