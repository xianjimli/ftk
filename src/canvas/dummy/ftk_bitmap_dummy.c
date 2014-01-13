/*
 * File: ftk_bitmap_dummy.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   dummy bitmap implemntation.
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
#include "ftk_globals.h"

FtkBitmap* ftk_bitmap_create(int w, int h, FtkColor color)
{
	FtkBitmap* thiz = (FtkBitmap*)FTK_ALLOC(sizeof(FtkBitmap));

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
	return 0;
}

int ftk_bitmap_height(FtkBitmap* thiz)
{
	return 0;
}

FtkColor* ftk_bitmap_lock(FtkBitmap* thiz)
{
	return NULL;
}

void    ftk_bitmap_unlock(FtkBitmap* thiz)
{
	return;
}

FtkColor ftk_bitmap_get_pixel(FtkBitmap* thiz, int x, int y)
{
	FtkColor c = {0, 0, 0, 0};	
	
	return c;
}

void ftk_bitmap_destroy(FtkBitmap* thiz)
{
	if(thiz != NULL)
	{

		FTK_ZFREE(thiz, sizeof(FtkBitmap));
	}

	return;
}

