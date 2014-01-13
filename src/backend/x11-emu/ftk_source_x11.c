/*
 * File: ftk_source_x11.c	 
 * Author:	Li XianJing <xianjimli@hotmail.com>
 * Brief:	x11 event handler
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	 02111-1307	 USA
 */

/*
 * History:
 * ================================================================
 * 2009-10-06 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#include "ftk.h"
#include "ftk_typedef.h"
#include "ftk_key.h"
#include "ftk_log.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "ftk_source_x11.h"
#include "ftk_display_x11.h"

typedef struct _PrivInfo
{
	int fd;
	Window win;
	FtkEvent event;
	FtkDisplay* display;
	FtkOnEvent on_event;
	void* ctx;
}PrivInfo;

static int ftk_source_x11_get_fd(FtkSource* thiz)
{
	DECL_PRIV(thiz, priv);

	return priv->fd;
}

static int ftk_source_x11_check(FtkSource* thiz)
{
	return -1;
}

static const int s_key_map[] = 
{
	[0x0a] = FTK_KEY_1,
	[0x0b] = FTK_KEY_2,
	[0x0c] = FTK_KEY_3,
	[0x0d] = FTK_KEY_4,
	[0x0e] = FTK_KEY_5,
	[0x0f] = FTK_KEY_6,
	[0x10] = FTK_KEY_7,
	[0x11] = FTK_KEY_8,
	[0x12] = FTK_KEY_9,
	[0x13] = FTK_KEY_0,
	[0x14] = FTK_KEY_MINUS,
	[0x15] = FTK_KEY_EQUAL,
	[0x16] = FTK_KEY_BACKSPACE,
	[0x17] = FTK_KEY_TAB,
	[0x18] = FTK_KEY_q,
	[0x19] = FTK_KEY_w,
	[0x1a] = FTK_KEY_e,
	[0x1b] = FTK_KEY_r,
	[0x1c] = FTK_KEY_t,
	[0x1d] = FTK_KEY_y,
	[0x1e] = FTK_KEY_u,
	[0x1f] = FTK_KEY_i,
	[0x20] = FTK_KEY_o,
	[0x21] = FTK_KEY_p,
	[0x22] = FTK_KEY_BRACKETLEFT,
	[0x23] = FTK_KEY_BRACKETRIGHT,
	[0x24] = FTK_KEY_ENTER,
	[0x26] = FTK_KEY_a,
	[0x27] = FTK_KEY_s,
	[0x28] = FTK_KEY_d,
	[0x29] = FTK_KEY_f,
	[0x2a] = FTK_KEY_g,
	[0x2b] = FTK_KEY_h,
	[0x2c] = FTK_KEY_j,
	[0x2d] = FTK_KEY_k,
	[0x2e] = FTK_KEY_l,
	[0x2f] = FTK_KEY_SEMICOLON,
	[0x30] = FTK_KEY_QUOTERIGHT,
	[0x31] = FTK_KEY_GRAVE, 
	[0x32] = FTK_KEY_LEFTSHIFT,
	[0x33] = FTK_KEY_BACKSLASH,
	[0x34] = FTK_KEY_z,
	[0x35] = FTK_KEY_x,
	[0x36] = FTK_KEY_c,
	[0x37] = FTK_KEY_v,
	[0x38] = FTK_KEY_b,
	[0x39] = FTK_KEY_n,
	[0x3a] = FTK_KEY_m,
	[0x3b] = FTK_KEY_COMMA,
	[0x3c] = FTK_KEY_DOT,
	[0x3d] = FTK_KEY_SLASH,
	[0x3e] = FTK_KEY_RIGHTSHIFT,
	[0x40] = FTK_KEY_LEFTALT,
	[0x41] = FTK_KEY_SPACE,
	[0x42] = FTK_KEY_CAPSLOCK,
	[0x43] = FTK_KEY_F1,
	[0x44] = FTK_KEY_F2,
	[0x45] = FTK_KEY_F3,
	[0x46] = FTK_KEY_F4,
	[0x47] = FTK_KEY_F5,
	[0x48] = FTK_KEY_F6,
	[0x49] = FTK_KEY_F7,
	[0x4a] = FTK_KEY_F8,
	[0x61] = FTK_KEY_HOME,
	[0x62] = FTK_KEY_UP,
	[0x63] = FTK_KEY_PAGEUP,
	[0x64] = FTK_KEY_LEFT,
	[0x66] = FTK_KEY_RIGHT,
	[0x67] = FTK_KEY_END,
	[0x68] = FTK_KEY_DOWN,
	[0x69] = FTK_KEY_PAGEDOWN,
	[0x6a] = FTK_KEY_INSERT,
	[0x6b] = FTK_KEY_DELETE,
	[0x6f] = FTK_KEY_UP,
	[0x71] = FTK_KEY_LEFT,
	[0x72] = FTK_KEY_RIGHT,
	[0x74] = FTK_KEY_DOWN,
	[0x77] = FTK_KEY_DELETE,
//	[0x71] = FTK_KEY_RIGHTALT,
};

static int ftk_source_x11_keymap(FtkSource* thiz, Display* display, int keycode)
{
	ftk_logd("0x%03x --> 0x%03x %c\n", keycode, s_key_map[0xff&keycode], (char)s_key_map[0xff&keycode]);

	return s_key_map[0xff&keycode] ? s_key_map[0xff&keycode] : keycode;
}

static int on_x11_error (Display* display, XErrorEvent* event)
{
	ftk_loge("%s\n", __func__);
	exit(0);

	return 0;
}

static Ret ftk_source_x11_dispatch(FtkSource* thiz)
{
	XEvent event = {0};
	DECL_PRIV(thiz, priv);
	Display* display = (Display*)ftk_display_x11_get_xdisplay(priv->display);
	Atom* win_del_atom = (Atom*)ftk_display_x11_get_win_del_atom(priv->display);
	return_val_if_fail(priv->fd > 0, RET_FAIL); 

	while(XPending (display))
	{
		XNextEvent(display, &event);
		switch(event.type)
		{
			case ConfigureNotify:
			{
				priv->event.type = FTK_EVT_DISPLAY_CHANGED;
				priv->event.u.display.width = event.xconfigure.width;
				priv->event.u.display.height = event.xconfigure.height;
				priv->event.u.display.display = thiz;

				ftk_display_x11_on_resize(priv->display, event.xconfigure.width, event.xconfigure.height);

				break;
			}
			case Expose:
			{
				ftk_display_update(priv->display, NULL, NULL, 0, 0);	
				break;
			}
			case KeyPress:
			case KeyRelease:
			{
			 	priv->event.u.key.code = ftk_source_x11_keymap(thiz, display, event.xkey.keycode);
				priv->event.type = event.type == KeyPress ? FTK_EVT_KEY_DOWN : FTK_EVT_KEY_UP;

				break;
			}
			case ButtonPress:
			{
				//ftk_logd("%s: ButtonPress: %d %d\n", __func__, event.xbutton.x, event.xbutton.y);
				priv->event.type = FTK_EVT_MOUSE_DOWN;
				priv->event.u.mouse.x = event.xbutton.x;
				priv->event.u.mouse.y = event.xbutton.y;
				break;
			}
			case ButtonRelease:
			{
				//ftk_logd("%s: ButtonRelease: %d %d\n", __func__, event.xbutton.x, event.xbutton.y);
				priv->event.type = FTK_EVT_MOUSE_UP;
				priv->event.u.mouse.x = event.xbutton.x;
				priv->event.u.mouse.y = event.xbutton.y;
				break;
			}
			case MotionNotify:
			{
				priv->event.u.mouse.x = event.xbutton.x;
				priv->event.u.mouse.y = event.xbutton.y;
				priv->event.type = FTK_EVT_MOUSE_MOVE;
				break;
			}
			case DestroyNotify:
			{
				ftk_logd("%s: window destroied\n", __func__);	
				break;
			}
			case ClientMessage:
			{
				if ((Atom)event.xclient.data.l[0] == *win_del_atom) {
					FTK_QUIT();
				}
				break;
			}
			default:break;
		}

		if(priv->on_event != NULL && priv->event.type != FTK_EVT_NOP)
		{
			priv->on_event(priv->ctx, &priv->event);
			priv->event.type = FTK_EVT_NOP;
		}
	}

	return RET_OK;
}

static void ftk_source_x11_destroy(FtkSource* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		close(priv->fd);
		FTK_ZFREE(thiz, sizeof(thiz) + sizeof(PrivInfo));
	}

	return;
}

FtkSource* ftk_source_x11_create(FtkDisplay* display, FtkOnEvent on_event, void* ctx)
{
	FtkSource* thiz = NULL;
	
	return_val_if_fail(display != NULL && on_event != NULL, NULL);

	thiz = (FtkSource*)FTK_ZALLOC(sizeof(FtkSource) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->get_fd   = ftk_source_x11_get_fd;
		thiz->check	   = ftk_source_x11_check;
		thiz->dispatch = ftk_source_x11_dispatch;
		thiz->destroy  = ftk_source_x11_destroy;

		thiz->ref = 1;
		priv->ctx = ctx;
		priv->on_event = on_event;
		priv->display = display;
		priv->fd  = ConnectionNumber(ftk_display_x11_get_xdisplay(display));
		priv->win = (Window)ftk_display_x11_get_xwindow(display);

		XSetErrorHandler(on_x11_error);
	}

	return thiz;
}

