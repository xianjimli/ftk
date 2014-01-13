/*
 * File: ftk_wnd_manager.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   a simple window manager. 
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
 * 2009-10-03 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_globals.h"
#include "ftk_status_panel.h"
#include "ftk_source_timer.h"
#include "ftk_source_primary.h"
#include "ftk_wnd_manager_default.h"

#define FTK_MAX_GLOBAL_LISTENER 12

typedef struct _WinManagerPrivInfo
{
	int top;
	int pressed;
	int caplock;
	int shift_down;
	int dieing;
	FtkEvent pressed_event;
	FtkMainLoop* main_loop;
	FtkWidget*   grab_widget;
	FtkWidget*   focus_widget;
	FtkWidget*   windows[FTK_MAX_WINDOWS];

	FtkWidget* top_window;
	FtkSource* long_press_timer;
	void* global_listeners_ctx[FTK_MAX_GLOBAL_LISTENER];
	FtkListener global_listeners[FTK_MAX_GLOBAL_LISTENER];

	int disable_anim;
}PrivInfo;

static int  ftk_wnd_manager_default_has_fullscreen_win(FtkWndManager* thiz);
static Ret  ftk_wnd_manager_default_map_panels(FtkWndManager* thiz, int map);
static Ret ftk_wnd_manager_default_emit_top_wnd_changed(FtkWndManager* thiz);

static Ret  ftk_wnd_manager_default_restack(FtkWndManager* thiz, FtkWidget* window, int offset)
{
	int index = 0;
	int new_index = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(window != NULL && offset != 0, RET_FAIL);

	for(index = 0; index < priv->top; index++)
	{
		if(priv->windows[index] == window)
		{
			break;
		}
	}
	return_val_if_fail(index < priv->top, RET_FAIL);

	new_index = index + offset;
	new_index = new_index < 0 ? 0 : new_index;
	new_index = new_index >= priv->top ? (priv->top - 1) : new_index;

	if(index == new_index)
	{
		return RET_OK;
	}

	if(index < new_index)
	{
		for(;index < new_index; index++)
		{
			priv->windows[index] = priv->windows[index+1];
		}
	}
	else
	{
		for(; index > new_index; index--)
		{
			priv->windows[index] = priv->windows[index-1];
		}
	}
	priv->windows[new_index] = window;
	
	if(!priv->dieing)
	{
		ftk_wnd_manager_default_emit_top_wnd_changed(thiz);
		ftk_wnd_manager_update(thiz);
	}

	return RET_OK;
}

static Ret  ftk_wnd_manager_default_grab(FtkWndManager* thiz, FtkWidget* window)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	priv->grab_widget = window;

	return RET_OK;
}

static Ret  ftk_wnd_manager_default_ungrab(FtkWndManager* thiz, FtkWidget* window)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(priv->grab_widget == window)
	{
		priv->grab_widget  = NULL;
	}

	return RET_OK;
}

static Ret ftk_wnd_manager_default_emit_top_wnd_changed(FtkWndManager* thiz)
{
	int i = 0;
	FtkEvent event;
	DECL_PRIV(thiz, priv);
	FtkWidget* win = NULL;
	ftk_event_init(&event, FTK_EVT_TOP_WND_CHANGED);
	/*find the topest app/dialog window*/
	for(i = priv->top - 1; i >=0; i--)
	{
		win = priv->windows[i];
		if((ftk_widget_type(win) == FTK_WINDOW 
			|| ftk_widget_type(win) == FTK_DIALOG
			|| ftk_widget_type(win) == FTK_WINDOW_MISC)
			&& (ftk_widget_is_visible(win)))
		{
			priv->focus_widget = win;
			break;
		}
		else
		{
			win = NULL;
		}
	}

	event.widget = win;
	ftk_wnd_manager_dispatch_event(thiz, &event);
	
	if(priv->top_window != NULL)
	{
		ftk_event_init(&event, FTK_EVT_UNMAP);
		event.widget = priv->top_window;
		ftk_wnd_manager_dispatch_event(thiz, &event);
	}
	priv->top_window = win;
	if(priv->top_window != NULL)
	{
		ftk_event_init(&event, FTK_EVT_MAP);
		event.widget = priv->top_window;
		ftk_wnd_manager_dispatch_event(thiz, &event);
	}

	return RET_OK;
}

static int ftk_wnd_manager_get_status_bar_height(FtkWndManager* thiz)
{
	(void)thiz;
	if(ftk_default_status_panel() != NULL)
	{
		return ftk_widget_height(ftk_default_status_panel());
	}
	else
	{
		return 0;
	}
}

static inline Ret  ftk_wnd_manager_default_get_work_area(FtkWndManager* thiz, FtkRect* rect)
{
	return_val_if_fail(thiz != NULL && rect != NULL, RET_FAIL);

	rect->x = 0;
	rect->y = ftk_wnd_manager_get_status_bar_height(thiz);
	rect->width = ftk_display_width(ftk_default_display());
	rect->height = ftk_display_height(ftk_default_display()) - rect->y;

	return RET_OK;
}

static Ret  ftk_wnd_manager_default_relayout_one(FtkWndManager* thiz, FtkWidget* window)
{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	int work_area_h = 0;
	FtkEvent event;
	return_val_if_fail(thiz != NULL && window != NULL, RET_FAIL);

	work_area_h = ftk_display_height(ftk_default_display()) - ftk_wnd_manager_get_status_bar_height(thiz);
	/*XXX: we assume panel is added as first window*/
	switch(ftk_widget_type(window))
	{
		case FTK_WINDOW:
		{
			w = ftk_display_width(ftk_default_display());
			h = ftk_display_height(ftk_default_display());

			if(!ftk_window_is_fullscreen(window))
			{
				h -= ftk_wnd_manager_get_status_bar_height(thiz);
				y = ftk_wnd_manager_get_status_bar_height(thiz);
			}

			break;
		}
		case FTK_DIALOG:
		{
			if(ftk_widget_has_attr(window, FTK_ATTR_AUTO_LAYOUT))
			{
				x = FTK_DIALOG_MARGIN;
				w = ftk_display_width(ftk_default_display()) - FTK_DIALOG_MARGIN * 2; 
				h = work_area_h < ftk_widget_height(window) ? work_area_h : ftk_widget_height(window);
				y = (work_area_h - h) / 2;
				
				if(!ftk_wnd_manager_default_has_fullscreen_win(thiz))
				{
					y += ftk_wnd_manager_get_status_bar_height(thiz);
				}
			}
			else
			{
				x = ftk_widget_left_abs(window);
				y = ftk_widget_top_abs(window);
				w = ftk_widget_width(window);
				h = ftk_widget_height(window);
			}
			break;
		}
		case FTK_STATUS_PANEL:
		{
			ftk_event_init(&event, FTK_EVT_MAP);
			event.widget = window;
			ftk_wnd_manager_dispatch_event(thiz, &event);
			w = ftk_display_width(ftk_default_display());
			h = FTK_STATUS_PANEL_HEIGHT;

			break;
		}
		case FTK_MENU_PANEL:
		{
			ftk_event_init(&event, FTK_EVT_MAP);
			event.widget = window;
			ftk_wnd_manager_dispatch_event(thiz, &event);
			w = ftk_display_width(ftk_default_display());
			h = ftk_widget_height(window);
			x = 0;
			y = ftk_display_height(ftk_default_display()) - h;

			break;
		}
		default:
		{
			x = ftk_widget_left_abs(window);
			y = ftk_widget_top_abs(window);
			w = ftk_widget_width(window);
			h = ftk_widget_height(window);

			break;
		}
	}
	
	ftk_widget_move_resize(window, x, y, w, h);
	ftk_logd("type=%d %d %d %d %d\n", ftk_widget_type(window), x, y, w, h);

	return RET_OK;
}

static Ret  ftk_wnd_manager_default_relayout(FtkWndManager* thiz)
{
	int i = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	for(i = 0; i < priv->top; i++)
	{
		ftk_wnd_manager_default_relayout_one(thiz, priv->windows[i]);
	}

	ftk_wnd_manager_update(thiz);

	return RET_OK;
}

static Ret  ftk_wnd_manager_default_add(FtkWndManager* thiz, FtkWidget* window)
{
	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && window != NULL, RET_FAIL);
	return_val_if_fail((priv->top+1) < FTK_MAX_WINDOWS, RET_FAIL);

	priv->windows[priv->top++] = window;

	priv->disable_anim++;
	ret = ftk_wnd_manager_default_relayout_one(thiz, window);
	priv->disable_anim--;

	return ret;
}

static Ret  ftk_wnd_manager_default_remove(FtkWndManager* thiz, FtkWidget* window)
{
	int i = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && window != NULL, RET_FAIL);
	if(priv->dieing && priv->top <= 0)
	{
		return RET_OK;
	}
	return_val_if_fail(priv->top > 0, RET_FAIL);

	if(priv->grab_widget == window)
	{
		priv->grab_widget = NULL;
	}
	
	if(priv->focus_widget == window)
	{
		priv->focus_widget = NULL;
	}

	if(priv->windows[priv->top - 1] == window)
	{
		priv->top--;
		priv->windows[priv->top] = NULL;
	}
	else
	{
		for(i = 0; i < priv->top; i++)
		{
			if(priv->windows[i] == window)
			{
				for(; i < priv->top; i++)
				{
					priv->windows[i] = priv->windows[i+1];
				}
				priv->top--;
				break;
			}
		}
	}

	if(!priv->dieing)
	{
		ftk_wnd_manager_default_emit_top_wnd_changed(thiz);
		ftk_wnd_manager_update(thiz);
	}

	return RET_OK;
}

static Ret  ftk_wnd_manager_dispatch_globals(FtkWndManager* thiz, FtkEvent* event)
{
	int i = 0;
	Ret ret = RET_OK;
	DECL_PRIV(thiz, priv);

	for(i = 0; i < FTK_MAX_GLOBAL_LISTENER; i++)
	{
		if(priv->global_listeners[i] != NULL)
		{
			ret = priv->global_listeners[i](priv->global_listeners_ctx[i], event);
			if(ret == RET_REMOVE)
			{
				return ret;
			}
		}
	}

	return ret;
}

static FtkWidget* ftk_wnd_manager_find_target(FtkWndManager* thiz, int x, int y)
{
	int i = 0;
	int top = 0;
	int left = 0;
	int w = 0;
	int h = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && priv->top > 0, NULL);	

	i = priv->top;
	for(; i > 0; i--)
	{
		FtkWidget* win = priv->windows[i-1];
		if(!ftk_widget_is_visible(win) || !ftk_window_is_mapped(win))
		{
			continue;
		}
	
		top = ftk_widget_top_abs(win);
		left = ftk_widget_left_abs(win);
		w = ftk_widget_width(win);
		h = ftk_widget_height(win);

		if(x >= left && y >= top && x < (left  + w)	&& y < (top + h))
		{
			return win;
		}
	}

	return NULL;
}

#if 0 
static const unsigned char const key_tanslate_table[0xff] = 
{
	[FTK_KEY_1]             =  FTK_KEY_EXCLAM,
	[FTK_KEY_2]             =  FTK_KEY_AT,
	[FTK_KEY_3]             =  FTK_KEY_NUMBERSIGN,
	[FTK_KEY_4]             =  FTK_KEY_DOLLAR,
	[FTK_KEY_5]             =  FTK_KEY_PERCENT,
	[FTK_KEY_6]             =  FTK_KEY_ASCIICIRCUM,
	[FTK_KEY_7]             =  FTK_KEY_AMPERSAND,
	[FTK_KEY_8]             =  FTK_KEY_ASTERISK,
	[FTK_KEY_9]             =  FTK_KEY_PARENLEFT,
	[FTK_KEY_0]             =  FTK_KEY_PARENRIGHT,
	[FTK_KEY_MINUS]         =  FTK_KEY_UNDERSCORE,
	[FTK_KEY_EQUAL]         =  FTK_KEY_PLUS,
	[FTK_KEY_COMMA]         =  FTK_KEY_LESS,
	[FTK_KEY_DOT]           =  FTK_KEY_GREATER,
	[FTK_KEY_SLASH]         =  FTK_KEY_QUESTION,
	[FTK_KEY_BACKSLASH]     =  FTK_KEY_OR,
	[FTK_KEY_BRACKETLEFT]   =  FTK_KEY_LEFTBRACE,
	[FTK_KEY_BRACKETRIGHT]  =  FTK_KEY_RIGHTBRACE,
	[FTK_KEY_GRAVE]         =  FTK_KEY_NOT,
	[FTK_KEY_SEMICOLON]     =  FTK_KEY_COLON,
	[FTK_KEY_QUOTERIGHT]    =  FTK_KEY_QUOTEDBL,
};
#else
static unsigned char key_tanslate_table[0xff] = {0};
#ifndef FTK_SUPPORT_C99
static void key_tanslate_table_init(void)
{
	key_tanslate_table[FTK_KEY_1]             =  FTK_KEY_EXCLAM;
	key_tanslate_table[FTK_KEY_2]             =  FTK_KEY_AT;
	key_tanslate_table[FTK_KEY_3]             =  FTK_KEY_NUMBERSIGN;
	key_tanslate_table[FTK_KEY_4]             =  FTK_KEY_DOLLAR;
	key_tanslate_table[FTK_KEY_5]             =  FTK_KEY_PERCENT;
	key_tanslate_table[FTK_KEY_6]             =  FTK_KEY_ASCIICIRCUM;
	key_tanslate_table[FTK_KEY_7]             =  FTK_KEY_AMPERSAND;
	key_tanslate_table[FTK_KEY_8]             =  FTK_KEY_ASTERISK;
	key_tanslate_table[FTK_KEY_9]             =  FTK_KEY_PARENLEFT;
	key_tanslate_table[FTK_KEY_0]             =  FTK_KEY_PARENRIGHT;
	key_tanslate_table[FTK_KEY_MINUS]         =  FTK_KEY_UNDERSCORE;
	key_tanslate_table[FTK_KEY_EQUAL]         =  FTK_KEY_PLUS;
	key_tanslate_table[FTK_KEY_COMMA]         =  FTK_KEY_LESS;
	key_tanslate_table[FTK_KEY_DOT]           =  FTK_KEY_GREATER;
	key_tanslate_table[FTK_KEY_SLASH]         =  FTK_KEY_QUESTION;
	key_tanslate_table[FTK_KEY_BACKSLASH]     =  FTK_KEY_OR;
	key_tanslate_table[FTK_KEY_BRACKETLEFT]   =  FTK_KEY_LEFTBRACE;
	key_tanslate_table[FTK_KEY_BRACKETRIGHT]  =  FTK_KEY_RIGHTBRACE;
	key_tanslate_table[FTK_KEY_GRAVE]         =  FTK_KEY_NOT;
	key_tanslate_table[FTK_KEY_SEMICOLON]     =  FTK_KEY_COLON;
	key_tanslate_table[FTK_KEY_QUOTERIGHT]    =  FTK_KEY_QUOTEDBL;

	return;
}
#endif /* Not FTK_SUPPORT_C99 */
#endif

static Ret  ftk_wnd_manager_default_key_translate(FtkWndManager* thiz, FtkEvent* event)
{
	DECL_PRIV(thiz, priv);
	if(event->type == FTK_EVT_KEY_DOWN)
	{
		if(event->u.key.code == FTK_KEY_LEFTSHIFT || event->u.key.code == FTK_KEY_RIGHTSHIFT)
		{
			priv->shift_down = 1;
		}
	}

	if(event->type == FTK_EVT_KEY_UP)
	{
		if(event->u.key.code == FTK_KEY_CAPSLOCK)
		{
			priv->caplock = !priv->caplock;
		}

		if(event->u.key.code == FTK_KEY_LEFTSHIFT || event->u.key.code == FTK_KEY_RIGHTSHIFT)
		{
			priv->shift_down = 0;
		}
	}

	/*CAPLOCK translate*/
	if(event->u.key.code >= FTK_KEY_a && event->u.key.code <= FTK_KEY_z)
	{
		event->u.key.code = priv->caplock ? event->u.key.code - FTK_KEY_a + FTK_KEY_A : event->u.key.code;
	}
	
	/*SHIFT translate*/
	if(event->u.key.code >= FTK_KEY_a && event->u.key.code <= FTK_KEY_z)
	{
		event->u.key.code = priv->shift_down ? event->u.key.code - FTK_KEY_a + FTK_KEY_A : event->u.key.code;
	}
	else if(event->u.key.code >= FTK_KEY_A && event->u.key.code <= FTK_KEY_Z)
	{
		event->u.key.code = priv->shift_down ? event->u.key.code - FTK_KEY_A + FTK_KEY_a : event->u.key.code;
	}
	else if(event->u.key.code < 0xff)
	{
		if(priv->shift_down && key_tanslate_table[event->u.key.code & 0xff]) 
		{
			event->u.key.code = key_tanslate_table[event->u.key.code & 0xff];
		}
	}

	return RET_OK;
}

static Ret  ftk_wnd_manager_default_do_animation(FtkWndManager* thiz, FtkEvent* event)
{
	DECL_PRIV(thiz, priv);
	if(event->type == FTK_EVT_SHOW || event->type == FTK_EVT_HIDE)
	{
		int i = 0;
		FtkWidget* peer_win = NULL;
		FtkAnimationEvent anim_event;
	
		memset(&anim_event, 0x00, sizeof(anim_event));
		anim_event.type = event->type;
		for(i = (priv->top - 1); i >= 0; i--)
		{
			FtkWidget* win = priv->windows[i];
			if(!ftk_widget_is_visible(win))
			{
				continue;
			}
		
			if(win == event->widget)
			{
				continue;
			}
			
			/*FIXME: If the top window is dialog, we disable animation.*/
			if(ftk_widget_type(win) == FTK_DIALOG)
			{
				return RET_OK;
			}
			
			if(ftk_widget_type(win) != FTK_WINDOW)
			{
				continue;
			}

			peer_win = win;
			break;
		}

		if(event->type == FTK_EVT_SHOW)
		{
			anim_event.old_window = peer_win;
			anim_event.new_window = (FtkWidget*)event->widget;
		}
		else
		{
			anim_event.new_window = peer_win;
			anim_event.old_window = (FtkWidget*)event->widget;
		}

		if(anim_event.new_window != NULL && anim_event.old_window != NULL)
		{
			FtkEvent e;
			ftk_event_init(&e, FTK_EVT_DISABLE_CURSOR);
			ftk_wnd_manager_dispatch_event(thiz, &e);
			ftk_animation_trigger_on_event(ftk_default_animation_trigger(), &anim_event);
			ftk_event_init(&e, FTK_EVT_ENABLE_CURSOR);
			ftk_wnd_manager_dispatch_event(thiz, &e);
		}
	}

	return RET_OK;
}

static Ret  ftk_wnd_manager_default_dispatch_event(FtkWndManager* thiz, FtkEvent* event)
{
	DECL_PRIV(thiz, priv);
	FtkWidget* target = NULL;
	return_val_if_fail(thiz != NULL && event != NULL, RET_FAIL);

	if(priv->dieing)
	{
		return RET_OK;
	}

	if(event->type == FTK_EVT_KEY_DOWN || event->type == FTK_EVT_KEY_UP)
	{
		ftk_wnd_manager_default_key_translate(thiz, event);
	}

	if(ftk_wnd_manager_dispatch_globals(thiz, event) != RET_OK)
	{
		return RET_REMOVE;
	}

	switch(event->type)
	{
		case FTK_EVT_DISPLAY_CHANGED:
		{
			int width = event->u.display.width;
			int height = event->u.display.height;
			FtkCanvas* canvas = ftk_shared_canvas();
			
			if(canvas == NULL) 
			{
				/*not init yet.*/
				break;
			}

			if(canvas->width != width || canvas->height != height)
			{
				FtkColor c = {0xff, 0xff, 0xff, 0xff};
				ftk_canvas_destroy(canvas);
				
				canvas = ftk_canvas_create(width, height, &c);
				ftk_set_shared_canvas(canvas);
				ftk_wnd_manager_default_relayout(thiz);

				ftk_logi("%s:%d FTK_EVT_DISPLAY_CHANGED: %d %d\n", __func__, __LINE__, width, height);
			}

			break;
		}
		case FTK_EVT_WND_DESTROY:
		{
			if(priv->top_window == event->widget)
			{
				priv->top_window = NULL;
			}

			priv->disable_anim++;
			ftk_wnd_manager_default_remove(thiz, (FtkWidget*)event->widget);	
			priv->disable_anim--;
			return RET_OK;
		}
		case FTK_EVT_HIDE:
		{
			priv->disable_anim++;
			ftk_wnd_manager_default_emit_top_wnd_changed(thiz);
			priv->disable_anim--;
			ftk_wnd_manager_default_do_animation(thiz, event);
			ftk_wnd_manager_update(thiz);
			return RET_OK;
		}
		case FTK_EVT_SHOW:
		{
			priv->disable_anim++;
			ftk_wnd_manager_default_emit_top_wnd_changed(thiz);
			priv->disable_anim--;
			ftk_wnd_manager_default_do_animation(thiz, event);
			if(ftk_widget_type((FtkWidget*)event->widget) == FTK_WINDOW 
				&& ftk_wnd_manager_default_map_panels(thiz, !ftk_wnd_manager_default_has_fullscreen_win(thiz)) == RET_OK)
			{
				ftk_wnd_manager_update(thiz);
			}
			return RET_OK;
		}
		case FTK_EVT_RELAYOUT_WND:
		{
			ftk_wnd_manager_default_relayout(thiz);
			break;
		}
		default:break;
	}

	if(event->type == FTK_EVT_MOUSE_DOWN || event->type == FTK_EVT_KEY_DOWN)
	{
		priv->pressed_event = *event;
		ftk_source_ref(priv->long_press_timer);
		ftk_source_timer_reset(priv->long_press_timer);
		ftk_main_loop_add_source(ftk_default_main_loop(), priv->long_press_timer);
	}
	
	if(event->type == FTK_EVT_MOUSE_UP|| event->type == FTK_EVT_KEY_UP)
	{
		ftk_main_loop_remove_source(ftk_default_main_loop(), priv->long_press_timer);
	}

	if((event->type == FTK_EVT_MOUSE_DOWN 
		|| event->type == FTK_EVT_MOUSE_UP
		|| event->type == FTK_EVT_MOUSE_MOVE) && priv->grab_widget == NULL)
	{
		int x = event->u.mouse.x;
		int y = event->u.mouse.y;
		
		target = ftk_wnd_manager_find_target(thiz, x, y);
		if(event->type == FTK_EVT_MOUSE_DOWN && !ftk_widget_has_attr(target, FTK_ATTR_NO_FOCUS))
		{
			priv->focus_widget = target;
		}
	}
	else if(event->widget != NULL)
	{
		target = (FtkWidget*)event->widget;
	}
	else if(priv->grab_widget != NULL)
	{
		target = priv->grab_widget;
	}
	else if(priv->focus_widget != NULL)
	{
		target = priv->focus_widget;
	}
	else if(priv->top > 0)
	{
		target = priv->windows[priv->top - 1];
	}

	if(target != NULL && !ftk_widget_is_insensitive(target))
	{
		ftk_widget_ref(target);
		ftk_widget_event(target, event);
		ftk_widget_unref(target);
	}

	return RET_OK;
}

static int  ftk_wnd_manager_default_has_fullscreen_win(FtkWndManager* thiz)
{
	int i = 0;
	FtkWidget* win = NULL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && priv->top > 0, 0);
	
	for(i = 0; i < priv->top; i++)
	{
		win = priv->windows[i];

		if(ftk_widget_is_visible(win) && ftk_window_is_fullscreen(win) && ftk_window_is_mapped(win))
		{
			return 1;
		}
	}

	return 0;
}

static Ret  ftk_wnd_manager_default_map_panels(FtkWndManager* thiz, int map)
{
	int i = 0;
	Ret ret = RET_IGNORED;
	FtkEvent event;
	FtkWidget* win = NULL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && priv->top > 0, RET_FAIL);
	
	for(i = 0; i < priv->top; i++)
	{
		win = priv->windows[i];
		if(ftk_widget_type(win) == FTK_STATUS_PANEL && ftk_widget_is_visible(win))
		{
			if(!(map && ftk_window_is_mapped(win)))
			{
				ftk_event_init(&event, map ? FTK_EVT_MAP : FTK_EVT_UNMAP);
				event.widget = win;
				ftk_wnd_manager_dispatch_event(thiz, &event);
				ret = RET_OK;
			}
			ftk_widget_invalidate(win);
		}
	}

	return ret;
}

static Ret  ftk_wnd_manager_default_update(FtkWndManager* thiz)
{
	int i = 0;
	FtkWidget* win = NULL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(priv->top == 0)
	{
		return RET_OK;
	}

	ftk_wnd_manager_default_map_panels(thiz, !ftk_wnd_manager_default_has_fullscreen_win(thiz));

	for(i = priv->top; i > 0; i--)
	{
		win = priv->windows[i - 1];
		if(ftk_widget_type(win) == FTK_WINDOW && ftk_widget_is_visible(win))
		{
			i--;
			break;
		}
	}
	
	for(; i < priv->top; i++)
	{
		win = priv->windows[i];

		if(ftk_widget_is_visible(win))
		{
			ftk_window_paint_forcely(win);
			if(ftk_window_is_fullscreen(win) && ftk_window_is_mapped(win))
			{
				return RET_OK;
			}
		}
	}
	
	for(i = 0; i < priv->top; i++)
	{
		win = priv->windows[i];
		if((ftk_widget_type(win) == FTK_STATUS_PANEL || ftk_widget_type(win) == FTK_MENU_PANEL)
			&& ftk_widget_is_visible(win))
		{
			ftk_window_paint_forcely(win);
		}
	}

	return RET_OK;
}

static Ret  ftk_wnd_manager_default_queue_event(FtkWndManager* thiz, FtkEvent* event)
{
	return_val_if_fail(thiz != NULL && event != NULL, RET_FAIL);
	return_val_if_fail(ftk_default_main_loop() != NULL, RET_FAIL);

	if(event->time == 0)
	{
		event->time = ftk_get_relative_time();
	}

	return ftk_source_queue_event(ftk_primary_source(), event);
}

static Ret  ftk_wnd_manager_default_add_global_listener(FtkWndManager* thiz, FtkListener listener, void* ctx)
{
	int i = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && listener != NULL, RET_FAIL);

	for(i = 0; i < FTK_MAX_GLOBAL_LISTENER; i++)
	{
		if(priv->global_listeners[i] == NULL)
		{
			priv->global_listeners[i] = listener;
			priv->global_listeners_ctx[i] = ctx;

			return RET_OK;
		}
	}

	return RET_OUT_OF_SPACE;
}

static Ret  ftk_wnd_manager_default_remove_global_listener(FtkWndManager* thiz, FtkListener listener, void* ctx)
{
	int i = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && listener != NULL, RET_FAIL);

	for(i = 0; i < FTK_MAX_GLOBAL_LISTENER; i++)
	{
		if(priv->global_listeners[i] == listener && priv->global_listeners_ctx[i] == ctx)
		{
			priv->global_listeners[i] = NULL;
			priv->global_listeners_ctx[i] = NULL;
		}
	}

	return RET_OK;
}

static void ftk_wnd_manager_default_destroy(FtkWndManager* thiz)
{
	int i = 0;
	DECL_PRIV(thiz, priv);
	if(thiz != NULL)
	{
		int nr = priv->top;
		priv->dieing = 1;
		for(i = 0; i < nr; i++)
		{
			FtkWidget* win = priv->windows[0];
			ftk_wnd_manager_remove(thiz, win);
			ftk_widget_unref(win);
		}

		ftk_source_unref(priv->long_press_timer);
		FTK_ZFREE(thiz, sizeof(FtkWndManager) + sizeof(PrivInfo));
	}

	return;
}

static Ret ftk_wnd_manager_default_long_press(FtkWndManager* thiz)
{
	FtkEvent event;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, RET_REMOVE);

	event = priv->pressed_event;
	event.type = (event.type == FTK_EVT_MOUSE_DOWN) ? FTK_EVT_MOUSE_LONG_PRESS : FTK_EVT_KEY_LONG_PRESS;

	ftk_wnd_manager_default_dispatch_event(thiz, &event);

	ftk_logd("%s: %d\n", __func__, event.type);

	return RET_REMOVE;
}

FtkWndManager* ftk_wnd_manager_default_create(FtkMainLoop* main_loop)
{
	FtkWndManager* thiz = NULL;
	return_val_if_fail(main_loop != NULL, NULL);

	if((thiz = (FtkWndManager*)FTK_ZALLOC(sizeof(FtkWndManager) + sizeof(PrivInfo))) != NULL)
	{
		DECL_PRIV(thiz, priv);

		priv->main_loop = main_loop;
		priv->long_press_timer = ftk_source_timer_create(1500, (FtkTimer)ftk_wnd_manager_default_long_press, thiz);
		ftk_set_primary_source(ftk_source_primary_create((FtkOnEvent)ftk_wnd_manager_default_dispatch_event, thiz));
		ftk_sources_manager_add(ftk_default_sources_manager(), ftk_primary_source());

		thiz->grab   = ftk_wnd_manager_default_grab;
		thiz->ungrab = ftk_wnd_manager_default_ungrab;
		thiz->add    = ftk_wnd_manager_default_add;
		thiz->remove = ftk_wnd_manager_default_remove;
		thiz->restack= ftk_wnd_manager_default_restack;
		thiz->update         = ftk_wnd_manager_default_update;
		thiz->get_work_area  = ftk_wnd_manager_default_get_work_area;
		thiz->queue_event    = ftk_wnd_manager_default_queue_event;
		thiz->dispatch_event         = ftk_wnd_manager_default_dispatch_event;
		thiz->add_global_listener    = ftk_wnd_manager_default_add_global_listener;
		thiz->remove_global_listener = ftk_wnd_manager_default_remove_global_listener;
		thiz->destroy                = ftk_wnd_manager_default_destroy;


#ifndef FTK_SUPPORT_C99
	key_tanslate_table_init();
#endif
	}

	return thiz;
}


#ifdef FTK_WND_MANAGER_TEST
int main(int argc, char* argv[])
{
	FtkWndManager* thiz = NULL;
	FtkMainLoop* main_loop = ftk_main_loop_create();
	
	thiz = ftk_wnd_manager_default_create(main_loop);

	ftk_wnd_manager_default_destroy(thiz);

	return 0;
}
#endif/*FTK_WND_MANAGER_TEST*/

