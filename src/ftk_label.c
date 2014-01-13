/*
 * File: ftk_label.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: label control.  
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

#include "ftk_globals.h"
#include "ftk_label.h"

static Ret ftk_label_on_event(FtkWidget* thiz, FtkEvent* event)
{
	return RET_OK;
}

static Ret ftk_label_on_paint(FtkWidget* thiz)
{
	int i = 0;
	int rows = 0;
	FtkAlignment alignment = (FtkAlignment)(int)(thiz->priv_subclass[0]);
	FTK_BEGIN_PAINT(x, y, width, height, canvas);

	if(ftk_widget_get_text(thiz) != NULL)
	{
		FtkTextLine line = {0};
		const char* text = ftk_widget_get_text(thiz);
		FtkTextLayout* text_layout = ftk_default_text_layout();

		x += FTK_LABEL_LEFT_MARGIN;
		width -= FTK_LABEL_LEFT_MARGIN * 2;
		ftk_canvas_reset_gc(canvas, ftk_widget_get_gc(thiz)); 
		rows = height / (ftk_widget_get_font_size(thiz) + FTK_LABEL_TOP_MARGIN);

		if(rows == 0) 
		{
			rows = 1;
			ftk_logi("%s: height is too small.\n", __func__);
		}

		ftk_text_layout_init(text_layout, text, -1, canvas, width); 
		ftk_text_layout_set_wrap_mode(text_layout, ftk_widget_get_wrap_mode(thiz));
		for(i = 0; i < rows; i++)
		{
			int xoffset = x;
			if(ftk_text_layout_get_visual_line(text_layout, &line) != RET_OK) break;
			y += ftk_widget_get_font_size(thiz) + FTK_LABEL_TOP_MARGIN;
			
			if(alignment == FTK_ALIGN_CENTER)
			{
				xoffset = x + ((width - line.extent)>>1);
			}
			else if(alignment == FTK_ALIGN_RIGHT)
			{
				xoffset = x + width - line.extent;
			}
			else
			{
				xoffset = x + line.xoffset;
			}

			ftk_canvas_draw_string(canvas, xoffset, y, line.text, line.len, 0);
		}
	}

	FTK_END_PAINT();
}

static void ftk_label_destroy(FtkWidget* thiz)
{
	return;
}

FtkWidget* ftk_label_create(FtkWidget* parent, int x, int y, int width, int height)
{
	FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	return_val_if_fail(thiz != NULL, NULL);

	thiz->on_event = ftk_label_on_event;
	thiz->on_paint = ftk_label_on_paint;
	thiz->destroy  = ftk_label_destroy;

	ftk_widget_init(thiz, FTK_LABEL, 0, x, y, width, height, FTK_ATTR_INSENSITIVE|FTK_ATTR_TRANSPARENT);
	ftk_widget_append_child(parent, thiz);
	ftk_widget_set_wrap_mode(thiz, FTK_WRAP_WORD);

	return thiz;
}

Ret ftk_label_set_alignment(FtkWidget* thiz, FtkAlignment alignment)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	thiz->priv_subclass[0] = (void*)alignment;

	return RET_OK;
}
