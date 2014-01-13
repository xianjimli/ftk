/*
 * File: ftk_scroll_bar.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   scroll bar
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
 * 2009-11-20 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_window.h"
#include "ftk_globals.h"
#include "ftk_scroll_bar.h"

typedef struct _ScrollBarPrivInfo
{
	int tracker_pos;
	int tracker_size;

	int drag_enabled;
	int mouse_pressed;
	int last_mouse_pos;

	int value;
	int vertical;
	int max_value;
	int page_delta;
	FtkBitmap* bitmap;
	void* listener_ctx;
	FtkListener listener;
}PrivInfo;

static Ret ftk_scroll_bar_on_event(FtkWidget* thiz, FtkEvent* event)
{
	int x = 0;
	int y = 0;
	Ret ret = RET_OK;
	DECL_PRIV0(thiz, priv);

	if(priv->page_delta == priv->max_value)
	{
		/*one page, no scroll*/
		return RET_OK;
	}

	switch(event->type)
	{
		case FTK_EVT_KEY_DOWN:
		{
			if(event->u.key.code == FTK_KEY_DOWN)
			{
				ftk_scroll_bar_inc(thiz);
				ret = RET_REMOVE;
			}
			else if(event->u.key.code == FTK_KEY_UP)
			{
				ftk_scroll_bar_dec(thiz);
				ret = RET_REMOVE;
			}
			else if(event->u.key.code == FTK_KEY_PAGEDOWN)
			{
				ftk_scroll_bar_pagedown(thiz);
			}
			else if(event->u.key.code == FTK_KEY_PAGEUP)
			{
				ftk_scroll_bar_pageup(thiz);
			}
			break;
		}
		case FTK_EVT_MOUSE_DOWN:
		{
			int pos = 0;
			int value = 0;

			x = event->u.mouse.x;
			y = event->u.mouse.y;

			priv->mouse_pressed = 1;
			ftk_window_grab(ftk_widget_toplevel(thiz), thiz);

			pos = priv->vertical ? y : x;
			priv->drag_enabled = pos >= priv->tracker_pos && pos < (priv->tracker_pos + priv->tracker_size);

			if(!priv->drag_enabled)
			{
				value = pos < priv->tracker_pos ? priv->value - priv->page_delta : priv->value + priv->page_delta;
				ftk_scroll_bar_set_value(thiz, value);
			}
		
			priv->last_mouse_pos = pos;

			break;
		}
		case FTK_EVT_MOUSE_MOVE:
		{
			int pos = 0;
			int delta =0;
			int value = 0;
			int width = 0;
			int height = 0;
			
			if(!priv->drag_enabled) break;
			
			x = event->u.mouse.x;
			y = event->u.mouse.y;
			pos = priv->vertical ? y : x;
			delta = pos - priv->last_mouse_pos;

			width = ftk_widget_width(thiz);
			height = ftk_widget_height(thiz);

			value = priv->vertical ? priv->max_value * delta/height : priv->max_value * delta/width;

			if(value != 0)
			{
				value += priv->value;
				priv->last_mouse_pos = pos;
				ftk_scroll_bar_set_value(thiz, value);
			}

			break;
		}
		case FTK_EVT_MOUSE_UP:
		{
			priv->drag_enabled = 0;
			priv->mouse_pressed = 0;
			ftk_window_ungrab(ftk_widget_toplevel(thiz), thiz);

			break;
		}
		default:break;
	}

	return ret;
}

static Ret ftk_scroll_bar_on_paint(FtkWidget* thiz)
{
	int i = 0;
	int dx = 0;
	int dy = 0;
	int fill_size = 0;
	int half_size = 0;
	int bitmap_width = 0;
	int bitmap_height = 0;
	DECL_PRIV0(thiz, priv);
	FTK_BEGIN_PAINT(x, y, width, height, canvas);

	return_val_if_fail(priv->bitmap != NULL, RET_FAIL);

	bitmap_width = ftk_bitmap_width(priv->bitmap);
	bitmap_height = ftk_bitmap_height(priv->bitmap);
	
	if(priv->vertical)
	{
		priv->tracker_size = height * priv->page_delta/priv->max_value;
		priv->tracker_size = priv->tracker_size < bitmap_height ? bitmap_height : priv->tracker_size;

		dy = height * priv->value / priv->max_value;
		dy = (dy + priv->tracker_size) < height ? dy : height - priv->tracker_size;
	
		priv->tracker_pos = dy + ftk_widget_top_abs(thiz);
		fill_size = priv->tracker_size - bitmap_height;

		dx += x;
		dy += y;
		half_size = bitmap_height/2;
		
		ftk_canvas_draw_bitmap_simple(canvas, priv->bitmap, 0, 0, bitmap_width, half_size, dx, dy);
		for(i = 0; i < fill_size; i++)
		{
			ftk_canvas_draw_bitmap_simple(canvas, priv->bitmap, 0, half_size, bitmap_width, 1, dx, dy + i + half_size);
		}
		ftk_canvas_draw_bitmap_simple(canvas, priv->bitmap, 0, half_size, bitmap_width, half_size, dx, dy + half_size + fill_size);
	}
	else
	{
		priv->tracker_size = width * priv->page_delta/priv->max_value;
		priv->tracker_size = priv->tracker_size < bitmap_width ? bitmap_width : priv->tracker_size;

		dx = width * priv->value / priv->max_value;
		dx = (dx + priv->tracker_size) < width ? dx : width - priv->tracker_size;
	
		priv->tracker_pos = dx + ftk_widget_left_abs(thiz);
		fill_size = priv->tracker_size - bitmap_width;

		dy += y;
		dx += x;
		half_size = bitmap_width/2;
		
		ftk_canvas_draw_bitmap_simple(canvas, priv->bitmap, 0, 0, half_size, bitmap_height, dx, dy);
		for(i = 0; i < fill_size; i++)
		{
			ftk_canvas_draw_bitmap_simple(canvas, priv->bitmap, half_size, 0, 1, bitmap_height, dx + i + half_size, dy);
		}
		ftk_canvas_draw_bitmap_simple(canvas, priv->bitmap, half_size, 0, half_size, bitmap_height, dx + fill_size + half_size, dy);
	}

	FTK_END_PAINT();
}

static void ftk_scroll_bar_destroy(FtkWidget* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV0(thiz, priv);
		ftk_bitmap_unref(priv->bitmap);
		FTK_ZFREE(priv, sizeof(PrivInfo));
	}

	return;
}

FtkWidget* ftk_scroll_bar_create(FtkWidget* parent, int x, int y, int width, int height)
{
	FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	return_val_if_fail(thiz != NULL, NULL);

	thiz->priv_subclass[0] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV0(thiz, priv);
		thiz->on_event = ftk_scroll_bar_on_event;
		thiz->on_paint = ftk_scroll_bar_on_paint;
		thiz->destroy  = ftk_scroll_bar_destroy;

		if(width < height)
		{
			/*vertical*/
			priv->vertical = 1;
			priv->bitmap = ftk_theme_load_image(ftk_default_theme(), 
				"scrollbar_handle_vertical"FTK_STOCK_IMG_SUFFIX);
			width = ftk_bitmap_width(priv->bitmap);
			assert(width < height);
		}
		else
		{
			priv->vertical = 0;
			priv->bitmap = ftk_theme_load_image(ftk_default_theme(), 
				"scrollbar_handle_horizontal"FTK_STOCK_IMG_SUFFIX);
			height = ftk_bitmap_height(priv->bitmap);	
			assert(width > height);
		}

		ftk_widget_init(thiz, width < height ? FTK_SCROLL_VBAR : FTK_SCROLL_HBAR, 0, 
			x, y, width, height, FTK_ATTR_TRANSPARENT);
		ftk_widget_append_child(parent, thiz);
	}

	return thiz;
}

Ret ftk_scroll_bar_set_bitmap(FtkWidget* thiz, FtkBitmap* bitmap)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);

	ftk_bitmap_unref(priv->bitmap);
	priv->bitmap = bitmap;
	ftk_bitmap_ref(priv->bitmap);

	return RET_OK;
}

Ret ftk_scroll_bar_set_param(FtkWidget* thiz, int value, int max_value, int page_delta)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);
	return_val_if_fail(value <= max_value && page_delta <= max_value, RET_FAIL);
	return_val_if_fail(max_value > 0 && page_delta > 0, RET_FAIL);

	if(priv->value == value 
		&& priv->max_value == max_value 
		&& priv->page_delta == page_delta)
	{
		return RET_OK;
	}

	priv->value      = value < 0 ? 0 : value;
	priv->max_value  = max_value;
	priv->page_delta = page_delta;
	ftk_widget_invalidate(thiz);	

	return RET_OK;
}

Ret ftk_scroll_bar_set_listener(FtkWidget* thiz, FtkListener listener, void* ctx)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);

	priv->listener = listener;
	priv->listener_ctx = ctx;

	return RET_OK;
}

int ftk_scroll_bar_get_value(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL, 0);

	return priv->value;
}

int ftk_scroll_bar_get_max_value(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL, 0);

	return priv->max_value;
}

Ret ftk_scroll_bar_inc(FtkWidget* thiz)
{
	int value = 0;
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);
	value = priv->value + 1;

	return ftk_scroll_bar_set_value(thiz, value);
}

Ret ftk_scroll_bar_dec(FtkWidget* thiz)
{
	int value = 0;
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);
	value = priv->value - 1;
	
	return ftk_scroll_bar_set_value(thiz, value);
}

Ret ftk_scroll_bar_pageup(FtkWidget* thiz)
{
	int value = 0;
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);
	value = priv->value - priv->page_delta;
	
	return ftk_scroll_bar_set_value(thiz, value);
}

Ret ftk_scroll_bar_pagedown(FtkWidget* thiz)
{
	int value = 0;
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);
	value = priv->value + priv->page_delta;
	
	return ftk_scroll_bar_set_value(thiz, value);
}

Ret ftk_scroll_bar_set_value(FtkWidget* thiz, int value)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);
	value = value < 0 ? 0 : value;
	value = (value + priv->page_delta) < priv->max_value ? value : priv->max_value - priv->page_delta;
	
	if(value != priv->value)
	{
		priv->value = value;
		ftk_widget_invalidate(thiz);	
		return FTK_CALL_LISTENER(priv->listener, priv->listener_ctx, thiz);
	}

	return RET_OK;
}

