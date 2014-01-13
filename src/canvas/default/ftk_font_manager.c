/*
 * File: ftk_font_manager.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   font manager.
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
 * 2011-03-20 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_util.h"
#include "ftk_font.h"
#include "ftk_globals.h"
#include "ftk_font_manager.h"

typedef struct _FtkFontEntry
{
	FtkFont* font;
	FtkFontDesc* font_desc;	
}FtkFontEntry;

struct _FtkFontManager
{
	int nr;
	int used_nr;
	FtkFontDesc* default_font_desc;
	FtkFontEntry fonts[ZERO_LEN_ARRAY];
};

FtkFontManager* ftk_font_manager_create(int max_font_nr)
{
	FtkFontManager* thiz = (FtkFontManager*)FTK_ZALLOC(sizeof(FtkFontManager) 
		+ sizeof(FtkFontEntry) * max_font_nr);

	if(thiz != NULL)
	{
		thiz->nr = max_font_nr;
		thiz->default_font_desc = ftk_font_desc_create(NULL);
	}

	return thiz;
}

FtkFont* ftk_font_manager_get_default_font(FtkFontManager* thiz)
{
	FtkFont* font = NULL;
	return_val_if_fail(thiz != NULL, NULL);

	font = ftk_font_manager_load(thiz, thiz->default_font_desc);
	ftk_font_ref(font);

	return font;
}

FtkFont* ftk_font_manager_load(FtkFontManager* thiz, FtkFontDesc* font_desc)
{
	int i = 0;
	FtkFont* font = NULL;
	char filename[FTK_MAX_PATH + 1] = {0};

	return_val_if_fail(thiz != NULL && font_desc != NULL, NULL);

#ifndef USE_FREETYPE
	font_desc = thiz->default_font_desc;
#endif

	for(i = 0; i < thiz->used_nr; i++)
	{
		if(ftk_font_desc_is_equal(thiz->fonts[i].font_desc, font_desc))
		{
			ftk_font_ref(thiz->fonts[i].font);
			
			return thiz->fonts[i].font;
		}
	}

	return_val_if_fail((thiz->used_nr+1) < thiz->nr, NULL);

#if defined(USE_FREETYPE) && defined(ANDROID) && defined(ANDROID_NDK)
	ftk_strcpy(filename, ftk_font_desc_get_fontname(font_desc));
#else
	ftk_strs_cat(filename, FTK_MAX_PATH, 
		ftk_config_get_data_dir(ftk_default_config()), "/data/", ftk_font_desc_get_fontname(font_desc), NULL);
	ftk_normalize_path(filename);
#endif
	font = ftk_font_create(filename, font_desc);
	
	if(font != NULL)
	{
#if defined(USE_FREETYPE) || defined(USE_DEFAULT_FONT) 	
		FtkFont* cached_font = ftk_font_cache_create(font, 512);
		ftk_font_unref(font);
		font = cached_font;
#endif		
		ftk_font_ref(font);
		ftk_font_desc_ref(font_desc);
		thiz->fonts[thiz->used_nr].font = font;
		thiz->fonts[thiz->used_nr].font_desc = font_desc;

		thiz->used_nr++;
	}

	return font;
}

void ftk_font_manager_destroy(FtkFontManager* thiz)
{
	int i = 0;

	if(thiz != NULL)
	{
		for(i = 0; i < thiz->used_nr; i++)
		{
			ftk_font_destroy(thiz->fonts[i].font);
			ftk_font_desc_unref(thiz->fonts[i].font_desc);
		}

		ftk_font_desc_unref(thiz->default_font_desc);
		FTK_FREE(thiz);
	}

	return;
}

