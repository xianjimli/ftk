/*
 * File: ftk_input_method_preeditor.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   default implementation of input candidate selector.
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
 * 2010-02-11 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_button.h"
#include "ftk_window.h"
#include "ftk_globals.h"
#include "ftk_list_view.h"
#include "ftk_text_buffer.h"
#include "ftk_list_model_default.h"
#include "ftk_list_render_default.h"
#include "ftk_input_method_preeditor.h"

typedef struct _InputMethodPreeditorPrivInfo
{
	FtkWidget* widget;
	FtkWidget* editor;
	FtkListModel* model;
	FtkWidget* raw_text_button;
	FtkTextBuffer* text_buffer;
}PrivInfo;

#define IDC_RAW_TEXT    1000
#define IDC_CANDIDATES  1001
#define FTK_IM_PREEDITOR_WIDTH       120
#define FTK_IM_PREEDITOR_ITEM_HEIGHT 20
#define FTK_ROW_HEIGHT (FTK_FONT_SIZE + 2 * FTK_V_MARGIN)

static Ret ftk_popup_on_raw_text_clicked(void* ctx, void* button)
{
	PrivInfo* priv = (PrivInfo*)ctx;
	FtkEvent event;
	
	ftk_event_init(&event,   FTK_EVT_IM_COMMIT);
	event.widget   = priv->editor;
	event.u.extra  = (char*)ftk_widget_get_text((FtkWidget*)button);

	ftk_widget_event(priv->editor, &event);
	ftk_widget_show_all(ftk_widget_toplevel((FtkWidget*)button), 0);
	ftk_widget_set_text((FtkWidget*)button, "");

	return RET_OK;
}

static Ret ftk_popup_on_item_clicked(void* ctx, void* list)
{
	PrivInfo* priv = (PrivInfo*)ctx;
	FtkListItemInfo* info = NULL;
	FtkListModel* model = priv->model;
	int i = ftk_list_view_get_selected((FtkWidget*)list);
	
	ftk_list_model_get_data(model, i, (void**)&info);
	if(info != NULL)
	{
		FtkEvent event;
		
		ftk_event_init(&event,   FTK_EVT_IM_COMMIT);
		event.widget   = priv->editor;
		event.u.extra  = (void*)info->text;

		ftk_widget_event(priv->editor, &event);
	}
	ftk_widget_show_all(ftk_widget_toplevel((FtkWidget*)list), 0);

	return RET_OK;
}

static Ret ftk_input_method_preeditor_default_resize(FtkImPreeditor* thiz, int candidate_nr, FtkPoint* caret)
{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	DECL_PRIV(thiz, priv);
	FtkRect work_rect = {0};
	FtkWidget* widget = priv->widget;

	w = FTK_IM_PREEDITOR_WIDTH;
	h = (candidate_nr + 1) * FTK_IM_PREEDITOR_ITEM_HEIGHT + 2 * FTK_V_MARGIN;
	ftk_wnd_manager_get_work_area(ftk_default_wnd_manager(), &work_rect);

	if((caret->y + h + FTK_ROW_HEIGHT) <= (work_rect.y + work_rect.height))
	{
		y = caret->y + FTK_ROW_HEIGHT;
	}
	else
	{
		y = caret->y - h;
	}

	if((caret->x + w) <= (work_rect.x + work_rect.width))
	{
		x = caret->x;
	}
	else
	{
		x = work_rect.x + work_rect.width - w;
	}

	ftk_widget_move_resize(widget, x, y, w, h);
	ftk_widget_move_resize(ftk_widget_lookup(widget, IDC_RAW_TEXT), 0, 
		0, w, FTK_IM_PREEDITOR_ITEM_HEIGHT);
	ftk_widget_move_resize(ftk_widget_lookup(widget, IDC_CANDIDATES), 0, 
		FTK_IM_PREEDITOR_ITEM_HEIGHT, w, h - FTK_IM_PREEDITOR_ITEM_HEIGHT);

	return RET_OK;
}

static Ret ftk_input_method_preeditor_default_reset(FtkImPreeditor* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	ftk_list_model_reset(priv->model);
	ftk_text_buffer_reset(priv->text_buffer);
	ftk_widget_set_text(priv->raw_text_button, "          ");

	return RET_OK;
}

static Ret ftk_input_method_preeditor_default_set_editor(FtkImPreeditor* thiz, FtkWidget* editor)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && editor != NULL, RET_FAIL);

	priv->editor = editor;

	return RET_OK;
}

static Ret ftk_input_method_preeditor_default_set_raw_text(FtkImPreeditor* thiz, const char* text)
{
	DECL_PRIV(thiz, priv);
	
	return_val_if_fail(priv != NULL, RET_FAIL);

	ftk_widget_set_text(priv->raw_text_button, text);

	return RET_OK;
}

static Ret ftk_input_method_preeditor_default_add_candidate(FtkImPreeditor* thiz, const char* text)
{
	FtkListItemInfo info = {0};
	DECL_PRIV(thiz, priv);

	return_val_if_fail(priv != NULL && text != NULL, RET_FAIL);
	
	info.user_data = thiz;
	info.type = FTK_LIST_ITEM_NORMAL;
	info.text = ftk_text_buffer_append_string(priv->text_buffer, text);

	return ftk_list_model_add(priv->model, &info);
}

static void ftk_input_method_preeditor_default_destroy(FtkImPreeditor* thiz)
{
	DECL_PRIV(thiz, priv);
	if(thiz != NULL)
	{
		ftk_list_model_unref(priv->model);
		ftk_text_buffer_destroy(priv->text_buffer);
		FTK_FREE(thiz);
	}

	return;
}

static Ret ftk_input_method_preeditor_default_hide(FtkImPreeditor* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	ftk_widget_show_all(priv->widget, 0);

	return RET_OK;
}

static Ret ftk_input_method_preeditor_default_show(FtkImPreeditor* thiz, FtkPoint* caret)
{
	DECL_PRIV(thiz, priv);
	int candidate_nr = 0;
	return_val_if_fail(thiz != NULL, RET_FAIL);

	ftk_wnd_manager_restack(ftk_default_wnd_manager(), priv->widget, FTK_MAX_WINDOWS);
	
	candidate_nr = ftk_list_model_get_total(priv->model);
	candidate_nr = FTK_MIN(candidate_nr, FTK_IM_PREEDITOR_MAX_ROW);
	ftk_input_method_preeditor_default_resize(thiz, candidate_nr, caret);
	ftk_widget_show_all(priv->widget, 1);

	return RET_OK;
}

FtkImPreeditor* ftk_input_method_preeditor_default_create(void)
{
	int x = 0;
	int y = 0;
	FtkGc gc = {0};
	FtkWidget* list  = NULL;
	FtkWidget* button = NULL;
	FtkWidget* popup = NULL;
	FtkListModel* model = NULL;
	FtkListRender* render = NULL;
	FtkImPreeditor* thiz = NULL;
	int w = FTK_IM_PREEDITOR_WIDTH;
	int h = FTK_IM_PREEDITOR_ITEM_HEIGHT * FTK_IM_PREEDITOR_MAX_ROW;

	model = ftk_list_model_default_create(10);
	return_val_if_fail(model != NULL, NULL);

	popup = ftk_window_create(FTK_WINDOW_MISC, 0, x, y, w, h);
	ftk_window_set_animation_hint(popup, "im_preeditor");
	ftk_widget_set_text(popup, "im preeditor");
	render = ftk_list_render_default_create();
	button = ftk_button_create(popup, 0, 0, w, FTK_IM_PREEDITOR_ITEM_HEIGHT);
	ftk_widget_set_id(button, IDC_RAW_TEXT);
	ftk_widget_unset_attr(button, FTK_ATTR_TRANSPARENT);

	gc.mask = FTK_GC_BG | FTK_GC_FG | FTK_GC_FONT;
	gc.font = ftk_default_font();
	gc.bg = ftk_theme_get_bg_color(ftk_default_theme(), FTK_BUTTON, FTK_WIDGET_NORMAL);
	gc.fg = ftk_theme_get_fg_color(ftk_default_theme(), FTK_BUTTON, FTK_WIDGET_NORMAL);
	ftk_widget_reset_gc(button, FTK_WIDGET_NORMAL, &gc);
	
	gc.bg = ftk_theme_get_bg_color(ftk_default_theme(), FTK_BUTTON, FTK_WIDGET_FOCUSED);
	gc.fg = ftk_theme_get_fg_color(ftk_default_theme(), FTK_BUTTON, FTK_WIDGET_FOCUSED);
	ftk_widget_reset_gc(button, FTK_WIDGET_FOCUSED, &gc);
	
	gc.bg = ftk_theme_get_bg_color(ftk_default_theme(), FTK_BUTTON, FTK_WIDGET_ACTIVE);
	gc.fg = ftk_theme_get_fg_color(ftk_default_theme(), FTK_BUTTON, FTK_WIDGET_ACTIVE);
	ftk_widget_reset_gc(button, FTK_WIDGET_ACTIVE, &gc);

	list = ftk_list_view_create(popup, 0, FTK_IM_PREEDITOR_ITEM_HEIGHT, 
		w, h - FTK_IM_PREEDITOR_ITEM_HEIGHT);
	ftk_widget_set_id(list, IDC_CANDIDATES);
	ftk_window_set_focus(popup, list);

	ftk_list_view_init(list, model, render, FTK_IM_PREEDITOR_ITEM_HEIGHT);

	thiz = FTK_NEW_PRIV(FtkImPreeditor);

	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->hide          = ftk_input_method_preeditor_default_hide;
		thiz->show          = ftk_input_method_preeditor_default_show;
		thiz->reset         = ftk_input_method_preeditor_default_reset;
		thiz->set_editor    = ftk_input_method_preeditor_default_set_editor;
		thiz->set_raw_text  = ftk_input_method_preeditor_default_set_raw_text;
		thiz->add_candidate = ftk_input_method_preeditor_default_add_candidate;
		thiz->destroy       = ftk_input_method_preeditor_default_destroy;

		priv->model = model;
		priv->widget = popup;
		priv->raw_text_button = button;
		priv->text_buffer = ftk_text_buffer_create(1024);

		ftk_button_set_clicked_listener(button, ftk_popup_on_raw_text_clicked, priv);
		ftk_list_view_set_clicked_listener(list, ftk_popup_on_item_clicked, priv);
	}
	else
	{
		ftk_widget_unref(popup);
	}

	return thiz;
}

