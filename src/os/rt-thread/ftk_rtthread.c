/*
 * File: ftk_rtthread.c
 * Author:  Jiao JinXing <jiaojinxing1987@gmail.com>
 * Brief:   RT-Thread specific functions.
 *
 * Copyright (c) 2009 - 2010  Jiao JinXing <jiaojinxing1987@gmail.com>
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
 * 2010-10-2 Jiao JinXing <jiaojinxing1987@gmail.com> created
 *
 */

#include "ftk_typedef.h"
#include <rtthread.h>

size_t ftk_get_relative_time(void)
{
	rt_tick_t tick = rt_tick_get();

	return tick/RT_TICK_PER_SECOND*1000;
}

static int ftk_deinited = 0;

static void ftk_cleanup(void)
{
    extern void ftk_deinit(void);

    if (ftk_deinited == 0)
    {
        ftk_deinited = 1;

        ftk_deinit();
    }
}

int  ftk_platform_init(int argc, char** argv)
{
	void ftk_rtthread_select_init(void);

	ftk_rtthread_select_init();

    return RET_OK;
}

void ftk_platform_deinit(void)
{
	void ftk_rtthread_select_deinit(void);

    ftk_deinited = 1;

	ftk_rtthread_select_deinit();
}

