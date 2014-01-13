/*
 * File: ftk_display_psp.c    
 * Author:  Tao Yu <yut616@gmail.com>
 * Brief:   PSP Display.
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
 * 2010-03-11 Tao Yu <yut616@gmail.com> created.
 * 2010-03-26 Tao Yu <yut616@gmail.com> change the format to rgba, use linelen 512 for pixel operation.
 */

#include <pspkernel.h>
#include <pspge.h>
#include <pspdebug.h>
#include "ftk_log.h"
#include "ftk_display_psp.h"

#define DISPLAY_LINELEN_PSP	512
#define DISPLAY_WIDTH_PSP	480
#define DISPLAY_HEIGHT_PSP	272

typedef struct _PrivInfo
{
	int width;
	int height;
	int linelen;
	int pixelsize;
	unsigned char* bits;
	FtkBitmapCopyFromData copy_from_data;
	FtkBitmapCopyToData   copy_to_data;
}PrivInfo;

static Ret ftk_display_psp_update(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	Ret ret = RET_FAIL;
	int display_width = 0;;
	int display_height = 0;
	DECL_PRIV(thiz, priv);

	display_width  = priv->linelen;
	display_height = priv->height;
	ret = priv->copy_to_data(bitmap, rect, 
			priv->bits, xoffset, yoffset, display_width, display_height); 
	ftk_logd("%s: %d %d %d %d\n", __func__, rect->x, rect->y, rect->width, rect->height);
	return ret;
}

static int ftk_display_psp_width(FtkDisplay* thiz)
{
	PrivInfo* priv = thiz != NULL ? (PrivInfo*)thiz->priv : NULL;
	return_val_if_fail(priv != NULL, 0);

	return priv->width;
}

static int ftk_display_psp_height(FtkDisplay* thiz)
{
	PrivInfo* priv = thiz != NULL ? (PrivInfo*)thiz->priv : NULL;
	return_val_if_fail(priv != NULL, 0);

	return priv->height;
}

static Ret ftk_display_psp_snap(FtkDisplay* thiz, FtkRect* r, FtkBitmap* bitmap)
{
	FtkRect rect = {0};
	DECL_PRIV(thiz, priv);
	int w = priv->linelen;
	int h = ftk_display_height(thiz);
	int bw = ftk_bitmap_width(bitmap);
	int bh = ftk_bitmap_height(bitmap);
	
	rect.x = r->x;
	rect.y = r->y;
	rect.width = FTK_MIN(bw, r->width);
	rect.height = FTK_MIN(bh, r->height);

	return priv->copy_from_data(bitmap, priv->bits, w, h, &rect);
}

static void ftk_display_psp_destroy(FtkDisplay* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(FtkDisplay) + sizeof(PrivInfo));
	}

	return;
}

FtkDisplay* ftk_display_psp_create(FtkSource** event_source, FtkOnEvent on_event, void* ctx)
{
	int width  = DISPLAY_WIDTH_PSP;
	int height = DISPLAY_HEIGHT_PSP;
	int linelen = DISPLAY_LINELEN_PSP;

	ftk_logd("%s width=%d height=%d\n", __func__, width, height);

	FtkDisplay* thiz = (FtkDisplay*)FTK_ZALLOC(sizeof(FtkDisplay) + sizeof(PrivInfo));
	
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->update   = ftk_display_psp_update;
		thiz->snap     = ftk_display_psp_snap;
		thiz->width    = ftk_display_psp_width;
		thiz->height   = ftk_display_psp_height;
		thiz->destroy  = ftk_display_psp_destroy;
	
		priv->width   = width;
		priv->height  = height;
		priv->linelen = linelen;
		priv->pixelsize = 4;

		if(priv->pixelsize == 4)
		{
			priv->copy_to_data   = ftk_bitmap_copy_to_data_rgba32;
			priv->copy_from_data = ftk_bitmap_copy_from_data_rgba32;
		}
		else
		{
			assert(!"not supported pixel format.");
		}
		priv->bits = (void *)(0x40000000 | (unsigned int)sceGeEdramGetAddr());

		*event_source = ftk_source_psp_create(thiz, on_event, ctx);
	}

	return thiz;
}

Ret ftk_display_psp_on_resize(FtkDisplay* thiz, int width, int height)
{
	PrivInfo* priv = thiz != NULL ? (PrivInfo*)thiz->priv : NULL;
	return_val_if_fail(priv != NULL, RET_FAIL);

	priv->width  = width;
	priv->height = height;
	ftk_display_psp_update(thiz, NULL, NULL, 0, 0);

	return RET_OK;
}

