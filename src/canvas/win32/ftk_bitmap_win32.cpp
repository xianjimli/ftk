#include "ftk_bitmap.h"
#include "ftk_globals.h"

#ifdef VC6
#define ULONG_PTR unsigned long*
#endif 

#include <windows.h>
#include <gdiplus.h>
#include "ftk_win32.h"

FtkBitmap* ftk_bitmap_create(int w, int h, FtkColor color)
{
	Bitmap* bitmap = NULL;
	FtkColor* bits = NULL;
	FtkBitmap* thiz = (FtkBitmap*)FTK_ZALLOC(sizeof(FtkBitmap));

	if(thiz != NULL)
	{
		thiz->ref = 1;

		bits = FTK_ZALLOC(w * h * 4);
		if(bits != NULL)
		{
			bitmap = new Bitmap(w, h, w * 4, PixelFormat32bppARGB, (char*)bits);
			if(bitmap != NULL)
			{
				thiz->data = bitmap;
				thiz->data1 = bits;
			}
			else
			{
				FTK_FREE(bits);
				FTK_FREE(thiz);
			}
		}
		else
		{
			FTK_FREE(thiz);
		}
	}

	return thiz;
}

FtkBitmap* ftk_bitmap_create_with_native(void* bitmap)
{
	FtkBitmap* thiz = (FtkBitmap*)FTK_ZALLOC(sizeof(FtkBitmap));

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
	Bitmap* bitmap = NULL;
	return_val_if_fail(thiz != NULL && thiz->data != NULL, 0);

	bitmap = (Bitmap*)thiz->data;

	return bitmap->GetWidth();
}

int ftk_bitmap_height(FtkBitmap* thiz)
{
	Bitmap* bitmap = NULL;
	return_val_if_fail(thiz != NULL && thiz->data != NULL, 0);

	bitmap = (Bitmap*)thiz->data;

	return bitmap->GetHeight();
}

FtkColor* ftk_bitmap_lock(FtkBitmap* thiz)
{
	Rect r(0, 0, w, h);
	Bitmap* bitmap = NULL;
	BitmapData *bitmapData = new BitmapData();
	return_val_if_fail(thiz != NULL && thiz->data != NULL, 0);

	bitmap = (Bitmap*)thiz->data;

#ifdef VC6
	bitmap->LockBits(r, ImageLockModeRead, PixelFormat32bppARGB, bitmapData);
#else
	bitmap->LockBits(&r, ImageLockModeRead, PixelFormat32bppARGB, bitmapData);
#endif

	thiz->data2 = bitmapData;

	return (FtkColor*)bitmapData->Scan0;
}

void    ftk_bitmap_unlock(FtkBitmap* thiz)
{
	Bitmap* bitmap = NULL;
	BitmapData *bitmapData = NULL;
	return_val_if_fail(thiz != NULL && thiz->data != NULL, 0);

	bitmap = (Bitmap*)thiz->data;
	bitmapData = (BitmapData)thiz->data2;
	bitmap->UnlockBits(bitmapData);
	delete bitmapData;
	thiz->data2 = NULL;

	return;
}

FtkColor ftk_bitmap_get_pixel(FtkBitmap* thiz, int x, int y)
{
	FtkColor c;
	Color cc = {0};
	Bitmap* bitmap = NULL;
	return_val_if_fail(thiz != NULL && thiz->data != NULL
		&& x < ftk_bitmap_width(thiz) && y < ftk_bitmap_height(thiz), c);
	
	bitmap = (Bitmap*)thiz->data;
	bitmap->GetPixel(x, y, &cc);

	c.a = cc.GetA();
	c.r = cc.GetR();
	c.g = cc.GetG();
	c.b = cc.GetB();

	return c;
}

void ftk_bitmap_destroy(FtkBitmap* thiz)
{
	if(thiz != NULL)
	{
		Bitmap* bitmap = (Bitmap*)thiz->data;
		if(bitmap != NULL)
		{
			delete bitmap;
		}

		if(thiz->data1 != NULL)
		{
			FTK_FREE(thiz->data1);
		}

		FTK_ZFREE(thiz, sizeof(FtkBitmap));
	}

	return;
}

