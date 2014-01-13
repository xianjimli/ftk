/*
 * File: ftk_font_freetype.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   freetype font.  
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
 * 2012-6-29  woodysu@gmail.com modified
 * 2009-10-03 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include <ft2build.h>
#include FT_GLYPH_H
#include "ftk_util.h"
#include "ftk_mmap.h"
#include "ftk_font.h"


typedef struct _FontFreetypePrivInfo
{
	int bold;
	int italic;
	size_t size;
	FT_Face     face;
	FT_Library  library;
}PrivInfo;

static int      ftk_font_freetype_height(FtkFont* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, 0);

	return priv->size;
}

static Ret ftk_font_freetype_lookup (FtkFont* thiz, unsigned short code, FtkGlyph* glyph)
{
	int index = 0;
	FT_Error err = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && glyph != NULL, RET_FAIL);

	index = FT_Get_Char_Index(priv->face, code);
	err = FT_Load_Glyph(priv->face, index, FT_LOAD_DEFAULT|FT_LOAD_RENDER);
	return_val_if_fail(err == 0, RET_FAIL);

	glyph->code = code;
	glyph->x    = priv->face->glyph->bitmap_left;
	glyph->y    = priv->face->glyph->bitmap_top;
	glyph->w    = priv->face->glyph->bitmap.width;
	glyph->h    = priv->face->glyph->bitmap.rows;
	glyph->data = priv->face->glyph->bitmap.buffer;
    // For fix a bug-- the actual bitmap of some fonts is larger than it's font size
	if (glyph->h > thiz->height(thiz)) {
		ftk_logw("Bug font code: %04x\n", code);
		glyph->h = thiz->height(thiz);
	}



	return RET_OK;
}

void      ftk_font_freetype_destroy(FtkFont* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		FT_Done_Face(priv->face);
		FT_Done_FreeType(priv->library);
		FTK_ZFREE(thiz, sizeof(*thiz) + sizeof(PrivInfo));
	}

	return;
}

FtkFont* ftk_font_create (const char* filename, FtkFontDesc* font_desc)
{
	int size = 0;
	FtkFont* thiz = NULL;
	return_val_if_fail(filename != NULL && font_desc != NULL, NULL);

	size = ftk_font_desc_get_size(font_desc);
	thiz = FTK_NEW_PRIV(FtkFont); 
	if(thiz != NULL)
	{
		FT_Error err = 0;
		DECL_PRIV(thiz, priv);

		thiz->ref = 1;
		thiz->height = ftk_font_freetype_height;
		thiz->lookup = ftk_font_freetype_lookup;
		thiz->destroy= ftk_font_freetype_destroy;

		priv->size = size;
		err = FT_Init_FreeType(&priv->library );
		if((err = FT_New_Face(priv->library, filename, 0, &priv->face)))
		{
			ftk_loge("load %s failed.\n", filename);
			FT_Done_FreeType(priv->library);
			FTK_ZFREE(thiz, sizeof(FtkFont));
		}
		else
		{
			err = FT_Select_Charmap(priv->face, ft_encoding_unicode);
			if(err)
			{
				err = FT_Select_Charmap(priv->face, ft_encoding_latin_1 );
			}
			assert(err == 0);
			err = FT_Set_Pixel_Sizes(priv->face, 0, size);
			assert(err == 0);
			ftk_logd("fonfile:%s\n", filename);
			ftk_logd("font family_name:%s\n", priv->face->family_name);
			ftk_logd("font style_name:%s\n", priv->face->style_name);
		}
	}

	return thiz;
}

