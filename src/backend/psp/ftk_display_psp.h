/*
 * File: ftk_display_psp.h    
 * Author:  Tao Yu <yut616@gmail.com> 
 * Brief:   PSP Display.
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
 * 2010-03-11 Tao Yu <yut616@gmail.com> created.
 *
 */

#ifndef FTK_DISPLAY_PSP_H
#define FTK_DISPLAY_PSP_H

#include "ftk_display.h"
#include "ftk_event.h"
#include "ftk_source.h"

FtkDisplay* ftk_display_psp_create(FtkSource** event_source, FtkOnEvent on_event, void* ctx);

Ret   ftk_display_psp_on_resize(FtkDisplay* thiz, int width, int height);
void* ftk_display_psp_get_xdisplay(FtkDisplay* thiz);
void* ftk_display_psp_get_xwindow(FtkDisplay* thiz);

#endif/*FTK_DISPLAY_PSP_H*/

