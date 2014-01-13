/*
 * File: ftk_bitmap.h    
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
#ifndef FTK_BITMAP_H
#define FTK_BITMAP_H

#include "ftk_typedef.h"
#include "ftk_log.h"
#include "ftk_allocator.h"

FTK_BEGIN_DECLS

struct _FtkBitmap;
typedef struct _FtkBitmap FtkBitmap;

struct _FtkBitmap
{
	int ref;
	void* data;
	void* data1;
	void* data2;
	char priv[1];
};

FtkBitmap* ftk_bitmap_create_with_native(void* bitmap);
FtkBitmap* ftk_bitmap_create(int w, int h, FtkColor clear_color);
void*      ftk_bitmap_get_native(FtkBitmap* thiz);
void ftk_bitmap_destroy(FtkBitmap* thiz);
FtkColor ftk_bitmap_get_pixel(FtkBitmap* thiz, int x, int y);

int     ftk_bitmap_width(FtkBitmap* thiz);
int     ftk_bitmap_height(FtkBitmap* thiz);
FtkColor*  ftk_bitmap_lock(FtkBitmap* thiz);
void    ftk_bitmap_unlock(FtkBitmap* thiz);
void       ftk_bitmap_ref(FtkBitmap* thiz);
void       ftk_bitmap_unref(FtkBitmap* thiz);
void       ftk_bitmap_clear(FtkBitmap* thiz, FtkColor c);
Ret        ftk_bitmap_copy_from_bitmap(FtkBitmap* thiz, FtkBitmap* other);

Ret ftk_bitmap_copy_from_data_bgr24(FtkBitmap* bitmap, 
	void* data, int dw, int dh, FtkRect* rect);
Ret ftk_bitmap_copy_to_data_bgr24(FtkBitmap* bitmap, FtkRect* rect, 
	void* data, int ox, int oy, int dw, int dh);

Ret ftk_bitmap_copy_from_data_bgra32(FtkBitmap* bitmap, 
	void* data, int dw, int dh, FtkRect* rect);
Ret ftk_bitmap_copy_to_data_bgra32(FtkBitmap* bitmap, FtkRect* rect, 
	void* data, int ox, int oy, int dw, int dh);

Ret ftk_bitmap_copy_from_data_argb32(FtkBitmap* bitmap, 
	void* data, int dw, int dh, FtkRect* rect);
Ret ftk_bitmap_copy_to_data_argb32(FtkBitmap* bitmap, FtkRect* rect, 
	void* data, int ox, int oy, int dw, int dh);

Ret ftk_bitmap_copy_from_data_rgb565(FtkBitmap* bitmap, 
	void* data, int dw, int dh, FtkRect* rect);
Ret ftk_bitmap_copy_to_data_rgb565(FtkBitmap* bitmap, FtkRect* rect, 
	void* data, int ox, int oy, int dw, int dh);

Ret ftk_bitmap_copy_from_data_rgba32(FtkBitmap* bitmap, 
	void* data, int dw, int dh, FtkRect* rect);
Ret ftk_bitmap_copy_to_data_rgba32(FtkBitmap* bitmap, FtkRect* rect, 
	void* data, int ox, int oy, int dw, int dh);

typedef Ret (*FtkBitmapCopyFromData)(FtkBitmap* bitmap, 
	void* data, int dw, int dh, FtkRect* rect);
typedef Ret (*FtkBitmapCopyToData)(FtkBitmap* bitmap, FtkRect* rect, 
	void* data, int ox, int oy, int dw, int dh);

#define FTK_BITMAP_UNREF(bitmap) if(bitmap != NULL) { ftk_bitmap_unref(bitmap); bitmap = NULL;}

Ret ftk_bitmap_auto_test(void);

FTK_END_DECLS

#endif/*FTK_BITMAP_H*/

