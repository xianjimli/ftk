/*
 * File: ftk_painter.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   painter
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
 * 2009-12-17 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_painter.h"

typedef struct _PainterPrivInfo
{
	FtkListener listener;
	void* listener_ctx;
}PrivInfo;

static Ret ftk_painter_on_event(FtkWidget* thiz, FtkEvent* event)
{
	return RET_OK;
}

static Ret ftk_painter_on_paint(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	
	if(priv->listener != NULL)
	{
		priv->listener(priv->listener_ctx, thiz);
	}

	return RET_OK;
}

static void ftk_painter_destroy(FtkWidget* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV0(thiz, priv);
		FTK_ZFREE(priv, sizeof(PrivInfo));
	}

	return;
}

FtkWidget* ftk_painter_create(FtkWidget* parent, int x, int y, int width, int height)
{
	FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	return_val_if_fail(thiz != NULL, NULL);
	
	thiz->priv_subclass[0] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[0] != NULL)
	{
		thiz->on_event = ftk_painter_on_event;
		thiz->on_paint = ftk_painter_on_paint;
		thiz->destroy  = ftk_painter_destroy;

		ftk_widget_init(thiz, FTK_PAINTER, 0, x, y, width, height, 0);
		ftk_widget_append_child(parent, thiz);
	}
	else
	{
		FTK_FREE(thiz);
	}

	return thiz;
}

Ret ftk_painter_set_paint_listener(FtkWidget* thiz, FtkListener listener, void* ctx)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	priv->listener_ctx = ctx;
	priv->listener = listener;

	return RET_OK;
}


