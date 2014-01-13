/*
 * File: ftk_group_box.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   radio group. 
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
 * 2009-11-15 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_theme.h"
#include "ftk_globals.h"
#include "ftk_group_box.h"
#include "ftk_check_button.h"

typedef struct _GroupBoxPrivInfo
{
	FtkBitmap* bg;
}PrivInfo;

static Ret ftk_group_box_on_event(FtkWidget* thiz, FtkEvent* event)
{
	return RET_OK;
}

static Ret ftk_group_box_on_paint(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	FTK_BEGIN_PAINT(x, y, width, height, canvas);

	return_val_if_fail(priv->bg != NULL, RET_FAIL);

	ftk_canvas_reset_gc(canvas, ftk_widget_get_gc(thiz)); 
	ftk_canvas_draw_bg_image(canvas, priv->bg, FTK_BG_FOUR_CORNER, x, y, width, height);	

	if(ftk_widget_get_text(thiz) != NULL)
	{
		int yoffset = 2;
		int xoffset = 10;
		FtkTextLine line = {0};
		int font_height = ftk_widget_get_font_size(thiz);
		const char* text = ftk_widget_get_text(thiz);
		FtkTextLayout* text_layout = ftk_default_text_layout();

		ftk_text_layout_init(text_layout, text, -1, canvas, width - 2 * xoffset); 
		if(ftk_text_layout_get_visual_line(text_layout, &line) == RET_OK)
		{
			FtkGc gc = {0};
			gc.mask = FTK_GC_FG;
			/*fill bg with the first pixel of bg image.*/
			gc.fg = ftk_bitmap_get_pixel(priv->bg, 0, 0);
			
			ftk_canvas_set_gc(canvas, &gc);
			ftk_canvas_draw_rect(canvas, x + xoffset, y + yoffset, line.extent, font_height, 0, 1);

			ftk_canvas_reset_gc(canvas, ftk_widget_get_gc(thiz)); 
			ftk_canvas_draw_string(canvas, x + xoffset, y + yoffset + font_height, line.text, line.len, 0);
		}
	}

	FTK_END_PAINT();
}

static void ftk_group_box_destroy(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);

	if(priv->bg != NULL)
	{
		ftk_bitmap_unref(priv->bg);
	}
	FTK_ZFREE(thiz->priv_subclass[0], sizeof(PrivInfo));

	return;
}

FtkWidget* ftk_group_box_create(FtkWidget* parent, int x, int y, int width, int height)
{
	FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget) + sizeof(PrivInfo));
	return_val_if_fail(thiz != NULL, NULL);

	thiz->priv_subclass[0] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[0] != NULL)
	{
		DECL_PRIV0(thiz, priv);
		thiz->on_event = ftk_group_box_on_event;
		thiz->on_paint = ftk_group_box_on_paint;
		thiz->destroy  = ftk_group_box_destroy;

		priv->bg = ftk_theme_load_image(ftk_default_theme(), "groupbox_bg"FTK_STOCK_IMG_SUFFIX);
		ftk_widget_init(thiz, FTK_GROUP_BOX, 0, x, y, width, height, FTK_ATTR_TRANSPARENT);
		ftk_widget_append_child(parent, thiz);
	}

	return thiz;
}

Ret  ftk_group_box_set_checked(FtkWidget* thiz, FtkWidget* radio)
{
	FtkWidget* iter = NULL;
	return_val_if_fail(thiz != NULL && radio != NULL, RET_FAIL);

	iter = thiz->children;

	while(iter != NULL)
	{
		ftk_check_button_set_checked(iter, iter == radio);

		iter = iter->next;
	}

	return RET_OK;
}

