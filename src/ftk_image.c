/*
 * File: ftk_image.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: image control.  
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

#include "ftk_image.h"
#include "ftk_globals.h"

static Ret ftk_image_on_event(FtkWidget* thiz, FtkEvent* event)
{
	return RET_OK;
}

static Ret ftk_image_on_paint(FtkWidget* thiz)
{
	
	FTK_END_PAINT();
}

static void ftk_image_destroy(FtkWidget* thiz)
{
	return;
}

FtkWidget* ftk_image_create(FtkWidget* parent, int x, int y, int width, int height)
{
	FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	return_val_if_fail(thiz != NULL, NULL);

	thiz->on_event = ftk_image_on_event;
	thiz->on_paint = ftk_image_on_paint;
	thiz->destroy  = ftk_image_destroy;

	ftk_widget_init(thiz, FTK_IMAGE, 0, x, y, width, height, FTK_ATTR_INSENSITIVE|FTK_ATTR_TRANSPARENT);
	ftk_widget_append_child(parent, thiz);

	return thiz;
}

Ret ftk_image_set_image(FtkWidget* thiz, FtkBitmap* image)
{
	FtkGc gc = {0};
	return_val_if_fail(thiz != NULL && image != NULL, RET_FAIL);

	gc.mask = FTK_GC_BITMAP;
	gc.bitmap = image;
	ftk_widget_set_gc(thiz, FTK_WIDGET_INSENSITIVE, &gc);
	ftk_bitmap_unref(image);

	ftk_widget_invalidate(thiz);

	return RET_OK;
}

