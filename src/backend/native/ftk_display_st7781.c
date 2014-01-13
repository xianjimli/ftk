/*
 * File: ftk_display_st7781.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   lcm st7781 implemented display.
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
 * 2010-08-02 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include <sys/mman.h>
#include "ftk_display_st7781.h"

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320

static int memfd;
static unsigned short volatile *lcmreg;
static unsigned short volatile *lcmdata;

#define WRITE_REG(cmd, data) \
   *(volatile unsigned short*)lcmreg=cmd; \
   *(volatile unsigned short*)lcmdata=data;

#define READ_REG(cmd, data) \
   *(volatile unsigned short*)lcmreg=cmd; \
   data = *(volatile unsigned short*)lcmdata;

#define SET_PIXEL(x, y, pixel) \
    WRITE_REG(0x0020, x); \
    WRITE_REG(0x0021, y); \
    WRITE_REG(0x0022, pixel)

#define SET_PIXEL_FAST(pixel) \
    WRITE_REG(0x0022, pixel)

#define GET_PIXEL(x, y, pixel) \
    WRITE_REG(0x0020, x); \
    WRITE_REG(0x0021, y); \
    READ_REG (0x0022, pixel)

static Ret st7781_init(void)
{
	int i;
	memfd=open("/dev/mem",O_RDWR,0777);
	return_val_if_fail(memfd > 0, RET_FAIL);

	lcmreg=(volatile unsigned short*)mmap(0,4,0x03, MAP_SHARED, memfd, 0x08000000);
	//lcmreg=(volatile unsigned short*)mmap(0,4,0x03, MAP_SHARED, memfd, 0x0000000);
	return_val_if_fail(lcmreg != MAP_FAILED, RET_FAIL);

	lcmdata=lcmreg;
	lcmdata++;
	WRITE_REG(0x00FF,0x0001);
	WRITE_REG(0x00F3,0x0008);
	WRITE_REG(0x0001,0x0100);
	WRITE_REG(0x0002,0x0700);
	WRITE_REG(0x0003,0x1030);  //0x1030
	WRITE_REG(0x0008,0x0302);
	WRITE_REG(0x0008,0x0207);
	WRITE_REG(0x0009,0x0000);
	WRITE_REG(0x000A,0x0000);
	WRITE_REG(0x0010,0x0000);  //0x0790
	WRITE_REG(0x0011,0x0005);
	WRITE_REG(0x0012,0x0000);
	WRITE_REG(0x0013,0x0000);
	usleep(50*1000);
	WRITE_REG(0x0010,0x12B0);
	usleep(50*1000);
	WRITE_REG(0x0011,0x0007);
	usleep(50*1000);
	WRITE_REG(0x0012,0x008B);
	usleep(50*1000);
	WRITE_REG(0x0013,0x1700);
	usleep(50*1000);
	WRITE_REG(0x0029,0x0022);

	//################# void Gamma_Set(void) ####################//
	WRITE_REG(0x0030,0x0000);
	WRITE_REG(0x0031,0x0707);
	WRITE_REG(0x0032,0x0505);
	WRITE_REG(0x0035,0x0107);
	WRITE_REG(0x0036,0x0008);
	WRITE_REG(0x0037,0x0000);
	WRITE_REG(0x0038,0x0202);
	WRITE_REG(0x0039,0x0106);
	WRITE_REG(0x003C,0x0202);
	WRITE_REG(0x003D,0x0408);
	usleep(50*1000);

	WRITE_REG(0x0050,0x0000);
	WRITE_REG(0x0051,0x00EF);
	WRITE_REG(0x0052,0x0000);
	WRITE_REG(0x0053,0x013F);
	WRITE_REG(0x0060,0xA700);
	WRITE_REG(0x0061,0x0001);
	WRITE_REG(0x0090,0x0033);
	WRITE_REG(0x002B,0x000B);
	WRITE_REG(0x0007,0x0133);
	usleep(50*1000);

	WRITE_REG(0x0020,0x0000);
	WRITE_REG(0x0021,0x0000);
	*(volatile unsigned short*)lcmreg=0x0022;

	for (i=0;i<76800;i++)
		*(volatile unsigned short*)lcmdata=0xffff;

	ftk_logd("%s done\n", __func__);

	return RET_OK;
}
 
typedef struct _PrivInfo
{
	int width;
	int height;
	FtkColor* bits;
}PrivInfo;

static int ftk_display_st7781_width(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);

	return priv->width;
}

static int ftk_display_st7781_height(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);

	return priv->height;
}

static Ret ftk_display_st7781_update(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int output_x = xoffset;
	int output_y = yoffset;
	FtkColor color = {0};
	DECL_PRIV(thiz, priv);
	unsigned short pixel = 0;
	int display_w = priv->width;
	int display_h = priv->height;
	int bitmap_w = ftk_bitmap_width(bitmap);
	int bitmap_h = ftk_bitmap_height(bitmap);
	int x = rect != NULL ? rect->x : 0;
	int y = rect != NULL ? rect->y : 0;
	int w = rect != NULL ? rect->width : bitmap_w;
	int h = rect != NULL ? rect->height : bitmap_h;
	FtkColor* src = ftk_bitmap_lock(bitmap);
	
	return_val_if_fail(output_x < display_w, RET_FAIL);
	return_val_if_fail(output_y < display_h, RET_FAIL);
	return_val_if_fail(x < bitmap_w, RET_FAIL);
	return_val_if_fail(y < bitmap_h, RET_FAIL);
	return_val_if_fail(src != NULL, RET_FAIL);
	
	x = x < 0 ? 0 : x;
	y = y < 0 ? 0 : y;
	w = (x + w) < bitmap_w ? w : bitmap_w - x;
	h = (y + h) < bitmap_h ? h : bitmap_h - y;
	w = (output_x + w) < display_w ? w : display_w - output_x;
	h = (output_y + h) < display_h ? h : display_h - output_y;
	
	w += x;
	h += y;
	src += y * bitmap_w;
	/*TODO: optimize*/
	for(i = y; i < h; i++)
	{
		for(j = x, k = output_x; j < w; j++, k++)
		{
			color = src[j];
			pixel = ((color.r >> 3) << 11) | ((color.g >> 2) << 5) | (color.b >> 3);
		
			SET_PIXEL(k, y, pixel);
		}
		src += bitmap_w;
	}

	return RET_OK;
}

static Ret ftk_display_st7781_snap(FtkDisplay* thiz, FtkRect* rect, FtkBitmap* bitmap)
{
	int x  = 0;
	int y  = 0;
	int w  = 0;
	int h  = 0;
	int pixel = 0;
	int output_x = 0;
	int output_y = 0;
	DECL_PRIV(thiz, priv);
	int display_w = priv->width;
	int display_h = priv->height;
	int bitmap_w = ftk_bitmap_width(bitmap);
	int bitmap_h = ftk_bitmap_height(bitmap);
	FtkColor* dst = ftk_bitmap_lock(bitmap);
	
	return_val_if_fail(dst != NULL, RET_FAIL);
	
	x = rect != NULL ? rect->x : 0;
	y = rect != NULL ? rect->y : 0;
	x = x < 0 ? 0 : x;
	y = y < 0 ? 0 : y;
	return_val_if_fail(x < display_w && y < display_h, RET_FAIL);
	
	w = rect != NULL ? rect->width  : bitmap_w;
	h = rect != NULL ? rect->height : bitmap_h;
	
	/*width/height must less than bitmap's width/height*/
	w = w < bitmap_w ? w : bitmap_w;
	h = w < bitmap_h ? h : bitmap_h;
	
	/*width/height must less than data's width/height*/
	w = (x + w) < display_w ? w : display_w - x;
	h = (y + h) < display_h ? h : display_h - y;
	
	for(output_y = 0; output_y < h; output_y++, y++)
	{
		for(output_x = 0; output_x < w; output_x++, x++)
		{
			GET_PIXEL(x, y, pixel);
			dst[output_x].a = 0xff;
			dst[output_x].r = (pixel >> 8) & 0xf8;
			dst[output_x].g = (pixel >> 3) & 0xfc;
			dst[output_x].b = (pixel << 3) & 0xff;
		}
		dst += bitmap_w;
	}

	return RET_OK;
}

static void ftk_display_st7781_destroy(FtkDisplay* thiz)
{
	if(thiz != NULL)
	{
		if(memfd > 0)
		{
			close(memfd);	
			memfd = -1;
		}
		FTK_ZFREE(thiz, sizeof(FtkDisplay) + sizeof(PrivInfo));
	}

	return;
}

FtkDisplay* ftk_display_st7781_create(void)
{
	FtkDisplay* thiz = NULL;
	return_val_if_fail(st7781_init() == RET_OK, NULL);

	thiz = (FtkDisplay*)FTK_ZALLOC(sizeof(FtkDisplay) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		priv->width = DISPLAY_WIDTH;
		priv->height = DISPLAY_HEIGHT;

		thiz->update   = ftk_display_st7781_update;
		thiz->width    = ftk_display_st7781_width;
		thiz->height   = ftk_display_st7781_height;
		thiz->snap     = ftk_display_st7781_snap;
		thiz->destroy  = ftk_display_st7781_destroy;
	}
		
	return thiz;
}

