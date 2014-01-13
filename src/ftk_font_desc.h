/*
 * 
 * File: ftk_font_desc.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   font description.
 *
 * Copyright (c) 2009 - 2011  Li XianJing <xianjimli@hotmail.com>
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
 * 2011-03-20 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_FONT_DESC_H
#define FTK_FONT_DESC_H

#include "ftk_typedef.h"

FTK_BEGIN_DECLS

#ifndef FTK_FONT
#define FTK_FONT "any"
#endif

#define FTK_DEFAULT_FONT "size:16 bold:0 italic:0 fontname:"FTK_FONT
#define FONT_DESC_FMT "size:%d bold:%d italic:%d fontname:%s"


struct _FtkFontDesc;
typedef struct _FtkFontDesc FtkFontDesc;

FtkFontDesc* ftk_font_desc_create(const char* font_desc);

int ftk_font_desc_is_equal(FtkFontDesc* thiz, FtkFontDesc* other);
int ftk_font_desc_is_bold(FtkFontDesc* thiz);
int ftk_font_desc_is_italic(FtkFontDesc* thiz);
int ftk_font_desc_get_size(FtkFontDesc* thiz);
Ret ftk_font_desc_set_bold(FtkFontDesc* thiz, int bold);
Ret ftk_font_desc_set_italic(FtkFontDesc* thiz, int italic);
Ret ftk_font_desc_set_size(FtkFontDesc* thiz, int size);
Ret ftk_font_desc_get_string(FtkFontDesc* thiz, char* desc, size_t len);
const char *ftk_font_desc_get_fontname(FtkFontDesc* thiz);

int ftk_font_desc_ref(FtkFontDesc* thiz);
int ftk_font_desc_unref(FtkFontDesc* thiz);

FTK_END_DECLS

#endif/*FTK_FONT_DESC_H*/

