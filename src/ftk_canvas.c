/*
 * File: ftk_canvas.c    
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
 * 2010-3-16 woodysu<yapo_su@hotmail.com> modified. 
 */

#include "ftk_log.h"
#include "ftk_util.h"
#include "ftk_bitmap.h"
#include "ftk_canvas.h"

Ret ftk_canvas_reset_gc(FtkCanvas* thiz, FtkGc* gc)
{
	return_val_if_fail(thiz != NULL && gc != NULL, RET_FAIL);

	ftk_gc_reset(&thiz->gc);
	
	ftk_gc_copy(&thiz->gc, gc);

	return ftk_canvas_sync_gc(thiz);
}

Ret ftk_canvas_set_gc(FtkCanvas* thiz, FtkGc* gc)
{
	return_val_if_fail(thiz != NULL && gc != NULL, RET_FAIL);

	ftk_gc_copy(&thiz->gc, gc);

	return ftk_canvas_sync_gc(thiz);
}

FtkGc* ftk_canvas_get_gc(FtkCanvas* thiz)
{
	return_val_if_fail(thiz != NULL, NULL);

	return &thiz->gc;
}

Ret    ftk_canvas_set_clip_rect(FtkCanvas* thiz, FtkRect* rect)
{
	if(rect != NULL)
	{
		FtkRegion region;
		region.rect = *rect;
		region.next = NULL;

		return ftk_canvas_set_clip(thiz, &region);
	}
	else
	{
		return ftk_canvas_set_clip(thiz, NULL);
	}
}

Ret    ftk_canvas_set_clip_region(FtkCanvas* thiz, FtkRegion* region)
{
	return ftk_canvas_set_clip(thiz, region);
}

Ret ftk_canvas_draw_vline(FtkCanvas* thiz, int x, int y, int h)
{
	return ftk_canvas_draw_line(thiz, x, y, x, y + h);
}

Ret ftk_canvas_draw_hline(FtkCanvas* thiz, int x, int y, int w)
{
	return ftk_canvas_draw_line(thiz, x, y, x + w, y);
}

Ret ftk_canvas_draw_bitmap_simple(FtkCanvas* thiz, FtkBitmap* b, int x, int y, int w, int h, int ox, int oy)
{
	FtkRect src_r;
	FtkRect dst_r;

	src_r.x = x;
	src_r.y = y;
	dst_r.x = ox;
	dst_r.y = oy;
	src_r.width = dst_r.width = w;
	src_r.height = dst_r.height = h;

	return ftk_canvas_draw_bitmap(thiz, b, &src_r, &dst_r, 0xff);
}

static Ret ftk_canvas_fill_background_four_corner(FtkCanvas* thiz, int x, int y, 
	int w, int h, FtkBitmap* bitmap)
{
	int i = 0;
	int ox = 0;
	int oy = 0;
	int ow = 0;
	int oh = 0;
	FtkColor fg  = thiz->gc.fg;
	int bw = ftk_bitmap_width(bitmap);
	int bh = ftk_bitmap_height(bitmap);
	int tile_w = FTK_MIN(bw, w) >> 1;
	int tile_h = FTK_MIN(bh, h) >> 1;
	
	if ( bw == w && bh == h )
	{
		ftk_canvas_draw_bitmap_simple(thiz, bitmap, 0, 0, w, h, x, y);
	}
	else
	{
		ftk_canvas_draw_bitmap_simple(thiz, bitmap, 0, 0, tile_w, tile_h, x, y);
		ftk_canvas_draw_bitmap_simple(thiz, bitmap, bw - tile_w, 0, tile_w, tile_h, x + w - tile_w, y);
		ftk_canvas_draw_bitmap_simple(thiz, bitmap, 0, bh - tile_h, tile_w, tile_h, x, y + h - tile_h);
		ftk_canvas_draw_bitmap_simple(thiz, bitmap, bw - tile_w, bh - tile_h, tile_w, tile_h, x + w - tile_w, y + h - tile_h);
	}

	if(bw < w)
	{
		ox = x + tile_w;
		ow = w - 2 * tile_w;
		for(i = 0; i < tile_h; i++)
		{
			thiz->gc.fg = ftk_bitmap_get_pixel(bitmap, tile_w, i);
			ftk_canvas_sync_gc(thiz);
			ftk_canvas_draw_hline(thiz, ox, y + i, ow);
		}
		
		oy = y + tile_h;
		oh = h - 2 * tile_h;
		thiz->gc.fg = ftk_bitmap_get_pixel(bitmap, tile_w, i);
		ftk_canvas_sync_gc(thiz);
		ftk_canvas_draw_rect(thiz, ox, oy, ow, oh, 0, 1); 
	
		oy = y + h - tile_h;
		for(i = 0; i < tile_h; i++)
		{
			thiz->gc.fg = ftk_bitmap_get_pixel(bitmap, tile_w, (bh - tile_h) + i);
			ftk_canvas_sync_gc(thiz);
			ftk_canvas_draw_hline(thiz, ox, (oy + i), ow);
		}
	}

	if(bh < h)
	{
		oy = y + tile_h;
		oh = h - 2 * tile_h;
		for(i = 0; i < tile_w; i++)
		{
			thiz->gc.fg = ftk_bitmap_get_pixel(bitmap, i, tile_h);
			ftk_canvas_sync_gc(thiz);
			ftk_canvas_draw_vline(thiz, x + i, oy, oh);
		}
		
		ox = x + w - tile_w;
		for(i = 0; i < tile_w; i++)
		{
			thiz->gc.fg = ftk_bitmap_get_pixel(bitmap, bw - tile_w + i, tile_h);
			ftk_canvas_sync_gc(thiz);
			ftk_canvas_draw_vline(thiz, ox + i, oy, oh);
		}
	}
	thiz->gc.fg = fg;
	ftk_canvas_sync_gc(thiz);

	return RET_OK;
}

static Ret ftk_canvas_fill_background_normal(FtkCanvas* thiz, int x, int y, 
	int w, int h, FtkBitmap* bitmap)
{
	int bw = ftk_bitmap_width(bitmap);
	int bh = ftk_bitmap_height(bitmap);

	w = FTK_MIN(bw, w);
	h = FTK_MIN(bh, h);

	return ftk_canvas_draw_bitmap_simple(thiz, bitmap, 0, 0, w, h, x, y);
}

static Ret ftk_canvas_fill_background_tile(FtkCanvas* thiz, int x, int y, 
	int w, int h, FtkBitmap* bitmap)
{
	int dx = 0;
	int dy = 0;
	int bw = ftk_bitmap_width(bitmap);
	int bh = ftk_bitmap_height(bitmap);

	if(bw > w && bh > h)
	{
		return ftk_canvas_fill_background_normal(thiz, x, y, w, h, bitmap);
	}

	for(dy = 0; dy < h; dy += bh)
	{
		for(dx = 0; dx < w; dx += bw)
		{
			int draw_w = (dx + bw) < w ? bw : w - dx;
			int draw_h = (dy + bh) < h ? bh : h - dy;

			ftk_canvas_draw_bitmap_simple(thiz, bitmap, 0, 0, draw_w, draw_h, x + dx, y + dy);
		}
	}
	
	return RET_OK;
}

static Ret ftk_canvas_fill_background_center(FtkCanvas* thiz, int x, int y, int w, int h, FtkBitmap* bitmap)
{
	int bw = ftk_bitmap_width(bitmap);
	int bh = ftk_bitmap_height(bitmap);
	int ox = bw < w ? x + (w - bw)/2 : x;
	int oy = bh < h ? y + (h - bh)/2 : y;
	int bx = bw < w ? 0 : (bw - w)/2;
	int by = bh < h ? 0 : (bh - h)/2;
	
	w = FTK_MIN(bw, w);
	h = FTK_MIN(bh, h);

	return ftk_canvas_draw_bitmap_simple(thiz, bitmap, bx, by, w, h, ox, oy);
}

Ret ftk_canvas_draw_bg_image(FtkCanvas* thiz, FtkBitmap* bitmap, FtkBgStyle style, 
	int x, int y, int w, int h)
{
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL && bitmap != NULL, ret);

	switch(style)
	{
		case FTK_BG_TILE: 
			ret = ftk_canvas_fill_background_tile(thiz, x, y, w, h, bitmap);break;
		case FTK_BG_CENTER: 
			ret = ftk_canvas_fill_background_center(thiz, x, y, w, h, bitmap);break;
		case FTK_BG_FOUR_CORNER:
		{
			int bw = ftk_bitmap_width(bitmap);
			int bh = ftk_bitmap_height(bitmap);
			if(w <= bw && h <= bh)
			{
				ret = ftk_canvas_fill_background_center(thiz, x, y, w, h, bitmap);break;
			}
			else
			{
				ret = ftk_canvas_fill_background_four_corner(thiz, x, y, w, h, bitmap);
			}
			break;
		}
		default:
			ret = ftk_canvas_fill_background_normal(thiz, x, y, w, h, bitmap);break;
	}

	return ret;
}

Ret ftk_canvas_show(FtkCanvas* thiz, FtkDisplay* display, FtkRect* rect, int ox, int oy)
{
	FtkBitmap* bitmap = NULL;

	ftk_canvas_lock_buffer(thiz, &bitmap);
	ftk_display_update_and_notify(display, bitmap, rect, ox, oy);
	ftk_canvas_unlock_buffer(thiz);

	return RET_OK;
}

const char* ftk_canvas_calc_str_visible_range(FtkCanvas* thiz, 
	const char* start, int vstart, int vend, int width, int* ret_extent)
{
	int extent = 0;
	int line_extent = 0;
	unsigned short unicode = 0;
	const char* iter = NULL;
	const char* prev_iter = NULL;

	if(vstart >= 0)
	{
		iter = start + vstart;
		prev_iter = iter;
		while(width > 0)
		{
			prev_iter = iter;
			unicode = utf8_get_char(iter, &iter);
			if(unicode == '\r') continue;
			if(unicode == 0 || unicode == 0xffff)
			{
				break;
			}
			else if( unicode == '\n') 
			{	
				prev_iter = iter;
				break;
			}

			extent = ftk_canvas_get_char_extent(thiz, unicode);
			if(extent > width) break;
			width -= extent;
			line_extent += extent;
			prev_iter = iter;
		}
		if(ret_extent != NULL) 
			*ret_extent = line_extent;
	
		return prev_iter;
	}
	else if(vend > 0)
	{
		iter = start + vend;
		prev_iter = iter;
		while(width > 0 && iter >= start)
		{
			prev_iter = iter;
			if(iter <= start) break;
			unicode = utf8_get_prev_char(iter, &iter);
			if(unicode == '\r') continue;
			if(unicode == 0 || unicode == 0xffff || unicode == '\n')
			{
			//	prev_iter = iter;
				break;
			}
			extent = ftk_canvas_get_char_extent(thiz, unicode);
			if(extent > width) break;
			width -= extent;
			line_extent += extent;
		}

		if(ret_extent != NULL) 
			*ret_extent = line_extent;

		return prev_iter;
	}
		
	return start;
}
