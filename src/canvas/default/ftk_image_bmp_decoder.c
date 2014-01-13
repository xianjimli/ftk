/*
 * File: ftk_image_bmp_decoder.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   bmp format image decoder.
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
 * 2009-11-28 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_mmap.h"
#include "ftk_image_bmp_decoder.h"

/**
 * http://atlc.sourceforge.net/bmp.html
 */
static Ret ftk_image_bmp_decoder_match(FtkImageDecoder* thiz, const char* filename)
{
	return_val_if_fail(filename != NULL, RET_FAIL);

	return (strstr(filename, ".bmp") != NULL) ? RET_OK : RET_FAIL;
}

static Ret ftk_image_bmp_copy32(FtkBitmap* bitmap, int compress, const unsigned char* data)
{
	int x = 0;
	int y = 0;
	const unsigned char* src = data;
	int width = ftk_bitmap_width(bitmap);
	int height = ftk_bitmap_height(bitmap);
	FtkColor* bits = ftk_bitmap_lock(bitmap);

	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x++, bits++)
		{
			bits->r = src[0];
			bits->g = src[1];	
			bits->b = src[2];	
			bits->a = 0xff;//src[3];	
			src += 4;
		}
	}

	return RET_OK;
}

static Ret ftk_image_bmp_copy24(FtkBitmap* bitmap, int compress, const unsigned char* data)
{
	int x = 0;
	int y = 0;
	const unsigned char* src = data;
	const unsigned char* lsrc = data;
	int width = ftk_bitmap_width(bitmap);
	int height = ftk_bitmap_height(bitmap);
	FtkColor* bits = ftk_bitmap_lock(bitmap);
	int line_delta = (width * 3 + 3) & 0xfffffffc;

	lsrc += line_delta * (height - 1);
	for(y = 0; y < height; y++)
	{
		src = lsrc;
		for(x = 0; x < width; x++, bits++)
		{
			bits->b = src[0];
			bits->g = src[1];	
			bits->r = src[2];	
			bits->a = 0xff;
			src += 3;
		}
		lsrc -= line_delta;
	}

	return RET_OK;
}

static Ret ftk_image_bmp_copy8(FtkBitmap* bitmap, int compress, FtkColor* palette, unsigned char* data)
{
	int x = 0;
	int y = 0;
	unsigned char index = 0;
	const unsigned char* src = data;
	const unsigned char* lsrc = data;
	int width = ftk_bitmap_width(bitmap);
	int height = ftk_bitmap_height(bitmap);
	FtkColor* bits = ftk_bitmap_lock(bitmap);
	int line_delta = (width + 3) & 0xfffffffc;

	lsrc +=line_delta * (height - 1);
	for(y = 0; y < height; y++)
	{
		src = lsrc;
		for(x = 0; x < width; x++, bits++)
		{
			index = src[x];
			bits->r = palette[index].r;
			bits->g = palette[index].g;
			bits->b = palette[index].b;
			bits->a = 0xff;
		}
		lsrc -= line_delta;
	}

	return RET_OK;
}

static Ret ftk_image_bmp_copy4(FtkBitmap* bitmap, int compress, FtkColor* palette, unsigned char* data)
{
	int x = 0;
	int y = 0;
	int r = 0;
	int w = 0;
	unsigned char index = 0;
	const unsigned char* src = data;
	const unsigned char* lsrc = data;
	int width = ftk_bitmap_width(bitmap);
	int height = ftk_bitmap_height(bitmap);
	FtkColor* bits = ftk_bitmap_lock(bitmap);
	int line_delta = (((width & 0x01) ? (width + 1) : width )/2 + 3) & 0xfffffffc;

	w = width>>1;
	r = width & 0x01;

	lsrc += line_delta * (height - 1);
	for(y = 0; y < height; y++)
	{
		src = lsrc;
		for(x = 0; x < w; x++)
		{
			index = (src[x] >> 4) & 0x0f;
			bits->r = palette[index].r;
			bits->g = palette[index].g;
			bits->b = palette[index].b;
			bits->a = 0xff;
			bits++;

			index = src[x] & 0x0f;
			bits->r = palette[index].r;
			bits->g = palette[index].g;
			bits->b = palette[index].b;
			bits->a = 0xff;
			bits++;
		}

		if(r)
		{
			index = (src[x] >> 4) & 0x0f;
			bits->r = palette[index].r;
			bits->g = palette[index].g;
			bits->b = palette[index].b;
			bits->a = 0xff;
			bits++;
		}
		lsrc -= line_delta;
	}

	return RET_OK;
}

#define SET_COLOR_1BIT(bit, color) if(bit) {color->r=color->g=color->b=0xff;color->a=0xff;}\
	else{color->r=color->g=color->b=0x00;color->a=0xff;}

static Ret ftk_image_bmp_copy1(FtkBitmap* bitmap, int compress, FtkColor* palette, unsigned char* data)
{
	int x = 0;
	int y = 0;
	int r = 0;
	int w = 0;
	unsigned char c = 0;
	const unsigned char* src = data;
	const unsigned char* lsrc = data;
	int width = ftk_bitmap_width(bitmap);
	int height = ftk_bitmap_height(bitmap);
	FtkColor* bits = ftk_bitmap_lock(bitmap);
	int line_delta = (width/8 + 3) & 0xfffffffc;

	w = (width+7)>>3;
	r = width % 8;

	lsrc += line_delta * (height - 1);
	for(y = 0; y < height; y++)
	{
		src = lsrc;
		for(x = 0; x < w; x++)
		{
			c = src[x];
			if(x == (w - 1))
			{
				SET_COLOR_1BIT(c & 0x80,  bits);bits++; if(r == 1) break;
				SET_COLOR_1BIT(c & 0x40,  bits);bits++; if(r == 2) break;
				SET_COLOR_1BIT(c & 0x20,  bits);bits++; if(r == 3) break;
				SET_COLOR_1BIT(c & 0x10,  bits);bits++; if(r == 4) break;
				SET_COLOR_1BIT(c & 0x08 , bits);bits++; if(r == 5) break;
				SET_COLOR_1BIT(c & 0x04,  bits);bits++; if(r == 6) break;
				SET_COLOR_1BIT(c & 0x02,  bits);bits++; if(r == 7) break;
				SET_COLOR_1BIT(c & 0x01,  bits);bits++; 
			}
			else
			{
				SET_COLOR_1BIT(c & 0x80,  bits);bits++; 
				SET_COLOR_1BIT(c & 0x40,  bits);bits++;
				SET_COLOR_1BIT(c & 0x20,  bits);bits++;
				SET_COLOR_1BIT(c & 0x10,  bits);bits++;
				SET_COLOR_1BIT(c & 0x08 , bits);bits++;
				SET_COLOR_1BIT(c & 0x04,  bits);bits++;
				SET_COLOR_1BIT(c & 0x02,  bits);bits++;
				SET_COLOR_1BIT(c & 0x01,  bits);bits++; 
			}
		}
		lsrc -= line_delta;
	}

	return RET_OK;
}

static FtkBitmap* load_bmp (const char *filename)
{
	size_t bpp = 0;
	size_t width = 0;
	size_t height = 0;
	size_t doffset = 0;
	int compress = 0;
	FtkColor bg = {0};
	FtkBitmap* bitmap = NULL;
	FtkColor* palette = NULL;
	unsigned char* src = NULL;
	unsigned char* data = NULL;
	FtkMmap* m = ftk_mmap_create(filename, 0, -1);

	return_val_if_fail(m != NULL, NULL);

	data = (unsigned char*)ftk_mmap_data(m);

	if(data[0] != 'B' || data[1] != 'M')
	{
		ftk_mmap_destroy(m);

		return NULL;
	}

	bg.a = 0xff;
	doffset  = *(unsigned int*)(data + 0x000a);
	width    = *(unsigned int*)(data + 0x0012);
	height   = *(unsigned int*)(data + 0x0016);
	bpp      = *(unsigned short*)(data + 0x001c);
	compress = *(unsigned int*)(data + 0x001e);
	palette  = (FtkColor*)(data + 0x0036);
	src      = data + doffset;

	bitmap = ftk_bitmap_create(width, height, bg);

	switch(bpp)
	{
		case 32:
		{
			ftk_image_bmp_copy32(bitmap, compress, src);
			break;
		}
		case 24:
		{
			ftk_image_bmp_copy24(bitmap, compress, src);
			break;
		}
		case 8:
		{
			ftk_image_bmp_copy8(bitmap, compress, palette, src);
			break;
		}
		case 4:
		{
			ftk_image_bmp_copy4(bitmap, compress, palette, src);
			break;
		}
		case 1:
		{
			ftk_image_bmp_copy1(bitmap, compress, palette, src);
			break;
		}
		default:break;
	}

	ftk_mmap_destroy(m);

	return bitmap;
}

static FtkBitmap* ftk_image_bmp_decoder_decode(FtkImageDecoder* thiz, const char* filename)
{
	return_val_if_fail(ftk_image_bmp_decoder_match(thiz, filename) == RET_OK, NULL);

	return load_bmp(filename);
}

static void ftk_image_bmp_decoder_destroy(FtkImageDecoder* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(thiz));
	}

	return;
}

FtkImageDecoder* ftk_image_bmp_decoder_create(void)
{
	FtkImageDecoder* thiz = (FtkImageDecoder*)FTK_ZALLOC(sizeof(FtkImageDecoder));

	if(thiz != NULL)
	{
		thiz->match   = ftk_image_bmp_decoder_match;
		thiz->decode  = ftk_image_bmp_decoder_decode;
		thiz->destroy = ftk_image_bmp_decoder_destroy;
	}

	return thiz;
}


