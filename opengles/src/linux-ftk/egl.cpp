// ==========================================================================
//
// gl.cpp	EGL Client API entry points
//
// --------------------------------------------------------------------------
//
// 08-04-2003	Hans-Martin Will	initial version
// 08-18-2010		Li XianJing	port to ftk
//
// --------------------------------------------------------------------------
//
// Copyright (c) 2004, Hans-Martin Will. All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are 
// met:
// 
//	 *	Redistributions of source code must retain the above copyright
//		notice, this list of conditions and the following disclaimer. 
//	 *	Redistributions in binary form must reproduce the above copyright
//		notice, this list of conditions and the following disclaimer in the 
//		documentation and/or other materials provided with the distribution. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
// THE POSSIBILITY OF SUCH DAMAGE.
//
// ==========================================================================


#include "stdafx.h"
#include "GLES/gl.h"
#include "GLES/egl.h"
#include "Context.h"
#include "Rasterizer.h"
#include "Config.h"
#include "Surface.h"
#include <pthread.h>

using namespace EGL;

pthread_key_t s_TlsIndexContext;
pthread_key_t s_TlsIndexError;

EGLDisplay g_display;

static EGLint  RefCount = 0;

// version numbers
#define EGL_VERSION_MAJOR 1
#define EGL_VERSION_MINOR 0

static void eglRecordError(EGLint error) 
{
	pthread_setspecific(s_TlsIndexError, reinterpret_cast<void *>(error));
}

static NativeDisplayType GetNativeDisplay (EGLDisplay display) 
{
	return (NativeDisplayType) display;
}

GLAPI EGLint APIENTRY eglGetError (void) 
{
	return	reinterpret_cast<EGLint>(pthread_getspecific(s_TlsIndexError));
}

GLAPI EGLDisplay APIENTRY eglGetDisplay (NativeDisplayType display) 
{
	return (EGLDisplay) display;
}

GLAPI EGLBoolean APIENTRY 
eglInitialize (EGLDisplay dpy, EGLint *major, EGLint *minor) 
{
	g_display = dpy;

	if (major != 0) 
	{
	*major = EGL_VERSION_MAJOR;
	}

	if (minor != 0) 
	{
	*minor = EGL_VERSION_MINOR;
	}

	if (RefCount == 0) 
	{
	if ((pthread_key_create(&s_TlsIndexContext, NULL) != 0)||
		(pthread_key_create(&s_TlsIndexError, NULL) != 0))
	{
		eglRecordError(EGL_NOT_INITIALIZED);
		return false;
	}			
	}
	
	RefCount++;
	eglRecordError(EGL_SUCCESS);

	return true;
}

GLAPI EGLBoolean APIENTRY eglTerminate (EGLDisplay dpy) 
{
	if (RefCount > 0) 
	{
	if (RefCount == 1) 
	{
		pthread_key_delete(s_TlsIndexContext);
		pthread_key_delete(s_TlsIndexError);
	}

	RefCount--;
	}

	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

GLAPI const char * APIENTRY eglQueryString (EGLDisplay dpy, EGLint name) 
{
	eglRecordError(EGL_SUCCESS);

	switch (name) 
	{
	case EGL_VENDOR:
	return EGL_CONFIG_VENDOR;

	case EGL_VERSION:
	return EGL_VERSION_NUMBER;

	case EGL_EXTENSIONS:
	return "";

	default:
	eglRecordError(EGL_BAD_PARAMETER);
	return 0;
	}
}

GLAPI EGLBoolean APIENTRY 
eglSaveSurfaceHM(EGLSurface surface, const char *filename) 
{
	return surface->Save(filename);
}

#define FunctionEntry(s) { #s, (void (APIENTRY *)(void)) s }

static const struct 
{
	const char * name;
	void (APIENTRY * ptr)(void);
	
} FunctionTable[] = {
	/* OES_query_matrix */
	FunctionEntry(glQueryMatrixxOES),
	
	/* OES_point_size_array */
	FunctionEntry(glPointSizePointerOES),
	
	/* OES_matrix_palette */
	FunctionEntry(glCurrentPaletteMatrixOES),
	FunctionEntry(glLoadPaletteFromModelViewMatrixOES),
	FunctionEntry(glMatrixIndexPointerOES),
	FunctionEntry(glWeightPointerOES),
	
	FunctionEntry(eglSaveSurfaceHM)
};


GLAPI void (APIENTRY * eglGetProcAddress(const char *procname))(void) 
{
	eglRecordError(EGL_SUCCESS);

	if (!procname) 
	{
	return 0;
	}

	size_t functions = sizeof(FunctionTable) / sizeof(FunctionTable[0]);

	for (size_t index = 0; index < functions; ++index) 
	{
	if (!strcmp(FunctionTable[index].name, procname))
		return (void (APIENTRY *)(void)) FunctionTable[index].ptr;
	}

	return 0;
}


GLAPI EGLBoolean APIENTRY 
eglGetConfigs (EGLDisplay dpy, EGLConfig *configs, EGLint config_size, 
		   EGLint *num_config) 
{
	eglRecordError(EGL_SUCCESS);
	return Config::GetConfigs(configs, config_size, num_config);
}

GLAPI EGLBoolean APIENTRY 
eglChooseConfig (EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs,
		 EGLint config_size, EGLint *num_config) 
{
	eglRecordError(EGL_SUCCESS);
	return Config::ChooseConfig(attrib_list, configs, config_size, num_config);
}

GLAPI EGLBoolean APIENTRY 
eglGetConfigAttrib (EGLDisplay dpy, EGLConfig config, EGLint attribute, 
			EGLint *value) 
{
	*value = config->GetConfigAttrib(attribute);
	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}


GLAPI EGLSurface APIENTRY 
eglCreateWindowSurface (EGLDisplay dpy, EGLConfig config, 
			NativeWindowType window, const EGLint *attrib_list) 
{
	int width = 0;
	int height = 0;
	return_val_if_fail(window != NULL, NULL);

	width  = ftk_widget_width(window);
	height = ftk_widget_height(window);

	Config surfaceConfig(*config);
	surfaceConfig.SetConfigAttrib(EGL_SURFACE_TYPE, EGL_WINDOW_BIT);
	surfaceConfig.SetConfigAttrib(EGL_WIDTH,  width);
	surfaceConfig.SetConfigAttrib(EGL_HEIGHT, height);
	
	((Config*)config)->SetConfigAttrib(EGL_WIDTH,  width);
	((Config*)config)->SetConfigAttrib(EGL_HEIGHT, height);
	
	EGLSurface new_surface = new EGL::Surface(surfaceConfig, 
		   (NativeDisplayType) dpy, (NativeWindowType) window);
	eglRecordError(EGL_SUCCESS);
	return (new_surface); 
}

GLAPI EGLSurface APIENTRY 
eglCreatePixmapSurface (EGLDisplay dpy, EGLConfig config, 
			NativePixmapType pixmap, const EGLint *attrib_list) 
{
	// Cannot support rendering to arbitrary native surfaces; use pbuffer 
	// surface and eglCopySurfaces instead
	eglRecordError(EGL_BAD_MATCH);
	return EGL_NO_SURFACE;
}

GLAPI EGLSurface APIENTRY 
eglCreatePbufferSurface (EGLDisplay dpy, EGLConfig config, 
			 const EGLint *attrib_list) 
{
	static const EGLint validAttributes[] = {
	EGL_WIDTH, 0,
	EGL_HEIGHT, 0,
	EGL_NONE
	};

	Config surfaceConfig(*config, attrib_list, validAttributes);
	surfaceConfig.SetConfigAttrib(EGL_SURFACE_TYPE, EGL_PBUFFER_BIT);
	eglRecordError(EGL_SUCCESS);
	return new EGL::Surface(surfaceConfig, GetNativeDisplay(dpy), NULL);
}

GLAPI EGLBoolean APIENTRY 
eglDestroySurface (EGLDisplay dpy, EGLSurface surface) 
{
	surface->Dispose();
	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

GLAPI EGLBoolean APIENTRY 
eglQuerySurface (EGLDisplay dpy, EGLSurface surface, EGLint attribute, 
		 EGLint *value) 
{
	*value = surface->GetConfig()->GetConfigAttrib(attribute);
	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

GLAPI EGLContext APIENTRY 
eglCreateContext (EGLDisplay dpy, EGLConfig config, EGLContext share_list, 
		  const EGLint *attrib_list) 
{
	eglRecordError(EGL_SUCCESS);
	return new Context(*config);
}

GLAPI EGLBoolean APIENTRY 
eglDestroyContext (EGLDisplay dpy, EGLContext ctx) 
{
	ctx->Dispose();
	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

GLAPI EGLBoolean APIENTRY 
eglMakeCurrent (EGLDisplay dpy, EGLSurface draw, EGLSurface read, 
		EGLContext ctx) 
{
	Context::SetCurrentContext(ctx);
	
	if (ctx) 
	{
	ctx->SetDrawSurface(draw);
	ctx->SetReadSurface(read);
	}
	
	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

GLAPI EGLContext APIENTRY eglGetCurrentContext (void) 
{
	eglRecordError(EGL_SUCCESS);
	return Context::GetCurrentContext();
}

GLAPI EGLSurface APIENTRY eglGetCurrentSurface (EGLint readdraw) 
{
	EGLContext currentContext = eglGetCurrentContext();
	eglRecordError(EGL_SUCCESS);
	
	if (currentContext != 0) 
	{
	switch (readdraw) 
	{
	case EGL_DRAW:
		return currentContext->GetDrawSurface();
		
	case EGL_READ:
		return currentContext->GetReadSurface();
		
	default: 
		return 0;
	}
	} 
	else 
	{
	return 0;
	}
}

GLAPI EGLDisplay APIENTRY eglGetCurrentDisplay (void) 
{
	eglRecordError(EGL_SUCCESS);
	return g_display;
}

GLAPI EGLBoolean APIENTRY 
eglQueryContext (EGLDisplay dpy, EGLContext ctx, EGLint attribute, 
		 EGLint *value) 
{
	*value = ctx->GetConfig()->GetConfigAttrib(attribute);
	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

GLAPI EGLBoolean APIENTRY eglWaitGL (void) 
{
	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

GLAPI EGLBoolean APIENTRY eglWaitNative (EGLint engine) 
{
	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

GLAPI EGLBoolean APIENTRY eglSwapBuffers (EGLDisplay dpy, EGLSurface draw) 
{
	int xoffset = 0;
	int yoffset = 0;
	FtkRect rect = {0};
	U16* colors = NULL;
	FtkWidget* widget = draw->GetWidget();
	FtkBitmap* bitmap = draw->GetBitmap();
	NativeDisplayType display = (NativeDisplayType) dpy;
	
	rect.x = 0;
	rect.y = 0;
	rect.width	= draw->GetWidth();
	rect.height = draw->GetHeight();
	colors = draw->GetColorBuffer();
	
	Context::GetCurrentContext()->Flush();
	xoffset = ftk_widget_left_abs(widget);
	yoffset = ftk_widget_top_abs(widget);

	if(ftk_default_display()->update_directly == NULL)
	{
		ftk_bitmap_copy_from_data_rgb565(bitmap, colors, rect.width, rect.height, &rect);
		ftk_display_update_and_notify(display, bitmap, &rect, xoffset, yoffset);
	}
	else
	{
		ftk_display_notify(display, 1, NULL, &rect, xoffset, yoffset);
		ftk_display_update_directly(display, FTK_PIXEL_RGB565, 
			colors, rect.width, rect.height, xoffset, yoffset);
		ftk_display_notify(display, 0, NULL, &rect, xoffset, yoffset);
	}

	eglRecordError(EGL_SUCCESS);

	return EGL_TRUE;
}

GLAPI EGLBoolean APIENTRY 
eglCopyBuffers (EGLDisplay dpy, EGLSurface surface, NativePixmapType target) 
{
	Context::GetCurrentContext()->Flush();

	/*TODO*/

	eglRecordError(EGL_SUCCESS);
	return EGL_TRUE;
}

GLAPI EGLBoolean APIENTRY 
eglSurfaceAttrib (EGLDisplay dpy, EGLSurface surface, EGLint attribute, 
		  EGLint value) 
{
	return EGL_FALSE;
}

GLAPI EGLBoolean APIENTRY 
eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer) 
{
	return EGL_FALSE;
}

GLAPI EGLBoolean APIENTRY 
eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer) 
{
	return EGL_FALSE;
}

GLAPI EGLBoolean APIENTRY eglSwapInterval(EGLDisplay dpy, EGLint interval) 
{
	return EGL_FALSE;
}
