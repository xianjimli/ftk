/*
 * File: ftk_image_win32_decoder.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   png/jpeg format image decoder on win32.
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
 * 2010-01-17 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifdef VC6
#define ULONG_PTR unsigned long*
#endif 

#include <windows.h>
#include <gdiplus.h>
#include "ftk_win32.h"

using namespace Gdiplus;
static ULONG_PTR gdiplusToken;

#include "ftk_image_win32_decoder.h"
static Ret ftk_image_win32_decoder_match(FtkImageDecoder* thiz, const char* filename)
{
	return_val_if_fail(filename != NULL, RET_FAIL);

	return (strstr(filename, ".png") != NULL 
		|| strstr(filename, ".jpg") != NULL
		|| strstr(filename, ".jpeg") != NULL) ? RET_OK : RET_FAIL;
}

static FtkBitmap* load_win32 (const char *filename)
{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	FtkColor bg = {0};
	FtkBitmap* bitmap = NULL;
	WCHAR wfilename[MAX_PATH] = {0};
	mbstowcs(wfilename, filename, MAX_PATH);
	Bitmap* img = Bitmap::FromFile(wfilename);

	return_val_if_fail(img != NULL, NULL);
	if(img->GetWidth() == 0 || img->GetHeight() == 0)
	{
		delete img;

		return NULL;
	}

	w = img->GetWidth();
	h = img->GetHeight();

	bg.a = 0xff;
	bitmap = ftk_bitmap_create(w, h, bg);
	Rect r(0, 0, w, h);
	BitmapData bitmapData;
#ifdef VC6
	img->LockBits(r, ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);
#else
	img->LockBits(&r, ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);
#endif
	FtkColor* src = (FtkColor*)bitmapData.Scan0;
	FtkColor* dst = ftk_bitmap_lock(bitmap);

	for(y = 0; y < h; y++)
	{
		for(x = 0; x < w; x++)
		{
			*dst = *src;
			dst++;
			src++;
		}
	}

	img->UnlockBits(&bitmapData);
	
	delete img;

	return bitmap;
}

static FtkBitmap* ftk_image_win32_decoder_decode(FtkImageDecoder* thiz, const char* filename)
{
	return_val_if_fail(ftk_image_win32_decoder_match(thiz, filename) == RET_OK, NULL);

	return load_win32(filename);
}

static void ftk_image_win32_decoder_destroy(FtkImageDecoder* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(thiz));
	}

	 GdiplusShutdown(gdiplusToken);

	return;
}

FtkImageDecoder* ftk_image_win32_decoder_create(void)
{
	FtkImageDecoder* thiz = (FtkImageDecoder*)FTK_ZALLOC(sizeof(FtkImageDecoder));

	if(thiz != NULL)
	{
		GdiplusStartupInput gdiplusStartupInput;

		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		thiz->match   = ftk_image_win32_decoder_match;
		thiz->decode  = ftk_image_win32_decoder_decode;
		thiz->destroy = ftk_image_win32_decoder_destroy;
	}

	return thiz;
}

