/*
 * File: ftk_file_system.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   adapter for file system.
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

#ifndef FTK_FILESYSTEM_H
#define FTK_FILESYSTEM_H

#include "ftk_typedef.h"

FTK_BEGIN_DECLS

typedef struct _FtkFsHandle
{
    char  name[FTK_MAX_PATH+1];
    void* handle;
} *FtkFsHandle;

#define FTK_MIME_TYPE_LEN 31
#define FTK_MIME_DIR "folder/*"
#define FTK_MIME_UNKNOWN "unknown/*"

typedef struct _FtkFileInfo
{
	int uid;
	int gid;
	int mode;
	int is_dir;
	size_t size;
	time_t last_access;
	time_t last_modify;
	char name[FTK_MAX_PATH+1];
}FtkFileInfo;

Ret ftk_file_get_info(const char* file_name, FtkFileInfo* info);
const char* ftk_file_get_mime_type(const char* file_name);

FtkFsHandle ftk_file_open(const char* file_name, const char* mode);
int  ftk_file_seek(FtkFsHandle file, size_t pos);
int  ftk_file_read(FtkFsHandle file, void* buffer, size_t length);
int  ftk_file_write(FtkFsHandle file, const void* buffer, size_t length);
void ftk_file_close(FtkFsHandle file);
int  ftk_file_exist(const char* file_name);
int  ftk_file_length(const char* file_name);

FtkFsHandle ftk_dir_open(const char* dir_name);
Ret  ftk_dir_read(FtkFsHandle dir, FtkFileInfo* info);
void ftk_dir_close(FtkFsHandle dir);

Ret ftk_fs_get_cwd(char cwd[FTK_MAX_PATH+1]);
Ret ftk_fs_delete(const char* dir);
Ret ftk_fs_delete_dir(const char* dir);
Ret ftk_fs_delete_file(const char* file_name);
Ret ftk_fs_create_dir(const char* dir);
Ret ftk_fs_change_dir(const char* dir);
Ret ftk_fs_move(const char* dir_from, const char* dir_to);
Ret ftk_fs_copy(const char* dir_from, const char* dir_to);
int ftk_fs_is_root(const char* path);

FTK_END_DECLS

#endif/*FTK_FILESYSTEM_H*/

