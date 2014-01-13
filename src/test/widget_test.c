/*
 * File: widget_test.c    
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
 *
 */
#include "ftk.h"

int main(int argc, char* argv[])
{
	FtkColor color = {0};
	ftk_init(argc, argv);
	FtkWidget* thiz = FTK_ZALLOC(sizeof(FtkWidget));
	FtkCanvas* canvas = ftk_canvas_create(240, 320, &color);
	ftk_widget_init(thiz, 1, 100, 0, 0, 0, 0, 0);
	ftk_widget_set_canvas(thiz, canvas);
	ftk_widget_resize(thiz, 240, 320);
	ftk_widget_move(thiz, 200, 100);
	assert(ftk_widget_left(thiz) == 200);
	assert(ftk_widget_top(thiz) == 100);
	assert(ftk_widget_width(thiz) == 240);
	assert(ftk_widget_height(thiz) == 320);
	assert(ftk_widget_type(thiz) == 1);
	assert(ftk_widget_id(thiz) == 100);
	assert(ftk_widget_top_abs(thiz) == 100);
	assert(ftk_widget_left_abs(thiz) == 200);
	ftk_widget_set_insensitive(thiz, 1);
	assert(ftk_widget_is_insensitive(thiz));
	ftk_widget_set_insensitive(thiz, 0);
	assert(!ftk_widget_is_insensitive(thiz));
	assert(!ftk_widget_is_visible(thiz));
	assert(!ftk_widget_is_focused(thiz));
	ftk_widget_set_focused(thiz, 1);	
	assert(ftk_widget_is_focused(thiz));
	assert(ftk_widget_parent(thiz) == NULL);
	assert(ftk_widget_child(thiz) == NULL);
	assert(ftk_widget_next(thiz) == NULL);
	assert(ftk_widget_toplevel(thiz) == thiz);
	assert(ftk_widget_lookup(thiz, 100) == thiz);

	FtkWidget* child1 = calloc(1, sizeof(FtkWidget));
	FtkWidget* child2 = calloc(1, sizeof(FtkWidget));
	ftk_widget_init(child1, 1, 200, 0, 0, 0, 0, 0);
	ftk_widget_init(child2, 1, 300, 0, 0, 0, 0, 0);

	ftk_widget_move(thiz, 0, 0);
	ftk_widget_move(child1, 0, 0);
	ftk_widget_move(child2, 100, 100);
	ftk_widget_resize(thiz, 320, 480);
	ftk_widget_resize(child1, 100, 100);
	ftk_widget_resize(child2, 100, 100);

	ftk_widget_append_child(thiz, child1);
	ftk_widget_append_child(thiz, child2);
	assert(ftk_widget_next(child1) == child2);
	assert(ftk_widget_prev(child1) == NULL);
	assert(ftk_widget_next(child2) == NULL);
	assert(ftk_widget_prev(child2) == child1);
	assert(ftk_widget_child(thiz) == child1);
	assert(ftk_widget_next(child1) == child2);
	assert(ftk_widget_lookup(thiz, 200) == child1);
	assert(ftk_widget_lookup(thiz, 300) == child2);

	assert(ftk_widget_find_target(thiz, 50, 50, 0) == child1);
	assert(ftk_widget_find_target(thiz, 150, 150, 0) == child2);

	ftk_canvas_destroy(canvas);
	ftk_widget_unref(thiz);
	return 0;
}
