/*
 * File: ftk_wnd_manager.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   window manager interface.
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

#ifndef FTK_WND_MANAGER_H
#define FTK_WND_MANAGER_H

#include "ftk_widget.h"
#include "ftk_main_loop.h"

FTK_BEGIN_DECLS

struct _FtkWndManager;
typedef struct _FtkWndManager FtkWndManager;

typedef Ret  (*FtkWndManagerRestack)(FtkWndManager* thiz, FtkWidget* window, int offset);
typedef Ret  (*FtkWndManagerGrab)(FtkWndManager* thiz, FtkWidget* window);
typedef Ret  (*FtkWndManagerUngrab)(FtkWndManager* thiz, FtkWidget* window);
typedef Ret  (*FtkWndManagerAdd)(FtkWndManager* thiz, FtkWidget* window);
typedef Ret  (*FtkWndManagerRemove)(FtkWndManager* thiz, FtkWidget* window);
typedef Ret  (*FtkWndManagerUpdate)(FtkWndManager* thiz);
typedef Ret  (*FtkWndManagerGetWorkArea)(FtkWndManager* thiz, FtkRect* rect);
typedef Ret  (*FtkWndManagerQueueEvent)(FtkWndManager* thiz, FtkEvent* event);
typedef Ret  (*FtkWndManagerDispatchEvent)(FtkWndManager* thiz, FtkEvent* event);
typedef Ret  (*FtkWndManagerAddGlobalListener)(FtkWndManager* thiz, FtkListener listener, void* ctx);
typedef Ret  (*FtkWndManagerRemoveGlobalListener)(FtkWndManager* thiz, FtkListener listener, void* ctx);
typedef void (*FtkWndManagerDestroy)(FtkWndManager* thiz);

struct _FtkWndManager
{
	FtkWndManagerGrab                 grab;
	FtkWndManagerUngrab               ungrab;
	FtkWndManagerAdd                  add;
	FtkWndManagerRemove               remove;
	FtkWndManagerUpdate               update;
	FtkWndManagerRestack              restack;
	FtkWndManagerGetWorkArea          get_work_area;
	FtkWndManagerQueueEvent           queue_event;
	FtkWndManagerDispatchEvent        dispatch_event;
	FtkWndManagerAddGlobalListener    add_global_listener;
	FtkWndManagerRemoveGlobalListener remove_global_listener;
	FtkWndManagerDestroy              destroy;

	char priv[ZERO_LEN_ARRAY];
};

static inline Ret  ftk_wnd_manager_restack(FtkWndManager* thiz, FtkWidget* window, int offset)
{
	return_val_if_fail(thiz != NULL && thiz->restack != NULL, RET_FAIL);

	return thiz->restack(thiz, window, offset);
}

static inline Ret  ftk_wnd_manager_grab(FtkWndManager* thiz, FtkWidget* window)
{
	return_val_if_fail(thiz != NULL && thiz->grab != NULL, RET_FAIL);

	return thiz->grab(thiz, window);
}

static inline Ret  ftk_wnd_manager_ungrab(FtkWndManager* thiz, FtkWidget* window)
{
	return_val_if_fail(thiz != NULL && thiz->ungrab != NULL, RET_FAIL);

	return thiz->ungrab(thiz, window);
}

static inline Ret  ftk_wnd_manager_add(FtkWndManager* thiz, FtkWidget* window)
{
	return_val_if_fail(thiz != NULL && thiz->add != NULL, RET_FAIL);

	return thiz->add(thiz, window);
}

static inline Ret  ftk_wnd_manager_remove(FtkWndManager* thiz, FtkWidget* window)
{
	return_val_if_fail(thiz != NULL && thiz->remove != NULL, RET_FAIL);

	return thiz->remove(thiz, window);
}

static inline Ret  ftk_wnd_manager_update(FtkWndManager* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->update != NULL, RET_FAIL);

	return thiz->update(thiz);
}

static inline Ret  ftk_wnd_manager_get_work_area(FtkWndManager* thiz, FtkRect* rect)
{
	return_val_if_fail(thiz != NULL && thiz->get_work_area != NULL && rect != NULL, RET_FAIL);

	return thiz->get_work_area(thiz, rect);
}

static inline Ret  ftk_wnd_manager_queue_event(FtkWndManager* thiz, FtkEvent* event)
{
	return_val_if_fail(thiz != NULL && thiz->queue_event != NULL, RET_FAIL);

	return thiz->queue_event(thiz, event);
}

static inline Ret  ftk_wnd_manager_dispatch_event(FtkWndManager* thiz, FtkEvent* event)
{
	return_val_if_fail(thiz != NULL && thiz->dispatch_event != NULL, RET_FAIL);

	return thiz->dispatch_event(thiz, event);
}

static inline Ret  ftk_wnd_manager_add_global_listener(FtkWndManager* thiz, FtkListener listener, void* ctx)
{
	return_val_if_fail(thiz != NULL && thiz->add_global_listener != NULL, RET_FAIL);

	return thiz->add_global_listener(thiz, listener, ctx);
}

static inline Ret  ftk_wnd_manager_remove_global_listener(FtkWndManager* thiz, FtkListener listener, void* ctx)
{
	return_val_if_fail(thiz != NULL && thiz->remove_global_listener != NULL, RET_FAIL);

	return thiz->remove_global_listener(thiz, listener, ctx);
}

static inline void ftk_wnd_manager_destroy(FtkWndManager* thiz)
{
	return_if_fail(thiz != NULL && thiz->destroy != NULL);

	thiz->destroy(thiz);

	return;
}

Ret  ftk_wnd_manager_set_rotate(FtkWndManager* thiz, FtkRotate rotate);
Ret  ftk_wnd_manager_queue_event_auto_rotate(FtkWndManager* thiz, FtkEvent* event);

FTK_END_DECLS

#endif/*FTK_WND_MANAGER_H*/

