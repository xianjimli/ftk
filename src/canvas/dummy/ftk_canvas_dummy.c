/*
 * File: ftk_canvas_dummy.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   dummy canvas implemntation.
 *
 * Copyright (c) 2009 - 2011  Li XianJing <xianjimli@hotmail.com>
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
 * 2011-04-18 Li XianJing <xianjimli@hotmail.com> created
 */

#include "ftk_log.h"
#include "ftk_util.h"
#include "ftk_bitmap.h"
#include "ftk_canvas.h"
#include "ftk_globals.h"

typedef struct _CanvasSkiaPrivInfo
{
	int w;
	int h;
}PrivInfo;

static Ret ftk_canvas_dummy_sync_gc(FtkCanvas* thiz)
{
	int size = 0;
	DECL_PRIV(thiz, priv);
	FtkColor c = thiz->gc.fg;
	
	if(thiz->gc.font != NULL)
	{
		char desc[64] = {0};
		size = ftk_font_desc_get_size(thiz->gc.font);
	}

	return RET_OK;
}

static Ret ftk_canvas_dummy_set_clip(FtkCanvas* thiz, FtkRegion* clip)
{
	return RET_OK;
}

static Ret ftk_canvas_dummy_draw_pixels(FtkCanvas* thiz, FtkPoint* points, int nr)
{
	DECL_PRIV(thiz, priv);

	return RET_OK;
}

static Ret ftk_canvas_dummy_draw_line(FtkCanvas* thiz, int x1, int y1, int x2, int y2)
{
	DECL_PRIV(thiz, priv);

	return RET_OK;
}

static Ret ftk_canvas_dummy_clear_rect(FtkCanvas* thiz, int x, int y, int w, int h)
{
	DECL_PRIV(thiz, priv);
	
	return RET_OK;
}

static Ret ftk_canvas_dummy_draw_rect(FtkCanvas* thiz, int x, int y, int w, int h, 
	int round, int fill)
{
	return RET_OK;
}

static Ret ftk_canvas_dummy_draw_bitmap(FtkCanvas* thiz, FtkBitmap* bitmap, 
	FtkRect* src_r, FtkRect* dst_r, int alpha)
{
	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);

	return ret;
}

static Ret ftk_canvas_dummy_draw_string(FtkCanvas* thiz, int x, int y, 
	const char* str, int len, int vcenter)
{
	DECL_PRIV(thiz, priv);

	return RET_OK;
}

static Ret fk_canvas_dummy_lock_buffer(FtkCanvas* thiz, FtkBitmap** bitmap)
{
	DECL_PRIV(thiz, priv);

	return RET_OK;
}

static Ret ftk_canvas_dummy_unlock_buffer(FtkCanvas* thiz)
{
	return RET_OK;
}

static void ftk_canvas_dummy_destroy(FtkCanvas* thiz)
{
	DECL_PRIV(thiz, priv);

	if(thiz != NULL)
	{
		FTK_FREE(thiz);
	}

	return;
}

static int ftk_canvas_dummy_get_char_extent(FtkCanvas* thiz, unsigned short code)
{
	return 0;
}

static int ftk_canvas_dummy_get_str_extent(FtkCanvas* thiz, const char* str, int len)
{
	return 0;
}

static const FtkCanvasVTable g_canvas_dummy_vtable=
{
	ftk_canvas_dummy_sync_gc,
	ftk_canvas_dummy_set_clip,
	ftk_canvas_dummy_draw_pixels,
	ftk_canvas_dummy_draw_line,
	ftk_canvas_dummy_clear_rect,
	ftk_canvas_dummy_draw_rect,

	ftk_canvas_dummy_draw_bitmap,
	ftk_canvas_dummy_draw_string,
	ftk_canvas_dummy_get_str_extent,
	ftk_canvas_dummy_get_char_extent,
	fk_canvas_dummy_lock_buffer,
	ftk_canvas_dummy_unlock_buffer,
	ftk_canvas_dummy_destroy
};

FtkCanvas* ftk_canvas_create(int w, int h, FtkColor* clear_color)
{
	FtkCanvas* thiz = NULL;
	return_val_if_fail(w > 0 && h > 0 && clear_color != NULL, NULL);

	thiz = (FtkCanvas*)FTK_ZALLOC(sizeof(FtkCanvas) + sizeof(PrivInfo));

	ftk_logd("ftk_canvas_create: %d %d\n", w, h);
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		thiz->gc.bg = *clear_color;
		thiz->gc.fg.a = 0xff;
		thiz->gc.fg.r = 0xff - clear_color->r;
		thiz->gc.fg.g = 0xff - clear_color->g;
		thiz->gc.fg.b = 0xff - clear_color->b;
		thiz->gc.mask = FTK_GC_FG | FTK_GC_BG;
		thiz->vtable = &g_canvas_dummy_vtable;
	}

	return thiz;
}

