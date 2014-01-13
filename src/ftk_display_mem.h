/*
 * File: ftk_display_men.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   memory based display.
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
 * 2010-03-21 Li XianJing <xianjimli@hotmail.com> from ftk_display_fb.h
 *
 */
#ifndef FTK_DISPLAY_MEM_H
#define FTK_DISPLAY_MEM_H

#include "ftk_display.h"

typedef void (*FtkDisplaySync)(void* ctx, FtkRect* rect);

FtkDisplay* ftk_display_mem_create(FtkPixelFormat format, 
	int width, int height, void* bits, FtkDestroy on_destroy, void* ctx);

Ret ftk_display_mem_set_sync_func(FtkDisplay* thiz, FtkDisplaySync sync, void* ctx);

int ftk_display_mem_is_active(FtkDisplay* thiz);
FtkPixelFormat ftk_display_mem_get_pixel_format(FtkDisplay* thiz);
Ret ftk_display_mem_update_directly(FtkDisplay* thiz, FtkPixelFormat format,
	void* bits, int width, int height, int xoffset, int yoffset);

#endif/*FTK_DISPLAY_MEM_H*/

