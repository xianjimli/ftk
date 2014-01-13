/*
 * File: ftk_progress_bar.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   progress bar
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
 * 2009-11-15 Li XianJing <xianjimli@hotmail.com> created
 * 2010-09-22 Li XianJing <xianjimli@hotmail.com> add interactive feature.
 *
 */

#include "ftk_window.h"
#include "ftk_globals.h"
#include "ftk_text_layout.h"
#include "ftk_progress_bar.h"

typedef struct _ProgressBarPrivInfo
{
	int percent;
	int interactive; 
	int mouse_pressed;
	FtkBitmap* bg;
	FtkBitmap* cursor;
}PrivInfo;

static Ret ftk_progress_bar_on_event(FtkWidget* thiz, FtkEvent* event)
{
	return RET_OK;
}

static Ret ftk_progress_bar_on_paint(FtkWidget* thiz)
{
	int fg_width = 0;
	DECL_PRIV0(thiz, priv);
	FtkGc gc = {0};
	FTK_BEGIN_PAINT(x, y, width, height, canvas);
	return_val_if_fail(width > 4 && height > 4, RET_FAIL);

	gc.mask = FTK_GC_FG;
	gc.fg = ftk_widget_get_gc(thiz)->bg;
	ftk_canvas_set_gc(canvas, &gc);
	ftk_canvas_draw_rect(canvas, x, y, width, height, 1, 1);
	
	gc.fg = ftk_widget_get_gc(thiz)->fg;
	ftk_canvas_set_gc(canvas, &gc);
	fg_width = width * priv->percent/100;

	if(fg_width >= 10)
	{
		ftk_canvas_draw_rect(canvas, x+1, y+1, fg_width-2, height-2, 1, 1);
		if(fg_width < width && fg_width > 2)
		{
			ftk_canvas_clear_rect(canvas, x + fg_width - 3, y+1, 3, height-2);
		}
	}
	
	ftk_canvas_reset_gc(canvas, ftk_widget_get_gc(thiz)); 
	if(ftk_widget_get_text(thiz) != NULL)
	{
		int xoffset = 0;
		int yoffset = FTK_HALF(height);
		FtkTextLine line = {0};
		const char* text = ftk_widget_get_text(thiz);
		FtkTextLayout* text_layout = ftk_default_text_layout();

		gc.fg.a = 0xff;
		gc.fg.r = 0x00;
		gc.fg.g = 0x00;
		gc.fg.b = 0x00;
		ftk_canvas_set_gc(canvas, &gc);
		ftk_text_layout_init(text_layout, text, -1, canvas, width); 
		if(ftk_text_layout_get_visual_line(text_layout, &line) == RET_OK)
		{
			xoffset = FTK_HALF(width - line.extent); 
			ftk_canvas_draw_string(canvas, x + xoffset, y + yoffset, text, -1, 1);
		}
	}

	FTK_END_PAINT();
}

static Ret ftk_progress_bar_on_event_interactive(FtkWidget* thiz, FtkEvent* event)
{
	DECL_PRIV0(thiz, priv);
	int percent = priv->percent;
	return_val_if_fail(thiz != NULL && event != NULL, RET_FAIL);
	
	if(event->type == FTK_EVT_KEY_UP)
	{
		switch(event->u.key.code)
		{
			case FTK_KEY_HOME:
			{
				percent  = 0;
				break;
			}
			case FTK_KEY_END:
			{
				percent  = 100;
				break;
			}
			case FTK_KEY_PAGEUP:
			{
				percent += 10;
				break;
			}
			case FTK_KEY_PAGEDOWN:
			{
				percent -= 10;
				break;
			}
			case FTK_KEY_LEFT:
			case FTK_KEY_DOWN:
			{
				percent--;
				break;
			}
			case FTK_KEY_UP:
			case FTK_KEY_RIGHT:
			{
				percent++;
				break;
			}
			default:break;
		}
	}
	else if(event->type == FTK_EVT_MOUSE_DOWN)
	{
		priv->mouse_pressed = 1;
		ftk_window_grab(ftk_widget_toplevel(thiz), thiz);
	}
	else if(event->type == FTK_EVT_MOUSE_MOVE && priv->mouse_pressed)
	{
		int width = ftk_widget_width(thiz);
		int offset = event->u.mouse.x - ftk_widget_left_abs(thiz);
		
		percent = 100 * offset / width;
	}
	else if(event->type == FTK_EVT_MOUSE_UP)
	{
		int width = ftk_widget_width(thiz);
		int offset = event->u.mouse.x - ftk_widget_left_abs(thiz);
		
		priv->mouse_pressed = 0;
		percent = 100 * offset / width;
		ftk_window_ungrab(ftk_widget_toplevel(thiz), thiz);
	}
	else
	{
		return RET_OK;
	}

	ftk_progress_bar_set_percent(thiz, percent);

	return RET_OK;
}

static Ret ftk_progress_bar_on_paint_interactive(FtkWidget* thiz)
{
	int ox = 0;
	int oy = 0;
	DECL_PRIV0(thiz, priv);
	FTK_BEGIN_PAINT(x, y, width, height, canvas);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	oy = y + FTK_HALF(height - ftk_bitmap_height(priv->bg));
	ftk_canvas_draw_bg_image(canvas, priv->bg, FTK_BG_FOUR_CORNER, x, oy, width,
		ftk_bitmap_height(priv->bg));

	ox = x + width * priv->percent / 100 - FTK_HALF(ftk_bitmap_width(priv->cursor));
	oy = y + FTK_HALF(height) - FTK_HALF(ftk_bitmap_height(priv->cursor));

	if(ox < x)
	{
		ox = x;
	}

	if(ox > (x + width - ftk_bitmap_width(priv->cursor)))
	{
		ox = x + width - ftk_bitmap_width(priv->cursor);
	}

	ftk_canvas_draw_bitmap_simple(canvas, priv->cursor, 0, 0, ftk_bitmap_width(priv->cursor), 
		ftk_bitmap_height(priv->cursor), ox, oy);
	
	FTK_END_PAINT();
}

static void ftk_progress_bar_destroy(FtkWidget* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV0(thiz, priv);

		FTK_BITMAP_UNREF(priv->bg);
		FTK_BITMAP_UNREF(priv->cursor);

		FTK_ZFREE(priv, sizeof(PrivInfo));
	}

	return;
}

FtkWidget* ftk_progress_bar_create(FtkWidget* parent, int x, int y, int width, int height)
{
	FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	return_val_if_fail(thiz != NULL, NULL);

	thiz->priv_subclass[0] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[0] != NULL)
	{
		thiz->on_event = ftk_progress_bar_on_event;
		thiz->on_paint = ftk_progress_bar_on_paint;
		thiz->destroy  = ftk_progress_bar_destroy;

		ftk_widget_init(thiz, FTK_PROGRESS_BAR, 0, x, y, width, height, 
			FTK_ATTR_TRANSPARENT|FTK_ATTR_INSENSITIVE);
		ftk_widget_append_child(parent, thiz);
	}
	else
	{
		FTK_FREE(thiz);
	}

	return thiz;
}

Ret ftk_progress_bar_set_percent(FtkWidget* thiz, int percent)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	percent = percent < 0 ? 0 : percent;
	percent = percent > 100 ? 100 : percent;
	if(percent != priv->percent)
	{
		priv->percent = percent;
		ftk_widget_invalidate(thiz);
	}

	return RET_OK;
}

int ftk_progress_bar_get_percent(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL, 0);

	return priv->percent;
}

Ret ftk_progress_bar_set_interactive(FtkWidget* thiz, int interactive)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	priv->interactive = interactive;
	if(interactive)
	{
		if(priv->bg == NULL)
		{
			priv->bg = ftk_theme_load_image(ftk_default_theme(), 
				"progressbar_i_bg"FTK_STOCK_IMG_SUFFIX); 
		}

		if(priv->cursor == NULL)
		{
			priv->cursor = ftk_theme_load_image(ftk_default_theme(), 
				"progressbar_i_cursor"FTK_STOCK_IMG_SUFFIX); 
		}

		thiz->on_event = ftk_progress_bar_on_event_interactive;
		thiz->on_paint = ftk_progress_bar_on_paint_interactive;
	}
	else
	{
		thiz->on_event = ftk_progress_bar_on_event;
		thiz->on_paint = ftk_progress_bar_on_paint;
	}
	ftk_widget_set_insensitive(thiz, !interactive);

	return RET_OK;
}

