/*
 * File: ftk_dlfcn.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:  wrap dlfcn
 *
 * Copyright (c) 2011  Li XianJing <xianjimli@hotmail.com>
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
 * 2011-05-01 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_dlfcn.h"

#if defined(FTK_HAS_DLOPEN)
#include <dlfcn.h>
void* ftk_dlopen(const char* filename)
{
	void* handle = NULL;
	return_val_if_fail(filename != NULL, NULL);

	if((handle = dlopen(filename, RTLD_NOW)) == NULL)
	{
		ftk_logd("dlopen %s failed(%s)\n", filename, dlerror());
	}

	return handle;
}

void* ftk_dlsym(void* handle, const char* func)
{
	void* fhandle = NULL;
	return_val_if_fail(handle != NULL && func != NULL, NULL);

	if((fhandle = dlsym(handle, func)) == NULL)
	{
		ftk_logd("dlsym %s failed(%s)\n", func, dlerror());
	}

	return fhandle;
}

void  ftk_dlclose(void* handle)
{
	if(handle != NULL)
	{
		dlclose(handle);
	}

	return;
}

char* ftk_dl_file_name(const char* name, char filename[FTK_MAX_PATH+1])
{
	ftk_snprintf(filename, FTK_MAX_PATH, "lib%s.so", name);

	return filename;
}

#elif defined(FTK_HAS_LOAD_LIBRARY)
void* ftk_dlopen(const char* filename)
{
	void* handle = NULL;
	return_val_if_fail(filename != NULL, NULL);

	handle = LoadLibrary(filename);

	return handle;
}

void* ftk_dlsym(void* handle, const char* func)
{
	void* fhandle = NULL;
	return_val_if_fail(handle != NULL && func != NULL, NULL);
	
	fhandle = GetProcAddress((HMODULE)handle, func);

	return fhandle;
}

void  ftk_dlclose(void* handle)
{
	if(handle != NULL)
	{
		FreeLibrary((HMODULE)handle);
	}

	return;
}

char* ftk_dl_file_name(const char* name, char filename[FTK_MAX_PATH+1])
{
	ftk_snprintf(filename, FTK_MAX_PATH, "%s.dll", name);

	return filename;
}

#else
void* ftk_dlopen(const char* filename)
{
	return NULL;
}

void* ftk_dlsym(void* handle, const char* func)
{
	return NULL;
}

void  ftk_dlclose(void* handle)
{
}

char* ftk_dl_file_name(const char* name, char filename[FTK_MAX_PATH+1])
{
	return NULL;
}

#endif
