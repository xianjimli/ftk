/*
 * File: ftk_image_jpeg_decoder.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   
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
#include <setjmp.h>
#include "ftk_mmap.h"
#include "ftk_log.h"
#include <jpeglib.h>
#include "ftk_image_jpeg_decoder.h"

#ifdef RT_THREAD
#define INPUT_BUFFER_SIZE	4096
typedef struct {
	struct jpeg_source_mgr pub;
	int fd;
	rt_uint8_t buffer[INPUT_BUFFER_SIZE];
} ftk_jpeg_source_mgr;

/*
* Initialize source --- called by jpeg_read_header
* before any data is actually read.
*/
static void ftk_jpeg_init_source (j_decompress_ptr cinfo)
{
	/* We don't actually need to do anything */
	return;
}

/*
* Fill the input buffer --- called whenever buffer is emptied.
*/
static boolean ftk_jpeg_fill_input_buffer (j_decompress_ptr cinfo)
{
	ftk_jpeg_source_mgr * src = (ftk_jpeg_source_mgr *) cinfo->src;
	int nbytes;

	nbytes = read(src->fd, src->buffer, INPUT_BUFFER_SIZE);
	if (nbytes <= 0)
	{
		/* Insert a fake EOI marker */
		src->buffer[0] = (rt_uint8_t) 0xFF;
		src->buffer[1] = (rt_uint8_t) JPEG_EOI;
		nbytes = 2;
	}

	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;

	return TRUE;
}

/*
* Skip data --- used to skip over a potentially large amount of
* uninteresting data (such as an APPn marker).
*
* Writers of suspendable-input applications must note that skip_input_data
* is not granted the right to give a suspension return.  If the skip extends
* beyond the data currently in the buffer, the buffer can be marked empty so
* that the next read will cause a fill_input_buffer call that can suspend.
* Arranging for additional bytes to be discarded before reloading the input
* buffer is the application writer's problem.
*/
static void ftk_jpeg_skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
	ftk_jpeg_source_mgr * src = (ftk_jpeg_source_mgr *) cinfo->src;

	/* Just a dumb implementation for now.	Could use fseek() except
	* it doesn't work on pipes.  Not clear that being smart is worth
	* any trouble anyway --- large skips are infrequent.
	*/
	if (num_bytes > 0)
	{
		while (num_bytes > (long) src->pub.bytes_in_buffer)
		{
			num_bytes -= (long) src->pub.bytes_in_buffer;
			(void) src->pub.fill_input_buffer(cinfo);
			/* note we assume that fill_input_buffer will never
			* return FALSE, so suspension need not be handled.
			*/
		}
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}

/*
* Terminate source --- called by jpeg_finish_decompress
* after all data has been read.
*/
static void ftk_jpeg_term_source (j_decompress_ptr cinfo)
{
	/* We don't actually need to do anything */
	return;
}

/*
* Prepare for input from a stdio stream.
* The caller must have already opened the stream, and is responsible
* for closing it after finishing decompression.
*/
static void ftk_jpeg_filerw_src_init(j_decompress_ptr cinfo, int fd)
{
	ftk_jpeg_source_mgr *src;

	/* The source object and input buffer are made permanent so that a series
	* of JPEG images can be read from the same file by calling jpeg_stdio_src
	* only before the first one.  (If we discarded the buffer at the end of
	* one image, we'd likely lose the start of the next one.)
	* This makes it unsafe to use this manager and a different source
	* manager serially with the same JPEG object.  Caveat programmer.
	*/
	if (cinfo->src == NULL) {	/* first time for this JPEG object? */
		cinfo->src = (struct jpeg_source_mgr *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			sizeof(ftk_jpeg_source_mgr));
		src = (ftk_jpeg_source_mgr *) cinfo->src;
	}

	src = (ftk_jpeg_source_mgr *) cinfo->src;
	src->pub.init_source = ftk_jpeg_init_source;
	src->pub.fill_input_buffer = ftk_jpeg_fill_input_buffer;
	src->pub.skip_input_data = ftk_jpeg_skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	src->pub.term_source = ftk_jpeg_term_source;
	src->fd = fd;
	src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL; /* until buffer loaded */
}
#endif

static Ret ftk_image_jpeg_decoder_match(FtkImageDecoder* thiz, const char* filename)
{
	unsigned char* data = NULL;
	size_t length = 0;
	char identifier[4] = {0};
	FtkMmap* m = ftk_mmap_create(filename, 0, -1);

	if(m == NULL) return RET_FAIL;

	data = (unsigned char*)ftk_mmap_data(m);
	length = ftk_mmap_length(m);

	/* check SOI */
	if (data[0] != 0xFF || data[1] != 0xD8)
		goto fail;	/* not JPEG image */
	/* check Identifier */
	memcpy(identifier, data+6, sizeof(identifier));
	if (strncmp(identifier, "JFIF", 4) != 0 && strncmp(identifier, "Exif", 4) != 0)
		goto fail;
	/* check EOI */
	if (data[length-2] == 0xFF && data[length-1] == 0xD9) 
	{
		ftk_mmap_destroy(m);
		return RET_OK;
	} 
	else
	{
		ftk_logd("%s: JPEG EOI is incorrect: 0x%02X%02X\n", __func__, data[length-2], data[length-1]);
		goto fail;	/* JPEG image has no end */
	}
fail:
	ftk_mmap_destroy(m);
	return RET_FAIL;
}

struct my_error_mgr
{
	struct jpeg_error_mgr pub;	/* "public" fields */
	jmp_buf setjmp_buffer;	/* for return to caller */
};
typedef struct my_error_mgr *my_error_ptr;
static void put_scanline_to_bitmap (FtkBitmap* bitmap, char *scanline, int y, int width)
{
	int i = 0;
	FtkColor* dst = NULL;
	char *pixels = scanline;

	dst = ftk_bitmap_lock(bitmap) + y * width;

	while (i < width)
	{
		dst->r = pixels[0];
		dst->g = pixels[1];
		dst->b = pixels[2];

		i++;
		dst++;
		pixels += 3;
	}

	return;
}

METHODDEF (void)
my_error_exit (j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	(*cinfo->err->output_message) (cinfo);

	longjmp (myerr->setjmp_buffer, 1);
}
static FtkBitmap* load_jpeg (const char *filename)
{
	int i = 0;
#ifndef RT_THREAD
	FILE *infile;
#else
	int fd;
#endif
	JSAMPARRAY buffer;	
	int row_stride;	
	FtkColor bg = {0};
	FtkBitmap* bitmap = NULL;
	struct my_error_mgr jerr;
	struct jpeg_decompress_struct cinfo;
	
	bg.a = 0xff;

#ifndef RT_THREAD
	if ((infile = fopen (filename, "rb")) == NULL)
#else
	if ((fd = open (filename, O_RDONLY, 0)) < 0)
#endif
	{
		ftk_logd("can't open %s\n", filename);
		return 0;
	}

	cinfo.err = jpeg_std_error (&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	
	if (setjmp (jerr.setjmp_buffer))
	{
		jpeg_destroy_decompress (&cinfo);
#ifndef RT_THREAD
		fclose (infile);
#else
		close(fd);
#endif
		return NULL;
	}
	jpeg_create_decompress (&cinfo);

#ifndef RT_THREAD
	jpeg_stdio_src (&cinfo, infile);
#else
	ftk_jpeg_filerw_src_init(&cinfo, fd);
#endif

	(void) jpeg_read_header (&cinfo, TRUE);

	(void) jpeg_start_decompress (&cinfo);
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) & cinfo, JPOOL_IMAGE, row_stride, 1);
	
	if((bitmap = ftk_bitmap_create(cinfo.output_width, cinfo.output_height, bg)) == NULL)
	{
		jpeg_destroy_decompress (&cinfo);
#ifndef RT_THREAD
		fclose (infile);
#else
		close(fd);
#endif

		return NULL;
	}

	while (cinfo.output_scanline < cinfo.output_height)
	{
		(void) jpeg_read_scanlines (&cinfo, buffer, 1);

		put_scanline_to_bitmap (bitmap, (char*)buffer[0], i, cinfo.output_width);
		i++;
	}

	(void) jpeg_finish_decompress (&cinfo);
	jpeg_destroy_decompress (&cinfo);

#ifndef RT_THREAD
	fclose (infile);
#else
	close(fd);
#endif

	return bitmap;
}
static FtkBitmap* ftk_image_jpeg_decoder_decode(FtkImageDecoder* thiz, const char* filename)
{
	return_val_if_fail(ftk_image_jpeg_decoder_match(thiz, filename) == RET_OK, NULL);

	return load_jpeg(filename);
}

static void ftk_image_jpeg_decoder_destroy(FtkImageDecoder* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(thiz));
	}

	return;
}

FtkImageDecoder* ftk_image_jpeg_decoder_create(void)
{
	FtkImageDecoder* thiz = (FtkImageDecoder*)FTK_ZALLOC(sizeof(FtkImageDecoder));

	if(thiz != NULL)
	{
		thiz->match   = ftk_image_jpeg_decoder_match;
		thiz->decode  = ftk_image_jpeg_decoder_decode;
		thiz->destroy = ftk_image_jpeg_decoder_destroy;
	}

	return thiz;
}

#ifdef FTK_IMAGE_DECODER_JPEG_TEST
int main(int argc, char* argv[])
{
	FtkBitmap* bitmap = NULL;
	const char* filename = argv[1];
	FtkImageDecoder* thiz = ftk_image_jpeg_decoder_create();
	
	assert(ftk_image_decoder_match(thiz, filename) == RET_OK);
	assert((bitmap = ftk_image_decoder_decode(thiz, filename)) != NULL);
	ftk_logd("w=%d h=%d\n", ftk_bitmap_width(bitmap), ftk_bitmap_height(bitmap));
	ftk_bitmap_unref(bitmap);
	ftk_image_decoder_destroy(thiz);

	return 0;
}
#endif/*FTK_IMAGE_JPEG_TEST*/
