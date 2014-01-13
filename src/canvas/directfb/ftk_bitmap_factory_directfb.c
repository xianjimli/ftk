/*
 * File: ftk_bitmap_factory.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   bitmap factory.
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
#include "ftk_log.h"
#include "ftk_allocator.h"
#include "directfb_common.h"
#include "ftk_bitmap_factory.h"

struct _FtkBitmapFactory
{
	int unused;
};

FtkBitmapFactory* ftk_bitmap_factory_create(void)
{
	FtkBitmapFactory* thiz = (FtkBitmapFactory*)FTK_ZALLOC(sizeof(FtkBitmapFactory));

	return thiz;
}

static DFBResult directfb_load_image (IDirectFB *dfb,
                 const char             *filename,
                 IDirectFBSurface      **surface)
{
     DFBResult               ret;
     DFBSurfaceDescription   dsc;
     IDirectFBSurface       *image;
     IDirectFBImageProvider *provider;
     
     if (!surface)
     {
          return DFB_INVARG;
	 }

     /* Create an image provider for loading the file */
     ret = dfb->CreateImageProvider (dfb, filename, &provider);
     if (ret) {
          fprintf (stderr,
                   "load_image: CreateImageProvider for '%s': %s\n",
                   filename, DirectFBErrorString (ret));
          return ret;
     }

     /* Retrieve a surface description for the image */
     ret = provider->GetSurfaceDescription (provider, &dsc);
     if (ret) {
          fprintf (stderr,
                   "load_image: GetSurfaceDescription for '%s': %s\n",
                   filename, DirectFBErrorString (ret));
          provider->Release (provider);
          return ret;
     }

	dsc.pixelformat = DSPF_ARGB;
     /* Create a surface using the description */
     ret = dfb->CreateSurface (dfb, &dsc, &image);
     if (ret) {
          fprintf (stderr,
                   "load_image: CreateSurface %dx%d: %s\n",
                   dsc.width, dsc.height, DirectFBErrorString (ret));
          provider->Release (provider);
          return ret;
     }

     /* Render the image to the created surface */
     ret = provider->RenderTo (provider, image, NULL);
     if (ret) {
          fprintf (stderr,
                   "load_image: RenderTo for '%s': %s\n",
                   filename, DirectFBErrorString (ret));
          image->Release (image);
          provider->Release (provider);
          return ret;
     }

     /* Return surface */
     *surface = image;

     /* Release the provider */
     provider->Release (provider);

     return DFB_OK;
}

FtkBitmap* ftk_bitmap_factory_load(FtkBitmapFactory* thiz, const char* filename)
{
	IDirectFBSurface* surface = NULL;
	IDirectFB* dfb = directfb_get();

	directfb_load_image(dfb, filename, &surface);

	if(surface != NULL)
	{
		return ftk_bitmap_create_with_native(surface);
	}
	else
	{
		return NULL;
	}
}

Ret        ftk_bitmap_factory_add_decoder(FtkBitmapFactory* thiz, FtkImageDecoder* decoder)
{
	return RET_OK;
}

void       ftk_bitmap_factory_destroy(FtkBitmapFactory* thiz)
{
	return;
}

