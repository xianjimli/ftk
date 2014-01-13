/*
 * File: ftk_text_layout_normal.c
 *
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: simple implementation for text layout interface.
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
#include "ftk_util.h"
#include "ftk_text_layout.h"

struct _FtkTextLayout
{
	int pos;
	int len;
	size_t width;
	FtkCanvas* canvas;
	const char* text;
	FtkWrapMode wrap_mode;
	int pos_v2l[FTK_LINE_CHAR_NR+1];
};

FtkTextLayout* ftk_text_layout_create(void)
{
	FtkTextLayout* thiz = FTK_NEW(FtkTextLayout);

	return thiz;
}

Ret ftk_text_layout_set_canvas(FtkTextLayout* thiz, FtkCanvas* canvas)
{
	return_val_if_fail(thiz != NULL && canvas != NULL, RET_FAIL);

	thiz->canvas = canvas;

	return RET_OK;
}

Ret ftk_text_layout_set_width(FtkTextLayout* thiz, size_t width)
{
	return_val_if_fail(thiz != NULL && width > 0, RET_FAIL);

	thiz->width = width;

	return RET_OK;
}

Ret ftk_text_layout_set_text(FtkTextLayout* thiz, const char* text, int len)
{
	return_val_if_fail(thiz != NULL && text != NULL, RET_FAIL);

	thiz->pos   = 0;
	thiz->text = text;
	thiz->len   = len < 0 ? strlen(text) : len;

	return RET_OK;
}

Ret ftk_text_layout_set_wrap_mode(FtkTextLayout* thiz, FtkWrapMode wrap_mode)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	thiz->wrap_mode = wrap_mode;

	return RET_OK;
}

Ret ftk_text_layout_init(FtkTextLayout* thiz, const char* text, int len, FtkCanvas* canvas, size_t width)
{
	return_val_if_fail(thiz != NULL && text != NULL && canvas != NULL && width > 0, RET_FAIL);

	thiz->pos   = 0;
	thiz->text  = text;
	thiz->canvas  = canvas;
	thiz->width = width;
	thiz->wrap_mode = FTK_WRAP_NONE;
	thiz->len   = len < 0 ? strlen(text) : len;

	return RET_OK;
}

Ret ftk_text_layout_skip_to(FtkTextLayout* thiz, int pos)
{
	return_val_if_fail(thiz != NULL && pos >= 0 && pos < thiz->len, RET_FAIL);

	thiz->pos = pos;

	return RET_OK;
}


Ret ftk_text_layout_get_visual_line(FtkTextLayout* thiz, FtkTextLine* line)
{
	int i = 0;
	int extent = 0;
	const char* end = NULL;
	return_val_if_fail(thiz != NULL && line != NULL, RET_FAIL);

	if(thiz->pos >= thiz->len)
	{
		return RET_FAIL;
	}

	line->pos_v2l = thiz->pos_v2l;
	line->attr = FTK_TEXT_ATTR_NORMAL;
	line->text = thiz->text + thiz->pos;
	end = ftk_canvas_calc_str_visible_range(thiz->canvas, thiz->text, thiz->pos, -1, thiz->width, &extent);

	if(thiz->wrap_mode == FTK_WRAP_WORD)
	{
		const char* start = thiz->text + thiz->pos;
		const char* new_end = ftk_line_break(start, end);

		if(end != new_end && new_end > (start + 2))
		{
			end = new_end;
			extent = ftk_canvas_get_str_extent(thiz->canvas, start, end - start);
		}
	}

	line->len = end - line->text;
	line->xoffset = 0;
	line->extent = extent;

	for(i = 0; i < line->len && i < FTK_LINE_CHAR_NR; i++)
	{
		line->pos_v2l[i] = i + thiz->pos;
	}
	thiz->pos += line->len;

	for(i = line->len - 1; i >= 0; i--)
	{
		if(line->text[i] == '\r' || line->text[i] == '\n')
			line->len--;
	}

	return RET_OK;
}

void ftk_text_layout_destroy(FtkTextLayout* thiz)
{
	if(thiz != NULL)
	{
		FTK_FREE(thiz);
	}

	return;
}


