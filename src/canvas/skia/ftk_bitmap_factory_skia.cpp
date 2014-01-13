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
#include "SkString.h"
#include "SkBitmap.h"
#include "SkColor.h"
#include "SkStream.h"
#include "SkTemplates.h"
#include "images/SkImageDecoder.h"
#include "effects/SkPorterDuff.h"

#include "ftk_log.h"
#include "ftk_allocator.h"
#include "ftk_bitmap_factory.h"

#ifdef ANDROID
using namespace android;
#include "ftk_jni.h"
#endif

struct _FtkBitmapFactory
{
	int unused;
};

FtkBitmapFactory* ftk_bitmap_factory_create(void)
{
	FtkBitmapFactory* thiz = (FtkBitmapFactory*)FTK_ZALLOC(sizeof(FtkBitmapFactory));

	return thiz;
}

static bool decodeFile(SkBitmap* bitmap, const char srcPath[]) {
    
    SkFILEStream stream(srcPath);
    if (!stream.isValid()) {
        SkDebugf("ERROR: bad filename <%s>\n", srcPath);
        return false;
    }

    SkImageDecoder* codec = SkImageDecoder::Factory(&stream);
    if (NULL == codec) {
        SkDebugf("ERROR: no codec found for <%s>\n", srcPath);
        return false;
    }

    SkAutoTDelete<SkImageDecoder> ad(codec);

    stream.rewind();
    if (!codec->decode(&stream, bitmap, SkBitmap::kARGB_8888_Config,
                       SkImageDecoder::kDecodePixels_Mode)) {
        SkDebugf("ERROR: codec failed for <%s>\n", srcPath);
        return false;
    }
    return true;
}

static void destroy_bitmap(void* data)
{
	SkBitmap* b = (SkBitmap*)data;

	delete b;

	return;
}

FtkBitmap* ftk_bitmap_factory_load(FtkBitmapFactory* thiz, const char* filename)
{	
#ifdef ANDROID
	return Android_LoadImage(filename);
#else
	SkBitmap* b = new SkBitmap();
	FtkBitmap* bmp = NULL;

	if (decodeFile(b, filename)) 
	{
		bmp = ftk_bitmap_create_with_native(b);
	}

	return bmp;
#endif
}

Ret        ftk_bitmap_factory_add_decoder(FtkBitmapFactory* thiz, FtkImageDecoder* decoder)
{
	return RET_OK;
}

void       ftk_bitmap_factory_destroy(FtkBitmapFactory* thiz)
{
	return;
}

