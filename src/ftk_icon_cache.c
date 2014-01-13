/*
 * File: ftk_icon_cache.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   cache to load icons.
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
 * 2009-11-04 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#include "ftk_util.h"
#include "ftk_globals.h"
#include "ftk_icon_cache.h"
#include "ftk_bitmap_factory.h"

typedef struct _FtkBitmapNamePair
{
	char* filename;
	FtkBitmap* bitmap;
}FtkBitmapNamePair;

struct _FtkIconCache
{
	int nr;
	char rel_path[FTK_MAX_PATH];
	char path[FTK_ICON_PATH_NR][FTK_MAX_PATH];
	FtkBitmapNamePair pairs[FTK_ICON_CACHE_MAX];
};

static FtkBitmap* ftk_icon_cache_find(FtkIconCache* thiz, const char* filename)
{
	int i = 0;
	return_val_if_fail(thiz != NULL && filename != NULL, NULL);

	for(i = 0; i < thiz->nr; i++)
	{
		if(strcmp(filename, thiz->pairs[i].filename) == 0)
		{
			ftk_bitmap_ref(thiz->pairs[i].bitmap);
			return thiz->pairs[i].bitmap;
		}
	}

	return NULL;
}

static FtkBitmap* ftk_icon_cache_real_load(FtkIconCache* thiz, const char* filename)
{
	size_t i = 0;
	FtkBitmap* bitmap = NULL;
	char path[FTK_MAX_PATH+1] = {0};
	return_val_if_fail(thiz != NULL && filename != NULL && strlen(filename) > 0, NULL);

	for(i = 0; i < FTK_ICON_PATH_NR; i++)
	{

		ftk_strs_cat(path, FTK_MAX_PATH, thiz->path[i], "/", thiz->rel_path, "/", filename, NULL);
		ftk_normalize_path(path);
		if((bitmap = ftk_bitmap_factory_load(ftk_default_bitmap_factory(), path)) != NULL)
		{
			return bitmap;
		}
	}

	return bitmap;
}

static const char* s_default_path[FTK_ICON_PATH_NR];
static void ftk_init_default_path()
{
	s_default_path[0] = ftk_config_get_data_root_dir(ftk_default_config());;
	s_default_path[1] = ftk_config_get_data_dir(ftk_default_config());
	s_default_path[2] = ftk_config_get_test_data_dir(ftk_default_config());

	return;
}

FtkIconCache* ftk_icon_cache_create(const char* root_path[FTK_ICON_PATH_NR], const char* rel_path)
{
	FtkIconCache* thiz = FTK_NEW(FtkIconCache);

	ftk_init_default_path();
	if(thiz != NULL)
	{
		size_t i = 0;
		if(root_path == NULL)
		{
			root_path = s_default_path;
		}

		if(rel_path != NULL)
		{
			ftk_strncpy(thiz->rel_path, rel_path, FTK_MAX_PATH);
		}
		else
		{
			thiz->rel_path[0] = '\0';
		}

		for(i = 0; i < FTK_ICON_PATH_NR; i++)
		{
			if(root_path[i] != NULL)
			{
				ftk_strncpy(thiz->path[i], root_path[i], FTK_MAX_PATH);
			}
			else
			{
				thiz->path[i][0] = '\0';
			}
		}
	}

	return thiz;
}

static Ret ftk_icon_cache_add(FtkIconCache* thiz, const char* filename, FtkBitmap* bitmap)
{
	if(thiz->nr < FTK_ICON_CACHE_MAX)
	{
		ftk_bitmap_ref(bitmap);
		thiz->pairs[thiz->nr].bitmap = bitmap;
		thiz->pairs[thiz->nr].filename = FTK_STRDUP(filename);
		thiz->nr++;
	}

	return RET_OK;
}

FtkBitmap* ftk_icon_cache_load(FtkIconCache* thiz, const char* filename)
{
	FtkBitmap* bitmap = NULL;
	return_val_if_fail(thiz != NULL && filename != NULL, NULL);

	if((bitmap = ftk_icon_cache_find(thiz, filename)) == NULL)
	{
		if((bitmap = ftk_icon_cache_real_load(thiz, filename)) != NULL)
		{
			ftk_icon_cache_add(thiz, filename, bitmap);
		}
	}

	return bitmap;
}

void ftk_icon_cache_destroy(FtkIconCache* thiz)
{
	int i = 0;
	
	if(thiz != NULL)
	{
		for(i = 0; i < thiz->nr; i++)
		{
			ftk_bitmap_unref(thiz->pairs[i].bitmap);
			FTK_FREE(thiz->pairs[i].filename);
		}

		FTK_ZFREE(thiz, sizeof(FtkIconCache));
	}

	return;
}


