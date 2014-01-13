/*
 * File: ftk_list_view.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   list view
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
 * 2009-11-15 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_LIST_VIEW_H
#define FTK_LIST_VIEW_H

#include "ftk_widget.h"
#include "ftk_list_model.h"
#include "ftk_list_render.h"

FTK_INHERITE_FROM(Widget)

FTK_BEGIN_DECLS

FtkWidget* ftk_list_view_create(FtkWidget* parent, int x, int y, int width, int height);
Ret ftk_list_view_init(FtkWidget* thiz, FtkListModel* model, FtkListRender* render, int item_height);

int ftk_list_view_get_selected(FtkWidget* thiz);
Ret ftk_list_view_set_cursor(FtkWidget* thiz, int current);
FtkListModel* ftk_list_view_get_model(FtkWidget* thiz);
Ret ftk_list_view_set_clicked_listener(FtkWidget* thiz, FtkListener listener, void* ctx);
Ret ftk_list_view_repaint_focus_item(FtkWidget* thiz);

FtkWidget* ftk_list_view_default_create(FtkWidget* parent, int x, int y, int width, int height);

FTK_END_DECLS

#endif/*FTK_LIST_VIEW_H*/

