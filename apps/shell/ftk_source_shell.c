/*
 * File: ftk_source_shell.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   source to handle shell output.
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
 * 2010-08-29 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk.h"
#include "ftk_source_shell.h"

typedef struct _PrivInfo
{
	int fd;
	FtkWidget* text_view;
}PrivInfo;

static int ftk_source_shell_get_fd(FtkSource* thiz)
{
	DECL_PRIV(thiz, priv);

	return priv->fd;
}

static int ftk_source_shell_check(FtkSource* thiz)
{
	return -1;
}

static Ret ftk_source_shell_dispatch(FtkSource* thiz)
{
	int ret = 0;
	DECL_PRIV(thiz, priv);
	char buffer[4096] = {0};
	return_val_if_fail(priv->fd > 0, RET_FAIL);	

	if(thiz->disable > 0)
	{
		return RET_REMOVE;
	}

	ret = read(priv->fd, buffer, sizeof(buffer)-1);

	if(ret > 0 && priv->text_view != NULL)
	{
		buffer[ret] = '\0';
		ftk_text_view_insert_text(priv->text_view, -1, buffer, ret);
	}

	return RET_OK;
}

static void ftk_source_shell_destroy(FtkSource* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		priv->text_view = NULL;
		close(priv->fd);
		FTK_ZFREE(thiz, sizeof(*thiz) + sizeof(PrivInfo));
	}

	return;
}

FtkSource* ftk_source_shell_create(int fd, FtkWidget* text_view)
{
	FtkSource* thiz = (FtkSource*)FTK_ZALLOC(sizeof(FtkSource) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->get_fd   = ftk_source_shell_get_fd;
		thiz->check    = ftk_source_shell_check;
		thiz->dispatch = ftk_source_shell_dispatch;
		thiz->destroy  = ftk_source_shell_destroy;

		thiz->ref = 1;
		priv->fd = fd;
		priv->text_view = text_view;
	}

	return thiz;
}

