/*
 * File: ftk_psp.c
 * Author:  Tao Yu <yut616@gmail.com>
 * Brief:   psp specific functions.
 *
 * Copyright (c) 2009 - 2010  Li XianJing <xianjimli@hotmail.com>
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * History:
 * ================================================================
 * 2010-03-11 Tao Yu <yut616@gmail.com> created.
 *
 */
#include <signal.h>
#include "ftk_typedef.h"
#include "ftk_psp.h"
#include "ftk_log.h"

static char g_work_dir[FTK_MAX_PATH+1] = {0};
static char g_data_dir[FTK_MAX_PATH+1] = {0};
static char g_testdata_dir[FTK_MAX_PATH+1] = {0};

/* Define the module info section */
PSP_MODULE_INFO("FTK", 0, 1, 1);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

/* Exit callback */
static int exit_callback(int arg1, int arg2, void *common) 
{
	sceKernelExitGame();
	return 0;
}

/* Callback thread */
static int CallbackThread(SceSize args, void *argp) 
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
	return 0;
}

/* Sets up the callback thread and returns its thread id */
static int SetupCallbacks(void) 
{
	int thid = 0;
	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0) 
		sceKernelStartThread(thid, 0, 0);
	return thid;
}

char* ftk_get_root_dir(void)
{
	return g_work_dir;
}

char* ftk_get_data_dir(void)
{
	return g_data_dir;
}

char* ftk_get_testdata_dir(void)
{
	return g_testdata_dir;
}

static Ret psp_init(int argc, char** argv)
{
	char *p = NULL;
	char *cmd = argv[0];
	pspDebugScreenInit();
	SetupCallbacks();

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

	if(cmd) 
	{
		ftk_logd("%s: cmd: %s\n", __func__, cmd);
		p = strrchr(cmd, '/');
		if(p != NULL)
		{
			*p = '\0';
			ftk_snprintf(g_work_dir, FTK_MAX_PATH, "%s", cmd);
			ftk_snprintf(g_data_dir, FTK_MAX_PATH, "%s/data", g_work_dir);
			ftk_snprintf(g_testdata_dir, FTK_MAX_PATH, "%s/testdata", g_work_dir);
		}
	}
	ftk_logd("%s: work dir: %s\n", __func__, g_work_dir);
	return RET_OK;
}

size_t ftk_get_relative_time(void)
{
	struct timeval now = {0};
	sceKernelLibcGettimeofday(&now, NULL);

	return now.tv_sec*1000 + now.tv_usec/1000;
}

int ftk_platform_init(int argc, char** argv)
{
	return psp_init(argc, argv);
}

void ftk_platform_deinit(void)
{
	return;
}

char *ftk_getcwd(char *buf, size_t size)
{
	return_val_if_fail(buf != NULL, NULL);
	snprintf(buf, size, "%s", g_work_dir);
	return buf;
}
