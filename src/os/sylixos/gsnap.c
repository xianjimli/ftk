/*
 * File:    gsnap.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   snap the linux mobile device screen.
 *
 * Copyright (c) 2009  Li XianJing <xianjimli@hotmail.com>
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
 * 2009-08-20 Li XianJing <xianjimli@hotmail.com> created
 * 2011-02-28 Li XianJing <xianjimli@hotmail.com> suppport RGB888 framebuffer.
 * 2011-04-09 Li XianJing <xianjimli@hotmail.com> merge figofuture's png output.
 * 	ref: http://blog.chinaunix.net/space.php?uid=15059847&do=blog&cuid=2040565
 * 2011-08-03 Jiao JinXing <jiaojinxing1987@gmail.com> port to sylixos
 */

#include <png.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <jpeglib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

struct _FBInfo;
typedef struct _FBInfo FBInfo;
typedef int (*UnpackPixel)(FBInfo* fb, unsigned char* pixel, 
	unsigned char* r, unsigned char* g, unsigned char* b);

struct _FBInfo
{
	int fd;
	UnpackPixel unpack;
	unsigned char *bits;
    LW_GM_VARINFO vi;
    LW_GM_SCRINFO fi;
};

#define fb_width(fb)  ((fb)->vi.GMVI_ulXRes)
#define fb_height(fb) ((fb)->vi.GMVI_ulYRes)
#define fb_bpp(fb)    ((fb)->vi.GMVI_ulBitsPerPixel >> 3)
#define fb_size(fb)   ((fb)->vi.GMVI_ulXRes * (fb)->vi.GMVI_ulYRes * fb_bpp(fb))

static int fb_unpack_rgb565(FBInfo* fb, unsigned char* pixel, 
	unsigned char* r, unsigned char* g, unsigned char* b)
{
	unsigned short color = *(unsigned short*)pixel;

	*r = ((color >> 11) & 0xff) << 3;
	*g = ((color >> 5) & 0xff)  << 2;
	*b = (color & 0xff )<< 3;

	return 0;
}

static int fb_unpack_rgb24(FBInfo* fb, unsigned char* pixel, 
	unsigned char* r, unsigned char* g, unsigned char* b)
{
	*r = pixel[0];
	*g = pixel[1];
	*b = pixel[2];

	return 0;
}

static int fb_unpack_argb32(FBInfo* fb, unsigned char* pixel, 
	unsigned char* r, unsigned char* g, unsigned char* b)
{
	*r = pixel[1];
	*g = pixel[2];
	*b = pixel[3];

	return 0;
}

static int fb_unpack_none(FBInfo* fb, unsigned char* pixel, 
	unsigned char* r, unsigned char* g, unsigned char* b)
{
	*r = *g = *b = 0;

	return 0;
}

static void set_pixel_unpacker(FBInfo* fb)
{
	if(fb_bpp(fb) == 2)
	{
		fb->unpack = fb_unpack_rgb565;
	}
	else if(fb_bpp(fb) == 3)
	{
		fb->unpack = fb_unpack_rgb24;
	}
	else if(fb_bpp(fb) == 4)
	{
		fb->unpack = fb_unpack_argb32;
	}
	else
	{
		fb->unpack = fb_unpack_none;
		printf("%s: not supported format.\n", __func__);
	}

	return;
}

static int fb_open(FBInfo* fb, const char* fbfilename)
{
	fb->fd = open(fbfilename, O_RDWR);

	if (fb->fd < 0)
	{
		fprintf(stderr, "can't open %s\n", fbfilename);

		return -1;
	}

	if (ioctl(fb->fd, LW_GM_GET_SCRINFO, &fb->fi) < 0)
		goto fail;

	if (ioctl(fb->fd, LW_GM_GET_VARINFO, &fb->vi) < 0)
		goto fail;

    fb->bits = mmap(0, fb_size(fb), PROT_READ | PROT_WRITE, MAP_SHARED, fb->fd, 0);

	if (fb->bits == MAP_FAILED)
		goto fail;

	set_pixel_unpacker(fb);

	return 0;

fail:
	printf("%s is not a framebuffer.\n", fbfilename);
	close(fb->fd);

	return -1;
}

static void fb_close(FBInfo* fb)
{
    munmap(fb->bits, fb_size(fb));

	close(fb->fd);

	return;
}

static int snap2jpg(const char * filename, int quality, FBInfo* fb)
{
	int row_stride = 0; 
	FILE * outfile = NULL;
	JSAMPROW row_pointer[1] = {0};
	struct jpeg_error_mgr jerr;
	struct jpeg_compress_struct cinfo;

	memset(&jerr, 0x00, sizeof(jerr));
	memset(&cinfo, 0x00, sizeof(cinfo));

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	if ((outfile = fopen(filename, "wb+")) == NULL) 
	{
		fprintf(stderr, "can't open %s\n", filename);

		return -1;
	}

	jpeg_stdio_dest(&cinfo, outfile);
	cinfo.image_width = fb_width(fb);
	cinfo.image_height = fb_height(fb);
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = fb_width(fb) * 2;
	JSAMPLE* image_buffer = malloc(3 * fb_width(fb));

	while (cinfo.next_scanline < cinfo.image_height) 
	{
		int i = 0;
		int offset = 0;
		unsigned char* line = fb->bits + cinfo.next_scanline * fb_width(fb) * fb_bpp(fb);

		for(i = 0; i < fb_width(fb); i++, offset += 3, line += fb_bpp(fb))
		{
			fb->unpack(fb, line, image_buffer+offset, image_buffer + offset + 1, image_buffer + offset + 2);
		}

		row_pointer[0] = image_buffer;
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(outfile);

	jpeg_destroy_compress(&cinfo);

	return 0;
}

//Ref: http://blog.chinaunix.net/space.php?uid=15059847&do=blog&cuid=2040565
static int snap2png(const char * filename, int quality, FBInfo* fb)
{
	FILE *outfile;
	if ((outfile = fopen(filename, "wb+")) == NULL)
	{
		fprintf(stderr, "can't open %s\n", filename);
		return -1;
	}

	/* prepare the standard PNG structures */
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,0,0,0);
	
	png_infop info_ptr = png_create_info_struct(png_ptr);

	/* setjmp() must be called in every function that calls a PNG-reading libpng function */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(outfile);
		return -1;
	}

	/* initialize the png structure */
	png_init_io(png_ptr, outfile);

	//
	int width = 0;
	int height = 0;
	int bit_depth = 8;
	int color_type = PNG_COLOR_TYPE_RGB;
	int interlace = 0;
	width = fb_width(fb);
	height = fb_height(fb);

	png_set_IHDR (png_ptr, info_ptr, width, height, bit_depth, color_type,
					(!interlace) ? PNG_INTERLACE_NONE : PNG_INTERLACE_ADAM7,
					PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	/* write the file header information */
	png_write_info(png_ptr, info_ptr);

	png_bytep row_pointers[height];
	png_byte* image_buffer = malloc(3 * width);

	int i = 0;
	int j = 0;
	unsigned char* line = NULL;
	for( ; i < height; i++ )
	{
		line = (char*)fb->bits + i * width * fb_bpp(fb);
		for(j = 0; j < width; j++, line += fb_bpp(fb))
		{
			int offset = j * 3;
			fb->unpack(fb, line, image_buffer+offset, image_buffer+offset+1, image_buffer+offset+2);
		}
		row_pointers[i] = image_buffer;
		png_write_rows(png_ptr, &row_pointers[i], 1);
	}
	
	png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(outfile);

	return 0;

}

int gsnap(int argc, char* argv[])
{
	FBInfo fb;
	const char* filename   = NULL;
	const char* fbfilename = NULL;

	if(argc != 3)
	{
		printf("\nUsage: %s [jpeg|png file] [framebuffer dev]\n", argv[0]);
		printf("Example: %s fb.jpg /dev/fb0\n", argv[0]);
		printf("-----------------------------------------\n");
		printf("Powered by broncho(www.broncho.cn)\n\n");

		return 0;
	}

	filename   = argv[1];
	fbfilename = argv[2];

	memset(&fb, 0x00, sizeof(fb));
	if (fb_open(&fb, fbfilename) == 0)
	{
		if(strstr(filename, ".png") != NULL)
		{
			snap2png(filename, 100, &fb);
		}
		else
		{
			snap2jpg(filename, 100, &fb);
		}
		fb_close(&fb);
	}

	return 0;
}

