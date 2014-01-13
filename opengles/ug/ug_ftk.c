/*
 * GLESonGL implementation
 * Version:  1.2
 *
 * Copyright (C) 2003  David Blythe   All Rights Reserved.
 * Copyright (C) 2010  Li XianJing	 All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * DAVID BLYTHE BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ftk.h>
#include <GLES/egl.h>
#include "ug.h"

typedef struct {
	EGLDisplay egldpy;
	NativeDisplayType dpy;
	struct ugwindow* win, * winlist;
} UGCtx_t;

static UGCtx_t* context = NULL;

typedef void (*uGIdle)(UGWindow w);
typedef void (*uGDraw)(UGWindow w);
typedef void (*uGMotion)(UGWindow w, int x, int y);
typedef void (*uGKbd)(UGWindow w, int key, int x, int y);
typedef void (*uGPointer)(UGWindow w, int button, int state, int x, int y);
typedef void (*uGReshape)(UGWindow w, int width,int height);
typedef EGLBoolean (*PFNSAVESURFACE)(EGLSurface surface, const char* filename);

typedef struct ugwindow {
	int width;
	int height;
	int redraw;
	UGCtx_t* ug;
	FtkWidget* win;
   
	int x;
	int y;
	uGKbd kbd;
	uGDraw draw;
	uGMotion motion;
	uGReshape reshape;
	uGPointer pointer;
	
	EGLContext eglctx;
	EGLSurface surface;
	EGLConfig  eglconfig;
	FtkSource* painter_idle;
	struct ugwindow* next, * prev;
} UGWindow_t;

static int argc = 1;
static int ug_user_ref = 0;
static char* argv[] = {"gles", NULL};
static PFNSAVESURFACE pfnSaveSurface;	 // function pointer used to save images 

UGCtx APIENTRY ugInit(void) 
{
	UGCtx_t* ug = NULL;
	
	ug_user_ref++;
	if(ug_user_ref > 1)
	{
		return context;
	}

	ug = calloc(1, sizeof *ug);
	return_val_if_fail(ug != NULL, 0);

	ftk_init(argc, argv);
	ug->dpy = ftk_default_display();
	ug->egldpy = eglGetDisplay(ug->dpy);

	context = ug;
	eglInitialize(ug->egldpy, NULL, NULL);
	pfnSaveSurface = (PFNSAVESURFACE)eglGetProcAddress("eglSaveSurfaceHM");

	return (UGCtx)ug;
}

void APIENTRY
ugFini(UGCtx ug) {
	UGCtx_t* _ug = (UGCtx_t*)ug;

	ug_user_ref--;
	if(ug_user_ref == 0)
	{
		eglTerminate(_ug->egldpy);
		free(_ug);
		context = NULL;
	}
}

UGCtx APIENTRY ugCtxFromWin(UGWindow uwin) {
	return (UGCtx)((UGWindow_t*)uwin)->ug;
}

static void
bind_context(UGCtx_t* ug, UGWindow_t* w) {
	if (!eglMakeCurrent(ug->egldpy, w->surface, w->surface, w->eglctx)) {
		printf("botch makecurrent\n");
		exit(1);
	}
	ug->win = w;
}

static int GetKey(int vk) 
{
	switch (vk) {
	case FTK_KEY_F1:			return UG_KEY_F1;
	case FTK_KEY_F2:			return UG_KEY_F2;
	case FTK_KEY_F3:			return UG_KEY_F3;
	case FTK_KEY_F4:			return UG_KEY_F4;
	case FTK_KEY_F5:			return UG_KEY_F5;
	case FTK_KEY_F6:			return UG_KEY_F6;
	case FTK_KEY_F7:			return UG_KEY_F7;
	case FTK_KEY_F8:			return UG_KEY_F8;
	case FTK_KEY_F9:			return UG_KEY_F9;
	case FTK_KEY_F10:		return UG_KEY_F10;
	case FTK_KEY_LEFT:		return UG_KEY_LEFT;
	case FTK_KEY_UP:			return UG_KEY_UP;
	case FTK_KEY_RIGHT:		return UG_KEY_RIGHT;
	case FTK_KEY_DOWN:		return UG_KEY_DOWN;
	case FTK_KEY_PRIOR:		return UG_KEY_PAGE_UP;
	case FTK_KEY_NEXT:		return UG_KEY_PAGE_DOWN;
	case FTK_KEY_HOME:		return UG_KEY_HOME;
	case FTK_KEY_END:		return UG_KEY_END;
	case FTK_KEY_INSERT:		return UG_KEY_INSERT;
	default:			return vk;
	}
}


static Ret	ug_win_on_event(void* user_data, void* obj)
{
	FtkEvent* event = obj;
	UGWindow_t* win = user_data;

	switch(event->type)
	{
		case FTK_EVT_KEY_DOWN:
		{
			int key = GetKey(event->u.key.code);

			if(win->kbd != NULL)
			{
				win->kbd((UGWindow)win, key, win->x, win->y); 
			}
			break;
		}
		case FTK_EVT_MOUSE_UP:
		case FTK_EVT_MOUSE_DOWN:
		{
			if(win->pointer != NULL)
			{
				int state = event->type == FTK_EVT_MOUSE_DOWN ? UG_BUT_DOWN : UG_BUT_UP;
				win->pointer((UGWindow)win, UG_BUT_LEFT, state, event->u.mouse.x, event->u.mouse.y);
			}

			break;
		}
		case FTK_EVT_MOUSE_MOVE:
		{
			if(win->motion != NULL)
			{
				win->motion((UGWindow)win, event->u.mouse.x, event->u.mouse.y);
			}
			break;
		}
		default:
		{
			return RET_OK;
		}
	}

	return RET_REMOVE;
}

static int attribs[] = { EGL_RED_SIZE, 1, EGL_NONE }; /*XXXblythe*/
static int attribs2[] = {EGL_RED_SIZE, 1, EGL_DEPTH_SIZE, 1, EGL_NONE};

UGWindow APIENTRY ugCreateWindow(UGCtx ug,	const char* config, 
	const char* title, int width, int height, int x, int y) {
	
	EGLint n, vid;
	EGLConfig configs[1];
	UGCtx_t* _ug = (UGCtx_t*)ug;
	UGWindow_t *w = calloc(1, sizeof *w);
	int depth = strstr(config, "UG_DEPTH") != 0;

	return_val_if_fail(ug != 0 && w != NULL, 0);

	w->ug = _ug;
	if (!eglChooseConfig(_ug->egldpy, depth ? attribs2 : attribs, configs, 1, &n)) 
	{
		free(w);
		return 0;
	}
	
	w->eglconfig = configs[0];
	eglGetConfigAttrib(_ug->egldpy, configs[0], EGL_NATIVE_VISUAL_ID, &vid);
	
	w->win = ftk_app_window_create();
	ftk_window_set_animation_hint(w->win, "app_main_window");
	return_val_if_fail(w->win != NULL, 0);

	ftk_widget_set_text(w->win, title);
	ftk_widget_set_event_listener(w->win, ug_win_on_event, w);
	ftk_widget_show(w->win, 1);
	ftk_widget_set_user_data(w->win, ugDestroyWindow, w);

	w->next = _ug->winlist;
	_ug->winlist = w;
	w->prev = 0;

	if (w->next)
	{
		w->next->prev = w;
	}

	w->width = ftk_widget_width(w->win);
	w->height = ftk_widget_height(w->win);
	
	w->surface = eglCreateWindowSurface(_ug->egldpy, w->eglconfig, (NativeWindowType)(w->win), 0);
	
	w->eglctx = eglCreateContext(_ug->egldpy, w->eglconfig, NULL, NULL);
	bind_context(_ug, w);
	
	return (UGWindow)w;
}

void APIENTRY ugDestroyWindow(UGWindow uwin) {
	UGWindow_t* w = (UGWindow_t*)uwin;
	UGCtx_t* ug = w->ug;
	eglDestroySurface(ug->egldpy, w->surface);

	if (ug->winlist == w) ug->winlist = w->next;
	if (w->next) w->next->prev = w->prev;
	if (w->prev) w->prev->next = w->next;
	if(w->painter_idle != NULL) ftk_source_unref(w->painter_idle);
	free(w);
}

void APIENTRY
ugReshapeFunc(UGWindow uwin, void (*f)(UGWindow uwin, int width, int height)) {
	UGWindow_t *w = (UGWindow_t*)uwin;
	w->reshape = f;
	if(f != NULL)
	{
		f(uwin, w->width, w->height);
	}

	return;
}

void APIENTRY
ugDisplayFunc(UGWindow uwin, void (*f)(UGWindow uwin)) {
	UGWindow_t *w = (UGWindow_t*)uwin;
	w->draw = f;

	ugPostRedisplay(uwin);
}

void APIENTRY
ugKeyboardFunc(UGWindow uwin, void (*f)(UGWindow uwin, int key, int x, int y)) {
	UGWindow_t *w = (UGWindow_t*)uwin;
	w->kbd = f;
}

void APIENTRY
ugPointerFunc(UGWindow uwin, void (*f)(UGWindow uwin, int button, int state, int x, int y)) {
	UGWindow_t *w = (UGWindow_t*)uwin;
	w->pointer = f;
}

void APIENTRY
ugMotionFunc(UGWindow uwin, void (*f)(UGWindow uwin, int x, int y)) {
	UGWindow_t *w = (UGWindow_t*)uwin;
	w->motion = f;
}

static Ret	on_idle(void* user_data)
{
	uGIdle idle = user_data;

	idle((UGWindow)context->win);

	return RET_REMOVE;
}

void APIENTRY ugIdleFunc(UGCtx ug, void (*f)(UGWindow w)) 
{
	FtkSource* source = ftk_source_idle_create(on_idle, f);

	ftk_main_loop_add_source(ftk_default_main_loop(), source);

	return;
}

void APIENTRY ugMainLoop(UGCtx ug) 
{
	context = (UGCtx_t *) ug;

	ftk_run();

	return;
}

static Ret redisplay(void* ctx)
{
	UGWindow_t* win = ctx;

	if(win != NULL && win->draw != NULL)
	{
		win->draw((UGWindow)win);
	}

	return RET_REMOVE;
}

void APIENTRY ugPostRedisplay(UGWindow uwin) 
{
	UGWindow_t* w = (UGWindow_t*)uwin;
	
	return_if_fail(w != NULL);

	if(w->painter_idle == NULL)
	{
		w->painter_idle = ftk_source_idle_create(redisplay, w);
	}

	ftk_source_ref(w->painter_idle);
	ftk_main_loop_add_source(ftk_default_main_loop(), w->painter_idle);

	return;
}

void APIENTRY ugSwapBuffers(UGWindow uwin) 
{
	UGWindow_t* w = (UGWindow_t*)uwin;

	eglSwapBuffers(context->dpy, w->surface);
}

