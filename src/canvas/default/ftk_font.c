/*
 * File: ftk_font.c
 *
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: common used font functions.
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
 * 2010-07-18 Li XianJing <xianjimli@hotmail.com> created
 * 2011-04-08 Li XianJing <xianjimli@hotmail.com> add ftk_font_cache.
 * 2011-04-12 Li XianJing <xianjimli@hotmail.com> add lru replace algo.
 *
 */

#include "ftk_log.h"
#include "ftk_font.h"
#include "ftk_util.h"

int ftk_font_get_char_extent(FtkFont* thiz, unsigned short unicode)
{
	int extent = 0;
	FtkGlyph glyph = {0};
	return_val_if_fail(thiz != NULL, 0);

	if(thiz->get_char_extent != NULL)
	{
		return thiz->get_char_extent(thiz, unicode);
	}

	if(unicode == ' ')
	{
		return FTK_SPACE_WIDTH;
	}
	else if(unicode == '\t')
	{
		return FTK_SPACE_WIDTH * FTK_TAB_WIDTH;
	}
	else if(unicode == '\r' || unicode == '\n')
	{
		return 0;
	}

	if(ftk_font_lookup(thiz, unicode, &glyph) == RET_OK)
	{
		extent = glyph.x + glyph.w + 1;
	}

	return extent;
}

FtkFontDesc* ftk_font_get_desc(FtkFont* thiz)
{
	return thiz != NULL ? thiz->font_desc : NULL;
}

int ftk_font_get_extent(FtkFont* thiz, const char* str, int len)
{
	int extent = 0;
	unsigned short code = 0;
	const char* iter = str;
	len = (len < 0 && str != NULL) ? strlen(str) : len;
	return_val_if_fail(thiz != NULL && str != NULL, 0);
	
	if(thiz->get_extent != NULL)
	{
		return thiz->get_extent(thiz, str, len);
	}

	len = len >= 0 ? len : (int)strlen(str);
	while(*iter && (iter - str) < len)
	{
		code = utf8_get_char(iter, &iter);
		if(code == 0 || code == 0xffff)
		{
			break;
		}
		extent += ftk_font_get_char_extent(thiz, code);
	}

	return extent;
}

//////////////////////////////////////////////////////
typedef struct _FtkGlyphCache
{
	FtkGlyph glyph;
	unsigned short access_nr;
	unsigned short load_time; /*unit: about one minute*/
}FtkGlyphCache;

typedef struct _FontPrivInfo
{
	FtkFont* font;
	FtkGlyph* glyphs;
	FtkGlyph* free_glyphs;
	FtkGlyph** glyphs_ptr;
	int glyph_nr;
	int font_height;
	int max_glyph_nr;
	int one_glyph_size;
	int last_replaced;
	FtkGlyph* lrc_glyph;
}PrivInfo;

#define SHRINK_TIME(t) ((t) >> 16)

static FtkGlyph* ftk_font_cache_alloc(FtkFont* thiz)
{
	char* end = NULL;
	FtkGlyph* p = NULL;
	DECL_PRIV(thiz, priv);

	if(priv->lrc_glyph)
	{
		p = priv->lrc_glyph;
		priv->lrc_glyph = NULL;

		return p;
	}

	end = (char*)priv->glyphs + priv->max_glyph_nr * priv->one_glyph_size;

	if((char*)priv->free_glyphs < end)
	{
		p = priv->free_glyphs;
		priv->free_glyphs = (FtkGlyph*) ((char*) priv->free_glyphs + priv->one_glyph_size);
	}

	return p;
}

static Ret ftk_font_cache_remove_lru(FtkFont* thiz, unsigned short older_than)
{
	int i = 0;
	int lru = 0;
	FtkGlyph* p = NULL;
	FtkGlyphCache* c = NULL;
	DECL_PRIV(thiz, priv);
	unsigned short lru_access_nr = 0xffff;
	unsigned short now = SHRINK_TIME(ftk_get_relative_time());
	return_val_if_fail(thiz != NULL, RET_FAIL);

	lru = priv->glyph_nr;
	i = (priv->last_replaced + 1) % priv->glyph_nr;
	for(; i < priv->glyph_nr; i++)
	{
		p = priv->glyphs_ptr[i];
		c = (FtkGlyphCache*)p;
		if((now - c->load_time) < older_than)
		{
			continue;
		}

		if(c->access_nr < lru_access_nr)
		{
			lru = i;
			lru_access_nr = c->access_nr;
		}
	}

	if(lru == priv->glyph_nr)
	{
		return ftk_font_cache_remove_lru(thiz, older_than >> 1);
	}

	i = lru;
	priv->last_replaced = lru;
	priv->lrc_glyph = priv->glyphs_ptr[i];
	for(; (i + 1) < priv->glyph_nr; i++)
	{
		priv->glyphs_ptr[i] = priv->glyphs_ptr[i+1];
	}
	priv->glyph_nr--;
	c = (FtkGlyphCache*)priv->lrc_glyph;
	
	ftk_logd("%s: remove %d access_nr=%d load_time =%d\n", 
		__func__, lru, c->access_nr, c->load_time);

	return RET_OK;
}

static Ret ftk_font_cache_add(FtkFont* thiz, FtkGlyph* glyph)
{
	int i = 0;
	int pos = 0;
	FtkGlyph* p = NULL;
	FtkGlyphCache* c = NULL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(glyph->w > priv->font_height || glyph->h > priv->font_height)
	{
		ftk_logd("%s: 0x%04x is too large to cache\n", __func__, glyph->code);
		return RET_FAIL;
	}

	for(pos = 0; pos < priv->glyph_nr; pos++)
	{
		if(priv->glyphs_ptr[pos]->code >= glyph->code)
		{
			break;
		}
	}

	if(priv->glyph_nr < priv->max_glyph_nr)
	{
		i = priv->glyph_nr;
		for(; i > pos; i--)
		{
			priv->glyphs_ptr[i] = priv->glyphs_ptr[i-1];
		}
		
		priv->glyph_nr++;
		priv->glyphs_ptr[pos] = ftk_font_cache_alloc(thiz);
	}
	else
	{
		return RET_FAIL;
	}

	p = priv->glyphs_ptr[pos];
	
	*p = *glyph;
	p->data = (unsigned char*)p + sizeof(FtkGlyphCache);
	memcpy(p->data, glyph->data, glyph->w * glyph->h);

	c = (FtkGlyphCache*)p;
	c->access_nr++;
	c->load_time = SHRINK_TIME(ftk_get_relative_time());

	ftk_logd("%s add 0x%04x at %d\n", __func__, p->code, pos);

	return RET_OK;
}

static Ret ftk_font_cache_lookup (FtkFont* thiz, unsigned short code, FtkGlyph* glyph)
{
	int low    = 0;
	int mid    = 0;
	int high   = 0;
	int result = 0;
	FtkGlyph* p = NULL;
	FtkGlyphCache* c = NULL;
	Ret ret =  RET_FAIL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	high = priv->glyph_nr-1;
	while(low <= high)
	{
		mid  = low + ((high - low) >> 1);
		p = priv->glyphs_ptr[mid];
		result = p->code - code;
		if(result == 0)
		{
			*glyph = *p;
			c = (FtkGlyphCache*)p;
			c->access_nr++;
			//ftk_logd("%s find %p at %d accces_nr=%d\n", __func__, p->code, mid, c->access_nr);
			return RET_OK;
		}
		else if(result < 0)
		{
			low = mid + 1;
		}
		else
		{
			high = mid - 1;
		}
	}

	if((ret = ftk_font_lookup(priv->font, code, glyph)) == RET_OK)
	{
		if(glyph->w > priv->font_height || glyph->h > priv->font_height)
		{
			ftk_logd("%s: 0x%04x is too large to cache%dx%d %dx%d\n",
				__func__, glyph->code, glyph->w, glyph->h, priv->font_height, priv->font_height);
			return RET_FAIL;
		}

		if(ftk_font_cache_add(thiz, glyph) != RET_OK)
		{
			if(ftk_font_cache_remove_lru(thiz, 64) == RET_OK)
			{
				ret = ftk_font_cache_add(thiz, glyph);
				//assert(ret == RET_OK);
			}
		}
//for test
#if 0
		FtkGlyph g = {0};
		assert(ftk_font_cache_lookup(thiz, code, &g) == RET_OK);
		assert(g.code == code);
		assert(memcmp(&g, glyph, sizeof(g) - 4) == 0);
		assert(memcmp(g.data, glyph->data, g.w * g.h) == 0);
#endif			
	}
	else
	{
		//assert(!"not found");
	}

	return RET_OK;
}

static int      ftk_font_cache_height(FtkFont* thiz)
{
	DECL_PRIV(thiz, priv);

	return ftk_font_height(priv->font);
}

static void		ftk_font_cache_destroy(FtkFont* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		FTK_FREE(priv->glyphs);
		FTK_FREE(priv->glyphs_ptr);
		ftk_font_unref(priv->font);
		FTK_FREE(thiz);
	}

	return;
}

FtkFont* ftk_font_cache_create (FtkFont* font, int max_glyph_nr)
{
	FtkFont* thiz = NULL;
	int font_height = ftk_font_height(font);
	return_val_if_fail(font != NULL && font_height > 0, NULL);

	if((thiz = FTK_NEW_PRIV(FtkFont)) != NULL)
	{
		DECL_PRIV(thiz, priv);

		thiz->ref = 1;
		thiz->height = ftk_font_cache_height;
		thiz->lookup = ftk_font_cache_lookup;
		thiz->destroy = ftk_font_cache_destroy;

		priv->font = font;
		ftk_font_ref(font);

		priv->glyph_nr = 0;
		thiz->font_desc = font->font_desc;
		priv->font_height = font_height;
		priv->max_glyph_nr = max_glyph_nr;
		priv->one_glyph_size = sizeof(FtkGlyphCache) + font_height * font_height ;

		priv->glyphs = (FtkGlyph*)FTK_ZALLOC(max_glyph_nr * priv->one_glyph_size);
		priv->glyphs_ptr = (FtkGlyph**)FTK_ZALLOC(max_glyph_nr * sizeof(FtkGlyphCache*));
		priv->free_glyphs = priv->glyphs;
		
		ftk_logd("%s: max_glyph_nr=%d memsize=%d\n", __func__, 
			max_glyph_nr, max_glyph_nr * priv->one_glyph_size);
	}

	return thiz;
}

