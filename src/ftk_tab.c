/*
 * File: ftk_tab.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   tab widget
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
 * 2010-09-23 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_tab.h"
#include "ftk_theme.h"
#include "ftk_globals.h"
#include "ftk_window.h"

typedef struct _TabPage
{
	char* text;
	FtkBitmap* icon;
	FtkWidget* page;
}TabPage;

typedef struct _TabPrivInfo
{
	TabPage* pages;

	int tab_width;
	int active_page;
	int page_use_nr;
	int page_total_nr;
	int mouse_pressed;

	FtkBitmap* normal;
	FtkBitmap* pressed;
	FtkBitmap* selected;
	FtkBitmap* bar_pressed;
	FtkBitmap* bar_selected;
}PrivInfo;

#define FTK_TAB_HANDLE_HEIGHT 48
#define FTK_TAB_HANDLE_MIN_WIDTH 48
#define FTK_TAB_HANDLE_MAX_WIDTH 120

static Ret ftk_tab_page_on_event(FtkWidget* thiz, FtkEvent* event)
{
	return RET_OK;
}

static Ret ftk_tab_page_on_paint(FtkWidget* thiz)
{

	return RET_OK;
}

static void ftk_tab_page_destroy(FtkWidget* thiz)
{
	return;
}

static FtkWidget* ftk_tab_page_create_internal(FtkWidget* parent)
{
	int x = 0;
	DECL_PRIV0(parent, priv);
	int y = FTK_TAB_HANDLE_HEIGHT + ftk_bitmap_height(priv->bar_pressed);
	int width = ftk_widget_width(parent);
	int height = ftk_widget_height(parent) - y;
	FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	return_val_if_fail(thiz != NULL, NULL);

	thiz->on_event = ftk_tab_page_on_event;
	thiz->on_paint = ftk_tab_page_on_paint;
	thiz->destroy = ftk_tab_page_destroy;

	ftk_widget_init(thiz, FTK_TAB_PAGE, 0, x, y, width, height, 
		FTK_ATTR_NO_FOCUS);
	ftk_widget_append_child(parent, thiz);

	return thiz;
}

static Ret ftk_tab_page_tab_calc_width(FtkWidget* thiz)
{
	int tab_width = 0;
	DECL_PRIV0(thiz, priv);

	if(priv->page_total_nr > 0)
	{
		tab_width = ftk_widget_width(thiz)/priv->page_use_nr;
		tab_width = FTK_MAX(tab_width, FTK_TAB_HANDLE_MIN_WIDTH);
		tab_width = FTK_MIN(tab_width, FTK_TAB_HANDLE_MAX_WIDTH);
		priv->tab_width = tab_width;
	}

	return RET_OK;
}

static Ret ftk_tab_on_event(FtkWidget* thiz, FtkEvent* event)
{
	int i = 0;
	Ret ret = RET_OK;
	int active_page = 0;
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL && event != NULL, RET_OK);

	if(priv->page_use_nr <= 0)
	{
		return RET_OK;
	}

	if(event->type == FTK_EVT_SHOW)
	{
		for(i = 0; i < priv->page_use_nr; i++)
		{
			ftk_widget_show_all(priv->pages[i].page, i == priv->active_page);
		}

		return RET_OK;
	}

	active_page = priv->active_page;
	if(event->type == FTK_EVT_MOVE_RESIZE || event->type == FTK_EVT_RESIZE)
	{
		int i = 0;
		int y = FTK_TAB_HANDLE_HEIGHT + ftk_bitmap_height(priv->bar_pressed);
		int w = ftk_widget_width(thiz);
		int h = ftk_widget_height(thiz) - y;

		for(i = 0; i < priv->page_use_nr; i++)
		{
			ftk_widget_move_resize(priv->pages[i].page, 0, y, w, h);
		}
		return RET_OK;
	}

	if(event->type == FTK_EVT_KEY_DOWN)
	{
		return RET_REMOVE;
	}
	else if(event->type == FTK_EVT_KEY_UP)
	{
		if(event->u.key.code == FTK_KEY_LEFT)
		{
			active_page--;
		}
		else if(event->u.key.code == FTK_KEY_RIGHT)
		{
			active_page++;
		}
		else if(event->u.key.code == FTK_KEY_TAB || event->u.key.code == FTK_KEY_DOWN)
		{
			TabPage* page = priv->pages+active_page;
			if(page != NULL && page->page->children != NULL)
			{
				ftk_window_set_focus(ftk_widget_toplevel(thiz), page->page->children);
			}
			return RET_REMOVE;
		}
		active_page = (active_page + priv->page_use_nr)%priv->page_use_nr;
	}
	else if(event->type == FTK_EVT_MOUSE_UP || event->type == FTK_EVT_MOUSE_DOWN)
	{
		int ox = event->u.mouse.x - ftk_widget_left_abs(thiz);
		int oy = event->u.mouse.y - ftk_widget_top_abs(thiz);

		priv->mouse_pressed = 0;
		if(oy <= FTK_TAB_HANDLE_HEIGHT && ox < (priv->page_use_nr * priv->tab_width))
		{
			active_page = ox / priv->tab_width;
			priv->mouse_pressed = event->type == FTK_EVT_MOUSE_DOWN;
		}
	
		if(active_page == priv->active_page)
		{
			ftk_widget_invalidate(thiz);
		}
	}
	
	if(active_page != priv->active_page)
	{
		ret = RET_REMOVE;
		ftk_tab_set_active_page(thiz, active_page);
	}

	return ret;
}

static Ret ftk_tab_paint_one_tab(FtkWidget* thiz, int index)
{
	int w = 0;
	int ox = 0;
	int oy = 0;
	DECL_PRIV0(thiz, priv);
	FtkTextLine line = {0};
	FtkBitmap* bitmap = NULL;
	TabPage* page = priv->pages+index;
	FtkTextLayout* text_layout = ftk_default_text_layout();
	FTK_BEGIN_PAINT(x, y, width, height, canvas);

	(void)height;
	(void)width;
	
	ftk_canvas_set_gc(canvas, ftk_widget_get_gc(thiz));
	if(index != priv->active_page)
	{
		bitmap = priv->normal;
	}
	else
	{
		bitmap = priv->mouse_pressed ? priv->pressed : priv->selected;
	}

	ox = x + index * priv->tab_width;
	ftk_canvas_draw_bg_image(canvas, bitmap, FTK_BG_FOUR_CORNER, 
			ox, y, priv->tab_width, FTK_TAB_HANDLE_HEIGHT);

	w = priv->tab_width;
	if(page->icon != NULL)
	{
		oy = y + FTK_HALF(FTK_TAB_HANDLE_HEIGHT - ftk_bitmap_height(page->icon));
		ftk_canvas_draw_bitmap_simple(canvas, page->icon, 0, 0, 
			ftk_bitmap_width(page->icon), ftk_bitmap_height(page->icon), ox, oy);

		w -= ftk_bitmap_width(page->icon);
		ox += ftk_bitmap_width(page->icon);
	}

	if(page->text != NULL)
	{
		const char* text = page->text;
		ftk_text_layout_init(text_layout, text, -1, canvas, w);

		oy = y + FTK_HALF(FTK_TAB_HANDLE_HEIGHT);
		if(ftk_text_layout_get_visual_line(text_layout, &line) == RET_OK)
		{
			ox = ox + FTK_HALF(w - line.extent); 
			ftk_canvas_draw_string(canvas, ox, oy, text, -1, 1);
		}
	}

	return RET_OK;
}

static Ret ftk_tab_on_paint(FtkWidget* thiz)
{
	int i = 0;
	DECL_PRIV0(thiz, priv);
	FtkBitmap* bitmap = NULL;
	FTK_BEGIN_PAINT(x, y, width, height, canvas);
	return_val_if_fail(priv != NULL, RET_FAIL);

	if(priv->page_use_nr > 0)
	{
		(void)height;
		for(i = 0; i < priv->page_use_nr; i++)
		{
			if(i == priv->active_page) continue;
			ftk_tab_paint_one_tab(thiz, i);	
		}
		ftk_tab_paint_one_tab(thiz, priv->active_page);	
	}
	else
	{
		ftk_canvas_reset_gc(canvas, ftk_widget_get_gc(thiz)); 
		ftk_canvas_draw_rect(canvas, x, y, width, height, 0, 0);
	}
	bitmap = priv->mouse_pressed ? priv->bar_pressed : priv->bar_selected;
	ftk_canvas_draw_bg_image(canvas, bitmap, FTK_BG_FOUR_CORNER, 
		x, y + FTK_TAB_HANDLE_HEIGHT, width, ftk_bitmap_height(bitmap));

	FTK_END_PAINT();
}

static void ftk_tab_destroy(FtkWidget* thiz)
{
	if(thiz != NULL)
	{
		int i = 0;
		TabPage* iter = NULL;
		DECL_PRIV0(thiz, priv);
		FTK_BITMAP_UNREF(priv->normal);
		FTK_BITMAP_UNREF(priv->pressed);
		FTK_BITMAP_UNREF(priv->selected);
		FTK_BITMAP_UNREF(priv->bar_selected);
		FTK_BITMAP_UNREF(priv->bar_pressed);

		for(i = 0; i < priv->page_use_nr; i++)
		{
			iter = priv->pages+i;
			FTK_FREE(iter->text);
			FTK_BITMAP_UNREF(iter->icon);
		}
		FTK_FREE(priv->pages);
		FTK_ZFREE(priv, sizeof(PrivInfo));
	}

	return;
}

FtkWidget* ftk_tab_create(FtkWidget* parent, int x, int y, int width, int height)
{
	FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	return_val_if_fail(thiz != NULL, NULL);
	
	thiz->priv_subclass[0] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[0] != NULL)
	{
		DECL_PRIV0(thiz, priv);
		thiz->on_event = ftk_tab_on_event;
		thiz->on_paint = ftk_tab_on_paint;
		thiz->destroy  = ftk_tab_destroy;

		ftk_widget_init(thiz, FTK_TAB, 0, 
			x, y, width, height, FTK_ATTR_TRANSPARENT|FTK_ATTR_BG_FOUR_CORNER);
		ftk_widget_append_child(parent, thiz);

		priv->pressed = ftk_theme_load_image(ftk_default_theme(),
			"tab-pressed"FTK_STOCK_IMG_SUFFIX);
		priv->selected = ftk_theme_load_image(ftk_default_theme(),
			"tab-selected"FTK_STOCK_IMG_SUFFIX);
		priv->normal = ftk_theme_load_image(ftk_default_theme(),
			"tab-normal"FTK_STOCK_IMG_SUFFIX);
		
		priv->bar_pressed = ftk_theme_load_image(ftk_default_theme(),
			"tab-bar-pressed"FTK_STOCK_IMG_SUFFIX);
		priv->bar_selected = ftk_theme_load_image(ftk_default_theme(),
			"tab-bar-selected"FTK_STOCK_IMG_SUFFIX);

	}
	else
	{
		FTK_FREE(thiz);
	}

	return thiz;
	
}

int     ftk_tab_get_page_count(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL, 0);

	return priv->page_use_nr;
}

FtkWidget* ftk_tab_get_page(FtkWidget* thiz, int index)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL && index < priv->page_use_nr, NULL);

	return priv->pages[index].page;
}

int        ftk_tab_get_page_index(FtkWidget* thiz, FtkWidget* page)
{
	int i = 0;
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL, -1);

	for(i = 0; i < priv->page_use_nr; i++)
	{
		if(priv->pages[i].page == page)
		{
			return i;
		}
	}

	return -1;
}

Ret        ftk_tab_remove_page(FtkWidget* thiz, int index)
{
	int i = 0;
	TabPage* iter = NULL;
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL && index < priv->page_use_nr, RET_FAIL);
	
	iter = priv->pages+index;
	
	FTK_FREE(iter->text);
	FTK_BITMAP_UNREF(iter->icon);
	ftk_widget_remove_child(thiz, iter->page);

	for(i = index; (i + 1) < priv->page_use_nr; i++)
	{
		priv->pages[i] = priv->pages[i+1];		
	}
	priv->page_use_nr--;
	ftk_tab_page_tab_calc_width(thiz);

	return RET_OK;
}

FtkWidget* ftk_tab_add_page(FtkWidget* thiz, const char* text, FtkBitmap* icon)
{
	TabPage* iter = NULL;
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL && text != NULL, NULL);

	if((priv->page_use_nr + 1) > priv->page_total_nr)
	{
		int page_total_nr = priv->page_total_nr + 3;
		TabPage* pages = (TabPage*)FTK_REALLOC(priv->pages, page_total_nr * sizeof(TabPage));
		if(pages != NULL)
		{
			priv->pages = pages;
			priv->page_total_nr = page_total_nr;
		}
	}

	return_val_if_fail((priv->page_use_nr + 1) <= priv->page_total_nr, NULL);
	
	iter = priv->pages + priv->page_use_nr;
	priv->page_use_nr++;

	iter->icon = icon;
	iter->text = FTK_STRDUP(text);
	if(icon != NULL)
	{
		ftk_bitmap_ref(icon);
	}
	iter->page = ftk_tab_page_create_internal(thiz);
	ftk_tab_page_tab_calc_width(thiz);
	ftk_tab_set_active_page(thiz, priv->page_use_nr - 1);
	
	return iter->page;
}

int ftk_tab_get_active_page(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL, 0);

	return priv->active_page;
}

Ret ftk_tab_set_active_page(FtkWidget* thiz, int index)
{
	int i = 0;
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL && index < priv->page_use_nr, RET_FAIL);

	if(priv->active_page != index)
	{
		FtkEvent event;
		int old = priv->active_page;

		priv->active_page = index;
		memset(&event, 0x00, sizeof(event));

		ftk_event_init(&event, FTK_EVT_TAB_PAGE_DEACTIVATE);
		event.widget = priv->pages[old].page;
		ftk_widget_event(thiz, &event);
		
		ftk_event_init(&event, FTK_EVT_TAB_PAGE_ACTIVATE);
		event.widget = priv->pages[index].page;
		ftk_widget_event(thiz, &event);
		
		for(i = 0; i < priv->page_use_nr; i++)
		{
			ftk_widget_show_all(priv->pages[i].page, i == priv->active_page);
		}

		ftk_widget_invalidate(thiz);
	}

	return RET_OK;
}

Ret ftk_tab_set_page_text(FtkWidget* thiz, int index, const char* text)
{
	TabPage* iter = NULL;
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL && index < priv->page_use_nr, RET_FAIL);

	iter = priv->pages+index;
	FTK_FREE(iter->text);
	iter->text = FTK_STRDUP(text);

	return RET_OK;
}

Ret ftk_tab_set_page_icon(FtkWidget* thiz, int index, FtkBitmap* icon)
{
	TabPage* iter = NULL;
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL && index < priv->page_use_nr, RET_FAIL);

	iter = priv->pages+index;
	FTK_BITMAP_UNREF(iter->icon);
	iter->icon = icon;
	if(icon != NULL)
	{
		ftk_bitmap_ref(icon);
	}

	return RET_OK;
}

const char* ftk_tab_get_page_text(FtkWidget* thiz, int index)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL && index < priv->page_use_nr, NULL);

	return priv->pages[index].text;
}

FtkBitmap* ftk_tab_get_page_icon(FtkWidget* thiz, int index)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL && index < priv->page_use_nr, NULL);

	return priv->pages[index].icon;
}

FtkWidget* ftk_tab_page_create(FtkWidget* parent, int x, int y, int width, int height)
{
	return_val_if_fail(ftk_widget_type(parent) == FTK_TAB, NULL);

	return ftk_tab_add_page(parent, "", NULL);
}

