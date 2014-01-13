/*
 * File: ftk_dialog.c
 * Author: Li XianJing <xianjimli@hotmail.com>
 * Brief:  dialog
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
 * 2009-10-15 Li XianJing <xianjimli@hotmail.com> created
 * 2012-5-3  woodysu@gmail.com modified
 */

#include "ftk_dialog.h"
#include "ftk_globals.h"
#include "ftk_main_loop.h"
#include "ftk_source_timer.h"

typedef struct _DialogPrivInfo
{
	int no_title;
	int title_height;
	FtkBitmap* icon;
	FtkBitmap* bg;
	FtkBitmap* title_bg;
	FtkSource* timer;
	FtkMainLoop* main_loop;
	FtkWidgetOnEvent parent_on_event;
	FtkWidgetOnPaint parent_on_paint;
	FtkWidgetDestroy parent_destroy;
}PrivInfo;

static int ftk_dialog_is_modal(FtkWidget* thiz)
{
	DECL_PRIV1(thiz, priv);

	return priv != NULL && priv->main_loop != NULL;
}

Ret ftk_dialog_set_icon(FtkWidget* thiz, FtkBitmap* icon)
{
	DECL_PRIV1(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(priv->icon != NULL)
	{
		ftk_bitmap_unref(priv->icon);
	}
	priv->icon = icon;
	if(priv->icon != NULL)
	{
		ftk_bitmap_ref(priv->icon);
	}

	return RET_OK;	
}

Ret ftk_dialog_hide_title(FtkWidget* thiz)
{
	DECL_PRIV1(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	priv->no_title = 1;

	return RET_OK;
}

static Ret  ftk_dialog_on_event(FtkWidget* thiz, FtkEvent* event)
{
	DECL_PRIV1(thiz, priv);
	Ret ret = priv->parent_on_event(thiz, event);

	if(ret == RET_QUIT && ftk_dialog_is_modal(thiz))
	{
		ftk_main_loop_quit(priv->main_loop);
	}

	switch(event->type)
	{
		case FTK_EVT_ADD_CHILD:
		{
			int x = 0;
			int y = 0;
			FtkWidget* child = (FtkWidget*)event->u.extra;
			x = ftk_widget_left(child) + FTK_DIALOG_BORDER;
			y = ftk_widget_top(child) + (priv->no_title ? FTK_DIALOG_BORDER : priv->title_height);

			ftk_widget_move(child, x, y);

			break;
		}
		case FTK_EVT_MOUSE_UP:
		{
			int p_x = event->u.mouse.x;
			int p_y = event->u.mouse.y;

			int x = ftk_widget_left_abs(thiz);
			int y = ftk_widget_top_abs(thiz);
			int w = ftk_widget_width(thiz);
			int h = ftk_widget_height(thiz);

			if(p_x < x || p_y < y || (p_x > (x + w)) || (p_y > (y + h)))
			{
				if(thiz->ref > 1 && ftk_widget_has_attr(thiz, FTK_ATTR_POPUP))
				{
					if(ftk_dialog_is_modal(thiz))
					{
						ftk_dialog_quit(thiz);
					}
					else
					{
						ftk_widget_unref(thiz);
					}
				}
			}
			break;
		}
		default:break;
	}

	return RET_OK;
}

static Ret  ftk_dialog_clear_alpha(FtkCanvas* canvas, int x, int y)
{
	return RET_OK;
}

static Ret  ftk_dialog_change_alpha(FtkWidget* thiz, FtkCanvas* canvas, int x, int y, int width, int height)
{
	ftk_dialog_clear_alpha(canvas, x, y);
	ftk_dialog_clear_alpha(canvas, x+1, y);
	ftk_dialog_clear_alpha(canvas, x, y+1);
	
	ftk_dialog_clear_alpha(canvas, x+width-1, y)  ;
	ftk_dialog_clear_alpha(canvas, x+width-2, y)  ;
	ftk_dialog_clear_alpha(canvas, x+width-1, y+1);
	
	ftk_dialog_clear_alpha(canvas, x, y+height-1)  ;
	ftk_dialog_clear_alpha(canvas, x+1, y+height-1);
	ftk_dialog_clear_alpha(canvas, x, y+height-2)  ;
	
	ftk_dialog_clear_alpha(canvas, x+width-1, y+height-1);
	ftk_dialog_clear_alpha(canvas, x+width-2, y+height-1);
	ftk_dialog_clear_alpha(canvas, x+width-1, y+height-2);

	return RET_OK;
}

static Ret  ftk_dialog_paint_border(FtkWidget* thiz, FtkCanvas* canvas, int x, int y, int width, int height)
{
	DECL_PRIV1(thiz, priv);
	return_val_if_fail(priv->bg != NULL && priv->title_bg != NULL, RET_FAIL);

	if(!ftk_window_is_opaque(thiz))
	{
		return RET_OK;
	}
	
	if(priv->no_title)
	{
		ftk_canvas_draw_bg_image(canvas, priv->bg, FTK_BG_FOUR_CORNER, x, y, width, height);	
	}
	else
	{
		int title_height = ftk_bitmap_height(priv->title_bg);
		
		return_val_if_fail(title_height < height, RET_FAIL);
		ftk_canvas_draw_bg_image(canvas, priv->title_bg, FTK_BG_TILE, x, y, width, title_height);
		ftk_canvas_draw_bg_image(canvas, priv->bg, FTK_BG_FOUR_CORNER, x, y+title_height, width, height-title_height);	
	}

	return RET_OK;
}

static Ret  ftk_dialog_paint_title(FtkWidget* thiz, FtkCanvas* canvas, int x, int y, int width, int height)
{
	DECL_PRIV1(thiz, priv);

	if(priv->no_title) return RET_OK;

	if(priv->icon != NULL)
	{
		ftk_canvas_draw_bg_image(canvas, priv->icon, FTK_BG_CENTER, x + FTK_DIALOG_BORDER, y, 
			priv->title_height, priv->title_height);
	}

	ftk_canvas_set_gc(canvas, ftk_widget_get_gc(thiz));
	if(ftk_widget_get_text(thiz) != NULL)
	{
		int xoffset = FTK_DIALOG_BORDER + (priv->icon != NULL ? priv->title_height : FTK_DIALOG_BORDER);
		int yoffset = priv->title_height >> 1;
		const char* text = ftk_widget_get_text(thiz);
		const char*	end = ftk_canvas_calc_str_visible_range(canvas, text, 0, -1, 
			width - xoffset - FTK_DIALOG_BORDER, NULL);

		ftk_canvas_draw_string(canvas, x + xoffset, y + yoffset, text, end - text, 1);
	}

	return RET_OK;
}

static Ret  ftk_dialog_on_paint(FtkWidget* thiz)
{
	DECL_PRIV1(thiz, priv);
	FTK_BEGIN_PAINT(x, y, width, height, canvas);
	
	return_val_if_fail(ftk_widget_is_visible(thiz), RET_FAIL);
	
	ftk_dialog_paint_border(thiz, canvas, x, y, width, height);

	ftk_dialog_paint_title(thiz, canvas, x, y, width, height);

	ftk_dialog_change_alpha(thiz, canvas, x, y, width, height);
	
	priv->parent_on_paint(thiz);

	return RET_OK;
}

static void ftk_dialog_destroy(FtkWidget* thiz)
{
	DECL_PRIV1(thiz, priv);

	if(priv->parent_destroy != NULL)
	{
		priv->parent_destroy(thiz);
	}

	if(priv->icon != NULL)
	{
		ftk_bitmap_unref(priv->icon);
	}

	if(priv->bg != NULL)
	{
		ftk_bitmap_unref(priv->bg);
	}
	
	if(priv->title_bg != NULL)
	{
		ftk_bitmap_unref(priv->title_bg);
	}

	if(priv->timer != NULL)
	{
		ftk_main_loop_remove_source(ftk_default_main_loop(), priv->timer);
	}

	FTK_ZFREE(thiz->priv_subclass[1], sizeof(PrivInfo));

	return;
}

FtkWidget* ftk_dialog_create_ex(int attr, int x, int y, int width, int height)
{
	FtkWidget* thiz = ftk_window_create(FTK_DIALOG, attr, x, y, width, height);
	return_val_if_fail(thiz != NULL, NULL);

	thiz->priv_subclass[1] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[1] != NULL)
	{
		DECL_PRIV1(thiz, priv);
		priv->parent_on_event = thiz->on_event;
		priv->parent_on_paint = thiz->on_paint;
		priv->parent_destroy  = thiz->destroy;
		thiz->on_event = ftk_dialog_on_event;
		thiz->on_paint = ftk_dialog_on_paint;
		thiz->destroy  = ftk_dialog_destroy;

		priv->bg = ftk_theme_load_image(ftk_default_theme(), "dialog_bg"FTK_STOCK_IMG_SUFFIX);
		priv->title_bg = ftk_theme_load_image(ftk_default_theme(), "dialog_title_bg"FTK_STOCK_IMG_SUFFIX);
		priv->title_height = ftk_bitmap_height(priv->title_bg);
	}
	else
	{
		ftk_widget_destroy(thiz);
		thiz = NULL;
	}

	return thiz;
}

Ret ftk_dialog_set_bg(FtkWidget* thiz, FtkBitmap* bitmap)
{
    DECL_PRIV1(thiz, priv);
    return_val_if_fail(priv != NULL, RET_FAIL);

    ftk_bitmap_unref(priv->bg);
    priv->bg = bitmap;
    ftk_bitmap_ref(priv->bg);
    return RET_OK;
}

FtkWidget* ftk_dialog_create(int x, int y, int width, int height)
{
	return ftk_dialog_create_ex(FTK_ATTR_AUTO_LAYOUT, x, y, width, height);
}

Ret ftk_dialog_quit(FtkWidget* thiz)
{
	DECL_PRIV1(thiz, priv);
	return_val_if_fail(ftk_dialog_is_modal(thiz), RET_FAIL);

	ftk_main_loop_quit(priv->main_loop);

	return RET_OK;
}

static Ret ftk_dialog_timeout_quit(void* ctx)
{
	FtkWidget* thiz = (FtkWidget*)ctx;
	DECL_PRIV1(thiz, priv);
	return_val_if_fail(priv != NULL && priv->timer != NULL, RET_REMOVE);

	priv->timer = NULL;
	if(ftk_dialog_is_modal(thiz))
	{
		ftk_dialog_quit(thiz);
	}
	else
	{
		ftk_widget_unref(thiz);
	}

	return RET_REMOVE;
}

Ret ftk_dialog_quit_after(FtkWidget* thiz, int ms)
{
	DECL_PRIV1(thiz, priv);
	return_val_if_fail(priv != NULL && priv->timer == NULL, RET_FAIL);
	
	priv->timer = ftk_source_timer_create(ms, ftk_dialog_timeout_quit, thiz);
	ftk_main_loop_add_source(ftk_default_main_loop(), priv->timer);

	return RET_OK;
}

int ftk_dialog_run(FtkWidget* thiz)
{
    FtkMainLoop* prev_main_loop = ftk_default_main_loop();
	DECL_PRIV1(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);
	return_val_if_fail(ftk_widget_type(thiz) == FTK_DIALOG, RET_FAIL);

    priv->main_loop = ftk_main_loop_create(ftk_default_sources_manager());

    ftk_set_main_loop(priv->main_loop);

    ftk_widget_show_all(thiz, 1);

	ftk_main_loop_run(priv->main_loop);

    ftk_set_main_loop(prev_main_loop);

	ftk_main_loop_destroy(priv->main_loop);
	priv->main_loop = NULL;

	return ftk_widget_id(ftk_window_get_focus(thiz));
}

int ftk_dialog_get_title_height(void)
{
	static int title_height = 0;
	if(title_height == 0)
	{
		FtkBitmap* title_bg = ftk_theme_load_image(ftk_default_theme(), "dialog_title_bg"FTK_STOCK_IMG_SUFFIX);
		title_height =  ftk_bitmap_height(title_bg);
		ftk_bitmap_unref(title_bg);
	}

	return title_height;
}

