/*
 * File: ftk_platform.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   
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
 * 2009-10-03 Li XianJing <xianjimli@hotmail.com> created
 * 2010-10-02 Jiao JinXing <jiaojinxing1987@gmail.com> add rt-thread support.
 *
 */
#ifndef FTK_PLATFORM_H
#define FTK_PLATFORM_H

#ifndef RT_THREAD
#include <stdio.h>
#endif
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#ifdef LINUX
#include "ftk_linux.h"
#endif

#ifdef WIN32
#ifdef UCOS_SIM
#include "ftk_ucos_sim.h"
#elif defined(WINCE)
#include "ftk_wince.h"
#else
#include "ftk_win32.h"
#endif
#endif

#ifdef ARM_ADS
#include "ftk_ads.h"
#endif

#ifdef PSP
#include "ftk_psp.h"
#endif

#ifdef IPHONE
#include "ftk_iphone.h"
#endif

#ifdef RT_THREAD
#include "ftk_rtthread.h"
#endif

#if defined(ANDROID) && defined(ANDROID_NDK)
#include "ftk_android.h"
#endif

#ifdef SYLIXOS_LIB
#include "ftk_sylixos.h"
#endif

int  ftk_platform_init(int argc, char** argv);
void ftk_platform_deinit(void);
size_t ftk_get_relative_time(void);

#endif/*FTK_PLATFORM_H*/

