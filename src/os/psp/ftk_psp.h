
/*
 * File: ftk_psp.h
 * Author:  Tao Yu <yut616@gmail.com>
 * Brief:   psp specific functions.
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
 * 2010-03-11 Tao Yu <yut616@gmail.com> created.
 *
 */

#ifndef FTK_PSP_H
#define FTK_PSP_H

#include <psputils.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspiofilemgr.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>

char* ftk_get_root_dir(void);
char* ftk_get_data_dir(void);
char* ftk_get_testdata_dir(void);

#define FTK_ROOT_DIR   ftk_get_root_dir()
#define DATA_DIR       ftk_get_root_dir()
#define LOCAL_DATA_DIR ftk_get_root_dir()
#define FTK_DATA_ROOT  ftk_get_data_dir()
#define TESTDATA_DIR   ftk_get_testdata_dir()

#define ftk_strncpy   strncpy
#define ftk_snprintf  snprintf
#define ftk_vsnprintf vsnprintf 
#define ftk_sscanf    sscanf

#define usleep		sceKernelDelayThread
#define exit(...)	sceKernelExitGame()

#ifdef FTK_PSP_DEBUG_ON_SCREEN
#define printf		pspDebugScreenPrintf
#endif

#define ftk_pipe_pair(fds)				pipe(fds)
#define ftk_pipe_close(fd)				close(fd)
#define ftk_pipe_recv(fd, buf, length)	read(fd, buf, length)
#define ftk_pipe_send(fd, buf, length)	write(fd, buf, length)

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define FTK_PATH_DELIM '/'
#define FTK_HAS_MAIN 1
char *ftk_getcwd(char *buf, size_t size);
#endif/*FTK_PSP_H*/

