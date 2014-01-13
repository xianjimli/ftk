/*
 * File: ftk_scroll_bar.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   scroll bar
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

#ifndef FTK_SCROLL_BAR_H
#define FTK_SCROLL_BAR_H
#include "ftk_widget.h"

FTK_INHERITE_FROM(Widget)

FTK_BEGIN_DECLS

FtkWidget* ftk_scroll_bar_create(FtkWidget* parent, int x, int y, int width, int height);
Ret ftk_scroll_bar_set_param(FtkWidget* thiz, int value, int max_value, int page_delta);
Ret ftk_scroll_bar_set_listener(FtkWidget* thiz, FtkListener listener, void* ctx);
int ftk_scroll_bar_get_value(FtkWidget* thiz);
int ftk_scroll_bar_get_max_value(FtkWidget* thiz);

Ret ftk_scroll_bar_inc(FtkWidget* thiz);
Ret ftk_scroll_bar_dec(FtkWidget* thiz);
Ret ftk_scroll_bar_pageup(FtkWidget* thiz);
Ret ftk_scroll_bar_pagedown(FtkWidget* thiz);
Ret ftk_scroll_bar_set_value(FtkWidget* thiz, int value);
Ret ftk_scroll_bar_set_bitmap(FtkWidget* thiz, FtkBitmap* bitmap);

FTK_END_DECLS

#endif/*FTK_SCROLL_BAR_H*/

