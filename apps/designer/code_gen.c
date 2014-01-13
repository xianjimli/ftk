/*
 * File: code_gen.c 
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: xul/c code generator
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

#include "code_gen.h"
#include "widgets_info.h"
#include "ftk_file_system.h"

static Ret code_gen_xul_file(FtkWidget* widget, FtkFsHandle f)
{
	FtkWidget* iter = widget;
	char buffer[20480] = {0};
	const WidgetInfo* info = NULL;
	
	for(iter = widget; iter != NULL; iter = iter->next)
	{
		if(ftk_widget_type(iter) == FTK_WINDOW)
		{
			ftk_snprintf(buffer, sizeof(buffer), "<window value=\"%s\" visible=\"1\"", ftk_widget_get_text(iter));
			ftk_file_write(f, buffer, strlen(buffer));
			
			if(ftk_window_is_fullscreen(iter))
			{
				ftk_snprintf(buffer, sizeof(buffer), "attr=\"$FTK_ATTR_FULLSCREEN\" ");
				ftk_file_write(f, buffer, strlen(buffer));		
			}
			
			if(ftk_window_get_animation_hint(iter) != NULL)
			{
				ftk_snprintf(buffer, sizeof(buffer), " anim_hint=\"%s\" ", ftk_window_get_animation_hint(iter));
				ftk_file_write(f, buffer, strlen(buffer));				
			}
			ftk_file_write(f, ">\n", strlen(">\n"));
			
			if(iter->children != NULL)
			{
				code_gen_xul_file(iter->children, f);
			}
			
			ftk_file_write(f, "</window>\n", strlen("</window>\n"));
			
			continue;
		}
		else
		{		
			info = widgets_info_find_by_type(ftk_widget_type(iter));
			if(info == NULL) continue;
			
			ftk_snprintf(buffer, sizeof(buffer), "<%s  id=\"%d\" x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\" visible=\"1\" value=\"%s\"",
				info->name, ftk_widget_id(iter), 
				ftk_widget_left(iter), ftk_widget_top(iter),
				ftk_widget_width(iter), ftk_widget_height(iter),
				ftk_widget_get_text(iter));
				
			ftk_file_write(f, buffer, strlen(buffer));
			if(iter->children != NULL)
			{
				ftk_file_write(f, ">\n", strlen(">\n"));
				code_gen_xul_file(iter->children, f);
				ftk_snprintf(buffer, sizeof(buffer), "</%s>\n", info->name);
				ftk_file_write(f, buffer, strlen(buffer));	
			}			
			else
			{
				ftk_file_write(f, " />\n", strlen(" />\n"));	
			}
		}
	}
	
	return RET_OK;
}

Ret code_gen_xul(FtkWidget* widget, const char* filename)
{
	const char* str = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	FtkFsHandle f = ftk_file_open(filename, "wb+");
	return_val_if_fail(widget != NULL && f != NULL, RET_FAIL);
	
	ftk_file_write(f, str, strlen(str));
	code_gen_xul_file(widget, f);
	ftk_file_close(f);
	
	return RET_OK;
}

static Ret code_gen_c_file(FtkWidget* widget, FtkFsHandle f)
{
	FtkWidget* iter = widget;
	char buffer[20480] = {0};
	const WidgetInfo* info = NULL;

	for(iter = widget; iter != NULL; iter = iter->next)
	{
		if(ftk_widget_type(iter) == FTK_WINDOW)
		{
			ftk_snprintf(buffer, sizeof(buffer), "\twin = ftk_app_window_create();\n\tftk_widget_set_text(win, _(\"%s\"));\n\tftk_window_set_fullscreen(win, %d);\n\tparent = win;\n\n",
				ftk_widget_get_text(iter), 
				ftk_window_is_fullscreen(iter));
			ftk_file_write(f, buffer, strlen(buffer));

			if(iter->children != NULL)
			{
				code_gen_c_file(iter->children, f);
			}
			
			continue;
		}
		else
		{		
			info = widgets_info_find_by_type(ftk_widget_type(iter));
			if(info == NULL) continue;
		
			ftk_snprintf(buffer, sizeof(buffer), "\twidget = %s(parent, %d, %d, %d, %d);\n\tftk_widget_set_text(win, _(\"%s\"));\n\n",
				info->create_func_name,
				ftk_widget_left(iter),
				ftk_widget_top(iter),
				ftk_widget_width(iter),
				ftk_widget_height(iter),
				ftk_widget_get_text(iter));
			ftk_file_write(f, buffer, strlen(buffer));
			if(iter->children != NULL)
			{
				ftk_snprintf(buffer, sizeof(buffer), "\tparent = widget;\n");
				code_gen_c_file(iter->children, f);
			}			
		}
	}
	
	return RET_OK;
}

Ret code_gen_c(FtkWidget* widget, const char* filename, int gen_main)
{
	char buffer[20480] = {0};
	const char* str = "/*Generate by FTK UI Designer.*/\n#include \"ftk.h\"\n\n";
	FtkFsHandle f = ftk_file_open(filename, "wb+");
	return_val_if_fail(widget != NULL && f != NULL, RET_FAIL);
	
	ftk_file_write(f, str, strlen(str));
	ftk_snprintf(buffer, sizeof(buffer), "FtkWidget* create_win(void)\n{\n\tFtkWidget* win = NULL;\n\tFtkWidget* parent = NULL;\n\tFtkWidget* widget = NULL;\n\n");
	ftk_file_write(f, buffer, strlen(buffer));

	code_gen_c_file(widget, f);
	
	ftk_snprintf(buffer, sizeof(buffer), "\treturn win;\n}\n");
	ftk_file_write(f, buffer, strlen(buffer));

	ftk_file_close(f);
	
	return RET_OK;

	return RET_OK;
}
