/*
 * File: ftk_display_sigma.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   sigma implemented display.
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
 * 2009-12-21 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "ftk_log.h"
#include "ftk_display_mem.h"
#include "ftk_display_sigma.h"

FtkDisplay* ftk_display_sigma_create(void* osd_addr, int width, int height, int bits_per_pixel, void* osd)
{
	FtkDisplay* thiz = NULL;
	FtkPixelFormat format = 0;
	return_val_if_fail(osd_addr != NULL, NULL);

	if(bits_per_pixel == 16)
	{
		format = FTK_PIXEL_RGB565;
	}
	else if(bits_per_pixel == 24)
	{
		format = FTK_PIXEL_BGR24;
	}
	else if(bits_per_pixel == 32)
	{
		format = FTK_PIXEL_BGRA32;
	}
	else
	{
		assert(!"not supported framebuffer format.");
	}
	
	return ftk_display_mem_create(format, width, height, osd_addr, NULL, osd);
}

