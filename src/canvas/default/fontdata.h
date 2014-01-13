/*
 * File: fontdata.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   functions to operate font data. 
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
 * 2009-09-11 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifdef WITHOUT_FTK
#include "typedef.h"
#define FTK_BEGIN_DECLS 
#define FTK_END_DECLS 
#else
#include "ftk_typedef.h"
#endif

#ifndef FONT_DATA_H
#define FONT_DATA_H

FTK_BEGIN_DECLS

typedef enum _Encoding
{
	ENC_UTF16 = 0,
	ENC_ANSI  = 1
}Encoding;

typedef struct _Glyph
{
	signed char x;
	signed char y;
	unsigned char w;
	unsigned char h;
	unsigned short code;
	unsigned short unused;
	unsigned char* data;
}Glyph;

struct _FontData;
typedef struct _FontData FontData;

FontData* font_data_create(int char_nr, Encoding encoding);
FontData* font_data_load(char* data, unsigned length);
FontData* font_data_load_file(const char* file_name);
Ret font_data_add_glyph(FontData* thiz, Glyph* glyph);
Ret font_data_get_glyph(FontData* thiz, unsigned short code, Glyph* glyph);

int  font_data_get_version(FontData* thiz);
int  font_data_get_width(FontData* thiz);
int  font_data_get_height(FontData* thiz);
const char* font_data_get_author(FontData* thiz);
const char* font_data_get_family(FontData* thiz);
const char* font_data_get_style(FontData* thiz);

void font_data_set_size(FontData* thiz, int width, int height);
void font_data_set_author(FontData* thiz, const char* author);
void font_data_set_family(FontData* thiz, const char* family);
void font_data_set_style(FontData* thiz, const char* style);

#ifdef HAS_FONT_DATA_SAVE
Ret font_data_save(FontData* thiz, const char* filename);
#endif
void font_data_destroy(FontData* thiz);

FTK_END_DECLS

#endif/*FONT_DATA_H*/

