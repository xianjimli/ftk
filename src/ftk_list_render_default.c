/*
 * File: ftk_list_render_default.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   default list render
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
 * 2009-11-28 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_util.h"
#include "ftk_globals.h"
#include "ftk_icon_cache.h"
#include "ftk_source_timer.h"
#include "ftk_list_model_default.h"
#include "ftk_list_render_default.h"

#define MARQUEE_MOVE_INTERVAL 200
#define MARQUEE_PAUSE_INTERVAL 2000

typedef enum _RenderMarqueeState
{
	MARQUEE_NONE,
	MARQUEE_BEGIN,
	MARQUEE_MOVE,
	MARQUEE_PAUSE,
	MARQUEE_END
}RenderMarqueeState;

typedef struct _ListRenderDefaultPrivInfo
{
	FtkBitmap* more;
	FtkBitmap* radio_off;
	FtkBitmap* radio_on;
	FtkBitmap* check_off;
	FtkBitmap* check_on;
	FtkBitmap* bg_disable;
	FtkListModel* model;
	FtkWidget* list_view;

	int focus_item;
	int times_to_move;
	int visible_start;
	int marquee_width;
	int marquee_paint_times;
	int marquee_cycle_times;
	FtkMarqueeAttr marquee_attr;
	FtkSource* marquee_timer;
	RenderMarqueeState marquee_state;
}PrivInfo;

static Ret ftk_list_render_default_init(FtkListRender* thiz, FtkListModel* model, FtkWidget* list_view)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && model != NULL, RET_FAIL);

	priv->model = model;
	priv->list_view = list_view;

	return RET_OK;
}

static Ret marquee_timer_func(void* ctx)
{
	FtkListRender* thiz = (FtkListRender*)ctx;
	DECL_PRIV(thiz, priv);
	FtkWidget* list_view = priv->list_view;

	if(priv->marquee_state == MARQUEE_NONE)
	{
		ftk_source_timer_modify(priv->marquee_timer, MARQUEE_PAUSE_INTERVAL);
		return RET_OK;
	}

	priv->visible_start++;
	switch(priv->marquee_state)
	{
		case MARQUEE_BEGIN:
		{
			priv->visible_start = 0;
			priv->marquee_paint_times = 0;
			priv->marquee_state = MARQUEE_MOVE;
			ftk_source_timer_modify(priv->marquee_timer, MARQUEE_PAUSE_INTERVAL);
			break;
		}
		case MARQUEE_MOVE:
		{
			if(priv->marquee_paint_times == 0)
			{
				ftk_source_timer_modify(priv->marquee_timer, MARQUEE_MOVE_INTERVAL);
			}
			if(priv->marquee_paint_times < priv->times_to_move)
			{
				priv->marquee_paint_times++;
			}
			else
			{
				priv->marquee_state = MARQUEE_PAUSE;
				ftk_source_timer_modify(priv->marquee_timer, MARQUEE_PAUSE_INTERVAL);
			}
			break;
		}
		case MARQUEE_PAUSE:
		{
			priv->marquee_paint_times = 0;
			priv->marquee_state = MARQUEE_MOVE;
			ftk_source_timer_modify(priv->marquee_timer, MARQUEE_MOVE_INTERVAL);
			break;
		}
		case MARQUEE_END:
		{
			priv->visible_start = 0;
			priv->marquee_cycle_times++;
			priv->marquee_paint_times = 0;
			priv->marquee_state = MARQUEE_BEGIN;
			ftk_source_timer_modify(priv->marquee_timer, MARQUEE_PAUSE_INTERVAL);
			break;
		}
		default:break;
	}

	//ftk_logd("%s: state=%d paint_times=%d cycle_times=%d vstart=%d\n", __func__, 
	//	priv->marquee_state, priv->marquee_paint_times, priv->marquee_cycle_times,
	//	priv->visible_start);
	ftk_list_view_repaint_focus_item(list_view);

	return RET_OK;
}

static Ret ftk_list_render_default_focus_check(FtkListRender* thiz)
{
	DECL_PRIV(thiz, priv);
	int focus_item = ftk_list_view_get_selected(priv->list_view);
	
	if(focus_item != priv->focus_item && priv->marquee_timer != 0)
	{
		priv->visible_start = 0;
		priv->focus_item = focus_item;
		priv->marquee_paint_times = 0;
		priv->marquee_cycle_times = 0;
		priv->times_to_move = priv->marquee_width/16;
		priv->marquee_state = MARQUEE_BEGIN;
		ftk_source_timer_modify(priv->marquee_timer, MARQUEE_PAUSE_INTERVAL);
	}
	
	return RET_OK;
}

static Ret ftk_list_render_default_paint(FtkListRender* thiz, FtkCanvas* canvas, int pos, int x, int y, int w, int h)
{
	int dx = 0;
	int dy = 0;
	DECL_PRIV(thiz, priv);
	FtkBitmap* right_icon = NULL;
	FtkListItemInfo* info = NULL;
	return_val_if_fail(thiz != NULL && canvas != NULL && w > 0 && h > 0, RET_FAIL);
	ftk_list_model_get_data(priv->model, pos, (void**)&info);
	return_val_if_fail(info != NULL, RET_FAIL);

	if(info->disable)
	{
		ftk_canvas_draw_bg_image(canvas, priv->bg_disable, FTK_BG_FOUR_CORNER, x, y, w, h);
	}

	if(info->left_icon != NULL)
	{
		ftk_canvas_draw_bg_image(canvas, info->left_icon, FTK_BG_CENTER, x, y, h, h);
	}
	
	switch(info->type)
	{
		case FTK_LIST_ITEM_RADIO:
		{
			right_icon = info->state ? priv->radio_on : priv->radio_off;
			break;
		}
		case FTK_LIST_ITEM_CHECK:
		{
			right_icon = info->state ? priv->check_on : priv->check_off;
			break;
		}
		case FTK_LIST_ITEM_MORE:
		{
			right_icon = priv->more;
			break;
		}
		default:
		{
			right_icon = info->right_icon;
			break;
		}
	}

	if(right_icon != NULL)
	{
		ftk_canvas_draw_bg_image(canvas, right_icon, FTK_BG_CENTER, x+w-h, y, h, h);
	}

	if(info->text != NULL)
	{
		int text_width = w;
		int visible_start = 0;
		FtkTextLine line = {0};
		const char* text = NULL;
		FtkTextLayout* text_layout = ftk_default_text_layout();
		text_width = info->left_icon != NULL  ? text_width - h : text_width;
		text_width = right_icon != NULL ? text_width - h : text_width;
		
		dy = y + FTK_HALF(h);
		dx = FTK_H_MARGIN + (info->left_icon != NULL  ? x + h : x);
	
		priv->marquee_width = text_width;
		ftk_list_render_default_focus_check(thiz);
		
		ftk_text_layout_init(text_layout, info->text, -1, canvas, text_width); 
		ftk_text_layout_get_visual_line(text_layout, &line);

		do
		{
			visible_start = 0;
			if(priv->marquee_attr == 0 || pos != priv->focus_item)
			{
				break;
			}
			else if(!(priv->marquee_attr & FTK_MARQUEE_RIGHT2LEFT))
			{
				/*NOT support down to up scroll yet.*/
				break;
			}
			else if(!(priv->marquee_attr & FTK_MARQUEE_AUTO))
			{
				/*IF text is short dont scroll it.*/
				break;
			}
			else if((priv->marquee_attr & FTK_MARQUEE_ONCE) && (priv->marquee_cycle_times) > 0)
			{
				break;
			}
			else if((priv->marquee_attr & FTK_MARQUEE_AUTO) && (line.len == strlen(info->text)))
			{
				priv->marquee_state = MARQUEE_NONE;
				break;
			}

			if(priv->marquee_state == MARQUEE_NONE)
			{
				priv->marquee_state = MARQUEE_BEGIN;
			}

			visible_start = priv->visible_start;
			ftk_text_layout_init(text_layout, utf8_move_forward(info->text, visible_start), 
				-1, canvas, text_width); 
			ftk_text_layout_get_visual_line(text_layout, &line);
		
			if((line.extent  + 20) < text_width)
			{
				priv->marquee_state = MARQUEE_END;
				ftk_source_timer_modify(priv->marquee_timer, MARQUEE_PAUSE_INTERVAL);
			}
		}while(0);

		text = utf8_move_forward(info->text, visible_start);
		ftk_text_layout_init(text_layout, text, -1, canvas, text_width); 

		if(ftk_text_layout_get_visual_line(text_layout, &line) == RET_OK)
		{
			ftk_canvas_draw_string(canvas, dx + line.xoffset, dy, line.text, line.len, 1);
		}
	}

	return RET_OK;
}

static void ftk_list_render_default_destroy(FtkListRender* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		ftk_bitmap_unref(priv->radio_on);
		ftk_bitmap_unref(priv->radio_off);
		ftk_bitmap_unref(priv->check_on);
		ftk_bitmap_unref(priv->check_off);
		ftk_bitmap_unref(priv->more);
		ftk_bitmap_unref(priv->bg_disable);
		if(priv->marquee_timer != NULL)
		{
			ftk_source_disable(priv->marquee_timer);
			ftk_main_loop_remove_source(ftk_default_main_loop(), priv->marquee_timer);
		}
		FTK_ZFREE(thiz, sizeof(FtkListRender) + sizeof(PrivInfo));
	}

	return;
}

FtkListRender* ftk_list_render_default_create(void)
{
	FtkListRender* thiz = FTK_NEW_PRIV(FtkListRender);
	
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->init    = ftk_list_render_default_init;
		thiz->paint   = ftk_list_render_default_paint;
		thiz->destroy = ftk_list_render_default_destroy;

		priv->focus_item = -1;
		priv->radio_off = ftk_theme_load_image(ftk_default_theme(), "btn_radio_off"FTK_STOCK_IMG_SUFFIX);
		priv->radio_on = ftk_theme_load_image(ftk_default_theme(),  "btn_radio_on"FTK_STOCK_IMG_SUFFIX);
		priv->check_off = ftk_theme_load_image(ftk_default_theme(), "btn_check_off"FTK_STOCK_IMG_SUFFIX);
		priv->check_on = ftk_theme_load_image(ftk_default_theme(),  "btn_check_on"FTK_STOCK_IMG_SUFFIX);
		priv->more = ftk_theme_load_image(ftk_default_theme(),      "more"FTK_STOCK_IMG_SUFFIX);
		priv->bg_disable = ftk_theme_load_image(ftk_default_theme(),  "list_selector_background_disabled"FTK_STOCK_IMG_SUFFIX);
	}

	return thiz;
}

Ret ftk_list_render_default_set_marquee_attr(FtkListRender* thiz, FtkMarqueeAttr marquee_attr)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	priv->marquee_attr = marquee_attr;

	if(priv->marquee_timer == NULL)
	{
		priv->marquee_timer = ftk_source_timer_create(MARQUEE_MOVE_INTERVAL, marquee_timer_func, thiz);
		ftk_main_loop_add_source(ftk_default_main_loop(), priv->marquee_timer);
	}

	return RET_OK;
}

