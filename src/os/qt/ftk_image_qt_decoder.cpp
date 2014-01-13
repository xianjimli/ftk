
extern "C" {
#include "ftk_typedef.h"
}
#include "ftk_log.h"
#include "ftk_image_qt_decoder.h"
#include <QtGui/QImage>

static Ret ftk_image_qt_decoder_match(FtkImageDecoder* thiz, const char* filename)
{
	return_val_if_fail(thiz != NULL && filename != NULL, RET_FAIL);

	return (strstr(filename, "bmp") != NULL
		|| strstr(filename, ".png") != NULL 
		|| strstr(filename, ".jpg") != NULL
		|| strstr(filename, ".jpeg") != NULL) ? RET_OK : RET_FAIL;
}

static FtkBitmap* ftk_image_qt_decoder_decode(FtkImageDecoder* thiz, const char* filename)
{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	FtkColor bg = {0};
	FtkBitmap* bitmap = NULL;
	FtkColor* src = NULL;
	FtkColor* dst = NULL;
	QImage* img = NULL;
	return_val_if_fail(thiz != NULL && filename != NULL, NULL);

	img = new QImage();
	if(!img->load(QString::fromUtf8(filename)))
	{
		delete img;
		return NULL;
	}

	w = img->width();
	h = img->height();
	bg.a = 0xff;
	bitmap = ftk_bitmap_create(w, h, bg);

	QImage::Format format = img->format();
	const unsigned char* bits = img->bits();//img->constBits();

	src = (FtkColor*)bits;
	dst = ftk_bitmap_lock(bitmap);

	// TODO: format

	for(y = 0; y < h; y++)
	{
		for(x = 0; x < w; x++)
		{
			*dst = *src;
			dst++;
			src++;
		}
	}

	delete img;

	return bitmap;
}

static void ftk_image_qt_decoder_destroy(FtkImageDecoder* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(FtkImageDecoder));
	}
}

FtkImageDecoder* ftk_image_qt_decoder_create(void)
{
	FtkImageDecoder* thiz = (FtkImageDecoder*)FTK_ZALLOC(sizeof(FtkImageDecoder));
	if(thiz != NULL)
	{
		thiz->match = ftk_image_qt_decoder_match;
		thiz->decode = ftk_image_qt_decoder_decode;
		thiz->destroy = ftk_image_qt_decoder_destroy;
	}

	return thiz;
}
