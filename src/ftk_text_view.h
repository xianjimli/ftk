/*
 * File: ftk_text_view.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   multi line editor
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
 * 2009-12-31 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_TEXT_VIEW_H
#define FTK_TEXT_VIEW_H

#include "ftk_widget.h"

FTK_INHERITE_FROM(Widget)

FTK_BEGIN_DECLS

FtkWidget* ftk_text_view_create(FtkWidget* parent, int x, int y, int width, int height);

const char* ftk_text_view_get_text(FtkWidget* thiz);
Ret ftk_text_view_set_text(FtkWidget* thiz, const char* text, int len);
Ret ftk_text_view_insert_text(FtkWidget* thiz, size_t pos, const char* text, int len);
Ret ftk_text_view_set_readonly(FtkWidget* thiz, int readonly);

int ftk_text_view_get_total_lines(FtkWidget* thiz);
Ret ftk_text_view_set_noborder(FtkWidget* thiz, int b); 
Ret ftk_text_view_v_move_caret(FtkWidget* thiz, int offset);

FTK_END_DECLS

#endif/*FTK_TEXT_VIEW_H*/

