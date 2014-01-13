/*
 * File: ftk_canvas_skia.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   skia canvas implemntation.
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
#include "SkColor.h"
#include "SkCanvas.h"
#include "ftk_globals.h"
#include "SkImageEncoder.h"
#include "effects/SkPorterDuff.h"

using namespace android;

typedef struct _CanvasSkiaPrivInfo
{
	int w;
	int h;
	SkPaint*   paint;
	SkCanvas*  canvas;
	FtkBitmap* bitmap;
}PrivInfo;

static Ret ftk_canvas_skia_sync_gc(FtkCanvas* thiz)
{
	int size = 0;
	DECL_PRIV(thiz, priv);
	FtkColor c = thiz->gc.fg;
#ifdef ANDROID
	SkColor color = SkColorSetARGB(c.a, c.r, c.g, c.b);
#else
	SkColor color = SkColorSetARGBInline(c.a, c.r, c.g, c.b);
#endif

	priv->paint->setColor(color);
	if(thiz->gc.font != NULL)
	{
		char desc[64] = {0};
		size = ftk_font_desc_get_size(thiz->gc.font);
		priv->paint->setTextSize(SkIntToScalar(size));
	}

	return RET_OK;
}

static Ret ftk_canvas_skia_set_clip(FtkCanvas* thiz, FtkRegion* clip)
{
	SkRegion clipReg;
	DECL_PRIV(thiz, priv);

	if(clip != NULL)
	{
		SkIRect rect;
		FtkRegion* iter = clip;

		while(iter != NULL)
		{
			rect.set(iter->rect.x, iter->rect.y, iter->rect.x + iter->rect.width, iter->rect.y + iter->rect.height);
			clipReg.op(rect, SkRegion::kUnion_Op);
			ftk_logd("set_clip: %d %d %d %d\n", iter->rect.x, iter->rect.y, iter->rect.width, iter->rect.height);
			iter = iter->next;
		}
	}
	else
	{
		clipReg.setRect(0, 0, priv->w, priv->h);
		ftk_logd("set_clip: %d %d %d %d\n", 0, 0, priv->w, priv->h);
	}

	priv->canvas->clipRegion(clipReg, SkRegion::kReplace_Op);

	return RET_OK;
}

static Ret ftk_canvas_skia_draw_pixels(FtkCanvas* thiz, FtkPoint* points, int nr)
{
	int i = 0;
	DECL_PRIV(thiz, priv);

	for(i = 0; i < nr; i++)
	{
		priv->canvas->drawPoint(SkIntToScalar(points[i].x), SkIntToScalar(points[i].y), *priv->paint);
	}

	return RET_OK;
}

static Ret ftk_canvas_skia_draw_line(FtkCanvas* thiz, int x1, int y1, int x2, int y2)
{
	DECL_PRIV(thiz, priv);

	priv->canvas->drawLine(SkIntToScalar(x1), SkIntToScalar(y1), SkIntToScalar(x2), SkIntToScalar(y2), *priv->paint);

	return RET_OK;
}

static Ret ftk_canvas_skia_clear_rect(FtkCanvas* thiz, int x, int y, int w, int h)
{
	SkRect rect;
	DECL_PRIV(thiz, priv);
	
	rect.iset(x, y, x + w, y + h);
	priv->paint->setStyle(SkPaint::kStrokeAndFill_Style);
	priv->canvas->drawRect(rect, *priv->paint);

	return RET_OK;
}

static Ret ftk_canvas_skia_draw_rect(FtkCanvas* thiz, int x, int y, int w, int h, 
	int round, int fill)
{
	SkRect rect;
	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);

	rect.iset(x, y, x + w, y + h);

	if(fill)
	{
		priv->paint->setStyle(SkPaint::kStrokeAndFill_Style);
	}
	else
	{
		priv->paint->setStyle(SkPaint::kStroke_Style);
	}

	if(round)
	{
		priv->canvas->drawRoundRect(rect, SkIntToScalar(5), SkIntToScalar(5), *priv->paint);
	}
	else
	{
		priv->canvas->drawRect(rect, *priv->paint);
	}
	priv->paint->setStyle(SkPaint::kStroke_Style);

	return ret;
}

static Ret ftk_canvas_skia_draw_bitmap(FtkCanvas* thiz, FtkBitmap* bitmap, 
	FtkRect* src_r, FtkRect* dst_r, int alpha)
{
	SkRect dst;
	SkIRect src;
	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && bitmap != NULL && dst_r != NULL && src_r != NULL, RET_FAIL);

	src.set(src_r->x, src_r->y, src_r->x + src_r->width, src_r->y + src_r->height);
	dst.iset(dst_r->x, dst_r->y, dst_r->x + dst_r->width, dst_r->y + dst_r->height);

	SkBitmap* b = (SkBitmap*)ftk_bitmap_get_native(bitmap);
	priv->canvas->drawBitmapRect(*b, &src, dst, priv->paint);

	return ret;
}

static Ret ftk_canvas_skia_draw_string(FtkCanvas* thiz, int x, int y, 
	const char* str, int len, int vcenter)
{
	DECL_PRIV(thiz, priv);

	if(vcenter)
	{
		y += ftk_font_desc_get_size(thiz->gc.font)/3;
	}

	priv->paint->setAntiAlias(true);
	priv->canvas->drawText(str, len, SkIntToScalar(x), SkIntToScalar(y), *priv->paint);
	priv->paint->setAntiAlias(false);

	return RET_OK;
}

static Ret fk_canvas_skia_lock_buffer(FtkCanvas* thiz, FtkBitmap** bitmap)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(bitmap != NULL, RET_FAIL);

	*bitmap = priv->bitmap;

	return RET_OK;
}

static Ret ftk_canvas_skia_unlock_buffer(FtkCanvas* thiz)
{
	return RET_OK;
}

static void ftk_canvas_skia_destroy(FtkCanvas* thiz)
{
	DECL_PRIV(thiz, priv);

	if(thiz != NULL)
	{
		priv->canvas->setBitmapDevice(SkBitmap());
		ftk_bitmap_unref(priv->bitmap);
		delete priv->canvas;
		delete priv->paint;
		FTK_FREE(thiz);
	}

	return;
}

static int ftk_canvas_skia_get_char_extent(FtkCanvas* thiz, unsigned short code)
{
	int width = 0;
	char utf8[8] = {0};
	DECL_PRIV(thiz, priv);
	SkScalar fwidth = 0;

	utf16_to_utf8(&code, 1, utf8, sizeof(utf8));
	fwidth = priv->paint->measureText(utf8, strlen(utf8));
	width = SkFixedFloor(SkScalarToFixed(fwidth));

	return width;
}

static int ftk_canvas_skia_get_str_extent(FtkCanvas* thiz, const char* str, int len)
{
	int width = 0;
	DECL_PRIV(thiz, priv);
	SkScalar fwidth = 0;
	
	fwidth = priv->paint->measureText(str, len);
	width = SkFixedFloor(SkScalarToFixed(fwidth));
	ftk_logd("%s: %s width=%d\n", __func__, str, width);

	return width;
}

static const FtkCanvasVTable g_canvas_skia_vtable=
{
	ftk_canvas_skia_sync_gc,
	ftk_canvas_skia_set_clip,
	ftk_canvas_skia_draw_pixels,
	ftk_canvas_skia_draw_line,
	ftk_canvas_skia_clear_rect,
	ftk_canvas_skia_draw_rect,

	ftk_canvas_skia_draw_bitmap,
	ftk_canvas_skia_draw_string,
	ftk_canvas_skia_get_str_extent,
	ftk_canvas_skia_get_char_extent,
	fk_canvas_skia_lock_buffer,
	ftk_canvas_skia_unlock_buffer,
	ftk_canvas_skia_destroy
};

static void destroy_bitmap(void* data)
{
    SkBitmap* b = (SkBitmap*)data;

    delete b;

    return;
}

FtkCanvas* ftk_canvas_create(int w, int h, FtkColor* clear_color)
{
	FtkCanvas* thiz = NULL;
	return_val_if_fail(w > 0 && h > 0 && clear_color != NULL, NULL);

	thiz = (FtkCanvas*)FTK_ZALLOC(sizeof(FtkCanvas) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		SkBitmap* bitmap = new SkBitmap();
		DECL_PRIV(thiz, priv);

		thiz->gc.bg = *clear_color;
		thiz->gc.fg.a = 0xff;
		thiz->gc.fg.r = 0xff - clear_color->r;
		thiz->gc.fg.g = 0xff - clear_color->g;
		thiz->gc.fg.b = 0xff - clear_color->b;
		thiz->gc.mask = FTK_GC_FG | FTK_GC_BG;
		thiz->vtable = &g_canvas_skia_vtable;

		bitmap->setConfig(SkBitmap::kARGB_8888_Config, w, h, w * 4);
		bitmap->allocPixels(NULL);

		priv->paint = new SkPaint();
		priv->canvas = new SkCanvas();
		priv->canvas->setBitmapDevice(*bitmap);
		
		priv->bitmap = ftk_bitmap_create_with_native(bitmap);
		priv->w = ftk_bitmap_width(priv->bitmap);
		priv->h = ftk_bitmap_height(priv->bitmap);
	}

	return thiz;
}

