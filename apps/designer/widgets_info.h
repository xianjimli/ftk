/*
 * File: widgets_info.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: widget infomation.
 *
 * Copyright (c) 2009 - 2011  Li XianJing <xianjimli@hotmail.com>
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
 * 2011-09-27 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_WIDGETS_INFO_H
#define FTK_WIDGETS_INFO_H
#include "ftk.h"

FTK_BEGIN_DECLS
typedef FtkWidget* (*WidgetCreate)(FtkWidget* parent, int x, int y, int width, int height);
typedef Ret (*WidgetPropEdit)(FtkWidget* widget);

typedef struct _WidgetInfo
{
	int type;
	const char* name;
	int min_width;
	int min_height;
	int default_width;
	int default_height;
	int is_leaf_widget;
	int is_resizable;
	int is_movable;
	WidgetCreate create;
	WidgetPropEdit edit;
	const char* create_func_name;
}WidgetInfo;

int widgets_info_get_nr(void);
const WidgetInfo* widgets_info_get(int index);
const WidgetInfo* widgets_info_find(const char* name);
const WidgetInfo* widgets_info_find_by_type(int type);

FTK_END_DECLS

#endif

