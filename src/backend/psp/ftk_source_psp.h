/*
 * File: ftk_source_psp.h    
 * Author:  Tao Yu <yut616@gmail.com>
 * Brief:   psp event handler
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
 * 2010-03-27 Tao Yu <yut616@gmail.com> created.
 *
 */
#ifndef FTK_SOURCE_PSP_H
#define FTK_SOURCE_PSP_H
#include "ftk_event.h"
#include "ftk_source.h"
#include "ftk_display.h"

FtkSource* ftk_source_psp_create(FtkDisplay* display, FtkOnEvent on_event, void* ctx);

#endif/*FTK_SOURCE_PSP_H*/

