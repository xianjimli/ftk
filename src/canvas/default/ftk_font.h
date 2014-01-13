/*
 * File: ftk_font.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   font interface.
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
 * 2009-10-03 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_FONT_H
#define FTK_FONT_H

#include "ftk_font_desc.h"
#include "ftk_allocator.h"
#include "ftk_log.h"

FTK_BEGIN_DECLS

typedef struct _FtkGlyph
{
	signed char x;
	signed char y;
	unsigned char w;
	unsigned char h;
	unsigned short code;
	unsigned short unused;
	unsigned char* data;
}FtkGlyph;

struct _FtkFont;
typedef struct _FtkFont FtkFont;

typedef int  (*FtkFontHeight)(FtkFont* thiz);
typedef Ret  (*FtkFontLookup)(FtkFont* thiz, unsigned short code, FtkGlyph* glyph);
typedef int  (*FtkFontGetCharExtent)(FtkFont* thiz, unsigned short code);
typedef int  (*FtkFontGetExtent)(FtkFont* thiz, const char* str, int len);
typedef void (*FtkFontDestroy)(FtkFont* thiz);

struct _FtkFont
{
	FtkFontHeight  height;
	FtkFontLookup  lookup;
	FtkFontGetExtent get_extent;
	FtkFontGetCharExtent get_char_extent;
	FtkFontDestroy destroy;

	int ref;
	FtkFontDesc* font_desc;
	char priv[ZERO_LEN_ARRAY];
};

static inline int      ftk_font_height(FtkFont* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->height != NULL, 16);

	return thiz->height(thiz);
}

static inline Ret ftk_font_lookup (FtkFont* thiz, unsigned short code, FtkGlyph* glyph)
{
	if(thiz == NULL || thiz->lookup == NULL)
	{
		return RET_FAIL;
	}
	return_val_if_fail(thiz != NULL && thiz->lookup != NULL, RET_FAIL);

	return thiz->lookup(thiz, code, glyph);
}

static inline int     ftk_font_ref(FtkFont* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	thiz->ref++;

	return thiz->ref;
}

static inline void     ftk_font_destroy(FtkFont* thiz)
{
	return_if_fail(thiz != NULL && thiz->destroy != NULL);

	thiz->destroy(thiz);

	return;
}

static inline int     ftk_font_unref(FtkFont* thiz)
{
	int ret = 0;
	return_val_if_fail(thiz != NULL, 0);

	ret = --thiz->ref;

	if(ret == 0)
	{
		ftk_font_destroy(thiz);
	}

	return ret;
}

FtkFont* ftk_font_cache_create (FtkFont* font, int max_glyph_nr);

FtkFontDesc* ftk_font_get_desc(FtkFont* thiz);
int ftk_font_get_extent(FtkFont* thiz, const char* str, int len);
int ftk_font_get_char_extent(FtkFont* thiz, unsigned short unicode);
const char* ftk_font_calc_str_visible_range(FtkFont* thiz, const char* start, int vstart, int vend, int width, int* extent);

FtkFont* ftk_font_create (const char* filename, FtkFontDesc* font_desc);

FTK_END_DECLS

#endif/*FTK_FONT_H*/

