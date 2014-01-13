/*
 * File: ftk_image_png_decoder.c	
 * Author:	Li XianJing <xianjimli@hotmail.com>
 * Brief:	png format image decoder.
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
 * 2010-10-02 Jiao JinXing <jiaojinxing1987@gmail.com> add rt-thread support.
 *
 */

#define PNG_SKIP_SETJMP_CHECK
#include "ftk_log.h"
#include "ftk_image_png_decoder.h"
#include <png.h>

#ifdef RT_THREAD
static void ftk_image_png_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	int fd = (int)png_ptr->io_ptr;

	read(fd, data, length);
}

static png_voidp ftk_png_malloc(png_structp png_ptr, png_size_t size)
{
	return malloc(size);
}

static void ftk_png_free(png_structp png_ptr, png_voidp ptr)
{
	free(ptr);
}
#endif

static Ret ftk_image_png_decoder_match(FtkImageDecoder* thiz, const char* filename)
{
	return_val_if_fail(filename != NULL, RET_FAIL);

	return (strstr(filename, ".png") != NULL) ? RET_OK : RET_FAIL;
}

static FtkBitmap* load_png (const char *filename)
{
	FtkColor  bg = {0};
	unsigned int x = 0;
	unsigned int y = 0;
	png_byte bit_depth = 0;
	unsigned int width = 0;
	unsigned int height = 0;
	int number_of_passes = 0;
	png_byte color_type = 0;

	png_infop info_ptr = NULL;
	png_structp png_ptr = NULL;
	png_bytep * row_pointers = NULL;
	
	char header[8];	   // 8 is the maximum size that can be checked
	int rowbytes = 0;
	FtkColor* dst = NULL;
	unsigned char* src = NULL;
	FtkBitmap* bitmap = NULL;

	FILE *fp = fopen(filename, "rb");
	return_val_if_fail(fp, NULL);

	bg.a = 0xff;

	fread(header, 1, 8, fp);
	return_val_if_fail(png_sig_cmp(header, 0, 8) == 0, NULL);

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	return_val_if_fail(png_ptr, NULL);

	info_ptr = png_create_info_struct(png_ptr);
	return_val_if_fail(info_ptr, NULL);

	return_val_if_fail(setjmp(png_jmpbuf(png_ptr)) == 0, NULL);

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	return_val_if_fail(setjmp(png_jmpbuf(png_ptr)) == 0, NULL);
	
	rowbytes = png_get_rowbytes(png_ptr,info_ptr);
	row_pointers = (png_bytep*) FTK_ZALLOC(sizeof(png_bytep) * height);

	for (y=0; y<height; y++) 
	{
		row_pointers[y] = (png_byte*) FTK_ZALLOC(rowbytes);
	}

	png_read_image(png_ptr, row_pointers);

	fclose(fp);
	
	bitmap = ftk_bitmap_create(width, height, bg);
	dst = ftk_bitmap_lock(bitmap);

	if (color_type == PNG_COLOR_TYPE_RGBA)
	{
		unsigned int w = width;
		unsigned int h = height;
		for(y = 0; y < h; y++)
		{
			src = row_pointers[y];
			for(x = 0; x < w; x++)
			{
				if(src[3])
				{
					dst->r = src[0];
					dst->g = src[1];
					dst->b = src[2];
					dst->a = src[3];
				}
				else
				{
					dst->r = 0xff;
					dst->g = 0xff;
					dst->b = 0xff;
					dst->a = 0;
				}
				src +=4;
				dst++;
			}
		}
	}
	else if(color_type == PNG_COLOR_TYPE_RGB)
	{
		unsigned int w = width;
		unsigned int h = height;
		for(y = 0; y < h; y++)
		{
			src = row_pointers[y];
			for(x = 0; x < w; x++)
			{
				dst->r = src[0];
				dst->g = src[1];
				dst->b = src[2];
				dst->a = 0xff;
				src += 3;
				dst++;
			}
		}
	}
	else
	{
		assert(!"not supported.");
	}

	for(y = 0; y < height; y++)
	{
		FTK_FREE(row_pointers[y]);
	}
	FTK_FREE(row_pointers);

	return bitmap;
}

static FtkBitmap* ftk_image_png_decoder_decode(FtkImageDecoder* thiz, const char* filename)
{
	return_val_if_fail(ftk_image_png_decoder_match(thiz, filename) == RET_OK, NULL);

	return load_png(filename);
}

static void ftk_image_png_decoder_destroy(FtkImageDecoder* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(thiz));
	}
}

FtkImageDecoder* ftk_image_png_decoder_create(void)
{
	FtkImageDecoder* thiz = (FtkImageDecoder*)FTK_ZALLOC(sizeof(FtkImageDecoder));

	if(thiz != NULL)
	{
		thiz->match   = ftk_image_png_decoder_match;
		thiz->decode  = ftk_image_png_decoder_decode;
		thiz->destroy = ftk_image_png_decoder_destroy;
	}

	return thiz;
}

