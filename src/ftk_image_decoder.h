/*
 * File: ftk_image_decoder.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   bitmap decoder interface.
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

#ifndef FTK_IMAGE_DECODER_H
#define FTK_IMAGE_DECODER_H

#include "ftk_bitmap.h"

FTK_BEGIN_DECLS

struct _FtkImageDecoder;
typedef struct _FtkImageDecoder FtkImageDecoder;

typedef Ret       (*FtkImageDecoderMatch)(FtkImageDecoder* thiz, const char* filename);
typedef FtkBitmap* (*FtkImageDecoderDecode)(FtkImageDecoder* thiz, const char* filename);
typedef void      (*FtkImageDecoderDestroy)(FtkImageDecoder* thiz);

struct _FtkImageDecoder
{
	FtkImageDecoderMatch   match;
	FtkImageDecoderDecode  decode;
	FtkImageDecoderDestroy destroy;

	char priv[ZERO_LEN_ARRAY];
};

static inline Ret ftk_image_decoder_match(FtkImageDecoder* thiz, const char* filename)
{
	return_val_if_fail(thiz != NULL && thiz->match != NULL, RET_FAIL);

	return thiz->match(thiz, filename);
}

static inline FtkBitmap* ftk_image_decoder_decode(FtkImageDecoder* thiz, const char* filename)
{
	return_val_if_fail(thiz != NULL && thiz->decode != NULL, NULL);

	return thiz->decode(thiz, filename);
}

static inline void ftk_image_decoder_destroy(FtkImageDecoder* thiz)
{
	if(thiz != NULL && thiz->destroy != NULL)
	{
		thiz->destroy(thiz);
	}

	return;
}

FTK_END_DECLS

#endif/*FTK_IMAGE_DECODER_H*/

