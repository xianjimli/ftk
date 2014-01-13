/*
 * File: display_fb_test.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   
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

#include "ftk.h"

void red_bitmap(FtkBitmap* bitmap)
{
	int x = 0;
	int y = 0;
	int w = ftk_bitmap_width(bitmap);
	int h = ftk_bitmap_height(bitmap);
	FtkColor* bits = ftk_bitmap_lock(bitmap);

	for(y = 0; y < h; y++)
	{
		for(x = 0; x < w; x++, bits++)
		{
			bits->g = 0;
			bits->b = 0;
			bits->r = 0xff;
		}
	}

	return;
}

void green_bitmap(FtkBitmap* bitmap)
{
	int x = 0;
	int y = 0;
	int w = ftk_bitmap_width(bitmap);
	int h = ftk_bitmap_height(bitmap);
	FtkColor* bits = ftk_bitmap_lock(bitmap);

	for(y = 0; y < h; y++)
	{
		for(x = 0; x < w; x++, bits++)
		{
			bits->r = 0;
			bits->b = 0;
			bits->g = 0xff;
		}
	}

	return;
}

void blue_bitmap(FtkBitmap* bitmap)
{
	int x = 0;
	int y = 0;
	int w = ftk_bitmap_width(bitmap);
	int h = ftk_bitmap_height(bitmap);
	FtkColor* bits = ftk_bitmap_lock(bitmap);

	for(y = 0; y < h; y++)
	{
		for(x = 0; x < w; x++, bits++)
		{
			bits->g = 0;
			bits->r = 0;
			bits->b = 0xff;
		}
	}

	return;
}

void mire_bitmap(FtkBitmap* bitmap)
{
	int x = 0;
	int y = 0;
	int w = ftk_bitmap_width(bitmap);
	int h = ftk_bitmap_height(bitmap);
	FtkColor* bits = ftk_bitmap_lock(bitmap);

	for (y = 0; y < h; y++)
	{
		for (x=0; x < w; x++, bits++)
		{
			unsigned int color = ((x-w/2)*(x-w/2) + (y-h/2)*(y-h/2))/64;
			bits->r = (color/8) % 256;
			bits->g = (color/4) % 256;
			bits->b = (color/2) % 256;
	//		bits->a = (color*2) % 256;
		}
	}

	return;
}

int main(int argc, char* argv[])
{
	FtkDisplay* thiz = NULL;

	ftk_init(argc, argv);

	thiz = ftk_default_display();
	if(thiz != NULL)
	{
		FtkBitmap* bitmap = NULL;
		FtkColor color = {.a=0xff};
		FtkRect rect = {0};
		rect.width = ftk_display_width(thiz);
		rect.height = ftk_display_height(thiz);

		bitmap = ftk_bitmap_create(rect.width, rect.height, color);
		red_bitmap(bitmap);
		ftk_display_update(thiz, bitmap, &rect, 0, 0);
		sleep(3);
		green_bitmap(bitmap);
		ftk_display_update(thiz, bitmap, &rect, 0, 0);
		sleep(3);
		blue_bitmap(bitmap);
		ftk_display_update(thiz, bitmap, &rect, 0, 0);
		sleep(3);
		mire_bitmap(bitmap);
		ftk_display_update(thiz, bitmap, &rect, 0, 0);
		sleep(3);

		ftk_display_destroy(thiz);
		ftk_bitmap_unref(bitmap);
	}

	return 0;
}
