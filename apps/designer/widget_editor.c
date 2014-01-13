/*
 * File: widget_editor.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: widget property editor.
 *
 * Copyright (c) 2009 - 2011  Li XianJing <xianjimli@hotmail.com>
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
 * 2011-09-27 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_util.h"
#include "widgets_info.h"
#include "widget_editor.h"

#define IS_WIN(widget) (widget != NULL && ftk_widget_type(widget) == FTK_WINDOW)

enum _WidgetId
{
	IDC_TYPE = 1,
	IDC_ID,
	IDC_X,
	IDC_Y,
	IDC_W,
	IDC_H,
	IDC_TEXT
};

typedef struct _Info
{
	FtkWidget* parent;
	FtkWidget* widget;
	FtkWidget* widget_type;
	FtkWidget* widget_id;
	FtkWidget* widget_x;
	FtkWidget* widget_y;
	FtkWidget* widget_w;
	FtkWidget* widget_h;
	FtkWidget* widget_text;
	FtkWidget* widget_fullscreen;
	FtkWidget* widget_anim_hint;
}Info;

static Info* info_create(void)
{
	Info* info = FTK_NEW(Info);
	
	return info;
}

static void info_destroy(void* data)
{
	FTK_FREE(data);
	
	return;
}

static Ret button_ok_clicked(void* ctx, void* obj)
{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	
	FtkWidget* win = (FtkWidget*)ctx;
	const WidgetInfo* widget_info = NULL;
	Info* info = (Info*)ftk_widget_user_data(win);
	FtkWidget* widget = info->widget;
	FtkWidget* parent = info->parent;
	
	if(IS_WIN(widget))
	{
		ftk_window_set_fullscreen(widget, ftk_check_button_get_checked(info->widget_fullscreen));
		ftk_window_set_animation_hint(widget, ftk_widget_get_text(info->widget_anim_hint));
	}
	else
	{
		widget_info = widgets_info_find(ftk_combo_box_get_text(info->widget_type));
		return_val_if_fail(widget_info != NULL, RET_FAIL);
		
		x = ftk_atoi(ftk_widget_get_text(ftk_widget_lookup(win, IDC_X)));
		y = ftk_atoi(ftk_widget_get_text(ftk_widget_lookup(win, IDC_Y)));
		w = ftk_atoi(ftk_widget_get_text(ftk_widget_lookup(win, IDC_W)));
		h = ftk_atoi(ftk_widget_get_text(ftk_widget_lookup(win, IDC_H)));
		
		x = x < 0 ? 0 : x;
		y = y < 0 ? 0 : y;
		w = w > widget_info->min_width ? w :  widget_info->min_width;
		h = h > widget_info->min_height ? h :  widget_info->min_height;

		if(info->widget != NULL && widget_info->type != ftk_widget_type(info->widget))
		{
			ftk_widget_remove_child(ftk_widget_parent(info->widget), info->widget);
			info->widget = NULL;
		}
		
		if(info->widget == NULL)
		{
			info->widget = widget_info->create(parent, x, y, w, h);
			if(info->widget != NULL)
			{
				ftk_widget_ref(info->widget);
				ftk_widget_show(info->widget, 1);
			}
		}
		else
		{
			ftk_widget_move_resize(info->widget, x, y, w, h);
		}
	}
	ftk_widget_set_text(info->widget, ftk_widget_get_text(ftk_widget_lookup(win, IDC_TEXT)));

	ftk_widget_unref(win);	

	return RET_OK;
}

static Ret button_cancel_clicked(void* ctx, void* obj)
{
	FtkWidget* win = (FtkWidget*)ctx;
	
	ftk_widget_unref(win);

	return RET_OK;
}

static Ret type_on_event(void* ctx, void* data)
{
	const char* type = NULL;
	Info* info = (Info*)ctx;
	WidgetInfo* widget_info = NULL;
	FtkEvent* event = (FtkEvent*)data;

	if(event->type == FTK_EVT_SET_TEXT)
	{
		char text[64] = {0};
		type =  (const char*)event->u.extra;

		widget_info = widgets_info_find(type);

		if(widget_info != NULL)
		{
			ftk_itoa(text, sizeof(text), widget_info->default_width);
			ftk_widget_set_text(info->widget_w, text);

			ftk_itoa(text, sizeof(text), widget_info->default_height);
			ftk_widget_set_text(info->widget_h, text);
		}
	}

	return RET_OK;
}

static FtkWidget* ftk_widget_editor_create(FtkWidget* parent, FtkWidget* widget)
{
	int i = 0;
	int nr = 0;	
	int width = 0;
	int height = 0;
	int y_offset = 0;
	Info* info = NULL;
	FtkWidget* win = NULL;
	FtkWidget* label = NULL;
	FtkWidget* entry = NULL;	
	FtkWidget* button = NULL;
	FtkWidget* combo_box = NULL;
	FtkWidget* check_button = NULL;
	const WidgetInfo* widget_info = NULL;
	return_val_if_fail(parent != NULL, NULL);
	
	info = info_create();
	return_val_if_fail(info != NULL, NULL);
	
	info->parent = parent;
	info->widget = widget;
	
	win = ftk_app_window_create();
	ftk_widget_set_user_data(win, info_destroy, info);
	ftk_widget_set_text(win, _("Widget Editor"));
	
	width = ftk_widget_width(win);
	height = ftk_widget_height(win);
	
	y_offset = 0;

	if(IS_WIN(widget))
	{
		info->widget_fullscreen = check_button = ftk_check_button_create(win, width/5, y_offset, width/2, 50);
		ftk_widget_set_text(check_button, _("Full screen"));
		y_offset += 50;
		
		label = ftk_label_create(win, 0, y_offset, width/5, 30);
		ftk_widget_set_text(label, _("AnimHint:"));
		info->widget_anim_hint = entry = ftk_entry_create(win, width/5, y_offset, width*4/5-2, 30);
		ftk_widget_set_text(entry, ftk_window_get_animation_hint(ftk_widget_toplevel(parent)));
		y_offset += 20;
	}
	else
	{
		label = ftk_label_create(win, 0, y_offset + 10, width/5, 30);
		ftk_widget_set_text(label, _("Type:"));
		
		info->widget_type = combo_box = ftk_combo_box_create(win, width/5, y_offset, width*4/5-2, 50);
		ftk_widget_set_id(combo_box, IDC_TYPE);
		ftk_entry_set_readonly(ftk_combo_box_get_entry(combo_box), 1);
		ftk_widget_set_event_listener(ftk_combo_box_get_entry(combo_box), type_on_event, info);

		nr = widgets_info_get_nr();
		for(i = 0; i < nr; i++)
		{
			widget_info = widgets_info_get(i);	
			ftk_combo_box_append(combo_box, NULL, widget_info->name);
		}
		
		y_offset += 50;
		label = ftk_label_create(win, 0, y_offset, width/5, 30);
		ftk_widget_set_text(label, _("ID:"));
		info->widget_id = entry = ftk_entry_create(win, width/5, y_offset, width*4/5-2, 30);
		ftk_widget_set_id(entry, IDC_ID);
		
		y_offset += 30;
		label = ftk_label_create(win, 0, y_offset, width/5, 30);
		ftk_widget_set_text(label, _("X:"));
		info->widget_x = entry = ftk_entry_create(win, width/5, y_offset, width*4/5-2, 30);
		ftk_widget_set_id(entry, IDC_X);

		y_offset += 30;
		label = ftk_label_create(win, 0, y_offset, width/5, 30);
		ftk_widget_set_text(label, _("Y:"));
		info->widget_y =entry = ftk_entry_create(win, width/5, y_offset, width*4/5-2, 30);
		ftk_widget_set_id(entry, IDC_Y);
		
		y_offset += 30;
		label = ftk_label_create(win, 0, y_offset, width/5, 30);
		ftk_widget_set_text(label, _("Width:"));
		info->widget_w = entry = ftk_entry_create(win, width/5, y_offset, width*4/5-2, 30);
		ftk_widget_set_id(entry, IDC_W);
		
		y_offset += 30;
		label = ftk_label_create(win, 0, y_offset, width/5, 30);
		ftk_widget_set_text(label, _("Height:"));
		info->widget_h = entry = ftk_entry_create(win, width/5, y_offset, width*4/5-2, 30);
		ftk_widget_set_id(entry, IDC_H);
	}

	y_offset += 30;
	label = ftk_label_create(win, 0, y_offset, width/5, 30);
	ftk_widget_set_text(label, _("Text:"));
	info->widget_text = entry = ftk_entry_create(win, width/5, y_offset, width*4/5-2, 30);
	ftk_widget_set_id(entry, IDC_TEXT);
	
	button = ftk_button_create(win, width/5, height-60, width/5, 50);
	ftk_widget_set_text(button, _("OK"));
	ftk_button_set_clicked_listener(button, button_ok_clicked, win);

	button = ftk_button_create(win, width*3/5, height-60, width/5, 50);
	ftk_widget_set_text(button, _("Cancel"));
	ftk_button_set_clicked_listener(button, button_cancel_clicked, win);
	
	ftk_widget_show_all(win, 1);
	
	return win;
}

Ret ftk_widget_editor_edit(FtkWidget* parent, FtkWidget* widget)
{
	char text[64];
	FtkWidget* win = NULL;
	Info* info = NULL;
	const WidgetInfo* widget_info = NULL;
	
	win = ftk_widget_editor_create(parent, widget);
	info = (Info*)ftk_widget_user_data(win);
	
	if(IS_WIN(widget))
	{
		ftk_check_button_set_checked(info->widget_fullscreen, ftk_window_is_fullscreen(widget));
	}
	else
	{
		widget_info = widgets_info_find_by_type(ftk_widget_type(widget));
		ftk_combo_box_set_text(info->widget_type, (widget_info->name));
		
		ftk_itoa(text, sizeof(text), ftk_widget_id(widget));
		ftk_widget_set_text(info->widget_id, text);
		
		ftk_itoa(text, sizeof(text), ftk_widget_left(widget));
		ftk_widget_set_text(info->widget_x, text);

		ftk_itoa(text, sizeof(text), ftk_widget_top(widget));
		ftk_widget_set_text(info->widget_y, text);

		ftk_itoa(text, sizeof(text), ftk_widget_width(widget));
		ftk_widget_set_text(info->widget_w, text);

		ftk_itoa(text, sizeof(text), ftk_widget_height(widget));
		ftk_widget_set_text(info->widget_h, text);
	}

	ftk_widget_set_text(info->widget_text, ftk_widget_get_text(widget));
	
	return RET_OK;
}

Ret ftk_widget_editor_new(FtkWidget* parent, int last_x, int last_y)
{
	char text[64];
	FtkWidget* win = NULL;
	Info* info = NULL;
	const WidgetInfo* widget_info = NULL;
	
	win = ftk_widget_editor_create(parent, NULL);
	info = (Info*)ftk_widget_user_data(win);

	widget_info = widgets_info_get(0);
	ftk_combo_box_set_text(info->widget_type, (widget_info->name));
		
	ftk_widget_set_text(info->widget_id, "0");
	
	ftk_itoa(text, sizeof(text), last_x);
	ftk_widget_set_text(info->widget_x, text);

	ftk_itoa(text, sizeof(text), last_y);
	ftk_widget_set_text(info->widget_y, text);

	ftk_widget_set_text(info->widget_w, "60");
	ftk_widget_set_text(info->widget_h, "60");
	
	return RET_OK;
}
