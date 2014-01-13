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

#ifndef __FTK_RTTHREAD_H__
#define __FTK_RTTHREAD_H__

#include <rtthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/dirent.h>
#include <dfs_posix.h>
#include "ftk_select_rtthread.h"

#define FTK_ROOT_DIR                    "/ftk"
#define DATA_DIR                        FTK_ROOT_DIR
#define LOCAL_DATA_DIR                  FTK_ROOT_DIR
#define FTK_DATA_ROOT                   FTK_ROOT_DIR"/data"
#define TESTDATA_DIR                    FTK_ROOT_DIR"/testdata"
#define APP_DATA_DIR                    FTK_ROOT_DIR"/apps"
#define APP_LOCAL_DATA_DIR              FTK_ROOT_DIR"/apps"

#define FTK_FONT                        "gb2312.fnt"

#define ftk_strncpy                     strncpy
#define ftk_snprintf                    rt_snprintf
#define ftk_vsnprintf                   vsnprintf
#define ftk_getcwd                      getcwd
#define ftk_sscanf                      sscanf

#define FTK_PATH_DELIM                  '/'

#define USE_FTK_MAIN
#define FTK_HAS_DLOPEN

#define usleep(us)			            rt_thread_sleep(us*1000/RT_TICK_PER_SECOND)
#define select 							ftk_rtthread_select
#define printf							rt_kprintf

#define HAS_BMP
#define HAS_PNG
#define HAS_JPEG

#endif // __FTK_RTTHREAD_H__
