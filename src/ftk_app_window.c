/*
 * File: ftk_app_window.c
 * Author: Li XianJing <xianjimli@hotmail.com>
 * Brief:  normal application window.
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
 * 2009-10-31 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_window.h"
#include "ftk_globals.h"
#include "ftk_menu_panel.h"
#include "ftk_app_window.h"

typedef struct _AppWinPrivInfo
{
	FtkPrepareOptionsMenu on_prepare_options_menu;
	void* on_prepare_options_menu_ctx;

	FtkWidgetOnEvent parent_on_event;
	FtkWidgetOnPaint parent_on_paint;
	FtkWidgetDestroy parent_destroy;
}PrivInfo;

static Ret  ftk_app_window_on_event(FtkWidget* thiz, FtkEvent* event)
{
	DECL_PRIV1(thiz, priv);

	if(event->type == FTK_EVT_KEY_UP 
		&& event->u.key.code == FTK_KEY_MENU
		&& priv->on_prepare_options_menu != NULL)
	{
		FtkWidget* menu_panel = ftk_menu_panel_create();
		if(priv->on_prepare_options_menu(priv->on_prepare_options_menu_ctx, menu_panel) == RET_OK)
		{
			ftk_menu_panel_relayout(menu_panel);
			ftk_widget_show_all(menu_panel, 1);
		}
		else
		{
			ftk_widget_unref(menu_panel);
		}
		ftk_logd("%s\n", __func__);

		return RET_REMOVE;
	}
	else if(event->type == FTK_EVT_KEY_UP && event->u.key.code == FTK_KEY_RETURN)
	{
		if(!ftk_widget_has_attr(thiz, FTK_ATTR_IGNORE_CLOSE))
		{
			ftk_widget_unref(thiz);
		}

		return RET_REMOVE;
	}

	return priv->parent_on_event(thiz, event);
}

static Ret  ftk_app_window_on_paint(FtkWidget* thiz)
{
	DECL_PRIV1(thiz, priv);

	return priv->parent_on_paint(thiz);
}

static void ftk_app_window_destroy(FtkWidget* thiz)
{
	DECL_PRIV1(thiz, priv);
	FtkWidgetDestroy parent_destroy = priv->parent_destroy;
	parent_destroy(thiz);
	FTK_ZFREE(thiz->priv_subclass[1], sizeof(PrivInfo));

	return;
}

FtkWidget* ftk_app_window_create(void)
{
	FtkWidget* thiz = ftk_window_create(FTK_WINDOW, 0, 0, 0, 0, 0);
	return_val_if_fail(thiz != NULL, NULL);

	thiz->priv_subclass[1] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[1] != NULL)
	{
		DECL_PRIV1(thiz, priv);
		priv->parent_on_event = thiz->on_event;
		priv->parent_on_paint = thiz->on_paint;
		priv->parent_destroy  = thiz->destroy;
		thiz->on_event = ftk_app_window_on_event;
		thiz->on_paint = ftk_app_window_on_paint;
		thiz->destroy  = ftk_app_window_destroy;
	}

	return thiz;
}

Ret ftk_app_window_set_on_prepare_options_menu(FtkWidget* thiz, 
	FtkPrepareOptionsMenu on_prepare_options_menu, void* ctx)
{
	DECL_PRIV1(thiz, priv);
	
	priv->on_prepare_options_menu = on_prepare_options_menu;
	priv->on_prepare_options_menu_ctx = ctx;

	return RET_OK;
}

