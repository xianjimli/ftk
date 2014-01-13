/*
 * File: app_info.h 
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   application infomation.
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
 * 2009-11-29 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#ifndef APP_INFO_H
#define APP_INFO_H

#include "ftk.h"
#include "ftk_app.h"
#include "ftk_xml_parser.h"

typedef Ret (*FtkMain)(int argc, char* argv[]);

typedef struct _AppInfo
{
	char name[32];
	char exec[260];
	char init[64];
	void* handle;
	FtkApp* app;
}AppInfo;

struct _AppInfoManager;
typedef struct _AppInfoManager AppInfoManager;

AppInfoManager* app_info_manager_create(void);
Ret  app_info_manager_load_file(AppInfoManager* thiz, const char* filename);
Ret  app_info_manager_load_dir(AppInfoManager* thiz, const char* path);
int  app_info_manager_get_count(AppInfoManager* thiz);
Ret  app_info_manager_init_app(AppInfoManager* thiz, AppInfo* info);
Ret  app_info_manager_get(AppInfoManager* thiz, size_t index, AppInfo** info);
Ret  app_info_manager_add(AppInfoManager* thiz, AppInfo* info);
Ret  app_info_manager_remove(AppInfoManager* thiz, size_t index);
void app_info_manager_destroy(AppInfoManager* thiz);

#endif/*APP_INFO_H*/

