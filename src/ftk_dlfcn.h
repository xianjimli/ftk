/*
 * File: ftk_dlfcn.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:  wrap dlfcn
 *
 * Copyright (c) 2011  Li XianJing <xianjimli@hotmail.com>
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
 * 2011-05-01 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_DLFCN_H
#define FTK_DLFCN_H

#include "ftk_typedef.h"

FTK_BEGIN_DECLS

void* ftk_dlopen(const char* filename);
void* ftk_dlsym(void* handle, const char* func);
void  ftk_dlclose(void* handle);
char* ftk_dl_file_name(const char* name, char filename[FTK_MAX_PATH+1]);

FTK_END_DECLS

#endif/*FTK_DLFCN_H*/

