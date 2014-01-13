/*
 * File: ftk_combo_box.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   combo_box
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
 * 2010-01-28 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_COMBO_BOX_H
#define FTK_COMBO_BOX_H

#include "ftk_widget.h"

FTK_INHERITE_FROM(Widget)

FTK_BEGIN_DECLS

FtkWidget* ftk_combo_box_create(FtkWidget* parent, int x, int y, int width, int height);

const char* ftk_combo_box_get_text(FtkWidget* thiz);
Ret ftk_combo_box_set_text(FtkWidget* thiz, const char* text);
Ret ftk_combo_box_append(FtkWidget* thiz, FtkBitmap* icon, const char* text);
int ftk_combo_box_get_item_nr(FtkWidget* thiz);
Ret ftk_combo_box_get_item(FtkWidget* thiz, size_t index, const FtkBitmap** icon, const char** text);
FtkWidget* ftk_combo_box_get_entry(FtkWidget* thiz);
Ret ftk_combo_box_reset(FtkWidget* thiz);
Ret ftk_combo_box_remove(FtkWidget* thiz, size_t index);
Ret ftk_combo_box_set_clicked_listener(FtkWidget* thiz, FtkListener listener, void* ctx);
int ftk_combo_box_get_selected(FtkWidget* thiz);

FTK_END_DECLS

#endif/*FTK_COMBO_BOX_H*/

