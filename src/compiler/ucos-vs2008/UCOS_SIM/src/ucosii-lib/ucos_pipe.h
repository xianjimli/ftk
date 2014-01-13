/*
* File: ucos_pipe.c    
* Author:  MinPengli <MinPengli@gmail.com>
* Brief: pipe implement
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

#ifndef UCOS_PIPE_H
#define UCOS_PIPE_H

#ifdef __cplusplus
extern "C" {
#endif

int ucos_pipe_init(void);
int ucos_pipe_deinit(void);
int ucos_pipe_create(int fd[]);
int ucos_pipe_read(int fd, void *buf, int count);
int ucos_pipe_write(int fd, void *buf, int count);
int ucos_pipe_close(int fd);
int ucos_fd_select(fd_set *fdset, struct timeval *tv);
int ucos_fd_isset(int fd, fd_set *fdset);

#ifdef __cplusplus
}
#endif

#endif
