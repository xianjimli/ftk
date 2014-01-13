/*
 * File: ftk_ucos_sim.h
 * Author:  MinPengli <MinPengli@gmail.com>
 * Brief:   ucos specific functions.
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
 * History:
 * ================================================================
 * 2010-03-19 MinPengli <MinPengli@gmail.com> created
 *
 */

#ifndef FTK_UCOS_SIM_H
#define FTK_UCOS_SIM_H

#ifdef WIN32
#include <io.h>
#include <time.h>
#include <stdio.h>
#include <windows.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef UCOS_SIM
#include <ucos_pipe.h>
#endif

#define HAVE_BOOLEAN
#define inline 

char* ftk_get_root_dir(void);
char* ftk_get_data_dir(void);
char* ftk_get_testdata_dir(void);

#define __func__       __FILE__
#define FTK_ROOT_DIR   ftk_get_root_dir()
#define FTK_FONT       "\\gb2312.fnt"
#define DATA_DIR       ftk_get_root_dir()
#define LOCAL_DATA_DIR ftk_get_root_dir()
#define FTK_DATA_ROOT  ftk_get_data_dir()
#define TESTDATA_DIR   ftk_get_testdata_dir()

#define _CRT_SECURE_NO_WARNINGS 1

#define usleep			Sleep
#define ftk_getcwd		_getcwd
#define ftk_sscanf    sscanf
#define strcasecmp stricmp
#define _S_ISDIR(a) 0
char* ftk_strncpy(char *dest, const char *src, size_t n);
int   ftk_snprintf(char *str, size_t size, const char *format, ...);
int   ftk_vsnprintf(char *str, size_t size, const char *format, va_list ap);

size_t ftk_get_relative_time(void);

#define ftk_pipe_pair(fds)				ucos_pipe_create(fds)
#define ftk_pipe_recv(fd, buf, c, x)	ucos_pipe_read(fd, buf, c)
#define ftk_pipe_send(fd, buf, c, x)	ucos_pipe_write(fd, buf, c)
#define ftk_pipe_close(fd)				ucos_pipe_close(fd);

#define ftk_select(n, fdset, tv)		ucos_fd_select(fdset, tv)
#define ftk_fd_isset(fd, fdset)			ucos_fd_isset(fd, fdset)
#define ftk_delay(t)					OSTimeDly(t>>1)

#define FTK_PATH_DELIM '/'
#define USE_FTK_MAIN 1

#endif

#endif/* FTK_UCOS_SIM_H */

