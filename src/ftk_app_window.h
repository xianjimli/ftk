/*
 * File: ftk_app_window.h    
 * Author: Li XianJing <xianjimli@hotmail.com>
 * Brief:  normal application window.
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
 * 2009-10-31 Li XianJing <xianjimli@hotmail.com> created
 *
 */

FTK_INHERITE_FROM(Window)

#ifndef FTK_APP_WINDOW_H
#define FTK_APP_WINDOW_H
#include "ftk_window.h"

FTK_BEGIN_DECLS

typedef Ret (*FtkPrepareOptionsMenu)(void* ctx, FtkWidget* menu_panel);

FtkWidget* ftk_app_window_create(void);

Ret ftk_app_window_set_on_prepare_options_menu(FtkWidget* thiz, 
	FtkPrepareOptionsMenu on_prepare_options_menu, void* ctx);

FTK_END_DECLS

#endif/*FTK_APP_WINDOWL*/

