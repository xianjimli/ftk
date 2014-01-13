/*
 * File: ftk_gc.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:  graphic context 
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
#ifndef FTK_GC_H
#define FTK_GC_H

#include "ftk_log.h"
#include "ftk_bitmap.h"
#include "ftk_font_desc.h"

FTK_BEGIN_DECLS

enum _FtkGcMask
{
	FTK_GC_BG        = 1,
	FTK_GC_FG        = 2,
	FTK_GC_FONT      = 4,
	FTK_GC_ALPHA     = 8,
	FTK_GC_BITMAP    = 16,
	FTK_GC_LINE_MASK = 32
};

typedef struct _FtkGc
{
	int ref;
	unsigned int mask;
	FtkColor   bg;
	FtkColor   fg;
	FtkFontDesc*   font;
	FtkBitmap* bitmap;
	unsigned char alpha;
	unsigned char unused[3];
	unsigned int line_mask;
}FtkGc;

static inline Ret ftk_gc_copy(FtkGc* dst, FtkGc* src)
{
	return_val_if_fail(dst != NULL && src != NULL, RET_FAIL);

	dst->mask |= src->mask;
	if(src->mask & FTK_GC_BG)
	{
		dst->bg = src->bg;
	}

	if(src->mask & FTK_GC_FG)
	{
		dst->fg = src->fg;
	}

	if(src->mask & FTK_GC_FONT)
	{
		if(dst->font != NULL)
		{
			ftk_font_desc_unref(dst->font);
		}

		dst->font = src->font;
		if(dst->font != NULL)
		{
			ftk_font_desc_ref(dst->font);
		}
	}

	if(src->mask & FTK_GC_BITMAP)
	{
		if(dst->bitmap != NULL)
		{
			ftk_bitmap_unref(dst->bitmap);
		}
		dst->bitmap = src->bitmap;
		if(dst->bitmap != NULL)
		{
			ftk_bitmap_ref(dst->bitmap);
		}
	}

	if(src->mask & FTK_GC_LINE_MASK)
	{
		dst->line_mask = src->line_mask;
	}

	if(src->mask & FTK_GC_ALPHA)
	{
		dst->alpha = src->alpha;
	}

	return RET_OK;
}

static inline Ret ftk_gc_reset(FtkGc* gc)
{
	if(gc != NULL)
	{
		if(gc->mask & FTK_GC_BITMAP && gc->bitmap != NULL)
		{
			ftk_bitmap_unref(gc->bitmap);
		}

		if(gc->mask & FTK_GC_FONT && gc->font != NULL)
		{
			ftk_font_desc_unref(gc->font);
		}

		memset(gc, 0x00, sizeof(*gc));
	}

	return RET_OK;
}

FTK_END_DECLS

#endif/*FTK_GC_H*/

