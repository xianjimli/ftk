/*
 * File: ftk_wait_box.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   wait box 
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
 * 2009-11-19 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_globals.h"
#include "ftk_wait_box.h"
#include "ftk_source_timer.h"

typedef struct _WaitBoxPrivInfo
{
	int offset;
	int waiting;
	FtkSource* timer;
	FtkBitmap* bitmap;
}PrivInfo;

static Ret ftk_wait_box_on_event(FtkWidget* thiz, FtkEvent* event)
{
	return RET_OK;
}

static Ret ftk_wait_box_on_paint(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	int bitmap_w = ftk_bitmap_width(priv->bitmap);
	int bitmap_h = ftk_bitmap_height(priv->bitmap);
	FTK_BEGIN_PAINT(x, y, width, height, canvas);

	(void)width;
	(void)height;

	if(priv->waiting)
	{
		priv->offset = priv->offset < bitmap_h ? priv->offset : 0;
		ftk_canvas_draw_bitmap_simple(canvas, priv->bitmap, 0, priv->offset, bitmap_w, bitmap_w, x, y);
		priv->offset += bitmap_w;
	}

	FTK_END_PAINT();
}

static void ftk_wait_box_destroy(FtkWidget* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV0(thiz, priv);

		if(priv->waiting)
		{
			ftk_wait_box_stop_waiting(thiz);
		}
		ftk_source_unref(priv->timer);
		ftk_bitmap_unref(priv->bitmap);
		FTK_ZFREE(priv, sizeof(PrivInfo));
	}

	return;
}

FtkWidget* ftk_wait_box_create(FtkWidget* parent, int x, int y, int w, int h)
{
	FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	return_val_if_fail(thiz != NULL, NULL);

	thiz->priv_subclass[0] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[0] != NULL)
	{
		int w = 0;
		DECL_PRIV0(thiz, priv);

		thiz->on_event = ftk_wait_box_on_event;
		thiz->on_paint = ftk_wait_box_on_paint;
		thiz->destroy  = ftk_wait_box_destroy;

		priv->bitmap = ftk_theme_load_image(ftk_default_theme(), "wait_box"FTK_STOCK_IMG_SUFFIX);
		assert(priv->bitmap != NULL);
		
		w = ftk_bitmap_width(priv->bitmap);
		ftk_widget_init(thiz, FTK_WAIT_BOX, 0, x, y, w, w, FTK_ATTR_TRANSPARENT|FTK_ATTR_INSENSITIVE);
		ftk_widget_set_attr(thiz, FTK_ATTR_TRANSPARENT|FTK_ATTR_INSENSITIVE);

		priv->timer = ftk_source_timer_create(500, (FtkTimer)ftk_widget_invalidate, thiz);
		ftk_widget_append_child(parent, thiz);
	}
	else
	{
		FTK_FREE(thiz);
	}

	return thiz;
}

Ret ftk_wait_box_start_waiting(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);
	return_val_if_fail(!priv->waiting, RET_FAIL);

	priv->offset = 0;
	priv->waiting = 1;
	ftk_source_ref(priv->timer);

	return ftk_main_loop_add_source(ftk_default_main_loop(), priv->timer);
}

Ret ftk_wait_box_stop_waiting(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);
	return_val_if_fail(priv->waiting, RET_FAIL);
	
	priv->waiting = 0;

	return ftk_main_loop_remove_source(ftk_default_main_loop(), priv->timer);
}

