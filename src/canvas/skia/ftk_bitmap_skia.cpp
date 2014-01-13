/*
 * File: ftk_bitmap_skia.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   skia bitmap implemntation.
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
 * 2011-10-03 Li XianJing <xianjimli@hotmail.com> created
 */

#include "ftk_bitmap.h"
#include "SkColor.h"
#include "SkCanvas.h"
#include "ftk_globals.h"
#include "SkImageEncoder.h"
#include "effects/SkPorterDuff.h"

using namespace android;

FtkBitmap* ftk_bitmap_create(int w, int h, FtkColor color)
{
	FtkBitmap* thiz = (FtkBitmap*)FTK_ALLOC(sizeof(FtkBitmap));

	if(thiz != NULL)
	{
		SkBitmap* bitmap = new SkBitmap();

		if(bitmap != NULL)
		{
			thiz->ref = 1;
			thiz->data = bitmap;
			bitmap->setConfig(SkBitmap::kARGB_8888_Config, w, h, 0);
			bitmap->allocPixels();
			bitmap->eraseARGB(color.a, color.r, color.g, color.b);
		}
		else
		{
			FTK_FREE(thiz);	
		}
	}

	return thiz;
}

FtkBitmap* ftk_bitmap_create_with_native(void* bitmap)
{
	FtkBitmap* thiz = (FtkBitmap*)FTK_ALLOC(sizeof(FtkBitmap));

	if(thiz != NULL)
	{
		thiz->ref = 1;
		thiz->data = bitmap;
	}

	return thiz;
}

void* ftk_bitmap_get_native(FtkBitmap* thiz)
{
	return_val_if_fail(thiz != NULL, NULL);

	return thiz->data;
}

int ftk_bitmap_width(FtkBitmap* thiz)
{
	SkBitmap* bitmap = NULL;
	return_val_if_fail(thiz != NULL, 0);

	bitmap = (SkBitmap*)thiz->data;

	return bitmap->width();
}

int ftk_bitmap_height(FtkBitmap* thiz)
{
	SkBitmap* bitmap = NULL;
	return_val_if_fail(thiz != NULL, 0);

	bitmap = (SkBitmap*)thiz->data;

	return bitmap->height();
}

FtkColor* ftk_bitmap_lock(FtkBitmap* thiz)
{
	SkBitmap* bitmap = NULL;
	return_val_if_fail(thiz != NULL, NULL);

	bitmap = (SkBitmap*)thiz->data;

	return (FtkColor*)bitmap->getPixels();
}

void    ftk_bitmap_unlock(FtkBitmap* thiz)
{
	return;
}

FtkColor ftk_bitmap_get_pixel(FtkBitmap* thiz, int x, int y)
{
	SkColor sc;
	FtkColor c = {0, 0, 0, 0};	
	return_val_if_fail(thiz != NULL && x < ftk_bitmap_width(thiz) && y < ftk_bitmap_height(thiz), c);
	
	SkBitmap* bitmap = (SkBitmap*)thiz->data;
#ifdef ANDROID
	sc = *bitmap->getAddr32(x, y);
	c.a = SkColorGetA(sc);
	c.r = SkColorGetR(sc);
	c.g = SkColorGetG(sc);
	c.b = SkColorGetB(sc);
#else
	sc = bitmap->getColor(x, y);
	c.a = SkColorGetA(sc);
	c.r = SkColorGetR(sc);
	c.g = SkColorGetG(sc);
	c.b = SkColorGetB(sc);

#endif

	return c;
}

void ftk_bitmap_destroy(FtkBitmap* thiz)
{
	if(thiz != NULL)
	{
		SkBitmap* bitmap = (SkBitmap*)thiz->data;

		delete bitmap;
		FTK_ZFREE(thiz, sizeof(FtkBitmap));
	}

	return;
}

