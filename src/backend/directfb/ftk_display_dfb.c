/*
 * File: ftk_display_dfb.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   display directfb implementation.
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
 * 2009-11-28 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include <directfb.h>
#include "ftk_globals.h"
#include "ftk_source_dfb.h"
#include "ftk_display_dfb.h"

#define DFBCHECK(x) x

typedef struct _PrivInfo
{
	int width;
	int height;
	IDirectFB* dfb;
	IDirectFBWindow* window;
	IDirectFBSurface* window_surface;
	IDirectFBEventBuffer* event_buffer;
	IDirectFBDisplayLayer* layer;
}PrivInfo;

static Ret ftk_display_dfb_update(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	Ret ret = RET_OK;
	DECL_PRIV(thiz, priv);
	DFBRectangle r = {0};
	int display_width  = ftk_display_width(thiz);
	int display_height = ftk_display_height(thiz);
	
	IDirectFBSurface* surface = (IDirectFBSurface*)ftk_bitmap_get_native(bitmap);

	r.w = display_width;
	r.h = display_height;
	
	priv->window_surface->Blit(priv->window_surface, surface, &r, 0, 0);
	priv->window_surface->Flip(priv->window_surface, NULL, 0);

	return ret;
}

static int ftk_display_dfb_width(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);	

	return priv->width;
}

static int ftk_display_dfb_height(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);	

	return priv->height;
}

static Ret ftk_display_dfb_snap(FtkDisplay* thiz, FtkRect* r, FtkBitmap* bitmap)
{
	int pitch = 0;
	void *data = NULL;
	FtkRect rect = {0};
	DECL_PRIV(thiz, priv);
	int w = ftk_display_width(thiz);
	int h = ftk_display_height(thiz);
	int bw = ftk_bitmap_width(bitmap);
	int bh = ftk_bitmap_height(bitmap);

	IDirectFBSurface* surface = priv->window_surface;
	surface->Lock(surface, DSLF_READ | DSLF_WRITE, &data, &pitch);
	
	rect.x = r->x;
	rect.y = r->y;
	rect.width = FTK_MIN(bw, r->width);
	rect.height = FTK_MIN(bh, r->height);
	ftk_bitmap_copy_from_data_bgra32(bitmap, data, w, h, &rect);

	surface->Unlock(surface);

	return RET_OK;
}

static void ftk_display_dfb_destroy(FtkDisplay* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		
		if(priv->event_buffer != NULL)
		{
			priv->event_buffer->Release(priv->event_buffer);
			priv->event_buffer = NULL;
		}

		if(priv->window_surface != NULL)
		{
			priv->window_surface->Release(priv->window_surface);
			priv->window_surface = NULL;
		}

		if(priv->window != NULL)
		{
			priv->window->Release(priv->window);
			priv->window= NULL;
		}
		
		if(priv->layer != NULL)
		{
			priv->layer->Release(priv->layer);
			priv->layer= NULL;
		}
		
		if(priv->dfb != NULL)
		{
			priv->dfb->Release( priv->dfb );
			priv->dfb = NULL;
		}

		FTK_ZFREE(thiz, sizeof(FtkDisplay) + sizeof(PrivInfo));
	}

	return;
}

static Ret ftk_display_dfb_init(FtkDisplay* thiz, IDirectFB* dfb)
{	
	DECL_PRIV(thiz, priv);
	int display_height = 0;
	int display_width  = 0;
	FtkSource* source = NULL;
	DFBWindowDescription win_desc;
	IDirectFBWindow* window = NULL;
	IDirectFBDisplayLayer* layer = NULL;
	IDirectFBSurface* window_surface = NULL;
	IDirectFBEventBuffer* event_buffer = NULL;
	DFBDisplayLayerConfig         layer_config;

	DFBCHECK(dfb->GetDisplayLayer( dfb, DLID_PRIMARY, &layer ));
	layer->SetCooperativeLevel( layer, DLSCL_ADMINISTRATIVE );
	layer->GetConfiguration( layer, &layer_config );

	display_width = layer_config.width;
	display_height = layer_config.height;

	win_desc.posx   = 0;
	win_desc.posy   = 0;
	win_desc.width  = display_width;
	win_desc.height = display_height;
	win_desc.flags  = ( DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT);

	DFBCHECK(layer->CreateWindow( layer, &win_desc, &window ));
    window->GetSurface( window, &window_surface );
	window->CreateEventBuffer( window, &event_buffer );

	priv->dfb     = dfb;
	priv->layer   = layer;
	priv->width   = display_width;
	priv->height  = display_height;
	priv->window = window;
	priv->event_buffer = event_buffer;
	priv->window_surface = window_surface;

	window->SetOpacity( window, 0xFF );
	if((source = ftk_source_dfb_create(dfb, event_buffer)) != NULL)
	{
		ftk_sources_manager_add(ftk_default_sources_manager(), source);
	}

	return RET_OK;
}

FtkDisplay* ftk_display_dfb_create(IDirectFB* dfb)
{
	FtkDisplay* thiz = NULL;
	return_val_if_fail(dfb != NULL, NULL);

	thiz = FTK_ZALLOC(sizeof(FtkDisplay)+sizeof(PrivInfo));
	if(thiz != NULL)
	{
		thiz->update  = ftk_display_dfb_update;
		thiz->width   = ftk_display_dfb_width;
		thiz->height  = ftk_display_dfb_height;
		thiz->snap    = ftk_display_dfb_snap;
		thiz->destroy = ftk_display_dfb_destroy;

		ftk_display_dfb_init(thiz, dfb);
	}
	
	return thiz;
}


