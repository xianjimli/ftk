/*
 * File: ftk_source_tslib.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   source to handle tslib events.
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
 * 2009-12-02 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tslib.h"
#include "ftk_log.h"
#include "ftk_key.h"
#include "ftk_source_tslib.h"

typedef struct _PrivInfo
{
	int pressed;
	FtkEvent event;
	struct tsdev* ts;
	void* user_data;
	FtkOnEvent on_event;
}PrivInfo;

static int ftk_source_tslib_get_fd(FtkSource* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && priv->ts != NULL, -1);

	return ts_fd(priv->ts);
}

static int ftk_source_tslib_check(FtkSource* thiz)
{
	return -1;
}

static Ret ftk_source_tslib_dispatch(FtkSource* thiz)
{
	int ret = 0;
	DECL_PRIV(thiz, priv);
	struct ts_sample sample = {0};
	return_val_if_fail(priv->ts != NULL, RET_FAIL);	
	ret = ts_read(priv->ts, &sample, 1);
	if(ret <= 0) return RET_OK;

	priv->event.type = FTK_EVT_NOP;
	priv->event.u.mouse.x = sample.x;
	priv->event.u.mouse.y = sample.y;
	ftk_logd("%s: sample.pressure=%d x=%d y=%d\n", 
		__func__, sample.pressure, sample.x, sample.y);
	if(sample.pressure > 0)
	{
		if(priv->pressed)
		{
			priv->event.type =  FTK_EVT_MOUSE_MOVE;
		}
		else
		{
			priv->event.type =  FTK_EVT_MOUSE_DOWN;
			priv->pressed = 1;
		}
	}
	else
	{
		if(priv->pressed)
		{
			priv->event.type =  FTK_EVT_MOUSE_UP;
		}
		priv->pressed = 0;
	}

	if(priv->on_event != NULL && priv->event.type != FTK_EVT_NOP)
	{
		priv->on_event(priv->user_data, &priv->event);
		ftk_logd("%s: type=%d x=%d y=%d\n", __func__, 
			priv->event.type, priv->event.u.mouse.x,
			priv->event.u.mouse.y);
		priv->event.type = FTK_EVT_NOP;
	}

	return RET_OK;
}

static void ftk_source_tslib_destroy(FtkSource* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		ts_close(priv->ts);
		FTK_ZFREE(thiz, sizeof(*thiz) + sizeof(PrivInfo));
	}

	return;
}

FtkSource* ftk_source_tslib_create(const char* filename, FtkOnEvent on_event, void* user_data)
{
	FtkSource* thiz = NULL;
	return_val_if_fail(filename != NULL && on_event != NULL, NULL);

	thiz = (FtkSource*)FTK_ZALLOC(sizeof(FtkSource) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->get_fd   = ftk_source_tslib_get_fd;
		thiz->check    = ftk_source_tslib_check;
		thiz->dispatch = ftk_source_tslib_dispatch;
		thiz->destroy  = ftk_source_tslib_destroy;

		thiz->ref = 1;
		priv->ts = ts_open(filename, 1);
		if(priv->ts != NULL)
		{
			ts_config(priv->ts);
			priv->on_event  = on_event;
			priv->user_data = user_data;
			ftk_logd("%s: %d=%s priv->user_data=%p\n", __func__, ts_fd(priv->ts), filename, priv->user_data);
		}
		else
		{
			FTK_ZFREE(thiz, sizeof(*thiz) + sizeof(PrivInfo));
		}
	}

	return thiz;
}

