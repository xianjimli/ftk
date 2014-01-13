/*
 * File: ftk_menu_panel.h    
 * Author: Li XianJing <xianjimli@hotmail.com>
 * Brief:  ftk menu panel
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
 * 2009-10-18 Li XianJing <xianjimli@hotmail.com> created
 *
 */


#ifndef FTK_MENU_PANEL_H
#define FTK_MENU_PANEL_H
#include "ftk_window.h"

FTK_INHERITE_FROM(Window)

FTK_BEGIN_DECLS

FtkWidget* ftk_menu_panel_create(void);
Ret ftk_menu_panel_relayout(FtkWidget* thiz);
Ret ftk_menu_panel_add(FtkWidget* thiz, FtkWidget* item);
Ret ftk_menu_panel_remove(FtkWidget* thiz, FtkWidget* item);

FTK_END_DECLS

#endif/*FTK_MENU_PANEL_H*/

