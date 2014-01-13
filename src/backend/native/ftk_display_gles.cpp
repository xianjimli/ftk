/*
 * File: ftk_display_gles.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   opengles based display.
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
 * 2010-09-27 Li XianJing <xianjimli@hotmail.com> created.
 *
 */

#ifdef ANDROID
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES/glext.h>

#include <ui/FramebufferNativeWindow.h>
#include <ui/EGLUtils.h>
using namespace android;
#define GLES_CREATE_WINDOW android_createDisplaySurface
#else
#include <egl.h>
#include <gl.h>
#include <glext.h>
#endif

#include "ftk_log.h"
#include "ftk_display_gles.h"

#ifndef EGLNativeWindowType
#define EGLNativeWindowType NativeWindowType
#endif

typedef struct _PrivInfo
{
	int width;
	int height;
	EGLDisplay dpy;
	EGLContext context;
	EGLSurface surface;
	FtkBitmap* bitmap;
}PrivInfo;

int opengles_init(PrivInfo* priv)
{
	int n = 0;
	EGLConfig config;
	EGLint majorVersion = 0;
	EGLint minorVersion = 0;
	
	const EGLint config_attr[] =
	{
		EGL_RED_SIZE,       5,
		EGL_GREEN_SIZE,     6,
		EGL_BLUE_SIZE,      5,

		EGL_ALPHA_SIZE,     EGL_DONT_CARE,
		EGL_DEPTH_SIZE,     16,
		EGL_STENCIL_SIZE,   EGL_DONT_CARE,
		EGL_SURFACE_TYPE,   EGL_WINDOW_BIT,
		EGL_NONE
	};


	EGLNativeWindowType window = GLES_CREATE_WINDOW();
	priv->dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(priv->dpy, &majorVersion, &minorVersion);

	if (!eglChooseConfig(priv->dpy, config_attr, &config, 1, &n))
	{
		printf("%s: eglChooseConfig failed.\n", __func__);
	}

    priv->surface = eglCreateWindowSurface(priv->dpy, config, window, NULL);
    priv->context = eglCreateContext(priv->dpy, config, NULL, NULL);
	eglMakeCurrent(priv->dpy, priv->surface, priv->surface, priv->context); 
	eglQuerySurface(priv->dpy, priv->surface, EGL_WIDTH,  &priv->width);
	eglQuerySurface(priv->dpy, priv->surface, EGL_HEIGHT, &priv->height);

	return 0;
}

int opengles_snap_bitmap(PrivInfo* priv, FtkBitmap* bitmap, 
	int xoffset, int yoffset, int width, int height)
{
	glReadPixels(xoffset, yoffset, width, height, GL_RGBA, GL_UNSIGNED_BYTE, ftk_bitmap_lock(bitmap));

	return 0;
}

int opengles_display_bitmap(PrivInfo* priv, FtkBitmap* bitmap, 
	int x, int y, int width, int height, int xoffset, int yoffset)
{
	GLint crop[4] = {0};
	crop[0] = x;
	crop[1] = y;
	crop[2] = width;
	crop[3] = height;

	glColor4f(1,1,1,1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, crop);
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_TEXTURE_2D);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ftk_bitmap_width(bitmap), ftk_bitmap_height(bitmap), 
		0, GL_RGBA, GL_UNSIGNED_BYTE, ftk_bitmap_lock(bitmap));
	glDrawTexiOES(xoffset, yoffset, 0, width, height);

	eglSwapBuffers(priv->dpy, priv->surface);

	printf("rect(%d %d %d %d) offset(%d %d) bitmap(%d %d)\n",
		x, y, width, height, xoffset, yoffset, ftk_bitmap_width(bitmap), ftk_bitmap_height(bitmap));

	return 0;
}

static int ftk_display_gles_width(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);

	return priv->width;
}

static int ftk_display_gles_height(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);

	return priv->height;
}

static Ret ftk_display_gles_update(FtkDisplay* thiz, FtkBitmap* bitmap, 
	FtkRect* rect, int xoffset, int yoffset)
{
	int i = 0;
	Ret ret = RET_OK;
	DECL_PRIV(thiz, priv);
	int width = rect->width;
	int height = rect->height;
	int src_width = ftk_bitmap_width(bitmap);
	int src_height = ftk_bitmap_height(bitmap);
	int dst_width = ftk_bitmap_width(priv->bitmap);
	int dst_height = ftk_bitmap_height(priv->bitmap);
	FtkColor* src = ftk_bitmap_lock(bitmap);
	FtkColor* dst = ftk_bitmap_lock(priv->bitmap);
	
	return_val_if_fail(rect->x < src_width && rect->y < src_height
		&& xoffset < dst_width && yoffset < dst_height, RET_FAIL);

	width = (xoffset + width) >= dst_width ? dst_width - xoffset : width;
	height = (yoffset + height) >= dst_height ? dst_height - yoffset : height;

	yoffset = dst_height  - yoffset - height - 1;
	src += rect->y * src_width + rect->x;
	dst += (yoffset + height) * dst_width + xoffset;

	for(i = 0; i < height; i++)
	{
		memcpy(dst, src, sizeof(FtkColor) * width);
		dst -= dst_width;
		src += src_width;
	}

	opengles_display_bitmap(priv, priv->bitmap, xoffset, yoffset, 
		width, height, xoffset, yoffset);
	
	return ret;
}

static Ret ftk_display_gles_snap(FtkDisplay* thiz, FtkRect* r, FtkBitmap* bitmap)
{
	int i = 0;
	int xoffset = r->x;
	int yoffset = r->y;
	int width = r->width;
	int height = r->height;
	DECL_PRIV(thiz, priv);
	FtkColor* dst = ftk_bitmap_lock(bitmap);
	FtkColor* src = ftk_bitmap_lock(priv->bitmap);

	/*TODO*/
	opengles_snap_bitmap(priv, bitmap, r->x, r->y, r->width, r->height);

	return RET_OK;
}

static void ftk_display_gles_destroy(FtkDisplay* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		
		eglTerminate(priv->dpy);
		FTK_ZFREE(thiz, sizeof(FtkDisplay) + sizeof(PrivInfo));
	}

	return;
}

extern "C" FtkDisplay* ftk_display_gles_create(void)
{
	FtkDisplay* thiz = NULL;

	thiz = (FtkDisplay*)FTK_ZALLOC(sizeof(FtkDisplay) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		FtkColor bg;
		DECL_PRIV(thiz, priv);
		thiz->update   = ftk_display_gles_update;
		thiz->width    = ftk_display_gles_width;
		thiz->height   = ftk_display_gles_height;
		thiz->snap     = ftk_display_gles_snap;
		thiz->destroy  = ftk_display_gles_destroy;

		bg.a = 0xff;
		bg.r = 0xff;
		bg.g = 0xff;
		bg.b = 0xff;
		opengles_init(priv);
		priv->bitmap = ftk_bitmap_create(priv->width, priv->height, bg);
	}
		
	return thiz;
}

#ifdef FTK_DISPLAY_GLES_TEST
#include "ftk.h"

void red_bitmap(FtkBitmap* bitmap)
{
	int x = 0;
	int y = 0;
	int w = ftk_bitmap_width(bitmap);
	int h = ftk_bitmap_height(bitmap);
	FtkColor* bits = ftk_bitmap_lock(bitmap);

	for(y = 0; y < h; y++)
	{
		for(x = 0; x < w; x++, bits++)
		{
			bits->g = 0;
			bits->b = 0;
			bits->r = 0xff;
		}
	}

	return;
}

void green_bitmap(FtkBitmap* bitmap)
{
	int x = 0;
	int y = 0;
	int w = ftk_bitmap_width(bitmap);
	int h = ftk_bitmap_height(bitmap);
	FtkColor* bits = ftk_bitmap_lock(bitmap);

	for(y = 0; y < h; y++)
	{
		for(x = 0; x < w; x++, bits++)
		{
			bits->r = 0;
			bits->b = 0;
			bits->g = 0xff;
		}
	}

	return;
}

void blue_bitmap(FtkBitmap* bitmap)
{
	int x = 0;
	int y = 0;
	int w = ftk_bitmap_width(bitmap);
	int h = ftk_bitmap_height(bitmap);
	FtkColor* bits = ftk_bitmap_lock(bitmap);

	for(y = 0; y < h; y++)
	{
		for(x = 0; x < w; x++, bits++)
		{
			bits->g = 0;
			bits->r = 0;
			bits->b = 0xff;
		}
	}

	return;
}

void mire_bitmap(FtkBitmap* bitmap)
{
	int x = 0;
	int y = 0;
	int w = ftk_bitmap_width(bitmap);
	int h = ftk_bitmap_height(bitmap);
	FtkColor* bits = ftk_bitmap_lock(bitmap);

	for (y = 0; y < h; y++)
	{
		for (x=0; x < w; x++, bits++)
		{
			unsigned int color = ((x-w/2)*(x-w/2) + (y-h/2)*(y-h/2))/64;
			bits->r = (color/8) % 256;
			bits->g = (color/4) % 256;
			bits->b = (color/2) % 256;
	//		bits->a = (color*2) % 256;
		}
	}

	return;
}

int main(int argc, char* argv[])
{
	FtkDisplay* thiz = NULL;

	ftk_init(argc, argv);

	thiz = ftk_default_display();
	if(thiz != NULL)
	{
		FtkBitmap* bitmap = NULL;
		FtkColor color;
		FtkRect rect = {0, 0, 0, 0};
		color.a = 0xff;

		rect.width = ftk_display_width(thiz);
		rect.height = ftk_display_height(thiz);

		bitmap = ftk_bitmap_create(rect.width, rect.height, color);
		red_bitmap(bitmap);
		ftk_display_update(thiz, bitmap, &rect, 0, 0);
		sleep(3);
		green_bitmap(bitmap);
		ftk_display_update(thiz, bitmap, &rect, 0, 0);
		sleep(3);
		blue_bitmap(bitmap);
		ftk_display_update(thiz, bitmap, &rect, 0, 0);
		sleep(3);
		mire_bitmap(bitmap);
		ftk_display_update(thiz, bitmap, &rect, 0, 0);
		sleep(3);

		ftk_display_destroy(thiz);
		ftk_bitmap_unref(bitmap);
	}

	return 0;
}
#endif

