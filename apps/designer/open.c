/*
 * File: open.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: ui to open a xul file.
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

#include "open.h"
#include "ftk_xul.h"
#include "select_file.h"
#include "ftk_file_system.h"

typedef struct _Info
{
	void* on_open_ctx;
	OnOpen on_open;
}Info;

static Info s_open_info = {0};

static const char* ftk_text_no_translate(void* ctx, const char* text)
{
    return text;
}

static FtkBitmap* ftk_default_load_image(void* ctx, const char* filename)
{
    return ftk_bitmap_factory_load(ftk_default_bitmap_factory(), filename);
}

static const FtkXulCallbacks default_callbacks =
{
    NULL,
    ftk_text_no_translate,
    ftk_default_load_image
};

static Ret on_selected_open_file(void* ctx, void* obj)
{
	FtkWidget* new_win = NULL;
	Info* info = (Info*)ctx;
	const char* filename = (char*)obj;

	return_val_if_fail(filename != NULL && filename[0], RET_FAIL);

	if(strstr(filename, ".xul") != NULL)
	{
		new_win = ftk_xul_load_file(filename, &default_callbacks);
	}

	if(new_win != NULL)
	{
		info->on_open(info->on_open_ctx, new_win);
	}

	return RET_OK;
}

Ret ftk_widget_editor_open(void* ctx, OnOpen on_open)
{
	s_open_info.on_open = on_open;
	s_open_info.on_open_ctx = ctx;

	return designer_select_file(_("Open..."), "/", on_selected_open_file, &s_open_info);
}

