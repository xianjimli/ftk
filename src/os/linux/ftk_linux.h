
/*
 * File: ftk_linux.h
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
 * 2009-11-22 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_LINUX_H
#define FTK_LINUX_H

#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/input.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#define ftk_strncpy   strncpy
#define ftk_snprintf  snprintf
#define ftk_vsnprintf vsnprintf 
#define ftk_getcwd    getcwd
#define ftk_sscanf    sscanf

#define ftk_pipe_pair(fds)				pipe(fds)
#define ftk_pipe_close(fd)				close(fd)
#define ftk_pipe_recv(fd, buf, length)	read(fd, buf, length)
#define ftk_pipe_send(fd, buf, length)	write(fd, buf, length)

#ifdef HAVE_CONFIG_H
#include "config_ftk.h"
#endif

#define FTK_PATH_DELIM '/'

#define FTK_HAS_MAIN 1
#define LINUX 1

#endif/*FTK_LINUX_H*/

