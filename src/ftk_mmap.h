/*
 * File: ftk_mmap.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   wrap mmap.
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
 * 2009-11-20 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_MMAP_H
#define FTK_MMAP_H

#include "ftk_typedef.h"

FTK_BEGIN_DECLS

struct _FtkMmap;
typedef struct _FtkMmap FtkMmap;

FtkMmap* ftk_mmap_create(const char* filename, size_t offset, size_t size);
void*    ftk_mmap_data(FtkMmap* thiz);
size_t   ftk_mmap_length(FtkMmap* thiz);
void     ftk_mmap_destroy(FtkMmap* thiz);
int      ftk_mmap_exist(const char* filename);

FTK_END_DECLS

#endif/*FTK_MMAP_H*/

