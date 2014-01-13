/*
 * File: ftk_display_fb.c	 
 * Author:	Li XianJing <xianjimli@hotmail.com>
 * Brief:	framebuffer implemented display.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "ftk_log.h"
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "ftk_display_mem.h"
#include "ftk_display_fb.h"

struct FbInfo 
{
	int fd;
	void* bits;
	struct fb_fix_screeninfo fix;
	struct fb_var_screeninfo var;
};

#define fb_width(fb) ((fb)->var.xres)
#define fb_height(fb) ((fb)->var.yres)
#define fb_size(fb) ((fb)->var.xres * (fb)->var.yres * fb->var.bits_per_pixel/8)
#ifndef FBIO_WAITFORVSYNC
#define FBIO_WAITFORVSYNC   _IOW('F', 0x20, u_int32_t)
#endif

static int fb_open(struct FbInfo *fb, const char* fbfilename)
{
	fb->fd = open(fbfilename, O_RDWR);
	if (fb->fd < 0)
	{
		return -1;
	}
	
	if (ioctl(fb->fd, FBIOGET_FSCREENINFO, &fb->fix) < 0)
		goto fail;
	if (ioctl(fb->fd, FBIOGET_VSCREENINFO, &fb->var) < 0)
		goto fail;
	
	fb->var.xoffset = 0; 
	fb->var.yoffset = 0; 
	ioctl (fb->fd, FBIOPAN_DISPLAY, &(fb->var)); 

	ftk_logi("FbInfo: %s\n", fbfilename);
	ftk_logi("FbInfo: xres=%d yres=%d bits_per_pixel=%d mem_size=%d\n", 
		fb->var.xres, fb->var.yres, fb->var.bits_per_pixel, fb_size(fb));
	ftk_logi("FbInfo: red(%d %d) green(%d %d) blue(%d %d)\n", 
		fb->var.red.offset, fb->var.red.length,
		fb->var.green.offset, fb->var.green.length,
		fb->var.blue.offset, fb->var.blue.length);

#ifdef FTK_FB_NOMMAP
	//uclinux doesn't support MAP_SHARED or MAP_PRIVATE with PROT_WRITE, so no mmap at all is simpler
	fb->bits = fb->fix.smem_start;
#else
	fb->bits = mmap(0, fb_size(fb), PROT_READ | PROT_WRITE, MAP_SHARED, fb->fd, 0);
#endif

	if (fb->bits == MAP_FAILED)
	{
		ftk_logi("map framebuffer failed.\n");
		goto fail;
	}

	memset(fb->bits, 0xff, fb_size(fb));
	
	ftk_logi("xres_virtual =%d yres_virtual=%d xpanstep=%d ywrapstep=%d\n",
		fb->var.xres_virtual, fb->var.yres_virtual, fb->fix.xpanstep, fb->fix.ywrapstep);

	return 0;
fail:
	ftk_logi("%s is not a framebuffer.\n", fbfilename);
	close(fb->fd);

	return -1;
}

static void fb_close(struct FbInfo *fb)
{
	if(fb != NULL)
	{
		munmap(fb->bits, fb_size(fb));
		close(fb->fd);
		FTK_FREE(fb);
	}

	return;
}

static Ret fb_pan(struct FbInfo* info, int xoffset, int yoffset, int onsync)
{
	struct fb_var_screeninfo *var = &info->var;

	return_val_if_fail(var->xres_virtual >= (xoffset + var->xres), RET_FAIL);
	return_val_if_fail(var->yres_virtual >= (yoffset + var->yres), RET_FAIL);

	if (!info->fix.xpanstep && !info->fix.ypanstep && !info->fix.ywrapstep)
	{
		return RET_OK;
	}

	if (info->fix.xpanstep)
	{
		var->xoffset = xoffset - (xoffset % info->fix.xpanstep);
	}
	else
	{
		var->xoffset = 0;
	}

	if (info->fix.ywrapstep) 
	{
		var->yoffset = yoffset - (yoffset % info->fix.ywrapstep);
		var->vmode |= FB_VMODE_YWRAP;
	}
	else if (info->fix.ypanstep) 
	{
		var->yoffset = yoffset - (yoffset % info->fix.ypanstep);
		var->vmode &= ~FB_VMODE_YWRAP;
	}
	else 
	{
		 var->yoffset = 0;
	}

	var->activate = onsync ? FB_ACTIVATE_VBL : FB_ACTIVATE_NOW;

	ftk_logi("%s: xoffset=%d yoffset=%d ywrapstep=%d\n", __func__,
		var->xoffset, var->yoffset, info->fix.ywrapstep);
	if (ioctl( info->fd, FBIOPAN_DISPLAY, var ) < 0)
	{
		return RET_FAIL;
	}

	return RET_OK;
}

static void fb_sync(void* ctx, FtkRect* rect)
{
	int zero = 0;
	int ret = 0;
	struct FbInfo* info = ctx;

	//ret = ioctl(info->fd, FBIO_WAITFORVSYNC, &zero);
	//ret = fb_pan(info, 0, 0, 1);
	//ret = ioctl(info->fd, FBIO_WAITFORVSYNC, &zero);
#ifdef USE_FB_ACTIVATE_ALL
	ret = ioctl(info->fd, FB_ACTIVATE_ALL, NULL);
	ftk_logi("%s: FB_ACTIVATE_ALL ret = %d\n", __func__, ret);
#endif	
	return;
}

FtkDisplay* ftk_display_fb_create(const char* filename)
{
	FtkDisplay* thiz = NULL;
	struct FbInfo* fb = NULL;
	return_val_if_fail(filename != NULL, NULL);

	fb = FTK_ZALLOC(sizeof(struct FbInfo));
	return_val_if_fail(fb != NULL, NULL);

	if(fb_open(fb, filename) == 0)
	{
		FtkPixelFormat format = 0;
		int bits_per_pixel = fb->var.bits_per_pixel;
		
		if(bits_per_pixel == 16)
		{
			format = FTK_PIXEL_RGB565;
		}
		else if(bits_per_pixel == 24)
		{
			format = FTK_PIXEL_BGR24;
		}
		else if(bits_per_pixel == 32)
		{
			format = FTK_PIXEL_BGRA32;
		}
		else
		{
			assert(!"not supported framebuffer format.");
		}
	
		thiz = ftk_display_mem_create(format, fb->var.xres, fb->var.yres, 
			fb->bits, fb_close, fb);
		ftk_display_mem_set_sync_func(thiz, fb_sync, fb);
	}
		
	return thiz;
}

