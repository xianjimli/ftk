/*
 * File: ftk_select_rtthread.c
 * Author:  Jiao JinXing <jiaojinxing1987@gmail.com>
 * Brief:   RT-Thread select implemention.
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

#include "ftk_rtthread.h"
#include "ftk_select_rtthread.h"
#include "ftk_pipe.h"

static struct rt_event 	ftk_event;
static rt_uint32_t 		ftk_select_fd_pool;

void ftk_rtthread_select_init(void)
{
	rt_event_init(&ftk_event, "ftk_evt", RT_IPC_FLAG_FIFO);

	ftk_select_fd_pool = 0;
}

void ftk_rtthread_select_deinit(void)
{
	rt_event_detach(&ftk_event);
}

int ftk_rtthread_select(int mfd, fd_set *read_fdset, fd_set *write_fdset, fd_set *err_fdset, struct timeval *tv)
{
	rt_uint32_t tick = 0;
	rt_uint32_t flags = 0;
	int i = 0, n = 0;

	if (tv != NULL)
	{
		tick = (rt_uint32_t)(tv->tv_sec + tv->tv_usec/1000000)*RT_TICK_PER_SECOND;
	}
	else
	{
	    tick = 3*RT_TICK_PER_SECOND;
	}

	flags = read_fdset->fds_bits[0];
	FD_ZERO(read_fdset);

	if (write_fdset) FD_ZERO(write_fdset);

	if (err_fdset) FD_ZERO(err_fdset);

	rt_event_recv(&ftk_event, flags, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, (rt_uint32_t)tick, &read_fdset->fds_bits[0]);

	for (i=0; i<32; i++)
	{
		if (read_fdset->fds_bits[0] & (0x01ul<<i))
		{
			n++;
		}
	}

	return n;
}

int ftk_rtthread_set_file_readble(int fd)
{
	rt_event_send(&ftk_event, (0x01ul<<fd));
}

int ftk_rtthread_select_fd_alloc(void)
{
	int i;

	for (i=0; i<32; i++)
	{
		if (!(ftk_select_fd_pool & (0x01ul<<i)))
		{
			ftk_select_fd_pool |= (0x01ul<<i);
			return i;	
		}
	}
	return -1;
}

void ftk_rtthread_select_fd_free(int fd)
{
	ftk_select_fd_pool &= ~(0x01ul<<fd);
}

