/*
 * File: ftk_messagebox.c
 * Author: Li XianJing <xianjimli@hotmail.com>
 * Brief:  messagebox
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
 * 2010-01-23 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_label.h"
#include "ftk_button.h"
#include "ftk_globals.h"
#include "ftk_message_box.h"

static Ret message_box_on_button_clicked(void* ctx, void* obj)
{
	*(int*)ctx = ftk_widget_id((FtkWidget*)obj);

	ftk_logd("%s: the %dth button is clicked.\n", __func__, ftk_widget_id((FtkWidget*)obj));

	return RET_QUIT;
}

#define FTK_BUTTON_DEFAULT_WIDTH   80
#define FTK_BUTTON_DEFAULT_HEIGHT  48
#define FTK_MESSAGE_BOX_MIN_HEIGHT 30

static Ret ftk_message_box_size(int has_title, int has_button, const char* text, int* w, int* h)
{
	int start  = 0;
	int width  = 0;
	int height = 0;
	int font_h = 0;
	FtkRect rect = {0};
	const char* end = text;
	FtkCanvas* canvas = ftk_shared_canvas();
	
	ftk_wnd_manager_get_work_area(ftk_default_wnd_manager(), &rect);

	width  = rect.width - 2 * (FTK_DIALOG_MARGIN + FTK_LABEL_LEFT_MARGIN + FTK_DIALOG_BORDER);

	height = 4 * FTK_V_MARGIN + FTK_DIALOG_BORDER;
	height += has_title ? ftk_dialog_get_title_height() : 0;
	height += has_button ? FTK_BUTTON_DEFAULT_HEIGHT : 0;

	font_h = ftk_font_desc_get_size(ftk_default_font());
	while(*end != '\0')
	{
		height += font_h + FTK_LABEL_TOP_MARGIN;
		end = ftk_canvas_calc_str_visible_range(canvas, text, start, -1, width, NULL);
		start = end - text;
	}

	height = height < FTK_MESSAGE_BOX_MIN_HEIGHT ? FTK_MESSAGE_BOX_MIN_HEIGHT : height;
	height = height < rect.height ? height : rect.height;

	*h = (height + 1) & 0xfffffffe;
	*w = (rect.width + 1)  & 0xfffffffe;

	return RET_OK;
}

static int ftk_count_strings(const char* buttons[FTK_MSGBOX_MAX_BUTTONS])
{
	int i = 0;
	int buttons_nr = 0;

	if(buttons != NULL)
	{
		for(i = 0; buttons[i] != NULL && i < FTK_MSGBOX_MAX_BUTTONS; i++)
		{
			buttons_nr++;
		}
	}

	return buttons_nr;
}

int ftk_message_box(FtkBitmap* icon, const char* title, const char* text, const char* buttons[FTK_MSGBOX_MAX_BUTTONS])
{
	int i = 0;
	int w = 0;
	int h = 0;
	int id = 0;
	int ret = 0;
	int width = 0;
	int height = 0;
	int xoffset = 0;
	int yoffset = 0;
	int h_margin = 0;
	int has_title = icon != NULL || title != NULL;
	int buttons_nr = ftk_count_strings(buttons);

	FtkWidget* label = NULL;
	FtkWidget* button = NULL;
	FtkWidget* dialog = NULL;
	
	return_val_if_fail(text != NULL, -1);

	ftk_message_box_size(has_title, buttons_nr, text, &width, &height);
	dialog = ftk_dialog_create(0, 0, width, height);
	ftk_window_set_animation_hint(dialog, "msgbox");
	if(has_title) 
	{
		ftk_dialog_set_icon(dialog, icon);
		ftk_widget_set_text(dialog, title);
	}
	else
	{
		ftk_dialog_hide_title(dialog);
		ftk_widget_set_attr(dialog, FTK_ATTR_POPUP);
	}

	width  = ftk_widget_width(dialog);
	height = ftk_widget_height(dialog);

	/*create label.*/
	xoffset = FTK_H_MARGIN;
	yoffset = FTK_V_MARGIN;
	w = width - 2 * (FTK_DIALOG_BORDER + FTK_H_MARGIN);
	h = height - FTK_DIALOG_BORDER - 4 * FTK_V_MARGIN;
	h -= has_title ? ftk_dialog_get_title_height() : 0;
	h -= buttons_nr > 0 ? FTK_BUTTON_DEFAULT_HEIGHT : 0;

	label = ftk_label_create(dialog, xoffset, yoffset, w, h);
	ftk_widget_set_text(label, text);

	/*create buttons*/

	if(buttons_nr > 0)
	{
		xoffset = 0;
		w = FTK_BUTTON_DEFAULT_WIDTH;
		h = FTK_BUTTON_DEFAULT_HEIGHT;
		w = ((buttons_nr + 1) * w) < width ? w : (width / (buttons_nr + 1));
		yoffset = height - h - FTK_V_MARGIN - (has_title ? ftk_dialog_get_title_height() : 0);
		h_margin = (width - buttons_nr * w) / (buttons_nr + 1);

		for(i = 0; i < buttons_nr; i++)
		{
			xoffset += h_margin;
			button = ftk_button_create(dialog, xoffset, yoffset, w, h);
			ftk_widget_set_id(button, i + 1);
			ftk_widget_set_text(button, buttons[i]);
			ftk_button_set_clicked_listener(button, message_box_on_button_clicked, &id);
			xoffset += w;
		}
	}
	else
	{
		/*if no buttons, quit when timeout.*/
		ftk_dialog_quit_after(dialog, 3000);
	}

	ftk_widget_show_all(dialog, 1);
	ret = ftk_dialog_run(dialog);
	ftk_widget_unref(dialog);
	
	if(icon != NULL)
	{
		ftk_bitmap_unref(icon);
	}

	return ret;
}

int ftk_tips(const char* text)
{
	return ftk_message_box(NULL, NULL, text, NULL);
}

int ftk_warning(const char* title, const char* text, const char* buttons[FTK_MSGBOX_MAX_BUTTONS + 1])
{
	FtkBitmap* icon = ftk_theme_load_image(ftk_default_theme(), "warning"FTK_STOCK_IMG_SUFFIX); 

	return ftk_message_box(icon, title, text, buttons);
}

int ftk_question(const char* title, const char* text, const char* buttons[FTK_MSGBOX_MAX_BUTTONS + 1])
{
	FtkBitmap* icon = ftk_theme_load_image(ftk_default_theme(), "question"FTK_STOCK_IMG_SUFFIX); 

	return ftk_message_box(icon, title, text, buttons);
}

int ftk_infomation(const char* title, const char* text, const char* buttons[FTK_MSGBOX_MAX_BUTTONS + 1])
{
	FtkBitmap* icon = ftk_theme_load_image(ftk_default_theme(), "info"FTK_STOCK_IMG_SUFFIX); 

	return ftk_message_box(icon, title, text, buttons);
}


