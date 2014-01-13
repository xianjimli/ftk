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
#include "ftk_globals.h"
#include "ftk_key.h"
#include "ftk_display.h"
#include <linux/input.h>
#include "ftk_source_input.h"

#ifndef EV_SYN
#define EV_SYN 0x00
#endif

typedef struct _PrivInfo
{
	int fd;
	int x;
	int y;
	FtkEvent event;
	FtkOnEvent on_event;
	void* user_data;
	char filename[64];
}PrivInfo;

#include <linux/input.h>

/*FIXME: complete the keymap table.*/
static unsigned short s_key_map[0x100] = 
{
	[KEY_1]           =  FTK_KEY_1,
	[KEY_2]           =  FTK_KEY_2,
	[KEY_3]           =  FTK_KEY_3,
	[KEY_4]           =  FTK_KEY_4,
	[KEY_5]           =  FTK_KEY_5,
	[KEY_6]           =  FTK_KEY_6,
	[KEY_7]           =  FTK_KEY_7,
	[KEY_8]           =  FTK_KEY_8,
	[KEY_9]           =  FTK_KEY_9,
	[KEY_0]           =  FTK_KEY_0,
	[KEY_A]           =  FTK_KEY_a,
	[KEY_B]           =  FTK_KEY_b,
	[KEY_C]           =  FTK_KEY_c,
	[KEY_D]           =  FTK_KEY_d,
	[KEY_E]           =  FTK_KEY_e,
	[KEY_F]           =  FTK_KEY_f,
	[KEY_G]           =  FTK_KEY_g,
	[KEY_H]           =  FTK_KEY_h,
	[KEY_I]           =  FTK_KEY_i,
	[KEY_J]           =  FTK_KEY_j,
	[KEY_K]           =  FTK_KEY_k,
	[KEY_L]           =  FTK_KEY_l,
	[KEY_M]           =  FTK_KEY_m,
	[KEY_N]           =  FTK_KEY_n,
	[KEY_O]           =  FTK_KEY_o,
	[KEY_P]           =  FTK_KEY_p,
	[KEY_Q]           =  FTK_KEY_q,
	[KEY_R]           =  FTK_KEY_r,
	[KEY_S]           =  FTK_KEY_s,
	[KEY_T]           =  FTK_KEY_t,
	[KEY_U]           =  FTK_KEY_u,
	[KEY_V]           =  FTK_KEY_v,
	[KEY_W]           =  FTK_KEY_w,
	[KEY_X]           =  FTK_KEY_x,
	[KEY_Y]           =  FTK_KEY_y,
	[KEY_Z]           =  FTK_KEY_z,
	[KEY_RIGHTCTRL]   =  FTK_KEY_RIGHTCTRL,
	[KEY_RIGHTALT]    =  FTK_KEY_RIGHTALT,
	[KEY_HOME]        =  FTK_KEY_HOME,
	[KEY_UP]          =  FTK_KEY_UP,
	[KEY_PAGEUP]      =  FTK_KEY_PAGEUP,
	[KEY_LEFT]        =  FTK_KEY_LEFT,
	[KEY_RIGHT]       =  FTK_KEY_RIGHT,
	[KEY_END]         =  FTK_KEY_END,
	[KEY_DOWN]        =  FTK_KEY_DOWN,
	[KEY_PAGEDOWN]    =  FTK_KEY_PAGEDOWN,
	[KEY_INSERT]      =  FTK_KEY_INSERT,
	[KEY_DELETE]      =  FTK_KEY_DELETE,
	[KEY_F1]          =  FTK_KEY_F1,
	[KEY_F2]          =  FTK_KEY_F2,
	[KEY_F3]          =  FTK_KEY_F3,
	[KEY_F4]          =  FTK_KEY_F4,
	[KEY_F5]          =  FTK_KEY_F5,
	[KEY_F6]          =  FTK_KEY_F6,
	[KEY_F7]          =  FTK_KEY_F7,
	[KEY_F8]          =  FTK_KEY_F8,
	[KEY_F9]          =  FTK_KEY_F9,
	[KEY_F10]         =  FTK_KEY_F10,
	[KEY_COMMA]       =  FTK_KEY_COMMA,
	[KEY_DOT]         =  FTK_KEY_DOT,
	[KEY_SLASH]       =  FTK_KEY_SLASH,
	[KEY_RIGHTSHIFT]  =  FTK_KEY_RIGHTSHIFT,
	[KEY_LEFTALT]     =  FTK_KEY_LEFTALT,
	[KEY_SPACE]       =  FTK_KEY_SPACE,
	[KEY_CAPSLOCK]    =  FTK_KEY_CAPSLOCK,
	[KEY_SEMICOLON]   =  FTK_KEY_SEMICOLON,
	[KEY_LEFTSHIFT]   =  FTK_KEY_LEFTSHIFT,
	[KEY_BACKSLASH]   =  FTK_KEY_BACKSLASH,
	[KEY_LEFTBRACE]   =  FTK_KEY_LEFTBRACE,
	[KEY_RIGHTBRACE]  =  FTK_KEY_RIGHTBRACE,
	[KEY_ENTER]       =  FTK_KEY_ENTER,
	[KEY_LEFTCTRL]    =  FTK_KEY_LEFTCTRL,
	[KEY_MINUS]       =  FTK_KEY_MINUS,
	[KEY_EQUAL]       =  FTK_KEY_EQUAL,
	[KEY_BACKSPACE]   =  FTK_KEY_BACKSPACE,
	[KEY_TAB]         =  FTK_KEY_TAB,
	[KEY_ESC]         =  FTK_KEY_ESC,
	[139]             =  FTK_KEY_F2, /*map menu to f2*/
//	[KEY_SEND]        =  FTK_KEY_SEND,
//	[KEY_REPLY]       =  FTK_KEY_REPLY,
//	[KEY_SAVE]        =  FTK_KEY_SAVE,
	[KEY_POWER]       =  FTK_KEY_POWER
};

static int ftk_source_input_get_fd(FtkSource* thiz)
{
	DECL_PRIV(thiz, priv);

	return priv->fd;
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
	DECL_PRIV(thiz, priv);
	struct input_event ievent;
	return_val_if_fail(priv->fd > 0, RET_FAIL);	
	ret = read(priv->fd, &ievent, sizeof(ievent));

	if(ret != sizeof(ievent))
	{
		ftk_logd("%s:%d read from %s failed(ret=%d, errno=%d)\n", __func__, __LINE__, priv->filename, ret, errno);
	}

	return_val_if_fail(ret == sizeof(ievent), RET_FAIL);

	switch(ievent.type)
	{
		case EV_KEY:
		{
			if(ievent.code == BTN_LEFT || ievent.code == BTN_RIGHT 
				|| ievent.code == BTN_MIDDLE || ievent.code == BTN_TOUCH)
			{
				priv->event.type = ievent.value ? FTK_EVT_MOUSE_DOWN : FTK_EVT_MOUSE_UP;
			}
			else
			{
				priv->event.type = ievent.value ? FTK_EVT_KEY_DOWN : FTK_EVT_KEY_UP;
				priv->event.u.key.code = ftk_key_map(thiz, ievent.code);
				if(priv->on_event != NULL && priv->event.type != FTK_EVT_NOP)
				{
					priv->on_event(priv->user_data, &priv->event);
					priv->event.type = FTK_EVT_NOP;
				}
				ftk_logd("%s: %02x --> %02x\n", __func__, ievent.code, priv->event.u.key.code);
			}

			break;
		}
		case EV_ABS:
		{
			switch(ievent.code)
			{
				case ABS_X:
				{
					priv->x = ievent.value;
					break;
				}
				case ABS_Y:
				{
					priv->y = ievent.value;
					break;
				}
				default:break;
			}
			if(priv->event.type == FTK_EVT_NOP)
			{
				priv->event.type = FTK_EVT_MOUSE_MOVE;
			}

			break;
		}
		case EV_REL:
		{
			switch(ievent.code)
			{
				case REL_X:
				{
					priv->x += ievent.value;
					break;
				}
				case REL_Y:
				{
					priv->y += ievent.value;
					break;
				}
				default:break;
			}

			if(priv->event.type == FTK_EVT_NOP)
			{
				priv->event.type = FTK_EVT_MOUSE_MOVE;
			}

			break;
		}
		case EV_SYN:
		{
			switch(priv->event.type)
			{
				case FTK_EVT_MOUSE_DOWN:
				case FTK_EVT_MOUSE_UP:
				case FTK_EVT_MOUSE_MOVE:
				{
					int max_x = ftk_display_width(ftk_default_display());
					int max_y = ftk_display_height(ftk_default_display());

					priv->x = priv->x > 0 ? priv->x : 0;
					priv->y = priv->y > 0 ? priv->y : 0;
					priv->x = priv->x < max_x ? priv->x : max_x;
					priv->y = priv->y < max_y ? priv->y : max_y;

					priv->event.u.mouse.x = priv->x;
					priv->event.u.mouse.y = priv->y;
					break;
				}
				default:break;
			}
			if(priv->on_event != NULL && priv->event.type != FTK_EVT_NOP)
			{
				priv->on_event(priv->user_data, &priv->event);
				priv->event.type = FTK_EVT_NOP;
			}
			break;
		}
		default:break;
	}

	return RET_OK;
}

static void ftk_source_input_destroy(FtkSource* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		close(priv->fd);
		FTK_ZFREE(thiz, sizeof(thiz) + sizeof(PrivInfo));
	}

	return;
}

FtkSource* ftk_source_input_create(const char* filename, FtkOnEvent on_event, void* user_data)
{
	FtkSource* thiz = (FtkSource*)FTK_ZALLOC(sizeof(FtkSource) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		int grab = 0;
		char name[100] = {0};
		DECL_PRIV(thiz, priv);
		thiz->get_fd   = ftk_source_input_get_fd;
		thiz->check    = ftk_source_input_check;
		thiz->dispatch = ftk_source_input_dispatch;
		thiz->destroy  = ftk_source_input_destroy;

		thiz->ref = 1;
		priv->fd = open(filename, O_RDONLY);
		ftk_strncpy(priv->filename, filename, sizeof(priv->filename));

		if(priv->fd < 0)
		{
			FTK_ZFREE(thiz, sizeof(thiz) + sizeof(PrivInfo));
			return NULL;
		}
#ifdef PC_EMU
		/*use uinput only.*/
		if(ioctl(priv->fd, EVIOCGNAME(sizeof(name) - 1), &name) < 0 || name[0] != 'v')
		{
			close(priv->fd);
			priv->fd = -1;

			FTK_ZFREE(thiz, sizeof(thiz) + sizeof(PrivInfo));
			return NULL;
		}

		/*grab uinput to prevent event broadcasting*/
		if(ioctl(priv->fd, EVIOCGRAB, &grab) < 0)
		{
			close(priv->fd);
			priv->fd = -1;
			FTK_ZFREE(thiz, sizeof(thiz) + sizeof(PrivInfo));
			return NULL;
		}
#else
		(void)name;
		(void)grab;
#endif
		priv->on_event = on_event;
		priv->user_data = user_data;
		printf("%s: %d=%s priv->user_data=%p\n", __func__, priv->fd, filename, priv->user_data);
	}

	return thiz;
}

