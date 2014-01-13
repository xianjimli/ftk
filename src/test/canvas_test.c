/*
 * File: canvas_test.c    
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
#include "ftk_canvas.h"

static void show_canvas(FtkDisplay* display, FtkCanvas* canvas)
{
	FtkBitmap* bitmap = NULL;
	FtkRect rect = {.x = 0, .y=0, .width=0, .height=0};
	rect.width = ftk_display_width(display);
	rect.height = ftk_display_height(display);

	ftk_canvas_lock_buffer(canvas, &bitmap);
	ftk_display_update(display, bitmap, &rect, 0, 0);
	ftk_canvas_unlock_buffer(canvas);

	return;
}
#if 1
void test_misc(FtkDisplay* display, FtkFontDesc* font)
{
	if(display != NULL)
	{
		int i = 0;
		FtkGc gc = {0};
		int extent = 0;
		FtkColor color = {0x0, 0, 0, 0x0};
		FtkRect rect = {.x = 0, .y=0, .width=0, .height=0};
		rect.width = ftk_display_width(display);
		rect.height = ftk_display_height(display);
		gc.mask = FTK_GC_FG | FTK_GC_FONT;
		gc.fg.a = 0xff;
		gc.fg.r = 0x00;
		gc.fg.g = 0x00;
		gc.fg.b = 0xff;
		gc.font = font;

		FtkCanvas* thiz = ftk_canvas_create(rect.width, rect.height, &color);
		show_canvas(display, thiz);
		for(i = 0; i < ftk_display_height(display); i++)
		{
			if(gc.fg.r < 0xff)
			{
				gc.fg.r++;
			}
			else
			{
				gc.fg.g++;
			}
			ftk_canvas_set_gc(thiz, &gc);
			ftk_canvas_draw_hline(thiz, 0, i, 320);
		}
		FtkBitmap* bitmap = ftk_bitmap_create(100, 100, color);
		ftk_canvas_draw_bitmap_simple(thiz, bitmap, 0, 0, 100, 100, 100, 100);
		ftk_canvas_draw_string(thiz, 0, 240, " Jim is a Programmer.", -1, 0);
		gc.fg.b = 0xff;
		ftk_canvas_set_gc(thiz, &gc);
		ftk_canvas_draw_string(thiz, 0, 220, "李先静是一个程序员", -1, 0);
	
		unsigned int line_mask = 0xaaaaaaaa;
		gc.line_mask = line_mask;
		gc.mask = FTK_GC_LINE_MASK;
		ftk_canvas_set_gc(thiz, &gc);
		show_canvas(display, thiz);

		extent = ftk_canvas_get_str_extent(thiz, "李先静", -1);
		printf("extent=%d\n", ftk_canvas_get_str_extent(thiz, "李先静", -1));


		ftk_bitmap_unref(bitmap);
		ftk_canvas_destroy(thiz);
	}

	sleep(3);

	return;
}

#if 1
void test_draw_point(FtkDisplay* display)
{
	int i = 0;
	FtkGc gc = {.mask = FTK_GC_FG};
	FtkPoint p = {0};
	FtkColor color = {.a = 0xff};
	int width = ftk_display_width(display);
	int height = ftk_display_height(display);
	FtkCanvas* thiz = ftk_canvas_create(width, height, &color);

	color.r = 0xff;
	color.a = 0xff;
	gc.fg = color;
	for(i = 0; i < width; i++)
	{
		p.x = i;
		p.y = 10;
		ftk_canvas_reset_gc(thiz, &gc);
		ftk_canvas_draw_pixels(thiz, &p, 1);
	}
	
	color.g = 0xff;
	color.r = 0;
	for(i = 0; i < width; i++)
	{
		color.a = 0xff - (0xff & i);
		gc.fg = color;
		p.x = i;
		p.y = 20;
		ftk_canvas_reset_gc(thiz, &gc);
		ftk_canvas_draw_pixels(thiz, &p, 1);
	}
	
	color.r = 0;
	color.g = 0;
	color.b = 0xff;
	color.a = 0xff;
	gc.fg = color;
	gc.mask |= FTK_GC_ALPHA;
	for(i = 0; i < width; i++)
	{
		gc.alpha = 0xff - (0xff & i);
		ftk_canvas_reset_gc(thiz, &gc);
		p.x = i;
		p.y = 20;
		ftk_canvas_draw_pixels(thiz, &p, 1);
	}

	show_canvas(display, thiz);
	
	ftk_canvas_destroy(thiz);

	sleep(3);

	return;
}
#endif
void test_draw_vline(FtkDisplay* display)
{
	int i = 0;
	FtkGc gc = {.mask = FTK_GC_FG};
	FtkColor color = {.a = 0xff};
	int width = ftk_display_width(display);
	int height = ftk_display_height(display);
	FtkCanvas* thiz = ftk_canvas_create(width, height, &color);

	color.r = 0xff;
	color.a = 0xff;
	gc.fg = color;
	for(i = 0; i < width; i++)
	{
		ftk_canvas_reset_gc(thiz, &gc);
		ftk_canvas_draw_vline(thiz, i, 0, 20);
	}
	
	color.g = 0xff;
	color.r = 0;
	for(i = 0; i < width; i++)
	{
		color.a = 0xff - (0xff & i);
		gc.fg = color;
		ftk_canvas_reset_gc(thiz, &gc);
		ftk_canvas_draw_vline(thiz, i, 30, 20);
	}
	
	color.r = 0;
	color.g = 0;
	color.b = 0xff;
	color.a = 0xff;
	gc.fg = color;
	gc.mask |= FTK_GC_ALPHA;
	for(i = 0; i < width; i++)
	{
		gc.alpha = 0xff - (0xff & i);
		ftk_canvas_reset_gc(thiz, &gc);
		ftk_canvas_draw_vline(thiz, i, 60, 20);
	}

	show_canvas(display, thiz);
	
	ftk_canvas_destroy(thiz);
	sleep(3);

	return;
}

void test_draw_hline(FtkDisplay* display)
{
	int i = 0;
	FtkGc gc = {.mask = FTK_GC_FG};
	FtkColor color = {.a = 0xff};
	int width = ftk_display_width(display);
	int height = ftk_display_height(display);
	FtkCanvas* thiz = ftk_canvas_create(width, height, &color);

	color.r = 0xff;
	color.a = 0xff;
	gc.fg = color;
	for(i = 0; i < height; i++)
	{
		ftk_canvas_reset_gc(thiz, &gc);
		ftk_canvas_draw_hline(thiz, 0, i, 20);
	}
	
	color.g = 0xff;
	color.r = 0;
	for(i = 0; i < height; i++)
	{
		color.a = 0xff - (0xff & i);
		gc.fg = color;
		ftk_canvas_reset_gc(thiz, &gc);
		ftk_canvas_draw_hline(thiz, 30, i, 20);
	}
	
	color.r = 0;
	color.g = 0;
	color.b = 0xff;
	color.a = 0xff;
	gc.fg = color;
	gc.mask |= FTK_GC_ALPHA;
	for(i = 0; i < height; i++)
	{
		gc.alpha = 0xff - (0xff & i);
		ftk_canvas_reset_gc(thiz, &gc);
		ftk_canvas_draw_hline(thiz, 60, i, 20);
	}

	show_canvas(display, thiz);
	
	ftk_canvas_destroy(thiz);

	sleep(3);

	return;
}

#if 1
void test_draw_line(FtkDisplay* display)
{
	int i = 0;
	FtkGc gc = {.mask = FTK_GC_FG};
	FtkRect rect = {0};
	FtkColor color = {.a = 0xff};
	int width = ftk_display_width(display);
	int height = ftk_display_height(display);
	FtkCanvas* thiz = ftk_canvas_create(width, height, &color);

	rect.width = width;
	rect.height = height;

	color.r = 0xff;
	color.a = 0xff;
	gc.fg = color;
	for(i = 0; i < height/2; i++)
	{
		ftk_canvas_reset_gc(thiz, &gc);
		ftk_canvas_draw_line(thiz, 0, i, 20, i+10);
	}
	
	color.g = 0xff;
	color.r = 0;
	for(i = 0; i < height/2; i++)
	{
		color.a = 0xff - (0xff & i);
		gc.fg = color;
		ftk_canvas_reset_gc(thiz, &gc);
		ftk_canvas_draw_line(thiz, 30, i, 50, i+10);
	}
	
	color.r = 0;
	color.g = 0;
	color.b = 0xff;
	color.a = 0xff;
	gc.fg = color;
	gc.mask |= FTK_GC_ALPHA;
	for(i = 0; i < height/2; i++)
	{
		gc.alpha = 0xff - (0xff & i);
		ftk_canvas_reset_gc(thiz, &gc);
		ftk_canvas_draw_line(thiz, 60, i, 80, i+10);
	}

	ftk_canvas_show(thiz, display, &rect, 0, 0);
	
	ftk_canvas_destroy(thiz);

	sleep(3);

	return;
}
#endif
void test_alpha(FtkDisplay* display)
{
	int i = 0;
	FtkGc gc = {.mask = FTK_GC_FG};
	FtkColor color = {.a = 0xff};
	int width = ftk_display_width(display);
	int height = ftk_display_height(display);
	FtkCanvas* thiz = ftk_canvas_create(width, height, &color);

	color.g = 0xff;
	color.r = 0;
	for(i = 0; i < 0xff; i += 4)
	{
		color.a = 0xff;
		color.g = 0;
		gc.fg = color;
		ftk_canvas_reset_gc(thiz, &gc);
		ftk_canvas_draw_rect(thiz, 0, 0, width, height, 0, 1);
		
		color.a = 0xff - i;
		color.g = 0xff;
		gc.fg = color;
		ftk_canvas_reset_gc(thiz, &gc);
		ftk_canvas_draw_rect(thiz, 0, 0, width, height, 0, 1);
		show_canvas(display, thiz);
		usleep(200000);
	}
	
	ftk_canvas_destroy(thiz);

	sleep(3);

	return;
}

void test_put_get_pixel(FtkDisplay* display)
{
	int i = 0;
	int j = 0;
	FtkColor color = {.a=0xff, .r=0xef, .g=0xdf, .b=0xcf};
	int width = ftk_display_width(display);
	int height = ftk_display_height(display);
	FtkCanvas* thiz = ftk_canvas_create(width, height, &color);

	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			FtkColor c = {0};
			FtkColor* colorp = NULL;
			colorp = &c;
			assert(colorp->r == color.r);
			assert(colorp->g == color.g);
			assert(colorp->b == color.b);
			assert(colorp->a == color.a);
		}
	}

	ftk_canvas_destroy(thiz);

	return;
}

void test_font(FtkDisplay* display, FtkFontDesc* font)
{
	int extent2 = 0;
	FtkGc gc = {.mask = FTK_GC_FONT};
	FtkColor color = {.a=0xff, .r=0xef, .g=0xdf, .b=0xcf};
	int width = ftk_display_width(display);
	int height = ftk_display_height(display);
	FtkCanvas* thiz = ftk_canvas_create(width, height, &color);
	const char* str = "隐式声明与内建函数";
	const char* other_side = NULL;
	
	gc.font = font;
	ftk_canvas_set_gc(thiz, &gc);
	
	other_side = ftk_canvas_calc_str_visible_range(thiz, str, 0, -1, 60, NULL);
	assert(strcmp(other_side, "明与内建函数") == 0);

	other_side = ftk_canvas_calc_str_visible_range(thiz, str, other_side-str, -1, 60, NULL);
	assert(strcmp(other_side, "建函数") == 0);
	
	other_side = ftk_canvas_calc_str_visible_range(thiz, str, other_side-str, -1, 60, NULL);
	assert(strcmp(other_side, "") == 0);

	other_side = ftk_canvas_calc_str_visible_range(thiz, str, -1, other_side-str, 60, NULL);
	assert(strcmp(other_side, "建函数") == 0);
	
	other_side = ftk_canvas_calc_str_visible_range(thiz, str, -1, other_side-str, 60, NULL);
	assert(strcmp(other_side, "明与内建函数") == 0);
	
	other_side = ftk_canvas_calc_str_visible_range(thiz, str, -1, other_side-str, 60, NULL);
	assert(strcmp(other_side, str) == 0);
	
	other_side = ftk_canvas_calc_str_visible_range(thiz, str, -1, other_side-str, 60, NULL);
	assert(strcmp(other_side, str) == 0);

	printf("other_side = %s\n", other_side);
	

	str = "Single line editor, that means you can input a one line only.";
	
	extent2 = ftk_canvas_get_str_extent(thiz, str, -1);

	ftk_canvas_destroy(thiz);
	sleep(3);

	return;
}

static void test_fill_bg(FtkDisplay* display)
{
	FtkColor color = {.a=0xff, .r=0xef, .g=0xdf, .b=0xcf};
	int width = ftk_display_width(display);
	int height = ftk_display_height(display);
	FtkCanvas* thiz = ftk_canvas_create(width, height, &color);
	FtkBitmap* bitmap = ftk_theme_load_image(ftk_default_theme(), "btn_default_pressed.9.png");
	ftk_canvas_draw_bg_image(thiz, bitmap, FTK_BG_FOUR_CORNER, 10, 10, 100, 60);
	ftk_canvas_draw_bg_image(thiz, bitmap, FTK_BG_FOUR_CORNER, 120, 10, 40, 60);
	
	ftk_canvas_draw_bg_image(thiz, bitmap, FTK_BG_FOUR_CORNER, 10, 80, 20, 20);
	ftk_canvas_draw_bg_image(thiz, bitmap, FTK_BG_FOUR_CORNER, 30, 80, 40, 20);
	ftk_canvas_draw_bg_image(thiz, bitmap, FTK_BG_FOUR_CORNER, 80, 80, 60, 20);
	show_canvas(display, thiz);
	ftk_canvas_destroy(thiz);

	sleep(3);
	
	return;
}

static void test_draw_rect(FtkDisplay* display)
{
	int i = 0;
	FtkColor color = {.a = 0xff};
	int width = ftk_display_width(display);
	int height = ftk_display_height(display);
	FtkGc gc = {.mask = FTK_GC_FG};
	FtkCanvas* thiz = ftk_canvas_create(width, height, &color);
	gc.fg.a = 0xff;

	gc.fg.r = 0xff;
	for(i = 0; i < width/8; i++)
	{
		gc.fg.r -= 0x10;
		ftk_canvas_set_gc(thiz, &gc);
		ftk_canvas_draw_rect(thiz, width * i/8, 0, width/8 - 1, height/8 - 1, 0, 1);
	}
	
	gc.fg.r = 0xff;
	for(i = 0; i < width/8; i++)
	{
		gc.fg.r -= 0x10;
		gc.fg.b += 0x10;
		ftk_canvas_set_gc(thiz, &gc);
		ftk_canvas_draw_rect(thiz, width * i/8, height/8, width/8 - 1, height/8 - 1, 0, 0);
	}
	
	gc.fg.r = 0xff;
	for(i = 0; i < width/8; i++)
	{
		gc.fg.r -= 0x10;
		ftk_canvas_set_gc(thiz, &gc);
		ftk_canvas_draw_rect(thiz, width * i/8, height/4, width/8 - 1, height/8 - 1, 1, 1);
	}
	
	gc.fg.r = 0xff;
	for(i = 0; i < width/8; i++)
	{
		gc.fg.r -= 0x10;
		gc.fg.b += 0x10;
		ftk_canvas_set_gc(thiz, &gc);
		ftk_canvas_draw_rect(thiz, width * i/8, 3*height/8, width/8 - 1, height/8 - 1, 1, 0);
	}
	show_canvas(display, thiz);
	ftk_canvas_destroy(thiz);

	sleep(3);

	return;
}

int main(int argc, char* argv[])
{
	ftk_init(argc, argv);

	FtkRect rect = {0};
	FtkColor bg = {.a = 0xff};
	FtkBitmap* bitmap = NULL;
	FtkFontDesc* font = ftk_default_font();
	FtkDisplay* display = ftk_default_display();

	rect.width = ftk_display_width(display);
	rect.height = ftk_display_height(display);
	
	test_draw_rect(display);
	test_alpha(display);
	test_draw_vline(display);
	bitmap = ftk_bitmap_create(ftk_display_width(display), ftk_display_height(display), bg);
	ftk_display_snap(display, &rect, bitmap);
	test_draw_hline(display);
	ftk_display_update(display, bitmap, &rect, 0, 0);
	test_fill_bg(display);
	test_font(display, font);
	test_put_get_pixel(display);
	test_draw_hline(display);
	test_draw_vline(display);
	ftk_bitmap_unref(bitmap);
	
	ftk_run();

	return 0;
}
#else
int main(int argc, char* argv[])
{
	return 0;
}
#endif
