/*
 * File: ftk_icon_cache.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   cache to load icons.
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
 * 2009-11-04 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#ifndef FTK_ICON_CACHE_H
#define FTK_ICON_CACHE_H
#include "ftk_bitmap.h"

#define FTK_ICON_PATH_NR   3
#define FTK_ICON_CACHE_MAX 64

FTK_BEGIN_DECLS

struct _FtkIconCache;
typedef struct _FtkIconCache FtkIconCache;

/*final filename = root_path/rel_path/filename */
FtkIconCache* ftk_icon_cache_create(const char* root_path[FTK_ICON_PATH_NR], const char* rel_path);
FtkBitmap* ftk_icon_cache_load(FtkIconCache* thiz, const char* filename);
void ftk_icon_cache_destroy(FtkIconCache* thiz);

FTK_END_DECLS

#endif/*FTK_ICON_CACHE_H*/

