/*
 * File: ftk_canvas_directfb.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   directfb canvas implemntation.
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
 * 2011-04-18 Li XianJing <xianjimli@hotmail.com> created
 */

#include "ftk_log.h"
#include "ftk_util.h"
#include "ftk_bitmap.h"
#include "ftk_canvas.h"
#include "ftk_globals.h"
#include "directfb_common.h"

typedef struct _CanvasDfbPrivInfo
{
	FtkBitmap* bitmap;
	IDirectFBFont* font;
	IDirectFBSurface* surface;
}PrivInfo;

static Ret ftk_canvas_directfb_sync_gc(FtkCanvas* thiz)
{
	DECL_PRIV(thiz, priv);
	FtkColor c = thiz->gc.fg;

	priv->surface->SetColor(priv->surface, c.r, c.g, c.b, c.a);
	if(thiz->gc.font != NULL)
	{
		//int size = ftk_font_desc_get_size(thiz->gc.font);
		/*TODO*/
	}

	return RET_OK;
}

static Ret ftk_canvas_directfb_set_clip(FtkCanvas* thiz, FtkRegion* clip)
{
	DECL_PRIV(thiz, priv);
	DFBRegion region = {0};

	if(clip != NULL && clip->next == NULL)
	{
		region.x1 = clip->rect.x;
		region.y1 = clip->rect.y;
		region.x2 = clip->rect.x + clip->rect.width;
		region.y2 = clip->rect.y + clip->rect.height;
	}
	else
	{
		region.x1 = 0;
		region.y1 = 0;
		region.x2 = thiz->width;
		region.y2 = thiz->height;
	}
	priv->surface->SetClip(priv->surface, &region);
	
	return RET_OK;
}

static Ret ftk_canvas_directfb_draw_pixels(FtkCanvas* thiz, FtkPoint* points, int nr)
{
	/*TODO:*/

	return RET_OK;
}

static Ret ftk_canvas_directfb_draw_line(FtkCanvas* thiz, int x1, int y1, int x2, int y2)
{
	DECL_PRIV(thiz, priv);

	priv->surface->DrawLine(priv->surface, x1, y1, x2, y2);

	return RET_OK;
}

static Ret ftk_canvas_directfb_clear_rect(FtkCanvas* thiz, int x, int y, int w, int h)
{
	DECL_PRIV(thiz, priv);
	
	priv->surface->FillRectangle(priv->surface, x, y, w, h);

	return RET_OK;
}

static Ret ftk_canvas_directfb_draw_rect(FtkCanvas* thiz, int x, int y, int w, int h, 
	int round, int fill)
{
	DECL_PRIV(thiz, priv);

	if(fill)
	{
		if(round)
		{
			/*TODO*/
			priv->surface->FillRectangle(priv->surface, x, y, w, h);
		}
		else
		{
			priv->surface->FillRectangle(priv->surface, x, y, w, h);
		}
	}
	else
	{
		if(round)
		{
			/*TODO*/
			priv->surface->DrawRectangle(priv->surface, x, y, w, h);
		}
		else
		{
			priv->surface->DrawRectangle(priv->surface, x, y, w, h);
		}
	}

	return RET_OK;
}

static Ret ftk_canvas_directfb_draw_bitmap(FtkCanvas* thiz, FtkBitmap* bitmap, 
	FtkRect* src_r, FtkRect* dst_r, int alpha)
{
	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	DFBRectangle dsrc_r;
	DFBRectangle ddst_r;
	IDirectFBSurface* src_surface = NULL;
	return_val_if_fail(bitmap != NULL && src_r != NULL && dst_r != NULL, RET_FAIL);

	dsrc_r.x = src_r->x;
	dsrc_r.y = src_r->y;
	dsrc_r.w = src_r->width;
	dsrc_r.h = src_r->height;
	
	ddst_r.x = dst_r->x;
	ddst_r.y = dst_r->y;
	ddst_r.w = dst_r->width;
	ddst_r.h = dst_r->height;

	src_surface = ftk_bitmap_get_native(bitmap);
	if(src_r->width == dst_r->width && src_r->height == dst_r->height)
	{
		priv->surface->Blit(priv->surface, src_surface, &dsrc_r, ddst_r.x, ddst_r.y);
	}
	else
	{
		priv->surface->StretchBlit(priv->surface, src_surface, &dsrc_r, &ddst_r);
	}

	return ret;
}

static Ret ftk_canvas_directfb_draw_string(FtkCanvas* thiz, int x, int y, 
	const char* str, int len, int vcenter)
{
	DECL_PRIV(thiz, priv);
	DFBSurfaceTextFlags flags = 0;
	
	if(vcenter)
	{
		y += ftk_font_desc_get_size(thiz->gc.font)/3;
	}

	priv->surface->DrawString(priv->surface, str, len, x, y, flags);

	return RET_OK;
}

static Ret fk_canvas_directfb_lock_buffer(FtkCanvas* thiz, FtkBitmap** bitmap)
{
	DECL_PRIV(thiz, priv);

	if(bitmap != NULL)
	{
		*bitmap = priv->bitmap;
	}

	return RET_OK;
}

static Ret ftk_canvas_directfb_unlock_buffer(FtkCanvas* thiz)
{
	return RET_OK;
}

static int ftk_canvas_directfb_get_str_extent(FtkCanvas* thiz, const char* str, int len)
{
	int width = 0;
	DECL_PRIV(thiz, priv);

	priv->font->GetStringWidth(priv->font, str, len, &width);

	return width;
}

static int ftk_canvas_directfb_get_char_extent(FtkCanvas* thiz, unsigned short code)
{
	char utf8[8] = {0};

	utf16_to_utf8(&code, 1, utf8, sizeof(utf8));
	return ftk_canvas_directfb_get_str_extent(thiz, utf8, strlen(utf8));
}

static void ftk_canvas_directfb_destroy(FtkCanvas* thiz)
{
	DECL_PRIV(thiz, priv);

	if(thiz != NULL)
	{
		ftk_bitmap_unref(priv->bitmap);
		priv->font->Release(priv->font);
		FTK_FREE(thiz);
	}

	return;
}

static const FtkCanvasVTable g_canvas_directfb_vtable=
{
	ftk_canvas_directfb_sync_gc,
	ftk_canvas_directfb_set_clip,
	ftk_canvas_directfb_draw_pixels,
	ftk_canvas_directfb_draw_line,
	ftk_canvas_directfb_clear_rect,
	ftk_canvas_directfb_draw_rect,

	ftk_canvas_directfb_draw_bitmap,
	ftk_canvas_directfb_draw_string,
	ftk_canvas_directfb_get_str_extent,
	ftk_canvas_directfb_get_char_extent,
	fk_canvas_directfb_lock_buffer,
	ftk_canvas_directfb_unlock_buffer,
	ftk_canvas_directfb_destroy
};

FtkCanvas* ftk_canvas_create(int w, int h, FtkColor* clear_color)
{
	int err = 0;
	FtkCanvas* thiz = NULL;
	char font_file[FTK_MAX_PATH+1] = {0};
	return_val_if_fail(w > 0 && h > 0 && clear_color != NULL, NULL);

	thiz = (FtkCanvas*)FTK_ZALLOC(sizeof(FtkCanvas) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		FtkColor color = *clear_color;
		DFBSurfaceDescription  desc = {0};
		IDirectFBSurface* surface = NULL;
		IDirectFB* dfb = directfb_get();
		DFBFontDescription font_desc = {0};

		ftk_strs_cat(font_file, FTK_MAX_PATH,
          ftk_config_get_data_dir(ftk_default_config()), "/data/", FTK_FONT, NULL);
		ftk_normalize_path(font_file);

		thiz->width = w;
		thiz->height = h;
		desc.width  = w;
		desc.height = h;
		desc.caps   = DSCAPS_SHARED;
		desc.pixelformat = DSPF_ARGB;
		desc.flags  = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_CAPS | DSDESC_PIXELFORMAT;

		thiz->gc.bg = *clear_color;
		thiz->gc.fg.a = 0xff;
		thiz->gc.fg.r = 0xff - clear_color->r;
		thiz->gc.fg.g = 0xff - clear_color->g;
		thiz->gc.fg.b = 0xff - clear_color->b;
		thiz->gc.mask = FTK_GC_FG | FTK_GC_BG;
		thiz->vtable = &g_canvas_directfb_vtable;

		font_desc.height  = 16;
		font_desc.flags = DFDESC_HEIGHT;
    	
		dfb->CreateSurface(dfb, &desc, &surface);
		surface->Clear(priv->surface, color.r, color.g, color.b, color.a);

    	DFBCHECK(dfb->CreateFont (dfb, font_file, &font_desc, &priv->font));
        surface->SetFont (surface, priv->font);
		surface->SetDrawingFlags(surface, DSDRAW_BLEND);
		surface->SetBlittingFlags(surface, DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA);

		priv->surface = surface;
		priv->bitmap = ftk_bitmap_create_with_native(surface);
	}

	return thiz;
}

