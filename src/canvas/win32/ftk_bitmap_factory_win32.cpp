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
#ifdef VC6
#define ULONG_PTR unsigned long*
#endif 

#include <windows.h>
#include <gdiplus.h>
#include "ftk_win32.h"

struct _FtkBitmapFactory
{
	int unused;
};

static FtkBitmap* load_win32 (const char *filename)
{
	FtkBitmap* bitmap = NULL;
	WCHAR wfilename[MAX_PATH] = {0};
	mbstowcs(wfilename, filename, MAX_PATH);
	Bitmap* bitmap = Bitmap::FromFile(wfilename);

	return_val_if_fail(bitmap != NULL, NULL);

	if(bitmap->GetWidth() == 0 || bitmap->GetHeight() == 0)
	{
		delete bitmap;

		return NULL;
	}

	return ftk_bitmap_create_with_native(bitmap);
}

FtkBitmapFactory* ftk_bitmap_factory_create(void)
{
	FtkBitmapFactory* thiz = (FtkBitmapFactory*)FTK_ZALLOC(sizeof(FtkBitmapFactory));

	return thiz;
}

FtkBitmap* ftk_bitmap_factory_load(FtkBitmapFactory* thiz, const char* filename)
{
	return_val_if_fail(thiz != NULL && filename != NULL, NULL);

	return load_win32(filename);
}

Ret        ftk_bitmap_factory_add_decoder(FtkBitmapFactory* thiz, FtkImageDecoder* decoder)
{
	return RET_OK;
}

void       ftk_bitmap_factory_destroy(FtkBitmapFactory* thiz)
{
	return;
}

