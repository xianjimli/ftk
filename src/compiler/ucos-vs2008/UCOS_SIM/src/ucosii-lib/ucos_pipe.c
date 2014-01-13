
/*
* File: ucos_pipe.c
* Author:  MinPengli <minpengli@gmail.com>
* Brief: pipe implement for ucos
*
* Copyright (c) 2009 - 2010  MinPengli <minpengli@gmail.com>
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
* 2010-03-19 MinPengli <minpengli@gmail.com> created
*
*/

#include <stdio.h>
#include <ucos_mutex.h>

#define NR_PIPE				16
#define MIN_PIPE_NO			3
#define MAX_PIPE_NO			(MIN_PIPE_NO + NR_PIPE)
#define PIPE_HANDLE(fd)		((fd-MIN_PIPE_NO)>>0x01)
#define IS_VALID(fd)		(fd >= MIN_PIPE_NO && fd < MIN_PIPE_NO+(NR_PIPE<<1))
#define IS_RD_FD(fd)		((fd-MIN_PIPE_NO+1)&0x01)
#define IS_WR_FD(fd)		((fd-MIN_PIPE_NO+0)&0x01)
#define BIT_FLAG(fd)		(0x01<<PIPE_HANDLE(fd))

typedef struct _ByteQueue
{
	INT8U *p;
	mutex_t *mtx;
	INT32U nbytes;
	INT32U maxcount;
	INT32U read_pos;
	INT32U write_pos;
	BOOL rd_opened;
	BOOL wr_opened;
}ByteQueue;

typedef struct _PipeMgr
{
	BOOL inited;
	OS_FLAG_GRP *fg;
	ByteQueue bq[NR_PIPE];
}PipeMgr;

static PipeMgr g_mgr = {0};

#define PTROFBYTEQ(fd)	(&g_mgr.bq[PIPE_HANDLE(fd)])

int ucos_pipe_init(void)
{
	INT8U err;

	if(g_mgr.inited)
		return -1;

	memset(&g_mgr, 0, sizeof(PipeMgr));
	g_mgr.fg = OSFlagCreate(0x00000000, &err);
	if(g_mgr.fg == NULL)
		return -1;

	g_mgr.inited = TRUE;

	return 0;
}

int ucos_pipe_deinit(void)
{
	INT8U err;
	INT32S i;
	ByteQueue *bq;

	OSFlagDel(g_mgr.fg, OS_DEL_ALWAYS, &err);
	for(i=0; i<NR_PIPE; i++)
	{
		bq = &g_mgr.bq[i];
		if(bq->rd_opened || bq->wr_opened)
		{
			mutex_destroy(bq->mtx);
			if(bq->p) free(bq->p);
			memset(bq, 0, sizeof(ByteQueue));
			return 0;
		}
	}
	return 0;
}

int ucos_pipe_create(int fd[])
{
	INT32S i = 0;

	for(i=0; i<NR_PIPE; i++)
	{
		if((!g_mgr.bq[i].rd_opened) && (!g_mgr.bq[i].wr_opened))
			break;
	}
	if(i == NR_PIPE)
	{
		return -1;
	}

	memset(&g_mgr.bq[i], 0, sizeof(ByteQueue));
	g_mgr.bq[i].maxcount = 1024 * 4;
	g_mgr.bq[i].p = malloc(g_mgr.bq[i].maxcount);
	if(g_mgr.bq[i].p == NULL)
	{
		return -1;
	}

	g_mgr.bq[i].mtx = mutex_init();
	if(g_mgr.bq[i].mtx == NULL)
	{
		free(g_mgr.bq[i].p);

		return -1;
	}

	g_mgr.bq[i].rd_opened = TRUE;
	g_mgr.bq[i].wr_opened = TRUE;
	fd[0] = (i<<1) + MIN_PIPE_NO + 0;
	fd[1] = (i<<1) + MIN_PIPE_NO + 1;

	return 0;
}

int ucos_pipe_read(int fd, void *buf, int count)
{
	INT8U err = 0, opt = 0;
	INT32S i = 0, j = 0;
	INT32U readbytes = 0;
	INT8U *p = NULL;
	ByteQueue *bq = NULL;
	OS_FLAGS flags = 0;

	if((!IS_VALID(fd)) || (!IS_RD_FD(fd)))
		return 0;
	else if(buf == NULL || count ==0)
		return 0;

	bq = PTROFBYTEQ(fd);
	if(!bq->rd_opened)
		return 0;

	do{
		flags = OSFlagPend(g_mgr.fg, BIT_FLAG(fd), OS_FLAG_WAIT_SET_ANY, 500, &err);
	}while(err != OS_NO_ERR || !(flags & BIT_FLAG(fd)));

	mutex_lock(bq->mtx);

	p = buf;
	for(i=bq->read_pos, j=0; i<bq->write_pos && j<count; i++, j++)
	{
		p[j] = bq->p[i%bq->maxcount];
		readbytes++;
	}
	bq->read_pos += readbytes;
	bq->nbytes = bq->write_pos - bq->read_pos;
	opt = bq->nbytes > 0 ? OS_FLAG_SET : OS_FLAG_CLR;

	do{	
		flags = OSFlagPost(g_mgr.fg, BIT_FLAG(fd), opt, &err);
	}while(err != OS_NO_ERR);

	mutex_unlock(bq->mtx);

	return readbytes;
}

int ucos_pipe_write(int fd, void *buf, int count)
{
	INT8U err;
	INT32S i, j, emptys;
	INT32S writebytes = 0;
	INT8U *p = NULL;
	ByteQueue *bq = NULL;

	if((!IS_VALID(fd)) || (!IS_WR_FD(fd)))
		return 0;
	else if(buf == NULL || count ==0)
		return 0;

	bq = PTROFBYTEQ(fd);
	if(!bq->wr_opened)
		return 0;
	mutex_lock(bq->mtx);

	p = buf;
	emptys = bq->maxcount - bq->nbytes;
	for(i=bq->write_pos, j=0; j<count && j<emptys; i++, j++)
	{
		bq->p[i%bq->maxcount] = p[j];
		writebytes++;
	}
	bq->write_pos += writebytes;
	bq->nbytes = bq->write_pos - bq->read_pos;

	do{
		OSFlagPost(g_mgr.fg, BIT_FLAG(fd), OS_FLAG_SET, &err);
	}while(err != OS_NO_ERR);

	mutex_unlock(bq->mtx);

	return writebytes;
}

int ucos_pipe_close(int fd)
{
	ByteQueue *bq = NULL;

	if(!IS_VALID(fd))
		return -1;

	bq = PTROFBYTEQ(fd);
	if(IS_RD_FD(fd))
	{
		if(!bq->rd_opened)
			return -1;

		bq->rd_opened = FALSE;
	}
	else if(IS_WR_FD(fd))
	{
		if(!bq->wr_opened)
			return -1;

		bq->wr_opened = FALSE;
	}
	else
	{
		return -1;
	}

	if((!bq->rd_opened) && (!bq->wr_opened))
	{
		mutex_destroy(bq->mtx);
		free(bq->p);
		memset(bq, 0, sizeof(ByteQueue));
		return 0;
	}

	return 0;
}

int ucos_fd_select(fd_set *fdset, struct timeval *tv)
{
	INT8U err;
	INT32U us = 0;
	INT32S i = 0, n = 0;
	OS_FLAGS flags = 0, curflags;

	for(i=0; i<fdset->fd_count && i<OS_FLAGS_NBITS; i++)
	{
		flags |= BIT_FLAG(fdset->fd_array[i]);
	}

	us = 0;
	if(tv != NULL)
	{
		us = (INT32U)(tv->tv_sec*1000000 + tv->tv_usec);
		if(us > 0)
			us = (us / 1000) / (1000 / OS_TICKS_PER_SEC);
		else
			us = 1; /* = return 0; */
	}

	do{
		curflags = OSFlagPend(g_mgr.fg, flags, OS_FLAG_WAIT_SET_ANY, (INT16U)us, &err);
		if(err == OS_TIMEOUT && us > 0)
			break;
	}while(err != OS_NO_ERR);

	if(curflags)
	{
		n = 0;
		/*
		i = fdset->fd_count-1;
		i = i > OS_FLAGS_NBITS ? OS_FLAGS_NBITS : i;
		for(i=i; i>=0; i--)
		{
			if(curflags & (0x01<<i))
				n ++;
		}
		*/
		n++;
	}

	return n;
}

int ucos_fd_isset(int fd, fd_set *fdset)
{
	unsigned char err;
	INT32S i;
	OS_FLAGS flags;

	if(fdset == NULL)
		return 0;

	for(i=0; i<fdset->fd_count && i<OS_FLAGS_NBITS; i++)
	{
		if(fdset->fd_array[i] == fd)
			break;
	}

	if(i < fdset->fd_count && i < OS_FLAGS_NBITS)
	{
		do{
			flags = OSFlagQuery(g_mgr.fg, &err);
		}while(err != OS_NO_ERR);
		if(flags & BIT_FLAG(fd))
			return 1;
	}

	return 0;
}

#ifdef UCOS_PIPE_TEST

static int fd_read = 0;
static int fd_write = 0;

static void AppStartTask (void *p_arg)
{
	char buf[5];
	int fd[2];
	fd_set fdset;

	ucos_pipe_init();
	OSTimeDly(20);	/* */

	if(ucos_pipe_create(fd))
		return 0;

	fd_read = fd[0];
	fd_write = fd[1];

	while(1)
	{
		if(fd_read > 0)
		{
			FD_ZERO(&fdset);
			FD_SET(fd_read, &fdset);

			_log("waiting...\n");
			if(ucos_fd_select(&fdset, NULL))
			{
				_log("isset...\n");
				if(ucos_fd_isset(fd_read, &fdset))
				{
					_log("task0 read...\n");
					if(4 == ucos_pipe_read(fd_read, buf, 4))
						_log(" OK\n");
					else
						_log("Read Error\n");
				}
			}
		}
	}
}

static void AppTask1 (void *p_arg)
{
	char buf[5];
	p_arg = p_arg;

	strcpy(buf, "good");

	OSTimeDly(20);

	while(1)
	{
		OSTimeDly(20);
		if(fd_write > 0)
		{
			_log("task1 write...\n");
			if(4 == ucos_pipe_write(fd_write, buf, 4))
				_log(" OK\n");
			else
				_log("Write Error\n");
		}
	}
}

#endif
