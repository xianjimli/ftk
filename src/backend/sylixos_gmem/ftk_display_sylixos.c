/*
 * File: ftk_display_fb.c
 * Author:  Han.hui <sylixos@gmail.com>
 * Brief:   sylixos gmem implemented display.
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
 * 2011-05-15 Han.hui <sylixos@gmail.com> created
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "ftk_allocator.h"
#include "ftk_log.h"
#include <mouse.h>
#include <keyboard.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "ftk_display_mem.h"
#include "ftk_display_sylixos.h"

struct FbInfo
{
    int             fd;
    void*           bits;
    LW_GM_VARINFO   varinfo;
    LW_GM_SCRINFO   scrinfo;
};

#define fb_width(fb)    ((fb)->varinfo.GMVI_ulXRes)
#define fb_height(fb)   ((fb)->varinfo.GMVI_ulYRes)
#define fb_bpp(fb)      ((fb)->varinfo.GMVI_ulBitsPerPixel)
#define fb_size(fb)     (fb_width(fb) * fb_height(fb) * fb_bpp(fb) >> 3)

static int gmem_open(struct FbInfo *fb, const char* fbfilename)
{
    fb->fd = open(fbfilename, O_RDWR);
    if (fb->fd < 0)
    {
        return -1;
    }

    if (ioctl(fb->fd, LW_GM_GET_VARINFO, &fb->varinfo) < 0)
        goto fail;

    if (ioctl(fb->fd, LW_GM_GET_SCRINFO, &fb->scrinfo) < 0)
        goto fail;

    fb->bits = mmap(0, fb_size(fb), PROT_READ | PROT_WRITE, MAP_SHARED, fb->fd, 0);
    if (fb->bits == MAP_FAILED)
    {
        ftk_logd("map framebuffer failed.\n");
        goto fail;
    }

    memset(fb->bits, 0xff, fb_size(fb));

    return 0;

fail:
    ftk_logd("%s is not a framebuffer.\n", fbfilename);
    close(fb->fd);

    return -1;
}

static void gmem_close(void *user)
{
    struct FbInfo *fb = (struct FbInfo *)user;

    if (fb != NULL)
    {
        munmap(fb->bits, fb_size(fb));
        close(fb->fd);
        FTK_FREE(fb);
    }

    return;
}

static void gmem_sync(void* ctx, FtkRect* rect)
{
    return;
}

FtkDisplay* ftk_display_sylixos_create(const char* filename)
{
    FtkDisplay*    thiz = NULL;
    struct FbInfo* fb   = NULL;
    return_val_if_fail(filename != NULL, NULL);

    fb = FTK_ZALLOC(sizeof(struct FbInfo));
    return_val_if_fail(fb != NULL, NULL);

    if (gmem_open(fb, filename) == 0)
    {
        FtkPixelFormat format         = FTK_PIXEL_NONE;
        int            bits_per_pixel = fb_bpp(fb);

        if (bits_per_pixel == 16)
        {
            format = FTK_PIXEL_RGB565;
        }
        else if (bits_per_pixel == 24)
        {
            format = FTK_PIXEL_BGR24;
        }
        else if (bits_per_pixel == 32)
        {
            format = FTK_PIXEL_BGRA32;
        }
        else
        {
            assert(!"not supported framebuffer format.");
        }

        thiz = ftk_display_mem_create(format,
                                      fb_width(fb),
                                      fb_height(fb),
                                      fb->bits,
                                      gmem_close,
                                      fb);

        ftk_display_mem_set_sync_func(thiz, gmem_sync, fb);
    }

    return thiz;
}
