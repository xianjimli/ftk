/*
 * File: ftk_sylixos.h
 * Author:  Han.hui <sylixos@gmail.com>
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
 * 2011-05-15 Han.hui <sylixos@gmail.com> created
 *
 */

#ifndef FTK_SYLIXOS_H
#define FTK_SYLIXOS_H

#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <mouse.h>
#include <keyboard.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#define div_t lib_div_t

#define FTK_ROOT_DIR                    "/ftk"
#define DATA_DIR                        FTK_ROOT_DIR
#define LOCAL_DATA_DIR                  FTK_ROOT_DIR
#define FTK_DATA_ROOT                   FTK_ROOT_DIR"/data"
#define TESTDATA_DIR                    FTK_ROOT_DIR"/testdata"
#define FTK_FONT_DIR                    FTK_ROOT_DIR"/data"
#define APP_DATA_DIR                    FTK_ROOT_DIR"/apps"
#define APP_LOCAL_DATA_DIR              FTK_ROOT_DIR"/apps"

#define FTK_FONT                        "font.ttf"

#define FTK_CNF                         FTK_ROOT_DIR"/ftk.conf"

#define ftk_strncpy                     strncpy
#define ftk_snprintf                    snprintf
#define ftk_vsnprintf                   vsnprintf
#define ftk_getcwd                      getcwd
#define ftk_sscanf                      sscanf

#define ftk_pipe_pair(fds)              pipe(fds)
#define ftk_pipe_close(fd)              close(fd)
#define ftk_pipe_recv(fd, buf, length)  read(fd, buf, length)
#define ftk_pipe_send(fd, buf, length)  write(fd, buf, length)

#ifndef __cplusplus
void ftk_assert(int cond, const char* func, const char* file, int line);
#undef assert
#define assert(cond)                    ftk_assert(cond, __func__, __FILE__, __LINE__)
#endif

#define min(a,b)                        ((a) < (b) ? (a) : (b))

#include "./config_ftk.h"

#define FTK_PATH_DELIM                  '/'

#define FTK_HAS_MAIN                    0
#define FTK_HAS_DLOPEN                  1
#define FTK_HAS_LUA                     1

#define FTK_OPTIMIZE_WIDGET_PAINT       0

#endif  /* FTK_SYLIXOS_H */
