/*
 * File: ftk_entry.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   single line editor
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
 * 2009-11-02 Li XianJing <xianjimli@hotmail.com> created
 * 2012-5-22  woodysu@gmail.com modify
 *
 */

#ifndef FTK_ENTRY_H
#define FTK_ENTRY_H

#include "ftk_widget.h"
#include "ftk_input_method.h"

FTK_INHERITE_FROM(Widget)

FTK_BEGIN_DECLS

FtkWidget* ftk_entry_create(FtkWidget* parent, int x, int y, int width, int height);

const char* ftk_entry_get_text(FtkWidget* thiz);
Ret ftk_entry_set_text(FtkWidget* thiz, const char* text);

/*tips is shown when there is no text in the entry.*/
Ret ftk_entry_set_tips(FtkWidget* thiz, const char* tips);
Ret ftk_entry_set_input_type(FtkWidget* thiz, FtkInputType type);
Ret ftk_entry_insert_text(FtkWidget* thiz, size_t pos, const char* text);
Ret ftk_entry_set_readonly(FtkWidget* thiz, int readonly);
Ret ftk_entry_set_noborder(FtkWidget* thiz, int b);

FTK_END_DECLS

#endif/*FTK_ENTRY_H*/

