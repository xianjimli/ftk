/*
 * File: ftk_progress_bar.h  
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   progress bar
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

#ifndef FTK_PROGRESS_BAR_H
#define FTK_PROGRESS_BAR_H

#include "ftk_widget.h"

FTK_INHERITE_FROM(Widget)

FTK_BEGIN_DECLS

FtkWidget* ftk_progress_bar_create(FtkWidget* parent, int x, int y, int width, int height);
int ftk_progress_bar_get_percent(FtkWidget* thiz);
Ret ftk_progress_bar_set_percent(FtkWidget* thiz, int percent);
Ret ftk_progress_bar_set_interactive(FtkWidget* thiz, int interactive);

FTK_END_DECLS

#endif/*FTK_PROGRESS_BAR_H*/



