/*
 * File: ftk_sources_manager.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   sources manager
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
 * 2009-10-15 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_SOURCE_MANAGER_H
#define FTK_SOURCE_MANAGER_H

#include "ftk_source.h"

FTK_BEGIN_DECLS

struct _FtkSourcesManager;
typedef struct _FtkSourcesManager FtkSourcesManager;

FtkSourcesManager* ftk_sources_manager_create(int max_source_nr);
Ret  ftk_sources_manager_add(FtkSourcesManager* thiz, FtkSource* source);
Ret  ftk_sources_manager_remove(FtkSourcesManager* thiz, FtkSource* source);

int  ftk_sources_manager_get_count(FtkSourcesManager* thiz);
FtkSource* ftk_sources_manager_get(FtkSourcesManager* thiz, int i);

/*Tell the main-loop break current loop and reselect.*/
int ftk_sources_manager_need_refresh(FtkSourcesManager* thiz);
Ret  ftk_sources_manager_set_need_refresh(FtkSourcesManager* thiz);

void ftk_sources_manager_destroy(FtkSourcesManager* thiz);

FTK_END_DECLS

#endif/*FTK_SOURCE_MANAGER_H*/

