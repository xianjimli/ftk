/*
 * File: ftk_cairo.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   cairo wrapper
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
 * 2009-12-18 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#include "ftk_cairo.h"

cairo_surface_t* ftk_cairo_surface_create(FtkWidget* window)
{
	FtkBitmap* bitmap = NULL;
	FtkCanvas* canvas = NULL;
	return_val_if_fail(window != NULL, NULL);
	canvas = ftk_widget_canvas(window);
	ftk_canvas_lock_buffer(canvas, &bitmap);
	return_val_if_fail(bitmap != NULL, NULL);
	return ftk_cairo_surface_create_with_bitmap(bitmap);
}

cairo_surface_t* ftk_cairo_surface_create_with_bitmap(FtkBitmap* bitmap)
{
	int width = 0;
	int height = 0;
	FtkColor*  data   = NULL;
	cairo_surface_t*  surface = NULL;
	return_val_if_fail(bitmap != NULL, NULL);

	data = ftk_bitmap_lock(bitmap);
	width = ftk_bitmap_width(bitmap);
	height = ftk_bitmap_height(bitmap);

	surface = cairo_image_surface_create_for_data(data, CAIRO_FORMAT_ARGB32, width, height, width * 4);

	return surface;
}
