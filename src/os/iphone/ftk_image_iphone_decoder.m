
#include <UIKit/UIKit.h>
#include "ftk_image_iphone_decoder.h"

static Ret ftk_image_iphone_decoder_match(FtkImageDecoder* thiz, const char* filename)
{
	return_val_if_fail(filename != NULL, RET_FAIL);

	return (strstr(filename, ".bmp") != NULL
		|| strstr(filename, ".png") != NULL
		|| strstr(filename, ".jpg") != NULL
		|| strstr(filename, ".jpeg") != NULL) ? RET_OK : RET_FAIL;
}

static FtkBitmap* load_image(const char* filename)
{
	int width, height;
	FtkColor bg = {0};
	FtkBitmap* bitmap = NULL;
	UIImage* img = NULL;
	NSString* path = NULL;
	CGRect rect;
	CGContextRef ctx = NULL;
	CGColorSpaceRef color_space = NULL;

	ftk_logd("%s:%d %s\n", __FILE__, __LINE__, filename);

	path = [[NSString alloc] initWithUTF8String:filename];
	img = [UIImage imageWithContentsOfFile:path];
	if(img == NULL)
	{
		[path release];
		return NULL;
	}
	[path release];

	width = CGImageGetWidth(img.CGImage);
	height = CGImageGetHeight(img.CGImage);

	bg.a = 0xff;
	bitmap = ftk_bitmap_create(width, height, bg);

	color_space = CGColorSpaceCreateDeviceRGB();
	ctx = CGBitmapContextCreate(ftk_bitmap_bits(bitmap), width, height, 8, width * 4, color_space, kCGImageAlphaPremultipliedLast);
	CGColorSpaceRelease(color_space);

	rect = CGRectMake(0, 0, width, height);
	CGContextSetFillColorWithColor(ctx, [UIColor clearColor].CGColor);
	CGContextClearRect(ctx, rect);
	CGContextDrawImage(ctx, rect, img.CGImage);

	CGContextRelease(ctx);
	[img release];

	return bitmap;
}

static FtkBitmap* ftk_image_iphone_decoder_decode(FtkImageDecoder* thiz, const char* filename)
{
	return_val_if_fail(ftk_image_iphone_decoder_match(thiz, filename) == RET_OK, NULL);
	return load_image(filename);
}

static void ftk_image_iphone_decoder_destroy(FtkImageDecoder* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(thiz));
	}
}

FtkImageDecoder* ftk_image_iphone_decoder_create(void)
{
	FtkImageDecoder* thiz = (FtkImageDecoder*)FTK_ZALLOC(sizeof(FtkImageDecoder));

	if(thiz != NULL)
	{
		thiz->match = ftk_image_iphone_decoder_match;
		thiz->decode = ftk_image_iphone_decoder_decode;
		thiz->destroy = ftk_image_iphone_decoder_destroy;
	}

	return thiz;
}
