
#include "ftk_typedef.h"
#include "ftk_log.h"
#include "ftk_image_gtk_decoder.h"
#include <gdk-pixbuf/gdk-pixbuf.h>

static Ret ftk_image_gtk_decoder_match(FtkImageDecoder* thiz, const char* filename)
{
	return_val_if_fail(thiz != NULL && filename != NULL, RET_FAIL);

	return (strstr(filename, "bmp") != NULL
		|| strstr(filename, ".png") != NULL 
		|| strstr(filename, ".jpg") != NULL
		|| strstr(filename, ".jpeg") != NULL) ? RET_OK : RET_FAIL;
}

static FtkBitmap* ftk_image_gtk_decoder_decode(FtkImageDecoder* thiz, const char* filename)
{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	FtkColor bg = {0};
	FtkBitmap* bitmap = NULL;
	FtkColor* src = NULL;
	FtkColor* dst = NULL;
	int bpp = 0;
	GError *error = NULL;
	GdkPixbuf* img = NULL;
	return_val_if_fail(thiz != NULL && filename != NULL, NULL);

	img = gdk_pixbuf_new_from_file(filename, &error);
	if(img == NULL)
	{
		ftk_loge("gdk_pixbuf_new_from_file() \"%s\" failed\n", filename);
		return NULL;
	}

	w = gdk_pixbuf_get_width(img);
	h = gdk_pixbuf_get_height(img);
	bpp = gdk_pixbuf_get_rowstride(img) / w * 8;

	bg.a = 0xff;
	bitmap = ftk_bitmap_create(w, h, bg);

	src = (FtkColor*)gdk_pixbuf_get_pixels(img);
	dst = ftk_bitmap_lock(bitmap);

	ftk_logd("ftk_image_gtk_decoder_decode() bpp:%d\n", bpp);

	// TODO: bpp

	for(y = 0; y < h; y++)
	{
		for(x = 0; x < w; x++)
		{
			if(bpp == 32)
			{
				*dst = *src;
				dst++;
				src++;
			}
			else if(bpp == 24)
			{
				dst->b = src->b;
				dst->g = src->g;
				dst->r = src->r;
				dst->a = 0xff;

				dst++;
				src = (FtkColor*)((unsigned char*)src + 3);
			}
		}
	}

	g_object_unref(img);

	return bitmap;
}

static void ftk_image_gtk_decoder_destroy(FtkImageDecoder* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(FtkImageDecoder));
	}
}

FtkImageDecoder* ftk_image_gtk_decoder_create(void)
{
	FtkImageDecoder* thiz = (FtkImageDecoder*)FTK_ZALLOC(sizeof(FtkImageDecoder));
	if(thiz != NULL)
	{
		thiz->match = ftk_image_gtk_decoder_match;
		thiz->decode = ftk_image_gtk_decoder_decode;
		thiz->destroy = ftk_image_gtk_decoder_destroy;
	}

	return thiz;
}
