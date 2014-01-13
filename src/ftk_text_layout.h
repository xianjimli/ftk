/*
 * File: ftk_text_layout.h   
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: interface for text layout(bidi,line break,shape join).
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
 * 2010-07-18 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#ifndef FTK_TEXT_LAYOUT_H
#define FTK_TEXT_LAYOUT_H

#include "ftk_canvas.h"

FTK_BEGIN_DECLS

#define FTK_LINE_CHAR_NR 255

struct _FtkTextLayout;
typedef struct _FtkTextLayout FtkTextLayout;

typedef enum _FtkTextAttr
{
	FTK_TEXT_ATTR_NORMAL = 0,
	FTK_TEXT_ATTR_SHAPED = 1,
	FTK_TEXT_ATTR_EXTENDED = 2,
	FTK_TEXT_ATTR_STANDARD = 4,
	FTK_TEXT_ATTR_RTOL = 8
}FtkTextAttr;

typedef struct _FtkTextLine
{
	int len;
	int extent;
	int xoffset;
	int* pos_v2l;
	const char* text;
	FtkTextAttr attr;
}FtkTextLine;

FtkTextLayout* ftk_text_layout_create(void);
Ret ftk_text_layout_set_canvas(FtkTextLayout* thiz, FtkCanvas* canvas);
Ret ftk_text_layout_set_width(FtkTextLayout* thiz, size_t width);
Ret ftk_text_layout_set_text(FtkTextLayout* thiz, const char* text, int len);
Ret ftk_text_layout_set_wrap_mode(FtkTextLayout* thiz, FtkWrapMode wrap_mode);
Ret ftk_text_layout_init(FtkTextLayout* thiz, const char* text, int len, FtkCanvas* canvas, size_t width);

Ret ftk_text_layout_skip_to(FtkTextLayout* thiz, int pos);
Ret ftk_text_layout_get_visual_line(FtkTextLayout* thiz, FtkTextLine* line);
void ftk_text_layout_destroy(FtkTextLayout* thiz);

FTK_END_DECLS

#endif/*FTK_TEXT_LAYOUT_H*/

