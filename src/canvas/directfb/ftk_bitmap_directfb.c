/*
 * File: ftk_bitmap_directfb.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   directfb bitmap implemntation.
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

#include "directfb_common.h"

FtkBitmap* ftk_bitmap_create(int w, int h, FtkColor color)
{
	FtkBitmap* thiz = (FtkBitmap*)FTK_ALLOC(sizeof(FtkBitmap));

	if(thiz != NULL)
	{
		DFBSurfaceDescription  desc;
		IDirectFBSurface* surface = NULL;
		IDirectFB* dfb = directfb_get();
	
		desc.width  = w;
		desc.height = h;
		desc.caps   = DSCAPS_NONE;
		desc.pixelformat = DSPF_ARGB;
		desc.flags  = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_CAPS | DSDESC_PIXELFORMAT;
	
		dfb->CreateSurface(dfb, &desc, &surface);
		surface->Clear(surface, color.r, color.g, color.b, color.a);

		thiz->ref = 1;
		thiz->data = surface;
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
	int w = 0;
	int h = 0;
	IDirectFBSurface* surface = NULL;
	return_val_if_fail(thiz != NULL && thiz->data != NULL, 0);

	surface = (IDirectFBSurface*)thiz->data;
	surface->GetSize(surface, &w, &h);

	return w;
}

int ftk_bitmap_height(FtkBitmap* thiz)
{
	int w = 0;
	int h = 0;
	IDirectFBSurface* surface = NULL;
	return_val_if_fail(thiz != NULL && thiz->data != NULL, 0);

	surface = (IDirectFBSurface*)thiz->data;
	surface->GetSize(surface, &w, &h);

	return h;
}

FtkColor* ftk_bitmap_lock(FtkBitmap* thiz)
{
	int err  = 0;
	int bitch = 0;
	FtkColor* bits = NULL;
	IDirectFBSurface* surface = NULL;
	return_val_if_fail(thiz != NULL && thiz->data != NULL, 0);
	
	surface = (IDirectFBSurface*)thiz->data;
	DFBCHECK(surface->Lock(surface, DSLF_READ, (void**)&bits, &bitch));

	return bits;
}

void    ftk_bitmap_unlock(FtkBitmap* thiz)
{
	IDirectFBSurface* surface = NULL;
	return_if_fail(thiz != NULL && thiz->data != NULL);
	
	surface = (IDirectFBSurface*)thiz->data;
	surface->Unlock(surface);

	return;
}


FtkColor ftk_bitmap_get_pixel(FtkBitmap* thiz, int x, int y)
{
	FtkColor* bits = NULL;
	FtkColor c = {0, 0, 0, 0};	
	return_val_if_fail(thiz != NULL && x < ftk_bitmap_width(thiz) && y < ftk_bitmap_height(thiz), c);

	bits = ftk_bitmap_lock(thiz);
	if(bits != NULL)
	{
		c = bits[y * ftk_bitmap_width(thiz) + x];
	}
	ftk_bitmap_unlock(thiz);

	return c;
}

void ftk_bitmap_destroy(FtkBitmap* thiz)
{
	if(thiz != NULL)
	{
		IDirectFBSurface* surface = NULL;
		surface = (IDirectFBSurface*)thiz->data;

		surface->Release(surface);
		FTK_ZFREE(thiz, sizeof(FtkBitmap));
	}

	return;
}

