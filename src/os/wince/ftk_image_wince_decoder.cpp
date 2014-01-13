
#include "ftk_wince.h"
#include "ftk_image_wince_decoder.h"
#include "ftk_log.h"
#include <initguid.h>
#include <Imaging.h>

static Ret ftk_image_wince_decoder_match(FtkImageDecoder* thiz, const char* filename)
{
	return_val_if_fail(filename != NULL, RET_FAIL);

	return (strstr(filename, "bmp") != NULL
		|| strstr(filename, ".png") != NULL 
		|| strstr(filename, ".jpg") != NULL
		|| strstr(filename, ".jpeg") != NULL) ? RET_OK : RET_FAIL;
}

static FtkBitmap* load_image(const char *filename)
{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	FtkColor bg = {0};
	FtkBitmap* bitmap = NULL;
	HRESULT hr;
	IImagingFactory *img_factory = NULL;
	IImage *img = NULL;
	IBitmapImage *bmp = NULL;
	SIZE size;
	BitmapData bmp_data;
	RECT rect;
	wchar_t buf[MAX_PATH];
	if (MultiByteToWideChar(CP_ACP, 0, filename, -1, buf, MAX_PATH) == 0)
	{
		return NULL;
	}
	hr = CoCreateInstance(CLSID_ImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IImagingFactory, (void **) &img_factory);
	if (FAILED(hr))
	{
		ftk_loge("CoCreateInstance(CLSID_ImagingFactory) failed\n");
		return NULL;
	}
	hr = img_factory->CreateImageFromFile(buf, &img);
	if (FAILED(hr))
	{
		ftk_loge("CreateImageFromFile() failed (%s)\n", filename);
		img_factory->Release();
		return NULL;
	}
	hr = img_factory->CreateBitmapFromImage(img, 0, 0, PixelFormatDontCare, InterpolationHintDefault, &bmp);
	if (FAILED(hr))
	{
		ftk_loge("CreateBitmapFromImage() failed\n");
		img->Release();
		img_factory->Release();
		return NULL;
	}
	size.cx = 0;
	size.cy = 0;
	hr = bmp->GetSize(&size);
	if (FAILED(hr))
	{
		ftk_loge("GetSize() failed\n");
	}
	if (size.cx == 0 || size.cy == 0)
	{
		bmp->Release();
		img->Release();
		img_factory->Release();
		return NULL;
	}
	w = size.cx;
	h = size.cy;
	bg.a = 0xff;
	bitmap = ftk_bitmap_create(w, h, bg);
	rect.left = 0;
	rect.top = 0;
	rect.right = w;
	rect.bottom = h;
	hr = bmp->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bmp_data);
	FtkColor* src = (FtkColor*)bmp_data.Scan0;
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
	hr = bmp->UnlockBits(&bmp_data);
	bmp->Release();
	img->Release();
	img_factory->Release();
	return bitmap;
}

static FtkBitmap* ftk_image_wince_decoder_decode(FtkImageDecoder* thiz, const char* filename)
{
	return_val_if_fail(ftk_image_wince_decoder_match(thiz, filename) == RET_OK, NULL);

	return load_image(filename);
}

static void ftk_image_wince_decoder_destroy(FtkImageDecoder* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(thiz));
	}
}

FtkImageDecoder* ftk_image_wince_decoder_create(void)
{
	FtkImageDecoder* thiz = (FtkImageDecoder*)FTK_ZALLOC(sizeof(FtkImageDecoder));

	if(thiz != NULL)
	{
		thiz->match   = ftk_image_wince_decoder_match;
		thiz->decode  = ftk_image_wince_decoder_decode;
		thiz->destroy = ftk_image_wince_decoder_destroy;
	}

	return thiz;
}
