/*
 * File: ftk_font_default.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   default font.  
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

#include "ftk_log.h"
#include "fontdata.h"
#include "ftk_util.h"
#include "ftk_font.h"

typedef struct _FontDefaultPrivInfo
{
	size_t size;
	char* filename;
	FontData* fontdata;
}PrivInfo;

static Ret ftk_font_default_load(FtkFont* thiz, const char* filename)
{
	DECL_PRIV(thiz, priv);

	priv->fontdata = font_data_load_file(filename);
	ftk_logd("%s: filename=%s\n", __func__, filename);
	ftk_logd("%s: author=%s\n", __func__, font_data_get_author(priv->fontdata));
	ftk_logd("%s: family=%s\n", __func__, font_data_get_family(priv->fontdata));
	ftk_logd("%s: style =%s\n", __func__, font_data_get_style(priv->fontdata));
	ftk_logd("%s: w=%d h=%d\n", __func__, font_data_get_width(priv->fontdata), font_data_get_height(priv->fontdata));
	
	return priv->fontdata != NULL ? RET_OK : RET_FAIL;
}

static int      ftk_font_default_height(FtkFont* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, 0);

	return font_data_get_height(priv->fontdata);
}

static Ret ftk_font_default_lookup (FtkFont* thiz, unsigned short code, FtkGlyph* glyph)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	return font_data_get_glyph(priv->fontdata, code, (Glyph*)glyph);
}

void      ftk_font_default_destroy(FtkFont* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		FTK_FREE(priv->filename);
		font_data_destroy(priv->fontdata);
		FTK_ZFREE(thiz, sizeof(*thiz) + sizeof(PrivInfo));
	}

	return;
}

FtkFont* ftk_font_create (const char* filename, FtkFontDesc* font_desc)
{
	FtkFont* thiz = NULL;
	return_val_if_fail(filename != NULL, NULL);

	thiz = FTK_NEW_PRIV(FtkFont); 
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		
		thiz->ref = 1;
		thiz->height = ftk_font_default_height;
		thiz->lookup = ftk_font_default_lookup;
		thiz->destroy= ftk_font_default_destroy;

		priv->size = ftk_font_desc_get_size(font_desc);
		if(ftk_font_default_load(thiz, filename) != RET_OK)
		{
			FTK_ZFREE(thiz, sizeof(FtkFont) + sizeof(PrivInfo));
		}
		else
		{
			priv->filename = FTK_STRDUP(filename);
		}
	}

	return thiz;
}

