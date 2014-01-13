/*
 * File: ftk_popup_menu.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   popup menu
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
 * 2010-01-26 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_dialog.h"
#include "ftk_list_view.h"
#include "ftk_popup_menu.h"
#include "ftk_list_render_default.h"

typedef struct _PopupMenuPrivInfo
{
	FtkWidget*     list;
	FtkListModel*  model;
	FtkListener    listener;
	void*          listener_ctx;
	FtkWidgetDestroy parent_destroy;
}PrivInfo;

static Ret ftk_popup_menu_init(FtkWidget* thiz);

static void ftk_popup_menu_destroy(FtkWidget* thiz)
{
	DECL_PRIV2(thiz, priv);

	if(priv->parent_destroy != NULL)
	{
		priv->parent_destroy(thiz);
	}

	FTK_ZFREE(priv, sizeof(PrivInfo));

	return;
}
static Ret ftk_popup_menu_on_item_clicked(void* ctx, void* list)
{
	FtkWidget* thiz = (FtkWidget*)ctx;
	FtkListItemInfo* info = NULL;
	int i = ftk_list_view_get_selected((FtkWidget*)list);
	FtkListModel* model = ftk_list_view_get_model((FtkWidget*)list);
	ftk_list_model_get_data(model, i, (void**)&info);
	
	if(info != NULL)
	{
		DECL_PRIV2(thiz, priv);
		if(priv->listener != NULL)
		{
			priv->listener(priv->listener_ctx, info);
		}
	}

	ftk_widget_unref(thiz);

	ftk_logd("%s: %d/%d\n", __func__, i, ftk_list_model_get_total(model));

	return RET_OK;
}

FtkWidget* ftk_popup_menu_create(int x, int y, int w, int h, FtkBitmap* icon, const char* title)
{
	PrivInfo* priv = NULL;
	int has_title = icon != NULL || title != NULL;
	FtkWidget* thiz = ftk_dialog_create(x, y, w, h);
	
	return_val_if_fail(thiz != NULL, NULL);

	thiz->priv_subclass[2] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	priv = (PrivInfo*)thiz->priv_subclass[2];
	if(has_title)
	{
		ftk_dialog_set_icon(thiz, icon);
		ftk_widget_set_text(thiz, title);
	}
	else
	{
		ftk_dialog_hide_title(thiz);
	}

	priv->parent_destroy = thiz->destroy;
	thiz->destroy = ftk_popup_menu_destroy;

	ftk_popup_menu_init(thiz);

	return thiz;
}

static Ret ftk_popup_menu_init(FtkWidget* thiz)
{
	int w = 0;
	int h = 0;
	DECL_PRIV2(thiz, priv);
	FtkWidget* list = NULL;
	FtkListModel* model = NULL;
	FtkListRender* render = NULL;
	
	return_val_if_fail(thiz != NULL, RET_FAIL);

	w = ftk_widget_width(thiz)  - 2 * FTK_DIALOG_BORDER;
	h = ftk_widget_height(thiz) - FTK_DIALOG_BORDER - ftk_dialog_get_title_height();

	list = ftk_list_view_create(thiz, 0, 0, w, h);
	ftk_list_view_set_clicked_listener(list, ftk_popup_menu_on_item_clicked, thiz);

	model = ftk_list_model_default_create(10);
	render = ftk_list_render_default_create();

	priv->model = model;
	priv->list = list;

	ftk_window_set_focus(thiz, list);
	ftk_list_view_init(list, model, render, FTK_POPUP_MENU_ITEM_HEIGHT);
	ftk_list_model_unref(model);

	return RET_OK;
}

Ret ftk_popup_menu_add(FtkWidget* thiz, FtkListItemInfo* info)
{
	DECL_PRIV2(thiz, priv);
	return_val_if_fail(priv != NULL && info != NULL, RET_FAIL);

	return ftk_list_model_add(priv->model, info);
}

Ret ftk_popup_menu_set_clicked_listener(FtkWidget* thiz, FtkListener listener, void* ctx)
{
	DECL_PRIV2(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);

	priv->listener = listener;
	priv->listener_ctx = ctx;

	return RET_OK;
}

int ftk_popup_menu_get_selected(FtkWidget* thiz)
{
	DECL_PRIV2(thiz, priv);
	return_val_if_fail(thiz != NULL, -1);

	return ftk_list_view_get_selected(priv->list);
}

int ftk_popup_menu_calc_height(int has_title, int visible_items)
{
	int height = visible_items * FTK_POPUP_MENU_ITEM_HEIGHT;
	
	height += 2 * FTK_DIALOG_BORDER;
	if(has_title)
	{
		height += ftk_dialog_get_title_height();
	}

	return height;
}

