/*
 * File: ftk_pipe_rtthread.c
 * Author:  Jiao JinXing <jiaojinxing1987@gmail.com>
 * Brief:   RT-Thread pipe implemention.
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

#include "ftk_pipe.h"
#include "ftk_allocator.h"
#include "ftk_select_rtthread.h"

struct _FtkPipe
{
    int  fd;
    char pipe[2048];
    struct rt_messagequeue mq;
};

FtkPipe* ftk_pipe_create(void)
{
    FtkPipe* thiz = FTK_ZALLOC(sizeof(FtkPipe));
    if (thiz != NULL)
    {
        rt_mq_init(&thiz->mq, "ftkpipe", 
		           &thiz->pipe[0], 
		           256, 
		           sizeof(thiz->pipe), 
		           RT_IPC_FLAG_FIFO); 
        
        thiz->fd = ftk_rtthread_select_fd_alloc();
    }
    return thiz;
}

void ftk_pipe_destroy(FtkPipe* thiz)
{
    if (thiz != NULL)
    {
        ftk_rtthread_select_fd_free(thiz->fd);

		rt_mq_detach(&thiz->mq);

        FTK_ZFREE(thiz, sizeof(*thiz));
    }
}

int ftk_pipe_get_read_handle(FtkPipe* thiz)
{
    return thiz != NULL ? thiz->fd : -1;
}

int ftk_pipe_get_write_handle(FtkPipe* thiz)
{
    return thiz != NULL ? thiz->fd : -1;
}

int ftk_pipe_read(FtkPipe* thiz, void* buff, size_t length)
{							 
    return_val_if_fail(thiz != NULL && buff != NULL, -1);

    rt_mq_recv(&thiz->mq, buff, length, RT_WAITING_FOREVER);

    return length;
}

int ftk_pipe_write(FtkPipe* thiz, const void* buff, size_t length)
{
    return_val_if_fail(thiz != NULL && buff != NULL, -1);

    rt_mq_send(&thiz->mq, (void *)buff, length);
    
    ftk_rtthread_set_file_readble(thiz->fd);

    return length;
}

int ftk_pipe_check(FtkPipe* thiz)
{
	if (thiz->mq.entry > 0)
	{
		ftk_rtthread_set_file_readble(thiz->fd);
		return 0;
	}

    return -1;
}
