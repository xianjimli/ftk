/*
 * File: widgets_info.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: widget infomation.
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

#include "widgets_info.h"

static const WidgetInfo s_widgets_info[] = 
{
 {FTK_BUTTON,       "button",       60, 40, 60, 40, 1, 1, 1,     ftk_button_create             ,NULL, "ftk_button_create"},
 {FTK_LABEL,        "label",        40, 22, 60, 22, 1, 1, 1,     ftk_label_create              ,NULL, "ftk_label_create"},
 {FTK_ENTRY,        "entry",        40, 24, 60, 24, 1, 1, 1,     ftk_entry_create              ,NULL, "ftk_entry_create"},
 {FTK_WAIT_BOX,     "wait_box",     32, 32, 32, 32, 1, 1, 1,     ftk_wait_box_create           ,NULL, "ftk_wait_box_create"},
 {FTK_PROGRESS_BAR, "progress_bar", 60, 22, 60, 22, 1, 1, 1,     ftk_progress_bar_create       ,NULL, "ftk_progress_bar_create"},
 {FTK_GROUP_BOX,    "group_box",    80, 80, 80, 80, 0, 1, 1,     ftk_group_box_create          ,NULL, "ftk_group_box_create"},
 {FTK_RADIO_BUTTON, "radio_button", 60, 50, 60, 50, 1, 1, 1,     ftk_check_button_create_radio ,NULL, "ftk_check_button_create_radio"},
 {FTK_CHECK_BUTTON, "check_button", 60, 50, 60, 50, 1, 1, 1,     ftk_check_button_create       ,NULL, "ftk_check_button_create"},
 {FTK_IMAGE,        "image",        32, 32, 80, 80, 1, 1, 1,     ftk_image_create              ,NULL, "ftk_image_create"},
 {FTK_SCROLL_VBAR,  "scroll_bar",    8,  8,  8, 32, 1, 1, 1,     ftk_scroll_bar_create         ,NULL, "ftk_scroll_bar_create"},
 {FTK_LIST_VIEW,    "list_view",    80, 80, 80, 80, 1, 1, 1,     ftk_list_view_default_create  ,NULL, "ftk_list_view_default_create"},
 {FTK_ICON_VIEW,    "icon_view",    80, 80, 80, 80, 1, 1, 1,     ftk_icon_view_create          ,NULL, "ftk_icon_view_create"},
 {FTK_COMBO_BOX,    "combo_box",    60, 40, 60, 40, 1, 1, 1,     ftk_combo_box_create          ,NULL, "ftk_combo_box_create"},
 {FTK_PAINTER,      "painter",      80, 80, 80, 80, 1, 1, 1,     ftk_painter_create            ,NULL, "ftk_painter_create"},
 {FTK_TAB,          "tab",          180,180,180,180,0, 1, 1,     ftk_tab_create                ,NULL, "ftk_tab_create"},
 {FTK_TAB_PAGE,     "page",         180,180,180,180,0, 1, 1,     ftk_tab_page_create           ,NULL, "ftk_tab_page_create"},
 {FTK_TEXT_VIEW,    "text_view",    80, 80, 80, 80, 1, 1, 1,     ftk_text_view_create          ,NULL, "ftk_text_view_create"}
};

#define WIDGETS_INFO_NR sizeof(s_widgets_info)/sizeof(s_widgets_info[0])

int widgets_info_get_nr(void)
{
	return WIDGETS_INFO_NR;
}

const WidgetInfo* widgets_info_get(int index)
{
	return_val_if_fail(index < WIDGETS_INFO_NR, NULL);

	return s_widgets_info+index;
}

const WidgetInfo* widgets_info_find(const char* name)
{
	size_t i = 0;

	return_val_if_fail(name != NULL, NULL);

	for(i = 0; i < WIDGETS_INFO_NR; i++)
	{
		if(strcmp(name, s_widgets_info[i].name) == 0)
		{
			return s_widgets_info+i;
		}
	}

	return NULL;
}

const WidgetInfo* widgets_info_find_by_type(int type)
{
	size_t i = 0;

	for(i = 0; i < WIDGETS_INFO_NR; i++)
	{
		if(type == s_widgets_info[i].type)
		{
			return s_widgets_info+i;
		}
	}

	return NULL;
}

