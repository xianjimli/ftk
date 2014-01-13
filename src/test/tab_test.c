/*
 * File: tab_test.c    
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
 * 2010-09-23 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#include "ftk.h"
#include "ftk_tab.h"

int main(int argc, char* argv[])
{
	int i = 0;
	FtkColor bg = {0};
	char text[32] = {0};
	ftk_init(argc, argv);
	FtkWidget* page = NULL;
	FtkWidget* tab = ftk_tab_create(NULL, 0, 0, 100, 100);

	FtkBitmap* bitmap = ftk_bitmap_create(32, 32, bg);
	assert(ftk_tab_get_page_count(tab) == 0);

	for(i = 0; i < 10; i++)
	{
		snprintf(text, sizeof(text), "text%d", i);
		assert(ftk_tab_get_page_count(tab) == i);
		assert((page = ftk_tab_add_page(tab, text, bitmap)) != NULL);
		assert(page == ftk_tab_get_page(tab, i));
		assert(strcmp(ftk_tab_get_page_text(tab, i), text) == 0);
		assert(ftk_tab_get_page_icon(tab, i) == bitmap);

		assert(ftk_tab_set_page_text(tab, i, text) == RET_OK);
		assert(ftk_tab_set_page_icon(tab, i, bitmap) == RET_OK);
		assert(strcmp(ftk_tab_get_page_text(tab, i), text) == 0);
		assert(ftk_tab_get_page_icon(tab, i) == bitmap);
		assert(ftk_tab_set_active_page(tab, i) == RET_OK);
	}

	ftk_widget_unref(tab);

	return 0;
}
