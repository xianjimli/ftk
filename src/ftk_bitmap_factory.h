/*
 * File: ftk_bitmap_factory.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   bitmap factory
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

#ifndef FTK_BITMAP_FACTORY_H
#define FTK_BITMAP_FACTORY_H

#include "ftk_image_decoder.h"

FTK_BEGIN_DECLS

struct _FtkBitmapFactory;
typedef struct _FtkBitmapFactory FtkBitmapFactory;

FtkBitmapFactory* ftk_bitmap_factory_create(void);
FtkBitmap* ftk_bitmap_factory_load(FtkBitmapFactory* thiz, const char* filename);
Ret        ftk_bitmap_factory_add_decoder(FtkBitmapFactory* thiz, FtkImageDecoder* decoder);
void       ftk_bitmap_factory_destroy(FtkBitmapFactory* thiz);

FTK_END_DECLS

#endif/*FTK_BITMAP_FACTORY_H*/

