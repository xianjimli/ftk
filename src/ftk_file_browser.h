/*
 * File: ftk_file_browser.h
 * Author: Li XianJing <xianjimli@hotmail.com>
 * Brief:  file browser window for file manager and choose.
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
 * 2010-08-15 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_FILE_BROWSER_H
#define FTK_FILE_BROWSER_H

#include "ftk_widget.h"

FTK_BEGIN_DECLS

typedef enum _FtkFileBrowserType
{
	FTK_FILE_BROWER_APP,
	FTK_FILE_BROWER_SINGLE_CHOOSER,
	FTK_FILE_BROWER_MULTI_CHOOSER
}FtkFileBrowserType;

typedef Ret (*FtkFileBrowserOnChoosed)(void* ctx, int index, const char* path); 

FtkWidget* ftk_file_browser_create(FtkFileBrowserType type);
Ret        ftk_file_browser_set_path(FtkWidget* thiz, const char* path);
Ret        ftk_file_browser_set_filter(FtkWidget* thiz, const char* mime_type);
Ret        ftk_file_browser_set_choosed_handler(FtkWidget* thiz, FtkFileBrowserOnChoosed on_choosed, void* ctx);
Ret        ftk_file_browser_load(FtkWidget* thiz);

FTK_END_DECLS

#endif/*FTK_FILE_BROWSER_H*/

