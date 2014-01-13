/*
 * File: ftk_display_rotate.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   decorate display to support screen rotation.
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
 * 2010-07-10 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_display_rotate.h"

typedef struct _DisplayRotatePrivInfo
{
	FtkRotate   rotate;
	FtkDisplay* display;
	FtkBitmap*  bitmap;
}PrivInfo;

static Ret ftk_display_rotate_update_directly(FtkDisplay* thiz, FtkPixelFormat format,
	void* bits, size_t width, size_t height, size_t xoffset, size_t yoffset)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	return ftk_display_update_directly(priv->display, format, bits, width, height, xoffset, yoffset);
}

static Ret ftk_display_rotate_update_0(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	return ftk_display_update(priv->display, bitmap, rect, xoffset, yoffset);
}

static Ret ftk_display_rotate_snap_0(FtkDisplay* thiz, FtkRect* rect, FtkBitmap* bitmap)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	return ftk_display_snap(priv->display, rect, bitmap);
}

#define UPDATE_DECL() \
	int x = 0;\
	int y = 0;\
	int w = 0;\
	int h = 0;\
	FtkRect r = {0};\
	FtkColor* src = NULL;\
	FtkColor* dst = NULL;\
	DECL_PRIV(thiz, priv);\
	int bm_w = ftk_bitmap_width(bitmap);\
	int bm_h = ftk_bitmap_height(bitmap);\
	int display_w = ftk_bitmap_width(priv->bitmap);\
	int display_h = ftk_bitmap_height(priv->bitmap);\
	\
	if(rect != NULL)\
	{\
		r = *rect;\
	}\
	else\
	{\
		memset(&r, 0x00, sizeof(r));\
	}\
	w = r.width;\
	w = (r.x + w) < bm_w ? w : bm_w - r.x;\
	h = r.height;\
	h = (r.y + h) < bm_h ? h : bm_h - r.y;\
	src = ftk_bitmap_lock(bitmap);\
	dst = ftk_bitmap_lock(priv->bitmap);\
	\
	return_val_if_fail(r.x < bm_w && r.y < bm_h, RET_FAIL);

static Ret ftk_display_rotate_update_180(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	UPDATE_DECL();
	return_val_if_fail(xoffset < display_w && yoffset < display_h, RET_FAIL);
	
	xoffset = display_w - xoffset;
	w = r.width;
	w = (xoffset - w) >= 0 ? w : xoffset;
	w = (r.x + w) < bm_w ? w : bm_w - r.x;
	
	yoffset = display_h - yoffset;
	h = r.height;
	h = (yoffset - h) >= 0 ? h : yoffset;
	h = (r.y + h) < bm_h ? h : bm_h - r.y;

	
	src += r.y * bm_w + r.x;
	dst += (yoffset - 1) * display_w + xoffset - 1;

	for(y = 0; y < h; y++)
	{
		FtkColor* s = src;
		FtkColor* d = dst;
		for(x = 0; x < w; x++, s++, d--)
		{
			*d = *s;
		}
		src += bm_w;
		dst -= display_w;
	}

	r.x = xoffset - w;
	r.y = yoffset - h;
	r.width  = w;
	r.height = h;

	return ftk_display_update(priv->display, priv->bitmap, &r, r.x, r.y);
}
#define SNAP_DECL() \
	int x = 0;\
	int y = 0;\
	int w = 0;\
	int h = 0;\
	FtkRect r = {0};\
	DECL_PRIV(thiz, priv);\
	int bm_w = ftk_bitmap_width(bitmap);\
	int display_w = ftk_bitmap_width(priv->bitmap);\
	int display_h = ftk_bitmap_height(priv->bitmap);\
	FtkColor* src = ftk_bitmap_lock(priv->bitmap);\
	FtkColor* dst = ftk_bitmap_lock(bitmap);\
	\
	w = FTK_MIN(rect->width, ftk_bitmap_width(bitmap));\
	h = FTK_MIN(rect->height, ftk_bitmap_height(bitmap));

static Ret ftk_display_rotate_snap_180(FtkDisplay* thiz, FtkRect* rect, FtkBitmap* bitmap)
{
	SNAP_DECL();

	r.x = display_w - rect->x - w;
	r.y = display_h - rect->y - h;
	r.width  = w;
	r.height = h;

	ftk_display_snap(priv->display, &r, priv->bitmap);

	src += (h - 1) * display_w;
	for(y = 0; y < h; y++)
	{
		FtkColor* d = dst;
		FtkColor* s = src + w - 1;
		for(x = 0; x < w; x++, d++, s--)
		{
			*d = *s;
		}
		src -= display_w;
		dst += bm_w;
	}

	return RET_OK;
}

static Ret ftk_display_rotate_update_90(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	UPDATE_DECL();
	return_val_if_fail(xoffset < display_h && yoffset < display_w, RET_FAIL);
	
	x = xoffset;
	xoffset = display_w - h - yoffset;
	yoffset = x;
	
	w = (yoffset + w) < display_h ? w : display_h - yoffset;
	h = (xoffset + h) < display_w ? h : display_w - xoffset;

	src += r.y * bm_w + r.x;
	dst += yoffset * display_w + xoffset + h - 1;

	for(y = 0; y < h; y++)
	{
		FtkColor* s = src;
		FtkColor* d = dst;
		for(x = 0; x < w; x++, s++)
		{
			d[x * display_w] = *s;
		}
		src += bm_w;
		dst -= 1;
	}

	r.x = xoffset;
	r.y = yoffset;
	r.width  = h;
	r.height = w;

	return ftk_display_update(priv->display, priv->bitmap, &r, r.x, r.y);
}

static Ret ftk_display_rotate_snap_90(FtkDisplay* thiz, FtkRect* rect, FtkBitmap* bitmap)
{
	SNAP_DECL();

	(void)display_h;
	r.y = rect->x;
	r.x = ftk_display_width(priv->display) - rect->y - h;
	r.width = h;
	r.height = w;

	ftk_display_snap(priv->display, &r, priv->bitmap);

	src += h - 1;
	for(y = 0; y < h; y++)
	{
		for(x = 0; x < w; x++)
		{
			dst[x] = src[x * display_w];
		}
		dst += bm_w;
		src--;
	}
	
	return RET_OK;
}

static Ret ftk_display_rotate_update_270(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	ftk_loge("%s: not supported yet.\n", __func__);
	
	return RET_FAIL;
}

static Ret ftk_display_rotate_snap_270(FtkDisplay* thiz, FtkRect* rect, FtkBitmap* bitmap)
{
	ftk_loge("%s: not supported yet.\n", __func__);

	return RET_FAIL;
}

static int ftk_display_rotate_width(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);

	return (priv->rotate == FTK_ROTATE_0 || priv->rotate == FTK_ROTATE_180) ? 
		ftk_display_width(priv->display) : ftk_display_height(priv->display);
}

static int ftk_display_rotate_height(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);

	return (priv->rotate != FTK_ROTATE_0 && priv->rotate != FTK_ROTATE_180) ? 
		ftk_display_width(priv->display) : ftk_display_height(priv->display);
}

static void ftk_display_rotate_destroy(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	if(priv != NULL)
	{
		ftk_display_destroy(priv->display);
		FTK_FREE(thiz);
	}

	return;
}

static const FtkDisplayUpdate updates[FTK_ROTATE_NR] = 
{
	ftk_display_rotate_update_0,
	ftk_display_rotate_update_90,
	ftk_display_rotate_update_180,
	ftk_display_rotate_update_270
};

static const FtkDisplaySnap snaps[FTK_ROTATE_NR] = 
{
	ftk_display_rotate_snap_0,
	ftk_display_rotate_snap_90,
	ftk_display_rotate_snap_180,
	ftk_display_rotate_snap_270
};

Ret ftk_display_set_rotate(FtkDisplay* thiz, FtkRotate rotate)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && priv->display != NULL, RET_FAIL);
	return_val_if_fail(thiz->destroy == ftk_display_rotate_destroy, RET_FAIL);

	if(priv->rotate == rotate && priv->bitmap != NULL)
	{
		return RET_OK;
	}

	priv->rotate = rotate;
	if(priv->bitmap != NULL)
	{
		ftk_bitmap_unref(priv->bitmap);
		priv->bitmap = NULL;
	}

	if(priv->rotate != FTK_ROTATE_0)
	{
		FtkColor bg = {0};
		memset(&bg, 0xff, sizeof(bg));
		priv->bitmap = ftk_bitmap_create(ftk_display_width(priv->display), ftk_display_height(priv->display), bg);
	}

	thiz->snap = snaps[rotate];
	thiz->update = updates[rotate];

	return RET_OK;
}

FtkRotate ftk_display_get_rotate(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && thiz->destroy == ftk_display_rotate_destroy, FTK_ROTATE_0);

	return priv->rotate;
}

FtkDisplay* ftk_display_get_real_display(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && thiz->destroy == ftk_display_rotate_destroy, NULL);

	return priv->display;
}

FtkDisplay* ftk_display_rotate_create(FtkDisplay* display, FtkRotate rotate)
{
	FtkDisplay* thiz = NULL;
	return_val_if_fail(display != NULL, NULL);

	thiz = FTK_NEW_PRIV(FtkDisplay);

	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		priv->display = display;
		thiz->width   = ftk_display_rotate_width;
		thiz->height  = ftk_display_rotate_height;
		thiz->destroy = ftk_display_rotate_destroy;
		if(display->update_directly != NULL)
		{
			thiz->update_directly = ftk_display_rotate_update_directly;
		}

		ftk_display_set_rotate(thiz, rotate);
	}

	return thiz;
}

