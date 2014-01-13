/*
 * File: ftk_canvas_win32.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   win32 canvas implemntation.
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

#ifdef VC6
#define ULONG_PTR unsigned long*
#endif 

#include <windows.h>
#include <gdiplus.h>
#include "ftk_win32.h"

typedef struct _CanvasWin32PrivInfo
{
	int w;
	int h;
	Pen* pen;
	Graphics* grapics;
	FtkBitmap* bitmap;
}PrivInfo;

static Ret ftk_canvas_win32_sync_gc(FtkCanvas* thiz)
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

static Ret ftk_canvas_win32_set_clip(FtkCanvas* thiz, FtkRegion* clip)
{
	return RET_OK;
}

static Ret ftk_canvas_win32_draw_pixels(FtkCanvas* thiz, FtkPoint* points, int nr)
{
	DECL_PRIV(thiz, priv);

	return RET_OK;
}

static Ret ftk_canvas_win32_draw_line(FtkCanvas* thiz, int x1, int y1, int x2, int y2)
{
	DECL_PRIV(thiz, priv);

	return RET_OK;
}

static Ret ftk_canvas_win32_clear_rect(FtkCanvas* thiz, int x, int y, int w, int h)
{
	DECL_PRIV(thiz, priv);

	priv->grapics->DrawLine(priv->pen, x, y, x + w, y + h);

	return RET_OK;
}

static Ret ftk_canvas_win32_draw_rect(FtkCanvas* thiz, int x, int y, int w, int h, 
	int round, int fill)
{
	DECL_PRIV(thiz, priv);

	if(fill)
	{
		Rect r(x, y, w, h);
		FtkColor c = thiz->gc.fg;
		SolidBrush brush(Color(c.a, c.r, c.g, c.b));
		
		if(round)
		{
			priv->grapics->FillRectangle(&brush, r);
		}
		else
		{
			priv->grapics->FillRectangle(&brush, r);
		}
	}
	else
	{
		if(round)
		{
			priv->grapics->DrawRectangle(priv->pen, x, y, w, h);
		}
		else
		{
			priv->grapics->DrawRectangle(priv->pen, x, y, w, h);
		}
	}
	return RET_OK;
}

static Ret ftk_canvas_win32_draw_bitmap(FtkCanvas* thiz, FtkBitmap* bitmap, 
	FtkRect* src_r, FtkRect* dst_r, int alpha)
{
	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);

	return ret;
}

static Ret ftk_canvas_win32_draw_string(FtkCanvas* thiz, int x, int y, 
	const char* str, int len, int vcenter)
{
	size_t size = 0;
	WCHAR* utf16 = NULL;
	DECL_PRIV(thiz, priv);
	
	FtkColor c = thiz->gc->fg;
	Font font(FontFamily::GenericSerif(),
		ftk_font_desc_get_size(thiz->gc.font));
	SolidBrush brush(Color(c.a, c.r, c.g, c.b));

	utf16 = new WCHAR[len+1];
	if((size = MultiByteToWideChar(CP_ACP, 0, str, len, utf16, len * sizeof(WCHAR))) > 0)
	{
		priv->grapics->DrawString(utf16, size, &font, Point(x, y), &brush);
	}
	delete utf16;
	return RET_OK;
}

static Ret fk_canvas_win32_lock_buffer(FtkCanvas* thiz, FtkBitmap** bitmap)
{
	DECL_PRIV(thiz, priv);

	return RET_OK;
}

static Ret ftk_canvas_win32_unlock_buffer(FtkCanvas* thiz)
{
	return RET_OK;
}

static void ftk_canvas_win32_destroy(FtkCanvas* thiz)
{
	DECL_PRIV(thiz, priv);

	if(thiz != NULL)
	{
		FTK_FREE(thiz);
	}

	return;
}

static int ftk_canvas_win32_get_char_extent(FtkCanvas* thiz, unsigned short code)
{
	return 0;
}

static int ftk_canvas_win32_get_str_extent(FtkCanvas* thiz, const char* str, int len)
{
	return 0;
}

static const FtkCanvasVTable g_canvas_win32_vtable=
{
	ftk_canvas_win32_sync_gc,
	ftk_canvas_win32_set_clip,
	ftk_canvas_win32_draw_pixels,
	ftk_canvas_win32_draw_line,
	ftk_canvas_win32_clear_rect,
	ftk_canvas_win32_draw_rect,

	ftk_canvas_win32_draw_bitmap,
	ftk_canvas_win32_draw_string,
	ftk_canvas_win32_get_str_extent,
	ftk_canvas_win32_get_char_extent,
	fk_canvas_win32_lock_buffer,
	ftk_canvas_win32_unlock_buffer,
	ftk_canvas_win32_destroy
};

FtkCanvas* ftk_canvas_create(int w, int h, FtkColor* clear_color)
{
	FtkCanvas* thiz = NULL;
	return_val_if_fail(w > 0 && h > 0 && clear_color != NULL, NULL);

	thiz = (FtkCanvas*)FTK_ZALLOC(sizeof(FtkCanvas) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		Color c(0xff, 0, 0, 0);
		Bitmap* bitmap = NULL;
		DECL_PRIV(thiz, priv);

		thiz->gc.bg = *clear_color;
		thiz->gc.fg.a = 0xff;
		thiz->gc.fg.r = 0xff - clear_color->r;
		thiz->gc.fg.g = 0xff - clear_color->g;
		thiz->gc.fg.b = 0xff - clear_color->b;
		thiz->gc.mask = FTK_GC_FG | FTK_GC_BG;
		thiz->vtable = &g_canvas_win32_vtable;

		priv->pen = new Pen(c, 1.0);
		priv->bitmap = ftk_bitmap_create(w, h, *clear_color);
		priv->grapics = new Graphics((Bitmap*)ftk_bitmap_get_native(priv->bitmap));
	}

	return thiz;
}

