/*
 * File: ftk_ucos_sim.c    
 * Author:  MinPengli <MinPengli@gmail.com>
 * Brief: uCOS-II specific functions.
 *
 * Copyright (c) 2009 - 2010  MinPengli <MinPengli@gmail.com>
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
 *  History:
 * ================================================================
 * 2010-03-19 MinPengli <MinPengli@gmail.com> created
 *
 */

#include <string.h>
#include <assert.h>
#include "ftk_log.h"
#include "includes.h"

static char g_work_dir[MAX_PATH+1] = {0};
static char g_data_dir[MAX_PATH+1] = {0};
static char g_testdata_dir[MAX_PATH+1] = {0};

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

int ftk_platform_init(int argc, char** argv)
{
	int Ret = 0;
	char* p = NULL;
	
	if(_getcwd(g_work_dir, MAX_PATH) != NULL)
	{
		p = strstr(g_work_dir, "\\src");
		if(p != NULL)
		{
			*p = '\0';
			ftk_snprintf(g_data_dir, MAX_PATH, "%s\\data", g_work_dir);
			ftk_snprintf(g_testdata_dir, MAX_PATH, "%s\\testdata", g_work_dir);
		}
	}

	ucos_pipe_init();

	return 0;
}

void ftk_platform_deinit(void)
{
	ucos_pipe_deinit();

	return;
}

char* ftk_strncpy(char *dest, const char *src, size_t n)
{
	return strncpy(dest, src, n);
}

int   ftk_snprintf(char *str, size_t size, const char *format, ...)
{
	int ret = 0;
	va_list args;

	OS_ENTER_CRITICAL();
	va_start(args, format);
	ret = _vsnprintf(str, size-1, format, args);
	str[size-1] = '\0';
	OS_EXIT_CRITICAL();

	return ret;
}

int   ftk_vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
	int ret = 0;

	OS_ENTER_CRITICAL();
	ret = _vsnprintf(str, size-1, format, ap);
	OS_EXIT_CRITICAL();

	return ret; 
}

size_t ftk_get_relative_time(void)
{
	FILETIME        ft;
	LARGE_INTEGER   li;
	__int64         t;

	GetSystemTimeAsFileTime(&ft);
	li.LowPart  = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	t  = li.QuadPart;       /* In 100-nanosecond intervals */
	t /= 10;                /* In microseconds */

	return t/1000;
}
