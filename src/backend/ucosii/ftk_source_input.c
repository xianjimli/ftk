/*
 * File: ftk_source_input.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   source to handle /dev/input/xx
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
#include "ftk_key.h"
#include "ftk_display.h"
#include "ftk_source_input.h"
#include "ftk_pipe.h"
#include "ftk_wnd_manager.h"

typedef struct _PrivInfo
{
	int x;
	int y;
	FtkPipe *pipe;
	FtkEvent event;
	FtkOnEvent on_event;
	void* user_data;
}PrivInfo;

static FtkPipe *s_pipe = NULL;

static int s_key_map[0xff] = {0};

static void ftk_keymap_init(void)
{
	s_key_map[0xBD] = FTK_KEY_MINUS;
	s_key_map[0x20] = FTK_KEY_SPACE;
	s_key_map[0xBB] = FTK_KEY_EQUAL;
	s_key_map[0x08] = FTK_KEY_BACKSPACE;
	s_key_map[0x09] = FTK_KEY_TAB;
	s_key_map[0xC0] = FTK_KEY_QUOTELEFT;
	s_key_map[0xDE] = FTK_KEY_QUOTERIGHT;
	s_key_map[0xDB] = FTK_KEY_BRACKETLEFT;
	s_key_map[0xDD] = FTK_KEY_BRACKETRIGHT;
	s_key_map[0x0d] = FTK_KEY_ENTER;
	s_key_map[0xBA] = FTK_KEY_SEMICOLON;
	s_key_map[0x30] = FTK_KEY_0;
	s_key_map[0x31] = FTK_KEY_1; 
	s_key_map[0x32] = FTK_KEY_2;
	s_key_map[0x33] = FTK_KEY_3;
	s_key_map[0x34] = FTK_KEY_4;
	s_key_map[0x35] = FTK_KEY_5;
	s_key_map[0x36] = FTK_KEY_6;
	s_key_map[0x37] = FTK_KEY_7;
	s_key_map[0x38] = FTK_KEY_8;
	s_key_map[0x39] = FTK_KEY_9;
	s_key_map[0x70] = FTK_KEY_F1; 
	s_key_map[0x71] = FTK_KEY_F2;
	s_key_map[0x72] = FTK_KEY_F3;
	s_key_map[0x73] = FTK_KEY_F4;
	s_key_map[0x74] = FTK_KEY_F5;
	s_key_map[0x75] = FTK_KEY_F6;
	s_key_map[0x76] = FTK_KEY_F7;
	s_key_map[0x77] = FTK_KEY_F8;
	s_key_map[0x78] = FTK_KEY_F9;
	s_key_map[0xBC] = FTK_KEY_COMMA;
	s_key_map[0xBE] = FTK_KEY_DOT;
	s_key_map[0xBF] = FTK_KEY_SLASH;
	s_key_map[0x10] = FTK_KEY_RIGHTSHIFT;
	s_key_map[0x11] = FTK_KEY_LEFTCTRL;
	s_key_map[0x14] = FTK_KEY_CAPSLOCK;
	s_key_map[0x40] = FTK_KEY_LEFTALT;
	s_key_map[0x41] = FTK_KEY_a;
	s_key_map[0x42] = FTK_KEY_b;
	s_key_map[0x43] = FTK_KEY_c;
	s_key_map[0x44] = FTK_KEY_d;
	s_key_map[0x45] = FTK_KEY_e;
	s_key_map[0x46] = FTK_KEY_f;
	s_key_map[0x47] = FTK_KEY_g;
	s_key_map[0x48] = FTK_KEY_h;
	s_key_map[0x49] = FTK_KEY_i;
	s_key_map[0x4a] = FTK_KEY_j;
	s_key_map[0x4b] = FTK_KEY_k;
	s_key_map[0x4c] = FTK_KEY_l;
	s_key_map[0x4d] = FTK_KEY_m;
	s_key_map[0x4e] = FTK_KEY_n;
	s_key_map[0x4f] = FTK_KEY_o;
	s_key_map[0x50] = FTK_KEY_p;
	s_key_map[0x51] = FTK_KEY_q;
	s_key_map[0x52] = FTK_KEY_r;
	s_key_map[0x53] = FTK_KEY_s;
	s_key_map[0x54] = FTK_KEY_t;
	s_key_map[0x55] = FTK_KEY_u;
	s_key_map[0x56] = FTK_KEY_v;
	s_key_map[0x57] = FTK_KEY_w;
	s_key_map[0x58] = FTK_KEY_x;
	s_key_map[0x59] = FTK_KEY_y;
	s_key_map[0x5a] = FTK_KEY_z;
	s_key_map[0x21] = FTK_KEY_PAGEUP;
	s_key_map[0x25] = FTK_KEY_LEFT;
	s_key_map[0x27] = FTK_KEY_RIGHT;
	s_key_map[0x24] = FTK_KEY_HOME;
	s_key_map[0x23] = FTK_KEY_END;
	s_key_map[0x28] = FTK_KEY_DOWN;
	s_key_map[0x22] = FTK_KEY_PAGEDOWN;
	s_key_map[0x2d] = FTK_KEY_INSERT;
	s_key_map[0x2e] = FTK_KEY_DELETE;
	s_key_map[0x26] = FTK_KEY_UP;

	return;
}

int ftk_add_event(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(s_pipe != NULL)
	{
		struct input_event ievent;

		ievent.hwnd = hwnd;
		ievent.message = message;
		ievent.wparam = wParam;
		ievent.lparam = lParam;

		ftk_pipe_write(s_pipe, &ievent, sizeof(struct input_event));
		return 0;
	}

	return -1;
}

static Ret ftk_on_key_event(PrivInfo* priv, int down, unsigned char vkey)
{
	priv->event.type = down ? FTK_EVT_KEY_DOWN : FTK_EVT_KEY_UP;
	priv->event.u.key.code = s_key_map[vkey];

	return RET_OK;
}


static int ftk_source_input_get_fd(FtkSource* thiz)
{
	DECL_PRIV(thiz, priv);

	return ftk_pipe_get_read_handle(priv->pipe);
}

static int ftk_key_map(FtkSource* thiz, int key)
{
	return s_key_map[key] != 0 ? s_key_map[key] : key;
}

static int ftk_source_input_check(FtkSource* thiz)
{
	return -1;
}

static Ret ftk_source_input_dispatch(FtkSource* thiz)
{
	int ret = 0;
	struct input_event ievent;
	DECL_PRIV(thiz, priv);
	ret = ftk_pipe_read(priv->pipe, &ievent, sizeof(ievent));
	return_val_if_fail(ret == sizeof(ievent), RET_FAIL);

	switch (ievent.message)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			ftk_on_key_event(priv, ievent.message==WM_KEYDOWN, 0xFF & ievent.wparam);
			break;
		}
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
		{
			int xPos, yPos;

			xPos = LOWORD(ievent.lparam); 
			yPos = HIWORD(ievent.lparam); 
			priv->event.u.mouse.x = xPos;
			priv->event.u.mouse.y = yPos;
			priv->event.type = ievent.message == WM_LBUTTONUP? FTK_EVT_MOUSE_UP : FTK_EVT_MOUSE_DOWN;

			break;
		}
	case WM_MOUSEMOVE:
		{
			int xPos = LOWORD(ievent.lparam); 
			int yPos = HIWORD(ievent.lparam); 
			priv->event.u.mouse.x = xPos;
			priv->event.u.mouse.y = yPos;
			priv->event.type = FTK_EVT_MOUSE_MOVE;

			break;
		}
	default:
		break;
	}

	if(priv->on_event != NULL && priv->event.type != FTK_EVT_NOP)
	{
		priv->on_event(priv->user_data, &priv->event);
		priv->event.type = FTK_EVT_NOP;
	}

	return RET_OK;
}

static void ftk_source_input_destroy(FtkSource* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		ftk_pipe_destroy(priv->pipe);
		FTK_ZFREE(thiz, sizeof(thiz) + sizeof(PrivInfo));

		s_pipe = NULL;
	}

	return;
}

FtkSource* ftk_source_input_create(FtkOnEvent on_event, void* user_data)
{
	FtkSource* thiz = (FtkSource*)FTK_ZALLOC(sizeof(FtkSource) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->get_fd   = ftk_source_input_get_fd;
		thiz->check    = ftk_source_input_check;
		thiz->dispatch = ftk_source_input_dispatch;
		thiz->destroy  = ftk_source_input_destroy;

		s_pipe = priv->pipe = ftk_pipe_create();
		if(priv->pipe == NULL)
		{
			FTK_ZFREE(thiz, sizeof(thiz) + sizeof(PrivInfo));
			return NULL;
		}

		thiz->ref = 1;
		priv->on_event = on_event;
		priv->user_data = user_data;
	}

	return thiz;
}

