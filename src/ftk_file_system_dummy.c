/*
 * File: ftk_file_system_posix.c
 * Author: Li XianJing <xianjimli@hotmail.com>
 * Brief:  posix implemented file system adaptor.
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
 * 2011-04-19 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_util.h"
#include "ftk_file_system.h"

Ret ftk_file_get_info(const char* file_name, FtkFileInfo* info)
{
	int ret = 0;
	struct stat st;
	return_val_if_fail(file_name != NULL && info != NULL, RET_FAIL);

	if((ret = stat(file_name, &st)) == 0)
	{
		memset(info, 0x00, sizeof(FtkFileInfo));

		info->uid  = st.st_uid;
		info->gid  = st.st_gid;
		info->mode = st.st_mode;
		info->size = st.st_size; 
		info->is_dir = 0;
		info->last_access = st.st_atime;
		info->last_modify = st.st_mtime;
		ftk_strncpy(info->name, file_name, FTK_MAX_PATH);

		return RET_OK;
	}

	return RET_FAIL;
}

FtkFsHandle ftk_file_open(const char* file_name, const char* mode)
{
	return_val_if_fail(file_name != NULL && mode != NULL, NULL);

	return fopen(file_name, mode);
}

int  ftk_file_seek(FtkFsHandle file, size_t pos)
{
	return fseek(file, pos, SEEK_SET);
}

int  ftk_file_read(FtkFsHandle file, void* buffer, size_t length)
{
	return fread(buffer, 1, length, file);
}

int  ftk_file_write(FtkFsHandle file, const void* buffer, size_t length)
{
	return fwrite(buffer, 1, length, file);
}

void ftk_file_close(FtkFsHandle file)
{
	return_if_fail(file != NULL);

	fclose(file);
}


FtkFsHandle ftk_dir_open(const char* dir_name)
{
	return NULL;
}

Ret  ftk_dir_read(FtkFsHandle dir, FtkFileInfo* info)
{
	return RET_FAIL;
}

void ftk_dir_close(FtkFsHandle dir)
{
	return;
}

Ret ftk_fs_get_cwd(char cwd[FTK_MAX_PATH+1])
{
	return RET_FAIL;
}

Ret ftk_fs_delete_dir(const char* dir)
{
	return RET_FAIL;
}

Ret ftk_fs_delete_file(const char* file_name)
{
	return RET_FAIL;
}

Ret ftk_fs_create_dir(const char* dir)
{
	return RET_FAIL;
}

Ret ftk_fs_change_dir(const char* dir)
{
	return RET_FAIL;
}

Ret ftk_fs_move(const char* dir_from, const char* dir_to)
{
	return RET_FAIL;
}

int ftk_fs_is_root(const char* path)
{
	return 0;
}
