/*
 * File: ftk_combo_box.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: combo_box control.  
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
 * 2010-01-28 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_globals.h"
#include "ftk_theme.h"
#include "ftk_entry.h"
#include "ftk_dialog.h"
#include "ftk_button.h"
#include "ftk_combo_box.h"
#include "ftk_list_view.h"
#include "ftk_list_model_default.h"
#include "ftk_list_render_default.h"

typedef struct _ComboBoxPrivInfo
{
	FtkWidget* entry;
	FtkWidget* button;
	FtkListModel* model;
	int selected;
	FtkListener listener;
	void* listener_ctx;
}PrivInfo;

static Ret ftk_combo_box_on_event(FtkWidget* thiz, FtkEvent* event)
{
	return RET_OK;
}

static Ret ftk_combo_box_on_paint(FtkWidget* thiz)
{
	return RET_OK;
}

static void ftk_combo_box_destroy(FtkWidget* thiz)
{
	
	DECL_PRIV0(thiz, priv);

	if(priv != NULL)
	{
		ftk_list_model_unref(priv->model);
		priv->model = NULL;
		FTK_ZFREE(thiz->priv_subclass[0], sizeof(PrivInfo));
	}

	return;
}

static Ret ftk_popup_on_item_clicked(void* ctx, void* list)
{
	FtkListItemInfo* info = NULL;
	int i = ftk_list_view_get_selected((FtkWidget*)list);
	FtkListModel* model = ftk_list_view_get_model((FtkWidget*)list);
	Ret ret = RET_OK;

	ftk_list_model_get_data(model, i, (void**)&info);
	if(info != NULL)
	{
	    FtkWidget* thiz = (FtkWidget*)info->user_data;
	    DECL_PRIV0(thiz, priv);

		ftk_combo_box_set_text(thiz, info->text);

		priv->selected = i;

        ret = FTK_CALL_LISTENER(priv->listener, priv->listener_ctx, thiz);

        priv->model->listener_ctx = NULL;
	}
	ftk_widget_unref((FtkWidget*)ctx);

	return ret;
}

static Ret ftk_combo_box_popup_rect(FtkWidget* thiz, int* x, int* y, int* w, int* h)
{
	int ox = 0;
	int oy = 0;
	int nr  = 0;
	int width  = 0;
	int height = 0;
	DECL_PRIV0(thiz, priv);
	FtkWidget* win = ftk_widget_toplevel(thiz);
	int win_heigth = ftk_widget_height(win);
	int margin = (FTK_H_MARGIN + FTK_DIALOG_BORDER)*2;

	ox = ftk_widget_left_abs(thiz);
	width = ftk_widget_width(thiz);
	oy = ftk_widget_top_in_window(priv->entry) + ftk_widget_height(priv->entry);

	height = ftk_list_model_get_total(priv->model) * FTK_POPUP_MENU_ITEM_HEIGHT + margin;
	
	if((height + oy) > win_heigth)
	{
		if(oy < (win_heigth)/2)
		{
			height = win_heigth - oy;
			nr = (height - margin)/FTK_POPUP_MENU_ITEM_HEIGHT;
			height = nr * FTK_POPUP_MENU_ITEM_HEIGHT + margin;
		}
		else
		{
			height = FTK_MIN(height, oy - ftk_widget_height(priv->entry));
			nr = (height - margin)/FTK_POPUP_MENU_ITEM_HEIGHT;
			height = nr * FTK_POPUP_MENU_ITEM_HEIGHT + margin;
			oy = oy - height - ftk_widget_height(priv->entry);
		}
	}
	oy += ftk_widget_top_abs(win);

	*x = ox;
	*y = oy;
	*w = width;
	*h = height;

	return RET_OK;
}

static Ret button_drop_down_clicked(void* ctx, void* obj)
{
	int x = 0;
	int y = 0;
	int w = 200;
	int h = 100;
	FtkWidget* thiz  = (FtkWidget*)ctx;
	DECL_PRIV0(thiz, priv);
	FtkWidget* popup = NULL;
	FtkWidget* list  = NULL;
	FtkListRender* render = NULL; 

	ftk_combo_box_popup_rect(thiz, &x, &y, &w, &h);

	popup = ftk_dialog_create_ex(FTK_ATTR_POPUP, x, y, w, h);
	
	if(y < ftk_widget_top_abs(thiz))
	{
		ftk_window_set_animation_hint(popup, "combobox_up");
	}
	else
	{
		ftk_window_set_animation_hint(popup, "combobox_down");
	}

	ftk_dialog_hide_title(popup);
	w = ftk_widget_width(popup) - FTK_DIALOG_BORDER * 2;
	h = ftk_widget_height(popup) - FTK_DIALOG_BORDER * 2;
	list = ftk_list_view_create(popup, 0, 0, w, h);
	ftk_widget_show(list, 1);

	render = ftk_list_render_default_create();
	ftk_list_view_init(list, priv->model, render, FTK_POPUP_MENU_ITEM_HEIGHT);
	ftk_list_view_set_clicked_listener(list, ftk_popup_on_item_clicked, popup);

	ftk_widget_show_all(popup, 1);

	return RET_OK;
}

FtkWidget* ftk_combo_box_create(FtkWidget* parent, int x, int y, int width, int height)
{
	FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	
	return_val_if_fail(thiz != NULL, NULL);

	width = width < height ? (height << 1) : width;
	thiz->priv_subclass[0] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[0] != NULL)
	{
		int h = 0;
		int w = 0;
		FtkGc gc = {0};
		DECL_PRIV0(thiz, priv);
		thiz->on_event = ftk_combo_box_on_event;
		thiz->on_paint = ftk_combo_box_on_paint;
		thiz->destroy  = ftk_combo_box_destroy;

		ftk_widget_init(thiz, FTK_COMBO_BOX, 0, x, y, width, height, FTK_ATTR_TRANSPARENT);
		ftk_widget_append_child(parent, thiz);

		priv->entry = ftk_entry_create(thiz, 0, 0, width-height, height);
		h = ftk_widget_height(priv->entry);
		w = ftk_widget_width(priv->entry);
		ftk_widget_move_resize(priv->entry, 0, FTK_HALF(height-h), width - h, h);
		ftk_widget_show(priv->entry, 1);

		priv->button = ftk_button_create(thiz, width - h, FTK_HALF(height-h), h, h); 
		ftk_button_set_clicked_listener(priv->button, button_drop_down_clicked, thiz);

		ftk_widget_set_attr(priv->button, FTK_ATTR_BG_CENTER);
		gc.mask = FTK_GC_BITMAP;
		gc.bitmap = ftk_theme_load_image(ftk_default_theme(), "drop_down_normal"FTK_STOCK_IMG_SUFFIX);
		ftk_widget_set_gc(priv->button, FTK_WIDGET_NORMAL, &gc);
		ftk_gc_reset(&gc);
		
		gc.mask = FTK_GC_BITMAP;
		gc.bitmap = ftk_theme_load_image(ftk_default_theme(), "drop_down_selected"FTK_STOCK_IMG_SUFFIX);
		ftk_widget_set_gc(priv->button, FTK_WIDGET_FOCUSED, &gc);
		ftk_gc_reset(&gc);
		
		gc.mask = FTK_GC_BITMAP;
		gc.bitmap = ftk_theme_load_image(ftk_default_theme(), "drop_down_pressed"FTK_STOCK_IMG_SUFFIX);
		ftk_widget_set_gc(priv->button, FTK_WIDGET_ACTIVE, &gc);
		ftk_gc_reset(&gc);

		ftk_widget_show(priv->button, 1);

		ftk_widget_resize(thiz, width, h + (FTK_V_MARGIN << 1));

		priv->model = ftk_list_model_default_create(10);

		priv->selected = -1;
	}
	else
	{
		FTK_FREE(thiz);
	}

	return thiz;
}

const char* ftk_combo_box_get_text(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL, NULL);

	return ftk_entry_get_text(priv->entry);
}

Ret ftk_combo_box_set_text(FtkWidget* thiz, const char* text)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL && text != NULL, RET_FAIL);

	return ftk_entry_set_text(priv->entry, text);	
}

FtkWidget* ftk_combo_box_get_entry(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL, NULL);

	return priv->entry;
}

int ftk_combo_box_get_selected(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL, -1);

    return priv->selected;
}

Ret ftk_combo_box_append(FtkWidget* thiz, FtkBitmap* icon, const char* text)
{
	DECL_PRIV0(thiz, priv);
	FtkListItemInfo info = {0};
	return_val_if_fail(thiz != NULL && text != NULL, RET_FAIL);

	info.type = FTK_LIST_ITEM_NORMAL;
	info.text = (char *)text;
	info.left_icon = icon;
	info.user_data = thiz;
	
	return ftk_list_model_add(priv->model, &info);
}

Ret ftk_combo_box_reset(FtkWidget* thiz)
{
    DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL, RET_FAIL);

    ftk_combo_box_set_text(thiz, " ");

    priv->selected = -1;

    return ftk_list_model_reset(priv->model);
}

Ret ftk_combo_box_remove(FtkWidget* thiz, size_t index)
{
    DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL, RET_FAIL);

    if(priv->selected == index)
    {
        ftk_combo_box_set_text(thiz, " ");

        priv->selected = -1;
    }

    return ftk_list_model_remove(priv->model, index);
}

Ret ftk_combo_box_set_clicked_listener(FtkWidget* thiz, FtkListener listener, void* ctx)
{
    DECL_PRIV0(thiz, priv);
    return_val_if_fail(thiz != NULL, RET_FAIL);

    priv->listener_ctx = ctx;
    priv->listener = listener;

    return RET_OK;
}

int ftk_combo_box_get_item_nr(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(thiz != NULL, 0);

	return ftk_list_model_get_total(priv->model);
}

Ret ftk_combo_box_get_item(FtkWidget* thiz, size_t index, const FtkBitmap** icon, const char** text)
{
	DECL_PRIV0(thiz, priv);
	FtkListItemInfo* info = NULL;
	return_val_if_fail(thiz != NULL, 0);

	ftk_list_model_get_data(priv->model, index, (void**)&info);
	if(info != NULL)
	{
		if(icon != NULL)
		{
			*icon = info->left_icon;
		}

		if(text != NULL)
		{
			*text = info->text;
		}
	}

	return info != NULL ? RET_OK : RET_FAIL;
}
