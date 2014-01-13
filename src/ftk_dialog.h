/*
 * File: ftk_dialog.h    
 * Author: Li XianJing <xianjimli@hotmail.com>
 * Brief:  dialog
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
 * 2012-05-09 yapo <woodysu@gmail.com> modify
 *
 */

#ifndef FTK_DIALOG_H
#define FTK_DIALOG_H
#include "ftk_window.h"

FTK_INHERITE_FROM(Window)

FTK_BEGIN_DECLS

FtkWidget* ftk_dialog_create(int x, int y, int width, int height);
FtkWidget* ftk_dialog_create_ex(int attr, int x, int y, int width, int height);
FtkWidget* ftk_sub_dialog_create_ex(FtkWidget* parent, int attr, int x, int y, int width, int height);
Ret ftk_dialog_set_icon(FtkWidget* thiz, FtkBitmap* icon);
Ret ftk_dialog_set_bg(FtkWidget* thiz, FtkBitmap* bitmap);
Ret ftk_dialog_hide_title(FtkWidget* thiz);
int ftk_dialog_run(FtkWidget* thiz);
Ret ftk_dialog_quit(FtkWidget* thiz);
Ret ftk_dialog_quit_after(FtkWidget* thiz, int ms);

/*internal used functions*/

int ftk_dialog_get_title_height(void);

Ret ftk_dialog_set_bg(FtkWidget* thiz, FtkBitmap* bitmap);

FTK_END_DECLS

#endif/*FTK_DIALOG_H*/


