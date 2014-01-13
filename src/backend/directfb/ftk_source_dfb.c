/*
 * File: ftk_source_dfb.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   source to handle directfb event.
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

#include "ftk_key.h"
#include "ftk_log.h"
#include "ftk_display.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_source_dfb.h"
#include "ftk_wnd_manager.h"

typedef struct _PrivInfo
{
	int fd;
	int x;
	int y;
	FtkEvent event;
	IDirectFB* dfb;
	IDirectFBEventBuffer   *event_buffer;
}PrivInfo;

static int  ftk_source_dfb_get_fd(FtkSource* thiz)
{
	DECL_PRIV(thiz, priv);
	
	return priv->fd;
}

static int  ftk_source_dfb_check(FtkSource* thiz)
{
	return -1;
}

static const int s_key_map[] = 
{
	[DIKI_A-DIKI_UNKNOWN]              =  FTK_KEY_a,
	[DIKI_B-DIKI_UNKNOWN]              =  FTK_KEY_b,
	[DIKI_C-DIKI_UNKNOWN]              =  FTK_KEY_c,
	[DIKI_D-DIKI_UNKNOWN]              =  FTK_KEY_d,
	[DIKI_E-DIKI_UNKNOWN]              =  FTK_KEY_e,
	[DIKI_F-DIKI_UNKNOWN]              =  FTK_KEY_f,
	[DIKI_G-DIKI_UNKNOWN]              =  FTK_KEY_g,
	[DIKI_H-DIKI_UNKNOWN]              =  FTK_KEY_h,
	[DIKI_I-DIKI_UNKNOWN]              =  FTK_KEY_i,
	[DIKI_J-DIKI_UNKNOWN]              =  FTK_KEY_j,
	[DIKI_K-DIKI_UNKNOWN]              =  FTK_KEY_k,
	[DIKI_L-DIKI_UNKNOWN]              =  FTK_KEY_l,
	[DIKI_M-DIKI_UNKNOWN]              =  FTK_KEY_m,
	[DIKI_N-DIKI_UNKNOWN]              =  FTK_KEY_n,
	[DIKI_O-DIKI_UNKNOWN]              =  FTK_KEY_o,
	[DIKI_P-DIKI_UNKNOWN]              =  FTK_KEY_p,
	[DIKI_Q-DIKI_UNKNOWN]              =  FTK_KEY_q,
	[DIKI_R-DIKI_UNKNOWN]              =  FTK_KEY_r,
	[DIKI_S-DIKI_UNKNOWN]              =  FTK_KEY_s,
	[DIKI_T-DIKI_UNKNOWN]              =  FTK_KEY_t,
	[DIKI_U-DIKI_UNKNOWN]              =  FTK_KEY_u,
	[DIKI_V-DIKI_UNKNOWN]              =  FTK_KEY_v,
	[DIKI_W-DIKI_UNKNOWN]              =  FTK_KEY_w,
	[DIKI_X-DIKI_UNKNOWN]              =  FTK_KEY_x,
	[DIKI_Y-DIKI_UNKNOWN]              =  FTK_KEY_y,
	[DIKI_Z-DIKI_UNKNOWN]              =  FTK_KEY_z,
	[DIKI_0-DIKI_UNKNOWN]              =  FTK_KEY_0,
	[DIKI_1-DIKI_UNKNOWN]              =  FTK_KEY_1,
	[DIKI_2-DIKI_UNKNOWN]              =  FTK_KEY_2,
	[DIKI_3-DIKI_UNKNOWN]              =  FTK_KEY_3,
	[DIKI_4-DIKI_UNKNOWN]              =  FTK_KEY_4,
	[DIKI_5-DIKI_UNKNOWN]              =  FTK_KEY_5,
	[DIKI_6-DIKI_UNKNOWN]              =  FTK_KEY_6,
	[DIKI_7-DIKI_UNKNOWN]              =  FTK_KEY_7,
	[DIKI_8-DIKI_UNKNOWN]              =  FTK_KEY_8,
	[DIKI_9-DIKI_UNKNOWN]              =  FTK_KEY_9,
	[DIKI_F1-DIKI_UNKNOWN]             =  FTK_KEY_F1,
	[DIKI_F2-DIKI_UNKNOWN]             =  FTK_KEY_F2,
	[DIKI_F3-DIKI_UNKNOWN]             =  FTK_KEY_F3,
	[DIKI_F4-DIKI_UNKNOWN]             =  FTK_KEY_F4,
	[DIKI_F5-DIKI_UNKNOWN]             =  FTK_KEY_F5,
	[DIKI_F6-DIKI_UNKNOWN]             =  FTK_KEY_F6,
	[DIKI_F7-DIKI_UNKNOWN]             =  FTK_KEY_F7,
	[DIKI_F8-DIKI_UNKNOWN]             =  FTK_KEY_F8,
	[DIKI_F9-DIKI_UNKNOWN]             =  FTK_KEY_F9,
	[DIKI_SHIFT_L-DIKI_UNKNOWN]        =  FTK_KEY_LEFTSHIFT,
	[DIKI_SHIFT_R-DIKI_UNKNOWN]        =  FTK_KEY_RIGHTSHIFT,
	[DIKI_CONTROL_L-DIKI_UNKNOWN]      =  FTK_KEY_LEFTCTRL,
	[DIKI_CONTROL_R-DIKI_UNKNOWN]      =  FTK_KEY_RIGHTCTRL,
	[DIKI_ALT_L-DIKI_UNKNOWN]          =  FTK_KEY_LEFTALT,
	[DIKI_ALT_R-DIKI_UNKNOWN]          =  FTK_KEY_RIGHTALT,
	[DIKI_CAPS_LOCK-DIKI_UNKNOWN]      =  FTK_KEY_CAPSLOCK,
	[DIKI_NUM_LOCK-DIKI_UNKNOWN]       =  FTK_KEY_NUMBERSIGN,
	[DIKI_ESCAPE-DIKI_UNKNOWN]         =  FTK_KEY_ESC,
	[DIKI_LEFT-DIKI_UNKNOWN]           =  FTK_KEY_LEFT,
	[DIKI_RIGHT-DIKI_UNKNOWN]          =  FTK_KEY_RIGHT,
	[DIKI_UP-DIKI_UNKNOWN]             =  FTK_KEY_UP,
	[DIKI_DOWN-DIKI_UNKNOWN]           =  FTK_KEY_DOWN,
	[DIKI_TAB-DIKI_UNKNOWN]            =  FTK_KEY_TAB,
	[DIKI_ENTER-DIKI_UNKNOWN]          =  FTK_KEY_ENTER,
	[DIKI_SPACE-DIKI_UNKNOWN]          =  FTK_KEY_SPACE,
	[DIKI_BACKSPACE-DIKI_UNKNOWN]      =  FTK_KEY_BACKSPACE,
	[DIKI_INSERT-DIKI_UNKNOWN]         =  FTK_KEY_INSERT,
	[DIKI_DELETE-DIKI_UNKNOWN]         =  FTK_KEY_DELETE,
	[DIKI_HOME-DIKI_UNKNOWN]           =  FTK_KEY_HOME,
	[DIKI_END-DIKI_UNKNOWN]            =  FTK_KEY_END,
	[DIKI_PAGE_UP-DIKI_UNKNOWN]        =  FTK_KEY_PAGEUP,
	[DIKI_PAGE_DOWN-DIKI_UNKNOWN]      =  FTK_KEY_PAGEDOWN,
	[DIKI_QUOTE_LEFT-DIKI_UNKNOWN]     =  FTK_KEY_QUOTELEFT,
	[DIKI_MINUS_SIGN-DIKI_UNKNOWN]     =  FTK_KEY_MINUS,
	[DIKI_EQUALS_SIGN-DIKI_UNKNOWN]    =  FTK_KEY_EQUAL,
	[DIKI_BRACKET_LEFT-DIKI_UNKNOWN]   =  FTK_KEY_BRACKETLEFT,
	[DIKI_BRACKET_RIGHT-DIKI_UNKNOWN]  =  FTK_KEY_BRACKETRIGHT,
	[DIKI_BACKSLASH-DIKI_UNKNOWN]      =  FTK_KEY_BACKSLASH,
	[DIKI_SEMICOLON-DIKI_UNKNOWN]      =  FTK_KEY_SEMICOLON,
	[DIKI_QUOTE_RIGHT-DIKI_UNKNOWN]    =  FTK_KEY_QUOTERIGHT,
	[DIKI_COMMA-DIKI_UNKNOWN]          =  FTK_KEY_COMMA,
	[DIKI_PERIOD-DIKI_UNKNOWN]         =  FTK_KEY_PERIOD,
	[DIKI_SLASH-DIKI_UNKNOWN]          =  FTK_KEY_SLASH,
	[DIKI_LESS_SIGN-DIKI_UNKNOWN]      =  FTK_KEY_LESS
};

static int ftk_source_dfb_keymap(FtkSource* thiz, int keycode)
{
	ftk_logd("0x%03x --> 0x%03x %c\n", keycode, s_key_map[0xff&keycode], (char)s_key_map[0xff&keycode]);

	return s_key_map[0xff&keycode] ? s_key_map[0xff&keycode] : keycode;
}

static Ret ftk_source_dfb_dispatch_input_event(FtkSource* thiz, DFBInputEvent* event)
{
	DECL_PRIV(thiz, priv);
	switch(event->type)
	{
		case DIET_KEYPRESS:
		case DIET_KEYRELEASE:
		{
			priv->event.u.key.code = ftk_source_dfb_keymap(thiz, event->key_id);
			priv->event.type = event->type == DIET_KEYPRESS ? FTK_EVT_KEY_DOWN : FTK_EVT_KEY_UP;
			break;
		}
		case DIET_BUTTONPRESS:
		case DIET_BUTTONRELEASE:
		{
			priv->event.type = event->type == DIET_BUTTONPRESS ? FTK_EVT_MOUSE_DOWN : FTK_EVT_MOUSE_UP;
			priv->event.u.mouse.x = priv->x;
			priv->event.u.mouse.y = priv->y;
			break;
		}
		case DIET_AXISMOTION:
		{
			switch(event->axis)
			{
				case DIAI_Y:
				{
					priv->y = event->axisabs;
					break;
				}
				case DIAI_X:
				{
					priv->x = event->axisabs;
					break;
				}
				default:
				{
					break;
				}
			}
			priv->event.type = FTK_EVT_MOUSE_MOVE;
			priv->event.u.mouse.x = priv->x;
			priv->event.u.mouse.y = priv->y;
			break;
		}
		default:break;
	}

	if(priv->event.type != FTK_EVT_NOP)
	{
		ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &priv->event);
		priv->event.type = FTK_EVT_NOP;
	}

	return RET_OK;
}

static Ret  ftk_source_dfb_dispatch(FtkSource* thiz)
{
	int i = 0;
	int nr = 0;
	DFBEvent   buff[10];
	DECL_PRIV(thiz, priv);
	DFBEvent* event = NULL;
	int size =  read(priv->fd, buff, sizeof(buff));
	return_val_if_fail(size > 0, RET_FAIL);

	nr = size/sizeof(DFBEvent);
	for(i = 0; i < nr; i++)
	{
		event = buff+i;
		switch(event->clazz)
		{
			case DFEC_INPUT:
			{
				ftk_source_dfb_dispatch_input_event(thiz, &(event->input));
				break;
			}
			case DFEC_USER:
			{
				break;
			}
			case DFEC_WINDOW:
			{
				break;
			}
			default:break;
		}
	}

	return RET_OK;
}

static void ftk_source_dfb_destroy(FtkSource* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		close(priv->fd);
		if(priv->event_buffer != NULL)
		{
			priv->event_buffer->Release(priv->event_buffer);
		}
		FTK_ZFREE(thiz, sizeof(FtkSource) + sizeof(PrivInfo));
	}

	return;
}

FtkSource* ftk_source_dfb_create(IDirectFB* dfb, IDirectFBEventBuffer* event_buffer1)
{
	IDirectFBEventBuffer* event_buffer = NULL;
	FtkSource* thiz = FTK_ZALLOC(sizeof(FtkSource)+sizeof(PrivInfo));

	if(thiz != NULL)
	{
		int fd = 0;
		DECL_PRIV(thiz, priv);

		thiz->get_fd   =  ftk_source_dfb_get_fd;
		thiz->check    = ftk_source_dfb_check;
		thiz->dispatch = ftk_source_dfb_dispatch;
		thiz->destroy  = ftk_source_dfb_destroy;

		if(event_buffer == NULL)
		{
			dfb->CreateInputEventBuffer( dfb, DICAPS_ALL, DFB_FALSE, &event_buffer );
		}

		if(event_buffer != NULL)
		{
			event_buffer->CreateFileDescriptor(event_buffer, &fd);
		}
		thiz->ref = 1;
		priv->fd = fd;
		priv->event_buffer = event_buffer;
	}

	return thiz;
}


