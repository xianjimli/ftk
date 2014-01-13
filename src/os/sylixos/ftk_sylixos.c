/*
 * File: ftk_sylixos.c
 * Author:  Han.hui <sylixos@hotmail.com>
 * Brief:   sylixos specific functions.
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
 * 2009-11-17 Han.hui <sylixos@hotmail.com> created
 *
 */

#include <signal.h>
#include "ftk.h"

static void signal_handler(int sig)
{
    ftk_deinit();

    _exit(-1);

    return;
}

static void signal_int_handler(int sig)
{
    ftk_deinit();

    exit(0);

    return;
}

static void ftk_install_crash_signal(void)
{
    signal(SIGABRT, signal_handler);
    signal(SIGSEGV, signal_handler);
    signal(SIGILL,  signal_handler);
    signal(SIGINT,  signal_int_handler);

    return;
}

size_t ftk_get_relative_time(void)
{
    struct timeval now = {0};

    gettimeofday(&now, NULL);

    return now.tv_sec*1000 + now.tv_usec/1000;
}

int ftk_platform_init(int argc, char** argv)
{
    int gsnap(int argc, char* argv[]);

    API_TShellKeywordAdd("gsnap", gsnap);

    ftk_install_crash_signal();

    return RET_OK;
}

void ftk_platform_deinit(void)
{
    return;
}

void ftk_assert(int cond, const char* func, const char* file, int line)
{
    if (!cond)
    {
        fprintf(stderr, "function : %s() in file : %s line : %d serious error!\n",
                func, file, line);

        ftk_deinit();

        exit(-1);
    }
}
