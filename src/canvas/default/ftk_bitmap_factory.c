/*
 * File: ftk_bitmap_factory.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   bitmap factory.
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
#include "ftk_allocator.h"
#include "ftk_bitmap_factory.h"

#ifdef HAS_BMP
#include "ftk_image_bmp_decoder.h"
#endif

#ifdef HAS_JPEG
#include "ftk_image_jpeg_decoder.h"
#endif

#ifdef HAS_PNG
#include "ftk_image_png_decoder.h"
#endif

#if defined(WIN32) && !defined(WINCE)
#include "ftk_image_win32_decoder.h"
#endif

#ifdef IPHONE
#include "ftk_image_iphone_decoder.h"
#endif

#if defined(ANDROID) && defined(ANDROID_NDK)
#include "ftk_image_android_decoder.h"
#endif

struct _FtkBitmapFactory
{
	int nr;
	FtkImageDecoder* decoders[FTK_MAX_IMAGE_DECODERS];
};

FtkBitmapFactory* ftk_bitmap_factory_create(void)
{
	FtkBitmapFactory* thiz = (FtkBitmapFactory*)FTK_ZALLOC(sizeof(FtkBitmapFactory));

	if(thiz != NULL)
	{
#ifdef HAS_BMP
		ftk_bitmap_factory_add_decoder(thiz, ftk_image_bmp_decoder_create());	
#endif

#ifdef HAS_JPEG
		ftk_bitmap_factory_add_decoder(thiz, ftk_image_jpeg_decoder_create());	
#endif

#ifdef HAS_PNG
		ftk_bitmap_factory_add_decoder(thiz, ftk_image_png_decoder_create());
#endif

#if defined(WIN32) && !defined(WINCE)
		ftk_bitmap_factory_add_decoder(thiz, ftk_image_win32_decoder_create());
#endif

#ifdef IPHONE
		ftk_bitmap_factory_add_decoder(thiz, ftk_image_iphone_decoder_create());
#endif

#if defined(ANDROID) && defined(ANDROID_NDK)
		ftk_bitmap_factory_add_decoder(thiz, ftk_image_android_decoder_create());
#endif
	}

	return thiz;
}

FtkBitmap* ftk_bitmap_factory_load(FtkBitmapFactory* thiz, const char* filename)
{
	int i = 0;
	return_val_if_fail(thiz != NULL && filename != NULL, NULL);

	for(i = 0; i < thiz->nr; i++)
	{
		if(thiz->decoders[i] != NULL && ftk_image_decoder_match(thiz->decoders[i], filename) == RET_OK)
		{
			return ftk_image_decoder_decode(thiz->decoders[i], filename);
		}
	}

	ftk_loge("%s:%d %s is not supported format.\n", __func__, __LINE__, filename);

	return NULL;
}

Ret        ftk_bitmap_factory_add_decoder(FtkBitmapFactory* thiz, FtkImageDecoder* decoder)
{
	return_val_if_fail(thiz != NULL && decoder != NULL, RET_FAIL);
	return_val_if_fail(thiz->nr < FTK_MAX_IMAGE_DECODERS, RET_FAIL);

	thiz->decoders[thiz->nr++] = decoder;

	return RET_OK;
}

void       ftk_bitmap_factory_destroy(FtkBitmapFactory* thiz)
{
	int i = 0;
	if(thiz != NULL)
	{
		for(i = 0; i < thiz->nr; i++)
		{
			if(thiz->decoders[i] != NULL)
			{
				ftk_image_decoder_destroy(thiz->decoders[i]);
			}
		}

		FTK_ZFREE(thiz, sizeof(FtkBitmapFactory));
	}

	return;
}

