/*
 * File: ftk_menu_panel.c
 * Author: Li XianJing <xianjimli@hotmail.com>
 * Brief:  ftk menu panel
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
 * 2009-10-30 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_window.h"
#include "ftk_globals.h"
#include "ftk_menu_item.h"

typedef struct _MenuPanelPrivInfo
{
	int changed;
	int items_nr;
	FtkWidget* items[FTK_MENU_MAX_ITEM];	
	FtkWidgetOnEvent parent_on_event;
	FtkWidgetOnPaint parent_on_paint;
	FtkWidgetDestroy parent_destroy;
}PrivInfo;

static int ftk_menu_panel_count_visible_items(FtkWidget* thiz)
{
	int i = 0;
	int n = 0;
	DECL_PRIV1(thiz, priv);
	for(i = 0; i < priv->items_nr; i++)
	{
		if(ftk_widget_is_visible(priv->items[i]))
		{
			n++;
		}
	}

	return n;
}

static FtkRect* ftk_menu_panel_calc_rects(FtkWidget* thiz, int* nr)
{
	int i = 0;
	int n = 0;
	int w = 0;
	FtkRect* rect = NULL;
	DECL_PRIV1(thiz, priv);
	int screen_width = ftk_display_width(ftk_default_display());
	int max_items_per_row = screen_width/FTK_MENU_ITEM_WIDTH;
	return_val_if_fail(priv->items_nr > 0, NULL);
	n = ftk_menu_panel_count_visible_items(thiz);
	return_val_if_fail(n > 0, NULL);

	n = n <= max_items_per_row * 2 ? n : max_items_per_row * 2;
	rect = (FtkRect*)FTK_ALLOC(sizeof(FtkRect) * n);
	return_val_if_fail(rect != NULL, NULL);

	*nr = n;
	if(n <= max_items_per_row)
	{
		w = screen_width/n;
		for(i = 0; i < n; i++)
		{
			rect[i].x = w * i;
			rect[i].y = 0;
			rect[i].width = w;
			rect[i].height = FTK_MENU_ITEM_HEIGHT;
		}
	}
	else
	{
		int first_row_nr = n/2;
		int second_row_nr = (n+1)/2;
		
		w = screen_width/first_row_nr;
		for(i = 0; i < first_row_nr; i++)
		{
			rect[i].x = w * i;
			rect[i].y = 0;
			rect[i].width = w;
			rect[i].height = FTK_MENU_ITEM_HEIGHT;
		}
		
		w = screen_width/second_row_nr;
		for(i = 0; i < second_row_nr; i++)
		{
			rect[first_row_nr + i].x = w * i;
			rect[first_row_nr + i].y = FTK_MENU_ITEM_HEIGHT;
			rect[first_row_nr + i].width = w;
			rect[first_row_nr + i].height = FTK_MENU_ITEM_HEIGHT;
		}
	}

	return rect;
}

Ret ftk_menu_panel_relayout(FtkWidget* thiz)
{
	int i = 0;
	int j = 0;
	int nr = 0;
	int h  = 0;
	DECL_PRIV1(thiz, priv);
	FtkRect* rects = ftk_menu_panel_calc_rects(thiz, &nr);
	int screen_height = ftk_display_height(ftk_default_display());
	int screen_width = ftk_display_width(ftk_default_display());
	int max_items_per_row = screen_width/FTK_MENU_ITEM_WIDTH;	
	return_val_if_fail(priv->items_nr > 0 && rects != NULL && nr > 0, RET_FAIL);

	h = nr > max_items_per_row ? FTK_MENU_ITEM_HEIGHT * 2 : FTK_MENU_ITEM_HEIGHT;
	ftk_widget_move_resize(thiz, 0, screen_height - h, screen_width, h);

	/*relayout the items*/
	for(i = 0, j = 0; i < nr; j++)
	{
		if(ftk_widget_is_visible(priv->items[j]))
		{
			ftk_widget_move_resize(priv->items[j], rects[i].x, rects[i].y, rects[i].width, rects[i].height);
			i++;
		}
	}

	/*hide items that there is no space for them*/
	for(; j < priv->items_nr; j++)
	{
		ftk_widget_move_resize(priv->items[j], 0, 0, 0, 0);
	}

	FTK_FREE(rects);

	return RET_OK;
}

static Ret  ftk_menu_panel_on_event(FtkWidget* thiz, FtkEvent* event)
{
	Ret ret = RET_OK;
	DECL_PRIV1(thiz, priv);

	if(event->type == FTK_EVT_KEY_UP && event->u.key.code == FTK_KEY_MENU)
	{
		ftk_widget_unref(thiz);

		return ret;
	}

	if(event->type == FTK_EVT_SHOW)
	{
		ftk_wnd_manager_grab(ftk_default_wnd_manager(), thiz);
	}
	
	if(event->type == FTK_EVT_HIDE)
	{
		ftk_wnd_manager_ungrab(ftk_default_wnd_manager(), thiz);
	}
	
	if((event->type == FTK_EVT_MOUSE_UP && ret != RET_IGNORED)
		|| (event->type == FTK_EVT_KEY_UP && FTK_IS_ACTIVE_KEY(event->u.key.code)))
	{
		ftk_widget_show(thiz, 0);
	}

	ret = priv->parent_on_event(thiz, event);

	if((event->type == FTK_EVT_MOUSE_UP && ret != RET_IGNORED)
		|| (event->type == FTK_EVT_KEY_UP && FTK_IS_ACTIVE_KEY(event->u.key.code)))
	{
		ftk_widget_unref(thiz);
	}

	return ret;
}

static Ret  ftk_menu_panel_on_paint(FtkWidget* thiz)
{
	int i = 0;
	int w = 0;
	int nr = 0;
	int first_row_nr = 0;
	int second_row_nr = 0;
	FtkGc gc = {0};
	DECL_PRIV1(thiz, priv);
	int screen_width = ftk_display_width(ftk_default_display());
	int max_items_per_row = screen_width/FTK_MENU_ITEM_WIDTH;	
	FTK_BEGIN_PAINT(x, y, width, height, canvas);
	return_val_if_fail(priv != NULL, RET_FAIL);
	return_val_if_fail(ftk_widget_is_visible(thiz), RET_FAIL);

	/*draw border*/
	gc.mask = FTK_GC_FG;
	gc.fg = ftk_theme_get_border_color(ftk_default_theme(), FTK_MENU_PANEL, ftk_widget_state(thiz));
	ftk_canvas_set_gc(canvas, &gc);
	ftk_canvas_draw_rect(canvas, x, y, width, height, 0, 0);
	ftk_canvas_draw_rect(canvas, x+1, y+1, width-2, height-2, 0, 0);

	/*draw grid*/
	gc.fg = ftk_widget_get_gc(thiz)->fg;
	ftk_canvas_set_gc(canvas, &gc);
	nr = ftk_menu_panel_count_visible_items(thiz);
	nr = nr <= max_items_per_row * 2 ? nr : max_items_per_row * 2;

	if(nr > max_items_per_row)
	{
		first_row_nr = nr/2;
		second_row_nr = (nr+1)/2;
		ftk_canvas_draw_hline(canvas, x, y+FTK_MENU_ITEM_HEIGHT, screen_width);
	}
	else
	{
		first_row_nr = nr;
	}

	if(first_row_nr > 0)
	{
		w = screen_width/first_row_nr;
		for(i = 1; i < first_row_nr; i++)
		{
			ftk_canvas_draw_vline(canvas, x+i*w, y, FTK_MENU_ITEM_HEIGHT);
		}
	}

	if(second_row_nr > 0)
	{
		w = screen_width/second_row_nr;
		for(i = 1; i < second_row_nr; i++)
		{
			ftk_canvas_draw_vline(canvas, x+i*w, y+FTK_MENU_ITEM_HEIGHT, FTK_MENU_ITEM_HEIGHT);
		}
	}

	priv->parent_on_paint(thiz);

	return RET_OK;
}

static void ftk_menu_panel_destroy(FtkWidget* thiz)
{
	DECL_PRIV1(thiz, priv);
	FtkWidgetDestroy parent_destroy = priv->parent_destroy;
	parent_destroy(thiz);
	FTK_ZFREE(thiz->priv_subclass[1], sizeof(PrivInfo));

	return;
}

FtkWidget* ftk_menu_panel_create(void)
{
	FtkWidget* thiz = ftk_window_create(FTK_MENU_PANEL, 0, 0, 0, 0, 0);
	return_val_if_fail(thiz != NULL, NULL);

	thiz->priv_subclass[1] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[1] != NULL)
	{
		DECL_PRIV1(thiz, priv);
		priv->parent_on_event = thiz->on_event;
		priv->parent_on_paint = thiz->on_paint;
		priv->parent_destroy  = thiz->destroy;
		thiz->on_event = ftk_menu_panel_on_event;
		thiz->on_paint = ftk_menu_panel_on_paint;
		thiz->destroy  = ftk_menu_panel_destroy;
	}
	else
	{
		ftk_widget_destroy(thiz);
		thiz = NULL;
	}

	return thiz;
}


Ret ftk_menu_panel_add(FtkWidget* thiz, FtkWidget* item)
{
	DECL_PRIV1(thiz, priv);
	return_val_if_fail(thiz != NULL && item != NULL, RET_FAIL);
	return_val_if_fail(priv->items_nr < FTK_MENU_MAX_ITEM, RET_FAIL);

	priv->items[priv->items_nr++] = item;
	priv->changed = 1;
	ftk_widget_append_child(thiz, item);

	return RET_OK;
}

Ret        ftk_menu_panel_remove(FtkWidget* thiz, FtkWidget* item)
{
	int i = 0;
	Ret ret = RET_FAIL;
	DECL_PRIV1(thiz, priv);
	return_val_if_fail(thiz != NULL && item != NULL, RET_FAIL);

	for(i = 0; i < priv->items_nr; i++)
	{
		if(priv->items[i] == item)
		{
			for(; i < priv->items_nr; i++)
			{
				priv->items[i] = priv->items[i+1];
			}
			priv->items_nr--;

			ret = RET_OK;
			break;
		}
	}

	if(ret == RET_OK)
	{
		priv->changed = 1;
		ftk_widget_remove_child(thiz, item);
	}

	return RET_OK;
}


