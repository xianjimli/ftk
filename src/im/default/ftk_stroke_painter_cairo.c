/*
 * File: ftk_stroke_painter_cairo.c 
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   handwrite stroke painter base on cairo.
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
 * 2010-02-10 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_cairo.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_stroke_painter.h"

struct _FtkStrokePainter
{
	int visible;
	int line_width;
	FtkColor line_color;

	cairo_t* cr;
	FtkRect rect;
	FtkBitmap* bitmap;
};

static Ret on_display_update(void* ctx, FtkDisplay* display, int before,
	FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	FtkStrokePainter* thiz = ctx;
	if(before || thiz == NULL) return RET_OK;

	if(thiz->visible)
	{
		/*FIXME: need not update the whole region.*/
		ftk_stroke_painter_update(thiz, NULL);
	}

	return RET_OK;
}

FtkStrokePainter* ftk_stroke_painter_create(void)
{
	FtkStrokePainter* thiz = FTK_ZALLOC(sizeof(FtkStrokePainter));

	return thiz;
}

Ret ftk_stroke_painter_begin(FtkStrokePainter* thiz)
{
	cairo_t* cr = NULL;
	return_val_if_fail(thiz != NULL, RET_FAIL);

	cr = thiz->cr;

	if(cr != NULL)
	{
		double r = (double)thiz->line_color.r / 255.0;
		double g = (double)thiz->line_color.g / 255.0;
		double b = (double)thiz->line_color.b / 255.0;
		
		cairo_new_path (cr);
		cairo_set_source_rgba(thiz->cr, r, g, b, 1.0);
		cairo_set_line_width(thiz->cr, thiz->line_width);
	}

	return RET_OK;
}

Ret ftk_stroke_painter_end(FtkStrokePainter* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);	

	ftk_stroke_painter_clear(thiz);
	ftk_stroke_painter_show(thiz, 0);

	return RET_OK;
}

Ret ftk_stroke_painter_init(FtkStrokePainter* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);	

	if(thiz->bitmap == NULL || ftk_bitmap_width(thiz->bitmap) != thiz->rect.width
		|| ftk_bitmap_height(thiz->bitmap) != thiz->rect.height)
	{
		FtkColor bg = {0};
		cairo_surface_t* surface = NULL;
		
		if(thiz->bitmap != NULL)
		{
			ftk_bitmap_unref(thiz->bitmap);
		}
		
		thiz->bitmap = ftk_bitmap_create(thiz->rect.width, thiz->rect.height, bg);
		surface = ftk_cairo_surface_create_with_bitmap(thiz->bitmap);

		if(thiz->cr != NULL)
		{
			cairo_destroy(thiz->cr);
		}
		thiz->cr = cairo_create(surface);

		cairo_surface_destroy(surface);
	}
	ftk_display_reg_update_listener(ftk_default_display(), on_display_update, thiz);

	return RET_OK;
}

Ret ftk_stroke_painter_deinit(FtkStrokePainter* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);	
	
	if(thiz->cr != NULL)
	{
		cairo_destroy(thiz->cr);
		thiz->cr = NULL;
	}

	if(thiz->bitmap != NULL)
	{
		ftk_bitmap_unref(thiz->bitmap);
		thiz->bitmap = NULL;
	}
	ftk_display_unreg_update_listener(ftk_default_display(), on_display_update, thiz);

	return RET_OK;
}

Ret ftk_stroke_painter_clear(FtkStrokePainter* thiz)
{
	FtkColor* bits = NULL;
	return_val_if_fail(thiz != NULL && thiz->bitmap != NULL, RET_FAIL);	

	bits = ftk_bitmap_lock(thiz->bitmap);
	memset(bits, 0x00, sizeof(FtkColor) * ftk_bitmap_width(thiz->bitmap) * ftk_bitmap_height(thiz->bitmap));

	return RET_OK;
}

Ret ftk_stroke_painter_update(FtkStrokePainter* thiz, FtkRect* rect)
{
	int x = 0;
	int y = 0;
	FtkRect r = {0};
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(rect != NULL)
	{
		r = *rect;
		x = r.x + thiz->rect.x;
		y = r.y + thiz->rect.y;
	}
	else
	{
		x = thiz->rect.x;
		y = thiz->rect.y;
		r.width  = thiz->rect.width;
		r.height = thiz->rect.height;
	}

	if(thiz->visible)
	{
		ftk_display_update(ftk_default_display(), thiz->bitmap, &r, x, y);
	}

	return RET_OK;
}

Ret ftk_stroke_painter_move_to(FtkStrokePainter* thiz, int x, int y)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	cairo_move_to(thiz->cr, x, y);

	return RET_OK;
}

Ret ftk_stroke_painter_line_to(FtkStrokePainter* thiz, int x, int y)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	cairo_line_to(thiz->cr, x, y);
	cairo_stroke(thiz->cr);

	return RET_OK;
}

Ret ftk_stroke_painter_show(FtkStrokePainter* thiz, int visible)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(thiz->visible == visible)
	{
		return RET_OK;
	}

	thiz->visible = visible;
	if(thiz->visible)
	{
		ftk_stroke_painter_update(thiz, NULL);
	}
	else
	{
		ftk_wnd_manager_update(ftk_default_wnd_manager());
	}
	
	return RET_OK;
}

Ret ftk_stroke_painter_set_rect(FtkStrokePainter* thiz, FtkRect* rect)
{
	return_val_if_fail(thiz != NULL && rect != NULL, RET_FAIL);

	thiz->rect = *rect;

	return RET_OK;
}

Ret ftk_stroke_painter_set_pen_width(FtkStrokePainter* thiz, int width)
{
	return_val_if_fail(thiz != NULL && width > 0, RET_FAIL);

	thiz->line_width = width;

	return RET_OK;
}

Ret ftk_stroke_painter_set_pen_color(FtkStrokePainter* thiz, FtkColor color)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	thiz->line_color = color;

	return RET_OK;
}

void ftk_stroke_painter_destroy(FtkStrokePainter* thiz)
{
	if(thiz != NULL)
	{
		ftk_stroke_painter_deinit(thiz);

		FTK_ZFREE(thiz, sizeof(FtkStrokePainter));
	}

	return;
}

