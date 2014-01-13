/*
 * File: ftk_canvas.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   drawing kit.
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
 * 2011-04-16 Li XianJing <xianjimli@hotmail.com> change to interface.
 *
 */

#ifndef FTK_CANVAS_H
#define FTK_CANVAS_H

#include "ftk_gc.h"
#include "ftk_display.h"

FTK_BEGIN_DECLS

typedef enum _FtkBgStyle
{
	FTK_BG_NORMAL = 0,
	FTK_BG_TILE,
	FTK_BG_CENTER,
	FTK_BG_FOUR_CORNER
}FtkBgStyle;

struct _FtkCanvas;
typedef struct _FtkCanvas FtkCanvas;

struct _FtkCanvasVTable;
typedef struct _FtkCanvasVTable FtkCanvasVTable;

typedef Ret (*FtkCanvasSyncGc)(FtkCanvas* thiz);
typedef Ret (*FtkCanvasSetClip)(FtkCanvas* thiz, FtkRegion* clip);

typedef Ret (*FtkCanvasDrawPixels)(FtkCanvas* thiz, FtkPoint* points, int nr);
typedef Ret (*FtkCanvasDrawLine)(FtkCanvas* thiz, int x1, int y1, int x2, int y2);
typedef Ret (*FtkCanvasClearRect)(FtkCanvas* thiz, int x, int y, int w, int h);
typedef Ret (*FtkCanvasDrawRect)(FtkCanvas* thiz, int x, int y, int w, int h, int round, int fill);
typedef Ret (*FtkCanvasDrawBitmap)(FtkCanvas* thiz, FtkBitmap* bitmap, FtkRect* s, FtkRect* d, int alpha);
typedef Ret (*FtkCanvasDrawString)(FtkCanvas* thiz, int x, int y, const char* str, int len, int vcenter);

typedef int (*FtkCanvasGetCharExtent)(FtkCanvas* thiz, unsigned short code);
typedef int (*FtkCanvasGetStrExtent)(FtkCanvas* thiz, const char* str, int len);

typedef Ret (*FtkCanvasLockBuffer)(FtkCanvas* thiz, FtkBitmap** bitmap);
typedef Ret (*FtkCanvasUnlockBuffer)(FtkCanvas* thiz);
typedef void (*FtkCanvasDestroy)(FtkCanvas* thiz);

struct _FtkCanvasVTable
{
	FtkCanvasSyncGc        sync_gc;
	FtkCanvasSetClip       set_clip;
	FtkCanvasDrawPixels    draw_pixels;
	FtkCanvasDrawLine      draw_line;
	FtkCanvasClearRect     clear_rect;
	FtkCanvasDrawRect      draw_rect;
	FtkCanvasDrawBitmap    draw_bitmap;
	FtkCanvasDrawString    draw_string;
	FtkCanvasGetStrExtent  get_str_extent;
	FtkCanvasGetCharExtent get_char_extent;
	FtkCanvasLockBuffer    lock_buffer;
	FtkCanvasUnlockBuffer  unlock_buffer;
	FtkCanvasDestroy       destroy;
};

struct _FtkCanvas
{
	FtkGc gc;
	int width;
	int height;

	const FtkCanvasVTable* vtable;	
	char priv[1];
};

static inline Ret ftk_canvas_sync_gc(FtkCanvas* thiz)
{
	Ret ret = RET_FAIL;

	if(thiz != NULL && thiz->vtable != NULL && thiz->vtable->sync_gc != NULL)
	{
		ret = thiz->vtable->sync_gc(thiz);
	}

	return ret;
}

static inline Ret ftk_canvas_set_clip(FtkCanvas* thiz, FtkRegion* clip)
{
	Ret ret = RET_FAIL;
	
	if(thiz != NULL && thiz->vtable != NULL && thiz->vtable->set_clip != NULL)
	{
		ret = thiz->vtable->set_clip(thiz, clip);
	}

	return ret;
}

static inline Ret ftk_canvas_draw_pixels(FtkCanvas* thiz, FtkPoint* points, int nr)
{
	return_val_if_fail(thiz != NULL && thiz->vtable != NULL 
		&& thiz->vtable->draw_pixels != NULL, RET_FAIL);

	return thiz->vtable->draw_pixels(thiz, points, nr);
}

static inline Ret ftk_canvas_draw_line(FtkCanvas* thiz, int x1, int y1, int x2, int y2)
{
	return_val_if_fail(thiz != NULL && thiz->vtable != NULL 
		&& thiz->vtable->draw_line != NULL, RET_FAIL);

	return thiz->vtable->draw_line(thiz, x1, y1, x2, y2);
}

static inline Ret ftk_canvas_clear_rect(FtkCanvas* thiz, int x, int y, int w, int h)
{
	return_val_if_fail(thiz != NULL && thiz->vtable != NULL 
		&& thiz->vtable->clear_rect != NULL, RET_FAIL);

	return thiz->vtable->clear_rect(thiz, x, y, w, h);
}

static inline Ret ftk_canvas_draw_rect(FtkCanvas* thiz, int x, int y, int w, int h, int round, int fill)
{
	return_val_if_fail(thiz != NULL && thiz->vtable != NULL 
		&& thiz->vtable->draw_rect != NULL, RET_FAIL);

	return thiz->vtable->draw_rect(thiz, x, y, w, h, round, fill);
}

static inline Ret ftk_canvas_draw_bitmap(FtkCanvas* thiz, FtkBitmap* bmp, FtkRect* s, FtkRect* d, int alpha)
{
	return_val_if_fail(thiz != NULL && thiz->vtable != NULL 
		&& thiz->vtable->draw_bitmap != NULL, RET_FAIL);

	return thiz->vtable->draw_bitmap(thiz, bmp, s, d, alpha);
}

static inline Ret ftk_canvas_draw_string(FtkCanvas* thiz, int x, int y, 
	const char* str, int len, int vcenter)
{
	len = (len < 0 && str != NULL) ? (int)strlen(str) : len;

	return_val_if_fail(thiz != NULL && thiz->vtable != NULL 
		&& thiz->vtable->draw_string != NULL, RET_FAIL);

	return thiz->vtable->draw_string(thiz, x, y, str, len, vcenter);
}

static inline int ftk_canvas_get_str_extent(FtkCanvas* thiz, const char* str, int len)
{
	len = len >= 0 ? len : strlen(str);
	return_val_if_fail(thiz != NULL && thiz->vtable != NULL 
		&& thiz->vtable->get_str_extent != NULL, 0);

	return thiz->vtable->get_str_extent(thiz, str, len);
}

static inline int ftk_canvas_get_char_extent(FtkCanvas* thiz, unsigned short code)
{
	return_val_if_fail(thiz != NULL && thiz->vtable != NULL 
		&& thiz->vtable->get_char_extent != NULL, 0);

	return thiz->vtable->get_char_extent(thiz, code);
}

static inline Ret ftk_canvas_lock_buffer(FtkCanvas* thiz, FtkBitmap** bitmap)
{
	return_val_if_fail(thiz != NULL && thiz->vtable != NULL 
		&& thiz->vtable->lock_buffer != NULL, RET_FAIL);

	return thiz->vtable->lock_buffer(thiz, bitmap);
}

static inline Ret ftk_canvas_unlock_buffer(FtkCanvas* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->vtable != NULL 
		&& thiz->vtable->unlock_buffer != NULL, RET_FAIL);

	return thiz->vtable->unlock_buffer(thiz);
}

static inline void ftk_canvas_destroy(FtkCanvas* thiz)
{
	if(thiz != NULL && thiz->vtable != NULL && thiz->vtable->destroy != NULL)
	{
		thiz->vtable->destroy(thiz);
	}

	return;
}

FtkGc* ftk_canvas_get_gc(FtkCanvas* thiz);
Ret    ftk_canvas_reset_gc(FtkCanvas* thiz, FtkGc* gc);
Ret    ftk_canvas_set_gc(FtkCanvas* thiz, FtkGc* gc);
Ret    ftk_canvas_set_clip_rect(FtkCanvas* thiz, FtkRect* rect);
Ret    ftk_canvas_set_clip_region(FtkCanvas* thiz, FtkRegion* region);
Ret    ftk_canvas_draw_vline(FtkCanvas* thiz, int x, int y, int h);
Ret    ftk_canvas_draw_hline(FtkCanvas* thiz, int x, int y, int w);

Ret ftk_canvas_draw_bitmap_simple(FtkCanvas* thiz, FtkBitmap* b, 
	int x, int y, int w, int h, int ox, int oy);
Ret ftk_canvas_draw_bg_image(FtkCanvas* thiz, FtkBitmap* bitmap, 
	FtkBgStyle style, int x, int y, int w, int h);

const char* ftk_canvas_calc_str_visible_range(FtkCanvas* thiz, 
	const char* start, int vstart, int vend, int width, int* ret_extent);

Ret ftk_canvas_show(FtkCanvas* thiz, FtkDisplay* display, FtkRect* rect, int ox, int oy);

FtkCanvas* ftk_canvas_create(int w, int h, FtkColor* clear_color);

FTK_END_DECLS

#endif/*FTK_CANVAS_H*/

