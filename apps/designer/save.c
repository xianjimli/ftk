/*
 * File: save.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: ui to save xul file.
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

#include "save.h"
#include "code_gen.h"
#include "select_file.h"
#include "ftk_file_system.h"

static Ret on_selected_save_file(void* ctx, void* obj)
{
	char* ext = NULL;
	FtkWidget* widget =(FtkWidget*)ctx;
	char filename[FTK_MAX_PATH+1] = {0};
	strcpy(filename, (char*)obj);

	ext = strrchr(filename, '.');
	if(ext != NULL)
	{
		*ext = '\0';
	}
	else 
	{
		ext = filename + strlen(filename);
	}

	strcpy(ext, ".xul");
	code_gen_xul(widget, filename);

	strcpy(ext, ".c");
	code_gen_c(widget, filename, 0);

	return RET_OK;
}


Ret ftk_widget_editor_save(FtkWidget* widget)
{
	return designer_select_file(_("Save..."), "/", on_selected_save_file, widget);
}

