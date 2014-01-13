/*
 * File: ftk_file_system.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:  os independent file system functions.
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
 * 2010-08-14 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_pairs.h"
#include "ftk_util.h"
#include "ftk_file_system.h"

/*FIXME: add more*/
static const FtkKeyValue g_ext_type_map[] = 
{
	{".html", "text/html"},
	{".htm",  "text/html"},
	{".c",    "text/plain"},
	{".h",    "text/plain"},
	{".cpp",  "text/plain"},
	{".bmp",  "image/bmp"},
	{".png",  "image/png"},
	{".jpeg", "image/jpeg"},
	{".jpg",  "image/jpeg"},
	{".mp3",  "audio/mp3"},
	{".wav",  "audio/wav"},
	{".amr",  "audio/amr"},
	{".ogg",  "audio/ogg"},
	{"", ""}
};

const char* ftk_file_get_mime_type(const char* file_name)
{
	size_t i = 0;
	const char* ext_name = NULL;
	return_val_if_fail(file_name != NULL, FTK_MIME_UNKNOWN);

	if((ext_name = strrchr(file_name, '.')) != NULL)
	{
		for(i = 0; g_ext_type_map[i].key[0]; i++)
		{
			if(strcasecmp(g_ext_type_map[i].key, ext_name) == 0)
			{
				return g_ext_type_map[i].value;
			}
		}
	}

	return FTK_MIME_UNKNOWN;
}

int  ftk_file_exist(const char* file_name)
{
	FtkFileInfo info;

	return ftk_file_get_info(file_name, &info) == RET_OK;
}

int  ftk_file_length(const char* file_name)
{
	FtkFileInfo info;

	return ftk_file_get_info(file_name, &info) == RET_OK ? (int)info.size : -1;
}

static Ret ftk_fs_delete_children(char* name)
{
	int ret = 0;
	FtkFileInfo info = {0};
	char cwd[FTK_MAX_PATH + 1] = {0};
	return_val_if_fail(name != NULL, RET_FAIL);

	ret = ftk_file_get_info(name, &info);
	return_val_if_fail(ret == RET_OK, RET_FAIL);

	if(info.is_dir)
	{
		FtkFsHandle handle = ftk_dir_open(name);
	
		ftk_fs_get_cwd(cwd);
		ftk_fs_change_dir(name);
		if(handle != NULL)
		{
			while(ftk_dir_read(handle, &info) == RET_OK)
			{
				if(strcmp(info.name, ".") == 0 || strcmp(info.name, "..") == 0)
				{
					continue;
				}

				if(info.is_dir)
				{
					ret = ftk_fs_delete_children(info.name);	
					ret = ftk_fs_delete_dir(info.name);
				}
				else
				{
					ret = ftk_fs_delete_file(info.name);
				}
			}
			ftk_dir_close(handle);
		}
		ftk_fs_change_dir(cwd);
	}

	return ret == 0 ? RET_OK : RET_FAIL;
}

Ret ftk_fs_delete(const char* dir_or_file)
{
	Ret ret = RET_FAIL;
	FtkFileInfo info = {0};
	char name[FTK_MAX_PATH + 1] = {0};
	return_val_if_fail(dir_or_file != NULL, RET_FAIL);

	ftk_strncpy(name, dir_or_file, FTK_MAX_PATH);
	ftk_normalize_path(name);
	
	if((ret = ftk_file_get_info(name, &info)) == RET_OK)
	{
		ret = ftk_fs_delete_children(name);

		if(info.is_dir)
		{
			ret = ftk_fs_delete_dir(name);
		}
		else
		{
			ret = ftk_fs_delete_file(name);
		}
	}

	return ret;
}

Ret ftk_fs_copy(const char* dir_from, const char* dir_to)
{
	/*TODO*/
	return RET_FAIL;
}
