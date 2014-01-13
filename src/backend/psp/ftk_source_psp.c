/*
 * File: ftk_source_psp.c	 
 * Author:  Tao Yu <yut616@gmail.com>
 * Brief:	psp event handler
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
 * 2010-03-27 Tao Yu <yut616@gmail.com> created.
 *
 */
#include <psputils.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspge.h>
#include <pspdebug.h>
#include "ftk_key.h"
#include "ftk_log.h"
#include "ftk_source_psp.h"
#include "ftk_display_psp.h"

typedef struct _PrivInfo
{
	int psp_keypress;  /* for keyboard driver */
	FtkEvent event;
	FtkDisplay* display;
	FtkOnEvent on_event;
	void* ctx;
}PrivInfo;

static int ftk_source_psp_get_fd(FtkSource* thiz)
{
	return -1;
}

static int ftk_source_psp_check(FtkSource* thiz)
{
	return 0;
}

static Ret ftk_source_psp_event_handler(FtkSource *thiz)
{
	DECL_PRIV(thiz, priv);
	if(priv->on_event != NULL && priv->event.type != FTK_EVT_NOP)
	{
		priv->on_event(priv->ctx, &priv->event);
		priv->event.type = FTK_EVT_NOP;
	}
	return RET_OK;
}

static Ret ftk_source_psp_dispatch(FtkSource* thiz)
{
	SceCtrlData pad;
	int new_keypress = 0;
	int x = 0, y = 0; 

	DECL_PRIV(thiz, priv);

	sceCtrlReadBufferPositive(&pad, 1); 

	/* Lx/Ly is (0~255) , x/y is 480x272 */
	x = (pad.Lx * ftk_display_width(priv->display)) >> 8;
	y = (pad.Ly * ftk_display_height(priv->display)) >> 8;

	if(pad.Buttons & PSP_CTRL_SQUARE)
	{
		/* mouse left button */
		new_keypress = FTK_KEY_ENTER;
	}
	if(pad.Buttons & PSP_CTRL_CROSS)
	{
		/* mouse right button */
	}
	if(pad.Buttons & PSP_CTRL_TRIANGLE)
	{
		/* mouse mid button */
	}

	if(pad.Buttons & PSP_CTRL_CIRCLE)	new_keypress = FTK_KEY_ENTER;

	if(pad.Buttons & PSP_CTRL_LEFT)		new_keypress = FTK_KEY_LEFT;
	if(pad.Buttons & PSP_CTRL_RIGHT) 	new_keypress = FTK_KEY_RIGHT;
	if(pad.Buttons & PSP_CTRL_UP) 		new_keypress = FTK_KEY_UP;
	if(pad.Buttons & PSP_CTRL_DOWN) 	new_keypress = FTK_KEY_DOWN;

	if(pad.Buttons & PSP_CTRL_LTRIGGER) new_keypress = FTK_KEY_PAGEUP;
	if(pad.Buttons & PSP_CTRL_RTRIGGER) new_keypress = FTK_KEY_PAGEDOWN;

	/* if keyboard driver got a key, only give it more once that
	   key was let go.  This is just a quick hack, there's surely a
	   better way to handle this.. */
	if(priv->psp_keypress < 0 && (-(priv->psp_keypress)) != new_keypress)
		priv->psp_keypress = 0;

	if(priv->psp_keypress == 0)
		priv->psp_keypress = new_keypress;
	
	if(priv->psp_keypress > 0)
	{
		if(priv->psp_keypress == FTK_KEY_ENTER)
		{
			if(pad.Buttons & PSP_CTRL_SQUARE)
			{
				priv->event.u.mouse.x = x;
				priv->event.u.mouse.y = y;
				priv->event.type = FTK_EVT_MOUSE_DOWN;
				ftk_source_psp_event_handler(thiz);

				priv->event.type = FTK_EVT_MOUSE_UP;
			}
			else
			{
				priv->event.u.key.code = priv->psp_keypress;
				priv->event.type = FTK_EVT_KEY_DOWN;
				ftk_source_psp_event_handler(thiz);

				priv->event.u.key.code = priv->psp_keypress;
				priv->event.type = FTK_EVT_KEY_UP;
			}
		}
		else
		{
			priv->event.u.key.code = priv->psp_keypress;
			priv->event.type = FTK_EVT_KEY_DOWN;
		}
		priv->psp_keypress = -(priv->psp_keypress);
	}
	else
	{
		priv->event.u.mouse.x = x;
		priv->event.u.mouse.y = y;
		priv->event.type = FTK_EVT_MOUSE_MOVE;
	}
	return ftk_source_psp_event_handler(thiz);
}

static void ftk_source_psp_destroy(FtkSource* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(thiz) + sizeof(PrivInfo));
	}

	return;
}

FtkSource* ftk_source_psp_create(FtkDisplay* display, FtkOnEvent on_event, void* ctx)
{
	FtkSource* thiz = NULL;
	
	return_val_if_fail(display != NULL && on_event != NULL, NULL);

	thiz = (FtkSource*)FTK_ZALLOC(sizeof(FtkSource) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->get_fd   = ftk_source_psp_get_fd;
		thiz->check	   = ftk_source_psp_check;
		thiz->dispatch = ftk_source_psp_dispatch;
		thiz->destroy  = ftk_source_psp_destroy;

		thiz->ref = 1;
		priv->ctx = ctx;
		priv->on_event = on_event;
		priv->display = display;

		priv->psp_keypress = 0;
	}

	return thiz;
}

