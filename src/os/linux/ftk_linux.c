/*
 * File: ftk_linux.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   linux specific functions.
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
 * 2009-11-17 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#include <signal.h>
#include "ftk_typedef.h"

#ifdef HAVE_EXECINFO_H
#include <execinfo.h>
#endif
static void signal_handler(int sig)
{
#ifdef HAVE_EXECINFO_H
	int i = 0;
	void* buffer[100] = {0};
	int n = backtrace(buffer, 100);
	const char** symbols = (const char**)backtrace_symbols(buffer, n);

	for(i = 0; i < n; i++)
	{
		printf("%p: %s\n", buffer[i], symbols[i]);
	}

	fflush(stdout);

	signal(SIGABRT, NULL);
	signal(SIGSEGV, NULL);
	signal(SIGILL,  NULL);
#endif
	_exit(0);

	return;
}

static void signal_int_handler(int sig)
{
	exit(0);

	return;
}

static void ftk_install_crash_signal(void)
{
	signal(SIGABRT, signal_handler);
	signal(SIGSEGV, signal_handler);
	signal(SIGILL,  signal_handler);
	signal(SIGINT,  signal_int_handler);

	return ;
}

static int ftk_set_tty_mode(int graphics)
{
    int r = 0;
#if !defined(PC_EMU) && defined(USE_LINUX_NATIVE)
	int fd = 0;
    fd = open("/dev/tty0", O_RDWR | O_SYNC);
    if (fd < 0)
        return -1;
    r = ioctl(fd, KDSETMODE, (void*) (graphics ? KD_GRAPHICS : KD_TEXT));
    close(fd);
#endif
    return r;
}

size_t ftk_get_relative_time(void)
{
	struct timeval now = {0};
	gettimeofday(&now, NULL);

	return now.tv_sec*1000 + now.tv_usec/1000;
}

int ftk_platform_init(int argc, char** argv)
{
	ftk_set_tty_mode(1);
	ftk_install_crash_signal();

	return RET_OK;
}

void ftk_platform_deinit(void)
{
	ftk_set_tty_mode(0);

	return;
}
