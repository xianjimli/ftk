/*
 * File: ftk_event.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   
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
#ifndef FTK_EVENT_H
#define FTK_EVENT_H
#include "ftk_typedef.h"

FTK_BEGIN_DECLS

typedef enum _FtkEventType
{
	FTK_EVT_NOP = 0,
	FTK_EVT_IDLE,
	FTK_EVT_INIT,
	FTK_EVT_TIMER,
	FTK_EVT_QUIT,
	FTK_EVT_KEY_UP,
	FTK_EVT_KEY_DOWN,
	FTK_EVT_MOUSE_MOVE,
	FTK_EVT_MOUSE_DOWN,
	FTK_EVT_MOUSE_LONG_PRESS,
	FTK_EVT_KEY_LONG_PRESS,
	FTK_EVT_MOUSE_UP,
	FTK_EVT_HIDE,
	FTK_EVT_SHOW,
	FTK_EVT_MAP,
	FTK_EVT_UNMAP,
	FTK_EVT_UPDATE,
	FTK_EVT_MOVE,
	FTK_EVT_RESIZE,
	FTK_EVT_MOVE_RESIZE,
	FTK_EVT_FOCUS_IN,
	FTK_EVT_FOCUS_OUT,
	FTK_EVT_ADD_CHILD,
	FTK_EVT_IM_COMMIT,
	FTK_EVT_IM_PREEDIT,
	FTK_EVT_SET_TEXT,
	FTK_EVT_GET_TEXT,
	FTK_EVT_IM_ACT_COMMIT,
	FTK_EVT_OS_IM_COMMIT,
	FTK_EVT_REMOVE_CHILD,
	FTK_EVT_WND_DESTROY,
	FTK_EVT_RELAYOUT_WND,
	FTK_EVT_ADD_SOURCE,
	FTK_EVT_REMOVE_SOURCE,
	FTK_EVT_THEME_CHANGED,
	FTK_EVT_SCREEN_ROTATED,
	FTK_EVT_DISABLE_CURSOR,
	FTK_EVT_ENABLE_CURSOR,
	FTK_EVT_OS_SCREEN_ROTATED,
	FTK_EVT_LOCALE_CHANGED,
	FTK_EVT_DISPLAY_CHANGED, 
	FTK_EVT_TOP_WND_CHANGED,
	FTK_EVT_WND_CONFIG_CHANGED,
	FTK_EVT_TAB_PAGE_ACTIVATE,
	FTK_EVT_TAB_PAGE_DEACTIVATE
}FtkEventType;

typedef struct _FtkEvent
{
	FtkEventType type;

	void* widget;
	size_t time;
	union
	{
		struct _Idle
		{
			FtkIdle action;
			void* user_data;
		}idle;
		struct _Timer
		{
			FtkTimer action;
			int interval;
			void* user_data;
		}timer;
		struct _Key
		{
			int code;
		}key;
		struct _Mouse
		{
			unsigned char press;
			unsigned char button;
			unsigned short x;
			unsigned short y;
		}mouse;
		
		struct _Display
		{
			void* display;
			int width;
			int height;
		}display;
		FtkRect rect;
		void* extra;
	}u;
}FtkEvent;

typedef Ret (*FtkOnEvent)(void* user_data, FtkEvent* event);

static inline Ret ftk_event_init(FtkEvent* event, FtkEventType type)
{
	if(event != NULL)
	{
		memset(event, 0x00, sizeof(FtkEvent));
		event->type = type;
	}

	return RET_OK;
}

FTK_END_DECLS

#endif/*FTK_EVENT_H*/
