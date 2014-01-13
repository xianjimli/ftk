/*
 * File: ftk_path.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   path.
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
 * 2010-08-01 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_PATH_H
#define FTK_PATH_H

#include "ftk_typedef.h"

FTK_BEGIN_DECLS
struct _FtkPath;
typedef struct _FtkPath FtkPath;

FtkPath* ftk_path_create(const char* path);
size_t ftk_path_get_levels(FtkPath* thiz);
Ret ftk_path_set_path(FtkPath* thiz, const char* path);
Ret ftk_path_up(FtkPath* thiz);
Ret ftk_path_down(FtkPath* thiz);
Ret ftk_path_root(FtkPath* thiz);
int ftk_path_is_leaf(FtkPath* thiz);
int ftk_path_current_level(FtkPath* thiz);
const char* ftk_path_full(FtkPath* thiz);
const char* ftk_path_current(FtkPath* thiz);
const char* ftk_path_get_sub(FtkPath* thiz, size_t level);
void ftk_path_destroy(FtkPath* thiz);

FTK_END_DECLS

#endif/*FTK_PATH_H*/
