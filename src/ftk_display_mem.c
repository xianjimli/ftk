/*
 * File: ftk_display_mem.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   memory based display.
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
 * 2010-03-21 Li XianJing <xianjimli@hotmail.com> from ftk_display_fb.c
 *
 */

#include "ftk_log.h"
#include "ftk_display_mem.h"

typedef struct _DisplayMemPrivInfo
{
	int bpp;
	int width;
	int height;
	void* bits;
	FtkDestroy on_destroy;
	void* on_destroy_ctx;
	FtkPixelFormat format;
	FtkBitmapCopyFromData copy_from_data;
	FtkBitmapCopyToData   copy_to_data;
	FtkDisplaySync sync;
	void* sync_ctx;
}PrivInfo;

static Ret ftk_display_mem_update(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	Ret ret = RET_OK;
	DECL_PRIV(thiz, priv);
	int display_width  = priv->width;
	int display_height = priv->height;
	return_val_if_fail(priv != NULL, RET_FAIL);

	ftk_logd("%s: ox=%d oy=%d (%d %d %d %d)\n", __func__, xoffset, yoffset, 
		rect->x, rect->y, rect->width, rect->height);
	ret = priv->copy_to_data(bitmap, rect, 
		priv->bits, xoffset, yoffset, display_width, display_height); 

	if(priv->sync != NULL && ret == RET_OK)
	{
		FtkRect r;
		r.x = xoffset;
		r.y = yoffset;
		r.width = rect->width;
		r.height = rect->height;
		priv->sync(priv->sync_ctx, &r);
	}

	return ret;
}

static int ftk_display_mem_width(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);

	return priv->width;
}

static int ftk_display_mem_height(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);

	return priv->height;
}

static Ret ftk_display_mem_snap(FtkDisplay* thiz, FtkRect* r, FtkBitmap* bitmap)
{
	FtkRect rect = {0};
	DECL_PRIV(thiz, priv);
	int w = ftk_display_width(thiz);
	int h = ftk_display_height(thiz);
	int bw = ftk_bitmap_width(bitmap);
	int bh = ftk_bitmap_height(bitmap);
	return_val_if_fail(priv != NULL, RET_FAIL);
	
	rect.x = r->x;
	rect.y = r->y;
	rect.width = FTK_MIN(bw, r->width);
	rect.height = FTK_MIN(bh, r->height);

	return priv->copy_from_data(bitmap, priv->bits, w, h, &rect);
}

static void ftk_display_mem_destroy(FtkDisplay* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		
		if(priv->on_destroy != NULL)
		{
			priv->on_destroy(priv->on_destroy_ctx);
		}

		FTK_ZFREE(thiz, sizeof(FtkDisplay) + sizeof(PrivInfo));
	}

	return;
}

FtkDisplay* ftk_display_mem_create(FtkPixelFormat format, 
	int width, int height, void* bits, FtkDestroy on_destroy, void* ctx)
{
	FtkDisplay* thiz = NULL;

	thiz = (FtkDisplay*)FTK_ZALLOC(sizeof(FtkDisplay) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->update   = ftk_display_mem_update;
		thiz->width    = ftk_display_mem_width;
		thiz->height   = ftk_display_mem_height;
		thiz->snap     = ftk_display_mem_snap;
		thiz->destroy  = ftk_display_mem_destroy;

		priv->bits = bits;
		priv->width = width;
		priv->height = height;
		priv->format = format;
		priv->on_destroy = on_destroy;
		priv->on_destroy_ctx = ctx;

		switch(format)
		{
			case FTK_PIXEL_RGB565:
			{
				priv->bpp = 2;
				priv->copy_to_data   = ftk_bitmap_copy_to_data_rgb565;
				priv->copy_from_data = ftk_bitmap_copy_from_data_rgb565;
				break;
			}
			case FTK_PIXEL_BGR24:
			{
				priv->bpp = 3;
				priv->copy_to_data   = ftk_bitmap_copy_to_data_bgr24;
				priv->copy_from_data = ftk_bitmap_copy_from_data_bgr24;
				break;
			}
			case FTK_PIXEL_BGRA32:
			{
				priv->bpp = 4;
#ifdef WORDS_BIGENDIAN				
				priv->copy_to_data   = ftk_bitmap_copy_to_data_argb32;
				priv->copy_from_data = ftk_bitmap_copy_from_data_argb32;
#else
				priv->copy_to_data   = ftk_bitmap_copy_to_data_bgra32;
				priv->copy_from_data = ftk_bitmap_copy_from_data_bgra32;
#endif
				break;
			}
			case FTK_PIXEL_RGBA32:
			{
				priv->bpp = 4;
				priv->copy_to_data   = ftk_bitmap_copy_to_data_rgba32;
				priv->copy_from_data = ftk_bitmap_copy_from_data_rgba32;
				break;
			}
			default:
			{
				assert(!"not supported framebuffer format.");
				break;
			}
		}
	}
		
	return thiz;
}

Ret ftk_display_mem_set_sync_func(FtkDisplay* thiz, FtkDisplaySync sync, void* ctx)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && thiz->update == ftk_display_mem_update, RET_FAIL);

	priv->sync = sync;
	priv->sync_ctx = ctx;

	return RET_OK;
}

int ftk_display_mem_is_active(FtkDisplay* thiz)
{
	return (thiz != NULL && thiz->update == ftk_display_mem_update);
}

FtkPixelFormat ftk_display_mem_get_pixel_format(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);

	return priv != NULL ? priv->format : FTK_PIXEL_NONE;
}

Ret ftk_display_mem_update_directly(FtkDisplay* thiz, FtkPixelFormat format,
	void* bits, int width, int height, int xoffset, int yoffset)
{
	int w = 0;
	int h = 0;
	char* src = NULL;
	char* dst = NULL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(bits != NULL, RET_FAIL);
	return_val_if_fail(ftk_display_mem_is_active(thiz), RET_FAIL);
	return_val_if_fail(xoffset < priv->width && yoffset < priv->height, RET_FAIL);

	w = (xoffset + width) < priv->width ? width : priv->width - xoffset;
	h = (yoffset + height) < priv->height ? height : priv->height - yoffset;

	if(format == priv->format)
	{
		src = (char*)bits;
		dst = (char*)priv->bits + priv->width * priv->bpp + xoffset;
		for(; h; h--)
		{
			memcpy(dst, src, priv->bpp * w);
			dst += priv->width * priv->bpp;
			src += width * priv->bpp;
		}
	}
	else
	{
		/*TODO*/
		assert(!"not supprted yet");
	}

	return RET_OK;
}

void* ftk_display_bits(FtkDisplay* thiz, int* bpp)
{
    DECL_PRIV(thiz, priv);
    return_val_if_fail(thiz != NULL && priv != NULL && priv->bits != NULL, NULL);
    return_val_if_fail(bpp != NULL, NULL);

    *bpp = priv->bpp;

    return priv->bits;
}
