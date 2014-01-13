/*
 * File: ftk_tester.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   event record/replay
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
 * 2010-09-04 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include <byteswap.h>
#include "ftk_tester.h"
#include "ftk_file_system.h"

static int ftk_host_is_le(void)
{
	int val = 0x11223344;
	unsigned char* p = (unsigned char*)&val;

	return (*p == 0x44);
}

static inline int ftk_htole32(int val)
{
	if(!ftk_host_is_le())
	{
		val = bswap_32(val);
	}

	return val;
}

static inline short ftk_htole16(short val)
{
	if(!ftk_host_is_le())
	{
		val = bswap_16(val);
	}

	return val;
}

static inline int ftk_le32toh(int val)
{
	if(!ftk_host_is_le())
	{
		val = bswap_32(val);
	}

	return val;
}

static inline short ftk_le16toh(short val)
{
	if(!ftk_host_is_le())
	{
		val = bswap_16(val);
	}

	return val;
}

static Ret ftk_event_tole(FtkEvent* event)
{
	event->time = ftk_htole32(event->time);
	event->type = ftk_htole32(event->type);
	if(event->type == FTK_EVT_KEY_DOWN || event->type == FTK_EVT_KEY_UP)
	{
		event->u.key.code = ftk_htole32(event->u.key.code);
	}

	if(event->type == FTK_EVT_MOUSE_MOVE || event->type == FTK_EVT_MOUSE_DOWN 
		|| event->type == FTK_EVT_MOUSE_UP)
	{
		event->u.mouse.x = ftk_htole16(event->u.mouse.x);
		event->u.mouse.y = ftk_htole16(event->u.mouse.y);
	}

	return RET_OK;
}

static Ret ftk_event_fromle(FtkEvent* event)
{
	event->time = ftk_le32toh(event->time);
	event->type = ftk_le32toh(event->type);
	if(event->type == FTK_EVT_KEY_DOWN || event->type == FTK_EVT_KEY_UP)
	{
		event->u.key.code = ftk_le32toh(event->u.key.code);
	}

	if(event->type == FTK_EVT_MOUSE_MOVE || event->type == FTK_EVT_MOUSE_DOWN 
		|| event->type == FTK_EVT_MOUSE_UP)
	{
		event->u.mouse.x = ftk_le16toh(event->u.mouse.x);
		event->u.mouse.y = ftk_le16toh(event->u.mouse.y);
	}

	return RET_OK;
}

static Ret ftk_event_record(void* ctx, FtkEvent* event)
{
	FtkFsHandle handle = ctx;
	return_val_if_fail(ctx != NULL && event != NULL, RET_OK);

	if(event->type == FTK_EVT_QUIT)
	{
		ftk_file_close(handle);
		ftk_wnd_manager_remove_global_listener(ftk_default_wnd_manager(), 
			(FtkListener)ftk_event_record, handle);
	}
	else if(event->type == FTK_EVT_KEY_UP || event->type == FTK_EVT_KEY_DOWN 
		|| event->type == FTK_EVT_MOUSE_DOWN || event->type == FTK_EVT_MOUSE_UP 
		|| event->type == FTK_EVT_MOUSE_MOVE)
	{
		FtkEvent evt = *event;
		evt.time = ftk_get_relative_time();
		ftk_logd("%s: type=%d\n", __func__, event->type);
		ftk_event_tole(&evt);
		ftk_file_write(handle, &evt, sizeof(FtkEvent));
	}

	return RET_OK;
}

Ret ftk_tester_start_record(const char* filename)
{
	FtkFsHandle handle = ftk_file_open(filename, "wb+");
	return_val_if_fail(handle != NULL, RET_FAIL);

	ftk_wnd_manager_add_global_listener(ftk_default_wnd_manager(), 
		(FtkListener)ftk_event_record, handle);

	ftk_logd("%s\n", __func__);

	return RET_OK;
}

typedef struct _FtkEventInjector
{
	FtkEvent event;
	FtkSource* timer;
	FtkFsHandle handle;
}FtkEventInjector;

static Ret ftk_event_inject(void* ctx)
{
	int len = 0;
	Ret ret = 0;
	time_t last_time = 0;
	FtkEventInjector* injector = ctx;
	return_val_if_fail(ctx != NULL, RET_REMOVE);
	
	if(injector->event.type == FTK_EVT_QUIT)
	{
		ftk_file_close(injector->handle);
		FTK_FREE(injector);
		ret = RET_REMOVE;
	}
	else
	{
		last_time = injector->event.time;
		injector->event.time = ftk_get_relative_time();
		ftk_wnd_manager_dispatch_event(ftk_default_wnd_manager(), &injector->event);

		ftk_logd("%s: type=%d\n", __func__, injector->event.type);

		len = ftk_file_read(injector->handle, &injector->event, sizeof(FtkEvent));
		ftk_event_fromle(&injector->event);
		if(len == sizeof(FtkEvent))
		{
			time_t interval = injector->event.time - last_time;
			ftk_source_timer_modify(injector->timer, interval);
			ret = RET_OK;
		}
		else
		{
			ftk_file_close(injector->handle);
			FTK_FREE(injector);
			ret = RET_REMOVE;
		}
	}

	return ret;
}

Ret ftk_tester_start_play(const char* filename)
{
	Ret ret = RET_FAIL;
	FtkEventInjector* injector = NULL;
	FtkFsHandle handle = ftk_file_open(filename, "r");

	ftk_logd("%s: is litteend=%d\n", __func__, ftk_host_is_le());
	return_val_if_fail(handle != NULL, RET_FAIL);

	if((injector =  FTK_ZALLOC(sizeof(FtkEventInjector))) != NULL)
	{
		injector->handle = handle;
		injector->timer = ftk_source_timer_create(3000, ftk_event_inject, injector);
		ftk_file_read(handle, &injector->event, sizeof(FtkEvent));
		ftk_event_fromle(&injector->event);
		ftk_main_loop_add_source(ftk_default_main_loop(), injector->timer);
		ret = RET_OK;
	}
	else
	{
		ftk_file_close(handle);
	}
	ftk_logd("%s\n", __func__);
	
	return ret;
}

