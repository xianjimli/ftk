/*
 * File: ftk_display_fb.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   framebuffer implemented display.
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
 * 2009-10-03 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "ftk_log.h"
#include "ftk_display_ucosii.h"
#include "ftk_display_mem.h"

#define DISPLAY_WIDTH			320
#define DISPLAY_HEIGHT			480
#define FB_BITS_PER_PIXEL		24

static char framebuffer[DISPLAY_WIDTH*DISPLAY_HEIGHT*((FB_BITS_PER_PIXEL+7) >> 3)];

void* GetFrameBufferAddr()
{
	return framebuffer;
}

FtkDisplay* ftk_display_ucosii_create()
{
	return ftk_display_mem_create(FTK_PIXEL_BGR24, DISPLAY_WIDTH, DISPLAY_HEIGHT, framebuffer, NULL, NULL); 
}

