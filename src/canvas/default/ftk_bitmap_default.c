/*
 * File: ftk_bitmap.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   Ftk bitmap
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
#include "ftk_bitmap.h"
#include "ftk_allocator.h"

typedef struct _PrivInfo
{
	int w;
	int h;
	int ref;
	int length;
	FtkColor* bits;
}PrivInfo;

FtkBitmap* ftk_bitmap_create(int w, int h, FtkColor color)
{
	int i = 0;
	int length = w * h;
	FtkBitmap* thiz = (FtkBitmap*)FTK_ZALLOC(sizeof(FtkBitmap) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		thiz->ref = 1;
		priv->w = w;
		priv->h = h;
		priv->length = length;
		
		priv->bits = FTK_ALLOC(length * sizeof(FtkColor));
		for(i = 0; i < length; i++) 
		{
			priv->bits[i] = color;
		}
	}

	return thiz;
}

void* ftk_bitmap_get_native(FtkBitmap* thiz)
{
	return_val_if_fail(thiz != NULL, NULL);

	return thiz;
}

int ftk_bitmap_width(FtkBitmap* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);

	return priv->w;
}

int ftk_bitmap_height(FtkBitmap* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);

	return priv->h;
}

FtkColor* ftk_bitmap_lock(FtkBitmap* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, NULL);

	return priv->bits;
}

void    ftk_bitmap_unlock(FtkBitmap* thiz)
{
	return;
}

FtkColor ftk_bitmap_get_pixel(FtkBitmap* thiz, int x, int y)
{
	DECL_PRIV(thiz, priv);
	FtkColor c = {0, 0, 0, 0};
	return_val_if_fail(thiz != NULL && x < priv->w && y < priv->h, c);

	c = *(priv->bits + y * priv->w + x);

	return c;
}

void ftk_bitmap_destroy(FtkBitmap* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		FTK_FREE(priv->bits);
		FTK_FREE(thiz);
	}

	return;
}

