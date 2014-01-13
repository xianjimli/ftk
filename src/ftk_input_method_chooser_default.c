/*
 * File: ftk_input_method_chooser_default.c
 * Author:  He HaiQiang <ngwsx2008@126.com>
 * Brief:   default input method chooser.
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
 * 2010-01-30 Li XianJing <xianjimli@hotmail.com> created
 * 2010-11-06 He HaiQiang <ngwsx2008@126.com> modified
 *
 */

#include "ftk_dialog.h"
#include "ftk_globals.h"
#include "ftk_popup_menu.h"
#include "ftk_input_method_manager.h"

static Ret on_im_selected(void* ctx, void* item)
{
	ftk_dialog_quit((FtkWidget*)ctx);

	return RET_QUIT;
}

int ftk_input_method_chooser(void)
{
	int i = 0;
	int h = 150;
	int nr = 0;
	FtkInputMethod* im = NULL;
	FtkWidget* im_chooser = NULL;
	FtkListItemInfo* im_infos = NULL;
	FtkInputMethodManager* im_mgr = ftk_default_input_method_manager();

	nr = (int)ftk_input_method_manager_count(im_mgr);

	im_infos = (FtkListItemInfo*)FTK_ZALLOC(sizeof(FtkListItemInfo) * (nr + 1));
	if(im_infos == NULL)
	{
		return -1;
	}

	h = ftk_popup_menu_calc_height(FTK_TRUE, nr + 1) + 5;
	im_chooser = ftk_popup_menu_create(0, 0, 0, h, NULL, "Input Method");

	for(i = 0; i < nr; i++)
	{
		ftk_input_method_manager_get(im_mgr, i, &im);
		im_infos[i].text = (char*)im->name;
		im_infos[i].type = FTK_LIST_ITEM_NORMAL;
		im_infos[i].user_data = im_chooser;
	}
	
	im_infos[i].text = (char*)"None";
	im_infos[i].type = FTK_LIST_ITEM_NORMAL;
	im_infos[i].user_data = im_chooser;
	
	for(i = 0; i < (nr + 1); i++)
	{
		ftk_popup_menu_add(im_chooser, im_infos+i);
	}

	ftk_popup_menu_set_clicked_listener(im_chooser, on_im_selected, im_chooser);	
	ftk_widget_ref(im_chooser);
	ftk_dialog_run(im_chooser);
	i = ftk_popup_menu_get_selected(im_chooser);
	ftk_widget_unref(im_chooser);

	ftk_input_method_manager_set_current(im_mgr, i);

	FTK_FREE(im_infos);

	return i;
}

