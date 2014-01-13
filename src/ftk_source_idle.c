/*
 * File: ftk_source_idle.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   idle source 
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
#include "ftk_source_idle.h"

typedef struct _SourceIdlePrivInfo
{
	FtkIdle action;
	void* user_data;
}PrivInfo;

static int ftk_source_idle_get_fd(FtkSource* thiz)
{
	return -1;
}

static int ftk_source_idle_check(FtkSource* thiz)
{
	return 0;
}

static Ret ftk_source_idle_dispatch(FtkSource* thiz)
{
	DECL_PRIV(thiz, priv);

	return_val_if_fail(priv->action != NULL, RET_REMOVE);

	if(thiz->disable > 0)
	{
		return RET_OK;
	}

	return priv->action(priv->user_data);
}

static void ftk_source_idle_destroy(FtkSource* thiz)
{
	FTK_ZFREE(thiz, sizeof(FtkSource) + sizeof(PrivInfo));

	return;
}

FtkSource* ftk_source_idle_create(FtkIdle action, void* user_data)
{
	FtkSource* thiz = NULL;
	return_val_if_fail(action != NULL, NULL);

	thiz = (FtkSource*)FTK_ZALLOC(sizeof(FtkSource) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->get_fd   = ftk_source_idle_get_fd;
		thiz->check    = ftk_source_idle_check;
		thiz->dispatch = ftk_source_idle_dispatch;
		thiz->destroy  = ftk_source_idle_destroy;

		thiz->ref = 1;
		priv->action = action;
		priv->user_data = user_data;
	}

	return thiz;
}

#ifdef FTK_SOURCE_IDLE_TEST

Ret my_action(void* user_data)
{
	int *p = (int*)user_data;
	(*p)++;

	return *p == 10 ? RET_REMOVE : RET_OK;
}

int main(int argc, char* argv[])
{
	int n = 0;
	FtkSource* thiz = ftk_source_idle_create(my_action, &n);

	while(1)
	{
		assert(ftk_source_check(thiz) == 0);
		assert(ftk_source_get_fd(thiz) < 0);
		if(n < 9)
		{
			assert(ftk_source_dispatch(thiz) == RET_OK);
		}
		else
		{
			assert(ftk_source_dispatch(thiz) == RET_REMOVE);
			break;
		}
	}
	ftk_source_destroy(thiz);

	return 0;
}
#endif/*FTK_SOURCE_IDLE_TEST*/
