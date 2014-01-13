/*
 * File: ftk_input_method_hw.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   hand write input method.
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
 * 2010-02-08 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_source_timer.h"
#include "ftk_stroke_painter.h"
#include "ftk_input_method_hw.h"
#include "ftk_handwrite_engine.h"

#define FTK_HW_UPDATE_SIZE   10
#define FTK_HW_STROKE_POINTS 1023
#define FTK_HW_CLICK_RANGE   FTK_HW_UPDATE_SIZE

typedef enum _HandWriteState
{
	FTK_HW_NONE = 0,
	FTK_HW_FIRST,
	FTK_HW_WRITING
}HandWriteState;

typedef struct _PrivInfo
{
	FtkRect rect;
	int pen_down;
	int click_timeout;
	int commit_timeout;
	
	FtkWidget*     editor;
	FtkInputType   input_type;
	FtkSource*     click_timer;
	FtkSource*     commit_timer;
	FtkStrokePainter* painter;
	FtkHandWriteEngine* engine;
	HandWriteState hand_write_state;
	
	int points_nr;
	FtkPoint   points[FTK_HW_STROKE_POINTS+1];
}PrivInfo;

static Ret  ftk_input_method_hw_end(FtkInputMethod* thiz);
static Ret  ftk_input_method_hw_reset(FtkInputMethod* thiz);
static Ret  ftk_input_method_hw_handle_event(FtkInputMethod* thiz, FtkEvent* event);

static Ret  ftk_input_method_hw_commit(FtkInputMethod* thiz)
{
	DECL_PRIV(thiz, priv);
	
	ftk_hand_write_engine_flush(priv->engine);
	
	ftk_input_method_hw_end(thiz);
	ftk_input_method_hw_reset(thiz);

	ftk_logd("%s:%d paints_nr=%d\n", __func__, __LINE__, priv->points_nr);

	return RET_REMOVE;
}

/*
 * guest the mouse event is a click event or handwrite stroke.
 * click: during specified time, the mouse don't move out of specified range.
 */
static Ret  ftk_input_method_hw_click_check(FtkInputMethod* thiz)
{
	int i = 0;
	DECL_PRIV(thiz, priv);

	if(priv->points_nr < 3)
	{
		priv->hand_write_state = FTK_HW_NONE;
	}
	else
	{
		priv->hand_write_state = FTK_HW_NONE;
		for(i = 1; i < priv->points_nr; i++)
		{
			if(FTK_ABS(priv->points[0].x - priv->points[i].x) > FTK_HW_CLICK_RANGE
				|| FTK_ABS(priv->points[0].y - priv->points[i].y) > FTK_HW_CLICK_RANGE)
			{
				priv->hand_write_state = FTK_HW_WRITING;
				break;
			}
		}
	}

	/*It is click event, re-dispatch the events.*/
	if(priv->hand_write_state == FTK_HW_NONE)
	{
		FtkEvent event = {0};
		int points_nr = priv->points_nr;
		
		ftk_wnd_manager_remove_global_listener(ftk_default_wnd_manager(), 
			(FtkListener)ftk_input_method_hw_handle_event, thiz);

		event.type = FTK_EVT_MOUSE_DOWN;
		event.u.mouse.x = priv->points[0].x;
		event.u.mouse.y = priv->points[0].y;
		ftk_wnd_manager_dispatch_event(ftk_default_wnd_manager(), &event);
		
		for(i = 1; i < (points_nr - 1); i++)
		{
			event.type = FTK_EVT_MOUSE_MOVE;
			event.u.mouse.x = priv->points[i].x;
			event.u.mouse.y = priv->points[i].y;
			ftk_wnd_manager_dispatch_event(ftk_default_wnd_manager(), &event);
		}
		
		if(points_nr > 1)
		{
			event.type = priv->pen_down ? FTK_EVT_MOUSE_MOVE : FTK_EVT_MOUSE_UP;
			event.u.mouse.x = priv->points[i].x;
			event.u.mouse.y = priv->points[i].y;
			ftk_wnd_manager_dispatch_event(ftk_default_wnd_manager(), &event);
		}
		ftk_wnd_manager_add_global_listener(ftk_default_wnd_manager(), 
			(FtkListener)ftk_input_method_hw_handle_event, thiz);

		ftk_input_method_hw_end(thiz);
		ftk_input_method_hw_reset(thiz);
		ftk_logd("%s: it is click event.\n", __func__);
	}
	else
	{
		ftk_stroke_painter_show(priv->painter, 1);
		ftk_stroke_painter_update(priv->painter, NULL);
		ftk_logd("%s: it is handwrite stroke\n", __func__);
	}

	return RET_REMOVE;
}

static Ret  ftk_input_method_hw_init(FtkInputMethod* thiz)
{
	DECL_PRIV(thiz, priv);
	
	if(priv->commit_timer != NULL) return RET_OK;

	ftk_stroke_painter_init(priv->painter);
	priv->commit_timer = ftk_source_timer_create(priv->commit_timeout, 
		(FtkTimer)ftk_input_method_hw_commit, thiz);
	priv->click_timer = ftk_source_timer_create(priv->click_timeout, 
		(FtkTimer)ftk_input_method_hw_click_check, thiz);

	return RET_OK;
}

static Ret  ftk_input_method_hw_reset(FtkInputMethod* thiz)
{
	DECL_PRIV(thiz, priv);

	priv->points_nr = 0;
	priv->hand_write_state = FTK_HW_NONE;

	ftk_main_loop_remove_source(ftk_default_main_loop(), priv->commit_timer);
	ftk_main_loop_remove_source(ftk_default_main_loop(), priv->click_timer);

	return RET_OK;
}

static Ret  ftk_input_method_hw_end(FtkInputMethod* thiz)
{
	DECL_PRIV(thiz, priv);

	ftk_stroke_painter_end(priv->painter);
	ftk_hand_write_engine_reset(priv->engine);

	return RET_OK;
}

static Ret  ftk_input_method_hw_activate(FtkInputMethod* thiz)
{
	DECL_PRIV(thiz, priv);

	ftk_input_method_hw_init(thiz);
	ftk_input_method_hw_reset(thiz);
	ftk_hand_write_engine_activate(priv->engine);

	ftk_wnd_manager_add_global_listener(ftk_default_wnd_manager(),
		(FtkListener)ftk_input_method_hw_handle_event, thiz);

	ftk_logd("%s\n", __func__);

	return RET_OK;
}

static Ret  ftk_input_method_hw_deactivate(FtkInputMethod* thiz)
{
	DECL_PRIV(thiz, priv);

	ftk_input_method_hw_end(thiz);
	ftk_input_method_hw_reset(thiz);
	ftk_hand_write_engine_deactivate(priv->engine);
	
	ftk_wnd_manager_remove_global_listener(ftk_default_wnd_manager(), 
		(FtkListener)ftk_input_method_hw_handle_event, thiz);

	ftk_logd("%s\n", __func__);

	return RET_OK;
}

static Ret  ftk_input_method_hw_focus_in(FtkInputMethod* thiz, FtkWidget* editor)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && editor != NULL, RET_FAIL);

	priv->editor = editor;

	return ftk_input_method_hw_activate(thiz);
}

static Ret  ftk_input_method_hw_focus_out(FtkInputMethod* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);

	priv->editor = NULL;
	
	return ftk_input_method_hw_deactivate(thiz);
}

static Ret  ftk_input_method_hw_set_type(FtkInputMethod* thiz, FtkInputType input_type)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);

	priv->input_type = input_type;

	return RET_OK;
}

static Ret  ftk_input_method_hw_add_point(FtkInputMethod* thiz, int x, int y)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv->points_nr < FTK_HW_STROKE_POINTS, RET_FAIL);
	
	if(!(x >= priv->rect.x && x < (priv->rect.x + priv->rect.width)
		&& y >= priv->rect.y && y < (priv->rect.y + priv->rect.height)))
	{
		return RET_OK;
	}

	priv->points[priv->points_nr].x = x;
	priv->points[priv->points_nr].y = y;
	
	if(priv->points_nr == 0)
	{
		ftk_stroke_painter_move_to(priv->painter, x, y);
	}
	else
	{
		int x2 = x;
		int y2 = y;
		FtkRect rect = {0};
		int x1 = priv->points[priv->points_nr-1].x;
		int y1 = priv->points[priv->points_nr-1].y;

		ftk_stroke_painter_move_to(priv->painter, x1, y1);
		ftk_stroke_painter_line_to(priv->painter, x2, y2);

		x1 = FTK_MIN(x1, x2);
		x2 = FTK_MAX(x1, x2);
		y1 = FTK_MIN(y1, y2);
		y2 = FTK_MAX(y1, y2);

		x1 = x1 < 0 ? 0 : x1;
		y1 = y1 < 0 ? 0 : y1;

		rect.x = x1 - FTK_HW_UPDATE_SIZE;
		rect.y = y1 - FTK_HW_UPDATE_SIZE;
		rect.width  = x2 - x1 + (FTK_HW_UPDATE_SIZE << 1);
		rect.height = y2 - y1 + (FTK_HW_UPDATE_SIZE << 1);

		ftk_stroke_painter_update(priv->painter, &rect);
	}
	priv->points_nr++;

	return RET_REMOVE;
}

static Ret  ftk_input_method_hw_handle_event(FtkInputMethod* thiz, FtkEvent* event)
{
	Ret ret = RET_OK;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && event != NULL, RET_FAIL);
	
	switch(event->type)
	{
		case FTK_EVT_MOUSE_DOWN:
		{
			if(priv->hand_write_state == FTK_HW_NONE)
			{
				priv->hand_write_state = FTK_HW_FIRST;
				ftk_stroke_painter_begin(priv->painter);
			}
			else if(priv->hand_write_state == FTK_HW_FIRST)
			{
				priv->hand_write_state = FTK_HW_WRITING;
			}

			priv->pen_down = 1;
			priv->points_nr = 0;
			
			ftk_source_disable(priv->commit_timer);
			ftk_main_loop_remove_source(ftk_default_main_loop(), priv->commit_timer);
			ret = ftk_input_method_hw_add_point(thiz, event->u.mouse.x, event->u.mouse.y);

			if(priv->hand_write_state == FTK_HW_FIRST)
			{
				ftk_source_timer_reset(priv->click_timer);
				ftk_main_loop_remove_source(ftk_default_main_loop(), priv->click_timer);
				if(ftk_main_loop_add_source(ftk_default_main_loop(), priv->click_timer) == RET_OK)
				{
					ftk_source_ref(priv->click_timer);
				}
			}

			ftk_logd("FTK_EVT_MOUSE_DOWN: %d\n", priv->points_nr);
			break;
		}
		case FTK_EVT_MOUSE_MOVE:
		{
			if(priv->pen_down && priv->hand_write_state != FTK_HW_NONE)
			{
				ret = ftk_input_method_hw_add_point(thiz, event->u.mouse.x, event->u.mouse.y);
			}

			break;
		}
		case FTK_EVT_MOUSE_UP:
		{
			priv->pen_down = 0;
			if(priv->hand_write_state != FTK_HW_NONE)
			{
				ret = ftk_input_method_hw_add_point(thiz, event->u.mouse.x, event->u.mouse.y);

				ftk_source_enable(priv->commit_timer);
				ftk_source_timer_reset(priv->commit_timer);
				if(ftk_main_loop_add_source(ftk_default_main_loop(), priv->commit_timer) == RET_OK)
				{
					ftk_source_ref(priv->commit_timer);
				}
	
				if(priv->points_nr < 5)
				{
					ret = RET_OK;
					break;
				}
				else
				{
					ftk_hand_write_engine_add_stroke(priv->engine, priv->points, priv->points_nr);
				}
			}
			ftk_logd("FTK_EVT_MOUSE_UP: %d\n", priv->points_nr);

			break;
		}
		case FTK_EVT_IM_ACT_COMMIT:
		{
			ftk_input_method_hw_end(thiz);
			ftk_input_method_hw_reset(thiz);

			break;
		}
		default:break;
	}

	return ret;
}

static void ftk_input_method_hw_destroy(FtkInputMethod* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		ftk_source_unref(priv->commit_timer);
		ftk_source_unref(priv->click_timer);
		ftk_stroke_painter_destroy(priv->painter);
		ftk_hand_write_engine_destroy(priv->engine);
	
		FTK_ZFREE(thiz, sizeof(FtkInputMethod) + sizeof(PrivInfo));
	}

	return;
}

static Ret on_handwrite_result(void* ctx, FtkCommitInfo* info)
{
	FtkInputMethod* thiz = ctx;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(priv->editor != NULL)
	{
		FtkEvent evt = {0};
		evt.type = FTK_EVT_IM_PREEDIT;
		evt.u.extra = info;
		evt.widget = priv->editor;

		ftk_widget_event(priv->editor, &evt);
	}

	return RET_OK;
}

FtkInputMethod* ftk_input_method_hw_create(void)
{
	FtkInputMethod* thiz = FTK_ZALLOC(sizeof(FtkInputMethod) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		FtkColor c = {0};
		DECL_PRIV(thiz, priv);

		thiz->ref  = 1;
		thiz->name = _("Hand Write");	
		thiz->set_type     = ftk_input_method_hw_set_type;
		thiz->focus_in     = ftk_input_method_hw_focus_in;
		thiz->focus_out    = ftk_input_method_hw_focus_out;
		thiz->handle_event = ftk_input_method_hw_handle_event;
		thiz->destroy      = ftk_input_method_hw_destroy;

		c.r = 0xff;
		c.b = 0xff;
		priv->painter = ftk_stroke_painter_create();
		priv->engine = ftk_hand_write_engine_create(on_handwrite_result, thiz);
		ftk_input_method_hw_set_rect(thiz, NULL);
		ftk_input_method_hw_set_pen_width(thiz, 3);
		ftk_input_method_hw_set_pen_color(thiz, c);
		ftk_input_method_hw_set_click_timeout(thiz, 300);
		ftk_input_method_hw_set_commit_timeout(thiz, 1500);
	}

	return thiz;
}

Ret ftk_input_method_hw_set_rect(FtkInputMethod* thiz, FtkRect* rect)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);

	if(rect != NULL)
	{
		priv->rect = *rect;
	}
	else
	{
		ftk_wnd_manager_get_work_area(ftk_default_wnd_manager(), &(priv->rect));
	}
	ftk_stroke_painter_set_rect(priv->painter, &(priv->rect));
	ftk_hand_write_engine_set_rect(priv->engine, &(priv->rect));

	return RET_OK;
}

Ret ftk_input_method_hw_set_pen_width(FtkInputMethod* thiz, int width)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && width > 0, RET_FAIL);

	ftk_stroke_painter_set_pen_width(priv->painter, width);

	return RET_OK;
}

Ret ftk_input_method_hw_set_pen_color(FtkInputMethod* thiz, FtkColor color)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);

	ftk_stroke_painter_set_pen_color(priv->painter, color);

	return RET_OK;
}

Ret ftk_input_method_hw_set_click_timeout(FtkInputMethod* thiz, int ms)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && ms > 0, RET_FAIL);
	
	priv->click_timeout = ms;

	return RET_OK;
}

Ret ftk_input_method_hw_set_commit_timeout(FtkInputMethod* thiz, int ms)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && ms > 0, RET_FAIL);
	
	priv->commit_timeout = ms;

	return RET_OK;
}

