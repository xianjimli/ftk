/*
 * File: ftk_icon_view.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   icon view
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
 * 2009-12-04 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_window.h"
#include "ftk_canvas.h"
#include "ftk_globals.h"
#include "ftk_icon_view.h"
#include "ftk_icon_cache.h"

typedef struct _IconViewPrivInfo
{
	int nr;
	int alloc_nr;
	FtkIconViewItem* items;
	
	int    current;
	int    visible_nr;
	int    visible_start;
	
	int cols;
	int rows;
	int left_margin;
	int top_margin;

	int item_width;
	int item_height;
	FtkListener listener;
	void* listener_ctx;

	int active;
	FtkBitmap* item_focus;
	FtkBitmap* item_active;
}PrivInfo;

static Ret ftk_icon_view_item_reset(FtkIconViewItem* item);

Ret ftk_widget_invalidate_item(FtkWidget* thiz, int item)
{
	int row = 0;
	int col = 0;
	int offset = 0;
	FtkRect rect = {0};
	DECL_PRIV0(thiz, priv);

	if(!ftk_widget_is_visible(ftk_widget_toplevel(thiz)))
	{
		return RET_FAIL;
	}

	/*FIXME:*/
	return ftk_widget_invalidate(thiz);

	offset = item - priv->visible_start;
	if(offset < 0) return RET_OK;

	row = offset / priv->cols;
	col = offset % priv->cols;

	if(row > priv->rows) return RET_OK;

	rect.width = priv->item_width;
	rect.height = priv->item_height;
	rect.x = ftk_widget_left_abs(thiz) + priv->left_margin + col * priv->item_width;
	rect.y = ftk_widget_top_abs(thiz) + priv->top_margin + row * priv->item_height;

	ftk_logd("%s: item=%d %d %d %d %d\n", __func__, item, rect.x, rect.y, rect.width, rect.height);

	return ftk_window_invalidate(ftk_widget_toplevel(thiz), &rect);
}

static Ret ftk_icon_view_set_cursor(FtkWidget* thiz, int current)
{
	DECL_PRIV0(thiz, priv);
	int	visible_start = priv->visible_start;

	ftk_widget_invalidate_item(thiz, priv->current);
	
	priv->current = current;
	if(priv->current < 0)
	{
		priv->current = 0;
	}

	if(priv->current >= priv->nr)
	{
		priv->current = priv->nr - 1;
	}

	while(priv->visible_start > priv->current)
	{
		priv->visible_start -= priv->cols;
	}

	while((priv->visible_start + priv->visible_nr) <= priv->current)
	{
		priv->visible_start += priv->cols;
	}

	if(priv->visible_start >= priv->nr)
	{
		priv->visible_start -= priv->cols * priv->rows;	
	}

	if(priv->visible_start < 0)
	{
		priv->visible_start = 0;
	}

	if(visible_start != priv->visible_start)
	{
		ftk_widget_invalidate(thiz);
	}
	else
	{
		ftk_widget_invalidate_item(thiz, priv->current);
	}

	return RET_REMOVE;
}

static Ret ftk_icon_view_move_cursor(FtkWidget* thiz, int offset)
{
	DECL_PRIV0(thiz, priv);
	return ftk_icon_view_set_cursor(thiz, priv->current + offset);
}

static Ret ftk_icon_view_on_event(FtkWidget* thiz, FtkEvent* event)
{
	int x = 0;
	int y = 0;
	int current = 0;
	Ret ret = RET_OK;
	DECL_PRIV0(thiz, priv);

	if(priv->nr < 1)
	{
		return RET_OK;
	}

	switch(event->type)
	{
		case FTK_EVT_MOUSE_DOWN:
		{
			x = event->u.mouse.x - ftk_widget_left_abs(thiz) - priv->left_margin;
			y = event->u.mouse.y - ftk_widget_top_abs(thiz) - priv->top_margin;

			if (y < 0)
			{
                if (priv->current - priv->visible_nr >= 0)
                {
                    ftk_window_grab(ftk_widget_toplevel(thiz), thiz);
                    ftk_icon_view_move_cursor(thiz, -priv->visible_nr);
                }
                break;
			}

			if (y > priv->item_height * priv->rows)
			{
                ftk_window_grab(ftk_widget_toplevel(thiz), thiz);
                ftk_icon_view_move_cursor(thiz, +priv->visible_nr);
                break;
			}

	        if (x < 0 || x > priv->item_width * priv->cols)
	        {
                break;
	        }

            current = (y / priv->item_height) * priv->cols + x / priv->item_width;

            if((priv->visible_start + current) < priv->nr)
            {
                priv->active = 1;
                ftk_window_grab(ftk_widget_toplevel(thiz), thiz);
                ftk_icon_view_set_cursor(thiz, priv->visible_start + current);
            }
			break;
		}
		case FTK_EVT_MOUSE_UP:
		{
			ftk_widget_invalidate_item(thiz, priv->current);
			ftk_window_ungrab(ftk_widget_toplevel(thiz), thiz);
			if(priv->current < priv->nr && priv->active)
			{
				FtkIconViewItem* item = priv->items + priv->current;
				ret = FTK_CALL_LISTENER(priv->listener, priv->listener_ctx, item);
			}
			priv->active = 0;

			break;
		}
		case FTK_EVT_KEY_DOWN:
		{
			if(FTK_IS_ACTIVE_KEY(event->u.key.code) && !priv->active)
			{
				priv->active = 1;
				ftk_widget_invalidate_item(thiz, priv->current);
			}

			switch(event->u.key.code)
			{
				case FTK_KEY_UP:
				{
					if(priv->current > 0)
					{
						ftk_icon_view_move_cursor(thiz, -priv->cols);
						ret = RET_REMOVE;
					}
					break;
				}
				case FTK_KEY_DOWN:
				{
					if((priv->current + 1) < priv->nr)
					{
						ftk_icon_view_move_cursor(thiz, priv->cols);
						ret = RET_REMOVE;
					}
					break;
				}
				case FTK_KEY_LEFT:
				{
					if(priv->current > 0)
					{
						ftk_icon_view_move_cursor(thiz, -1);
						ret = RET_REMOVE;
					}
					break;
				}
				case FTK_KEY_RIGHT:
				{
					if((priv->current + 1) < priv->nr)
					{
						ftk_icon_view_move_cursor(thiz, 1);
						ret = RET_REMOVE;
					}
					break;
				}
				default:break;
			}
			break;
		}
		case FTK_EVT_KEY_UP:
		{
			if(FTK_IS_ACTIVE_KEY(event->u.key.code) && priv->active) 
			{
				priv->active = 0;
				ftk_widget_invalidate_item(thiz, priv->current);
				if(priv->current < priv->nr)
				{
					FtkIconViewItem* item = priv->items + priv->current;
					ret = FTK_CALL_LISTENER(priv->listener, priv->listener_ctx, item);
				}
			}
			ret = RET_REMOVE;
			break;
		}
		default:break;
	}

	return ret;
}

static Ret ftk_icon_view_calc(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	int visible_nr = 0;
	int width = ftk_widget_width(thiz) - 2 * FTK_H_MARGIN;
	int height = ftk_widget_height(thiz) - 2 * FTK_V_MARGIN;

	priv->cols = width/priv->item_width;
	priv->left_margin = FTK_HALF(width%priv->item_width) + FTK_H_MARGIN;
	priv->rows = height/priv->item_height;
	priv->top_margin = FTK_HALF(height%priv->item_height) + FTK_V_MARGIN;

	visible_nr = priv->cols * priv->rows;
	priv->visible_nr = priv->nr - priv->visible_start;
	priv->visible_nr = FTK_MIN(visible_nr, priv->visible_nr);

	return RET_OK;
}

static Ret ftk_icon_view_on_paint(FtkWidget* thiz)
{
	int x1 = 0;
	int dx = 0;
	int dy = 0;
	int fw = 0;
	int i = 0;
	int j = 0;
	int item = 0;
	int icon_height = 0;
	const char* text = NULL;
	DECL_PRIV0(thiz, priv);
	FtkIconViewItem* item_info = NULL;
	FTK_BEGIN_PAINT(x, y, width, height, canvas);

	(void)width;
	(void)height;
	if(priv->cols < 1 || priv->rows < 1 || priv->nr < 1)
	{
		FTK_END_PAINT();
	}

	dy = y + priv->top_margin;
	item = priv->visible_start;
	ftk_canvas_reset_gc(canvas, ftk_widget_get_gc(thiz)); 

	for(i = 0; i < priv->rows; i++)
	{
		dx = x + priv->left_margin;
		for(j = 0; j < priv->cols; j++, item++)
		{
			if(item >= priv->nr || item >= (priv->visible_start + priv->visible_nr))
			{
				break;
			}
			item_info = priv->items + item;
			text = item_info->text;
			if(item == priv->current)
			{
				FtkBitmap* bg = priv->active ? priv->item_active : priv->item_focus;
				ftk_canvas_draw_bg_image(canvas, bg, FTK_BG_FOUR_CORNER, dx, dy, priv->item_width, priv->item_height);
			}
			
			if(text == NULL || text[0] == '\0')
			{
				if(item_info->icon != NULL)
				{
					ftk_canvas_draw_bg_image(canvas, item_info->icon, 
						FTK_BG_CENTER, dx, dy, priv->item_width, priv->item_height);
				}
			}
			else
			{
				icon_height = 0;
				fw = ftk_canvas_get_str_extent(canvas, text, -1);

				if(item_info->icon != NULL)
				{
					icon_height = priv->item_height - ftk_widget_get_font_size(thiz) - 2*FTK_V_MARGIN;
					ftk_canvas_draw_bg_image(canvas, item_info->icon, 
						FTK_BG_CENTER, dx, dy, priv->item_width, icon_height);
				}

				x1 = dx +  ((priv->item_width > fw) ? FTK_HALF(priv->item_width - fw) : 0);
				ftk_canvas_draw_string(canvas, x1,
					dy + icon_height + FTK_HALF(priv->item_height - icon_height), text, -1, 1);	
			}

			dx += priv->item_width;
		}
		dy += priv->item_height;
	}

	FTK_END_PAINT();
}

static void ftk_icon_view_destroy(FtkWidget* thiz)
{
	if(thiz != NULL)
	{
		int i = 0;
		DECL_PRIV0(thiz, priv);

		for(i = 0; i < priv->nr; i++)
		{
			ftk_icon_view_item_reset(priv->items+i);
		}
		FTK_FREE(priv->items);
		ftk_bitmap_unref(priv->item_focus);
		ftk_bitmap_unref(priv->item_active);
		FTK_ZFREE(priv, sizeof(PrivInfo));
	}

	return;
}

FtkWidget* ftk_icon_view_create(FtkWidget* parent, int x, int y, int width, int height)
{
	FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	return_val_if_fail(thiz != NULL, NULL);
	
	thiz->priv_subclass[0] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[0] != NULL)
	{
		DECL_PRIV0(thiz, priv);
		thiz->on_event = ftk_icon_view_on_event;
		thiz->on_paint = ftk_icon_view_on_paint;
		thiz->destroy  = ftk_icon_view_destroy;

		ftk_widget_init(thiz, FTK_ICON_VIEW, 0, x, y, width, height, FTK_ATTR_BG_FOUR_CORNER);
		priv->item_focus  = ftk_theme_load_image(ftk_default_theme(), "menuitem_background_focus"FTK_STOCK_IMG_SUFFIX);
		priv->item_active = ftk_theme_load_image(ftk_default_theme(), "menuitem_background_pressed"FTK_STOCK_IMG_SUFFIX);
		ftk_widget_append_child(parent, thiz);

		ftk_icon_view_set_item_size(thiz, FTK_ICON_VIEW_ITEM_SIZE);
	}
	else
	{
		FTK_FREE(thiz);
	}

	return thiz;
}

Ret ftk_icon_view_set_clicked_listener(FtkWidget* thiz, FtkListener listener, void* ctx)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	priv->listener = listener;
	priv->listener_ctx = ctx;

	return RET_OK;
}

Ret ftk_icon_view_set_item_size(FtkWidget* thiz, int size)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);

	size = size < FTK_ICON_VIEW_ITEM_MIN ? FTK_ICON_VIEW_ITEM_MIN : size;
	size = size > FTK_ICON_VIEW_ITEM_MAX ? FTK_ICON_VIEW_ITEM_MAX : size;

	priv->item_width  = size;
	priv->item_height = size;

	return RET_OK;
}

int ftk_icon_view_get_count(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL, 0);

	return priv->nr;
}

Ret ftk_icon_view_remove(FtkWidget* thiz, int index)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL && index < priv->nr, RET_FAIL);

	if((index + 1) < priv->nr)
	{
		for(; (index + 1) < priv->nr; index++)
		{
			priv->items[index] = priv->items[index + 1];
		}
	}

	priv->nr--;
	ftk_icon_view_calc(thiz);
	ftk_widget_invalidate(thiz);

	return RET_OK;
}

static Ret ftk_icon_view_extend(FtkWidget* thiz, int delta)
{
	int alloc_nr = 0;
	DECL_PRIV0(thiz, priv);
	FtkIconViewItem* items = NULL;

	if((priv->nr + delta) < priv->alloc_nr)
	{
		return RET_OK;
	}

	alloc_nr = priv->nr + delta + FTK_HALF(priv->nr + delta) + 5;
	items = (FtkIconViewItem*)FTK_REALLOC(priv->items, sizeof(FtkIconViewItem) * alloc_nr);
	if(items != NULL)
	{
		priv->items    = items;
		priv->alloc_nr = alloc_nr;
	}

	return (priv->nr + delta) < priv->alloc_nr ? RET_OK : RET_FAIL;
}

static Ret ftk_icon_view_item_copy(FtkIconViewItem* dst, const FtkIconViewItem* src)
{
	return_val_if_fail(dst != NULL && src != NULL, RET_FAIL);

	memset(dst, 0x00, sizeof(FtkIconViewItem));

	if(src->text != NULL)
	{
		dst->text = ftk_strdup(src->text);
	}

	if(src->icon != NULL)
	{
		dst->icon = src->icon;
		ftk_bitmap_ref(dst->icon);
	}

	dst->user_data = src->user_data;

	return RET_OK;
}

static Ret ftk_icon_view_item_reset(FtkIconViewItem* item)
{
	return_val_if_fail(item != NULL, RET_FAIL);

	if(item->icon != NULL)
	{
		ftk_bitmap_unref(item->icon);
	}

	if(item->text != NULL)
	{
		FTK_FREE(item->text);
	}

	memset(item, 0x00, sizeof(item));

	return RET_OK;
}

Ret ftk_icon_view_add(FtkWidget* thiz, const FtkIconViewItem* item)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL && item != NULL, RET_FAIL);
	return_val_if_fail(ftk_icon_view_extend(thiz, 1) == RET_OK, RET_FAIL);

	ftk_icon_view_item_copy(priv->items+priv->nr, item);
	priv->nr++;

	ftk_icon_view_calc(thiz);
	ftk_widget_invalidate(thiz);

	return RET_OK;
}

Ret ftk_icon_view_get(FtkWidget* thiz, int index, const FtkIconViewItem** item)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL && index < priv->nr && item != NULL, RET_FAIL);

	*item = priv->items+index;

	return RET_OK;
}

