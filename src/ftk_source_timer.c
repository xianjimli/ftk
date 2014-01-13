/*
 * File: ftk_source_timer.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   timer.
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
#include "ftk_typedef.h"
#include "ftk_source_timer.h"

typedef struct _SourceTimerPrivInfo
{
	FtkTimer action;
	void* user_data;
	size_t interval;
	size_t next_time;
}PrivInfo;

static int ftk_source_timer_get_fd(FtkSource* thiz)
{
	return -1;
}

static int ftk_source_timer_check(FtkSource* thiz)
{
	DECL_PRIV(thiz, priv);
	int t = priv->next_time - ftk_get_relative_time();

	t = t < 0 ? 0 : t;

	return t;
}

static void ftk_source_timer_calc_timer(PrivInfo* priv)
{
	priv->next_time = ftk_get_relative_time() + priv->interval;

	return;
}

Ret ftk_source_timer_reset(FtkSource* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);

	thiz->disable = 0;
	ftk_source_timer_calc_timer(priv);

	return RET_OK;
}

Ret ftk_source_timer_modify(FtkSource* thiz, int interval)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);

	priv->interval = interval;
	ftk_source_timer_calc_timer(priv);

	return RET_OK;
}

static Ret ftk_source_timer_dispatch(FtkSource* thiz)
{
	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv->action != NULL, RET_REMOVE);

	if(thiz->disable > 0)
	{
		ftk_source_timer_calc_timer(priv);
		return RET_OK;
	}

	ret = priv->action(priv->user_data);
	ftk_source_timer_calc_timer(priv);

	return ret;
}

static void ftk_source_timer_destroy(FtkSource* thiz)
{
	FTK_ZFREE(thiz, sizeof(FtkSource) + sizeof(PrivInfo));

	return;
}

FtkSource* ftk_source_timer_create(int interval, FtkTimer action, void* user_data)
{
	FtkSource* thiz = NULL;
	return_val_if_fail(interval > 0 && action != NULL, NULL);

	thiz = (FtkSource*)FTK_ZALLOC(sizeof(FtkSource) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->get_fd   = ftk_source_timer_get_fd;
		thiz->check    = ftk_source_timer_check;
		thiz->dispatch = ftk_source_timer_dispatch;
		thiz->destroy  = ftk_source_timer_destroy;

		thiz->ref = 1;
		priv->interval  = interval;
		priv->user_data = user_data;
		priv->action    = action;
		ftk_source_timer_calc_timer(priv);
	}

	return thiz;
}

