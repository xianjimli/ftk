/*
 * File: ftk_input_method_manager.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   input method manager.
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
 * 2010-01-30 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_allocator.h"
#include "ftk_input_method_manager.h"

#ifdef USE_HANDWRITE
#include "ftk_input_method_hw.h"
#endif

#ifdef IPHONE
#include "ftk_input_method_iphone.h"
#define IME_CREATE ftk_input_method_iphone_create 
#elif defined(ANDROID) && defined(ANDROID_NDK)
#define IME_CREATE ftk_input_method_android_create 
#include "ftk_input_method_android.h"
#elif defined(USE_GPINYIN)
#include "ftk_input_method_gpinyin.h"
#define IME_CREATE ftk_input_method_gpinyin_create 
#elif defined(WINCE)
#include "ftk_input_method_wince.h"
#define IME_CREATE ftk_input_method_wince_create
#elif defined(UCOS_SIM) || defined(RT_THREAD)
#define IME_CREATE NULL
#elif defined(WIN32)
#include "ftk_input_method_win32.h"
#define IME_CREATE ftk_input_method_win32_create
#endif

#define FTK_INPUT_METHOD_MAX_NR 6

struct _FtkInputMethodManager
{
	size_t nr;
	size_t current;
	FtkWidget* widget;
	FtkInputMethod* methods[FTK_INPUT_METHOD_MAX_NR+1];
};

FtkInputMethodManager* ftk_input_method_manager_create(void)
{
	FtkInputMethodManager* thiz = FTK_NEW(FtkInputMethodManager);
	if(thiz != NULL)
	{
		thiz->current = 0;
#if !(defined(UCOS_SIM) || defined(RT_THREAD))
#ifdef IME_CREATE
		ftk_input_method_manager_register(thiz, IME_CREATE());
#endif
#endif

#ifdef USE_HANDWRITE
		ftk_input_method_manager_register(thiz, ftk_input_method_hw_create());
#endif

	}
	return thiz;
}

size_t  ftk_input_method_manager_count(FtkInputMethodManager* thiz)
{
	return thiz != NULL ? thiz->nr : 0;
}

Ret  ftk_input_method_manager_get(FtkInputMethodManager* thiz, size_t index, FtkInputMethod** im)
{
	return_val_if_fail(thiz != NULL && im != NULL, RET_FAIL);

	if(index >= thiz->nr)
	{
		*im = NULL;
		return RET_FAIL;
	}

	*im = thiz->methods[index];

	return RET_OK;
}

Ret  ftk_input_method_manager_get_current(FtkInputMethodManager* thiz, FtkInputMethod** im)
{
	return ftk_input_method_manager_get(thiz, thiz->current, im);
}

Ret  ftk_input_method_manager_set_current(FtkInputMethodManager* thiz, size_t index)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	thiz->current = index;

	return RET_OK;
}

Ret  ftk_input_method_manager_set_current_type(FtkInputMethodManager* thiz, FtkInputType type)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(thiz->current < thiz->nr)
	{
		ftk_input_method_set_type(thiz->methods[thiz->current], type);
	}

	return RET_OK;
}

Ret  ftk_input_method_manager_register(FtkInputMethodManager* thiz, FtkInputMethod* im)
{
	return_val_if_fail(thiz != NULL && im != NULL && thiz->nr < FTK_INPUT_METHOD_MAX_NR, RET_FAIL);

	thiz->methods[thiz->nr] = im;
	thiz->nr++;

	return RET_OK;
}

Ret  ftk_input_method_manager_unregister(FtkInputMethodManager* thiz, FtkInputMethod* im)
{
	size_t i = 0;
	return_val_if_fail(thiz != NULL && im != NULL, RET_FAIL);

	for(i = 0; i < thiz->nr; i++)
	{
		if(thiz->methods[i] == im)
		{
			ftk_input_method_destroy(thiz->methods[i]);
			thiz->methods[i] = NULL;
			break;
		}
	}
	
	for(; i < thiz->nr; i++)
	{
		thiz->methods[i] = thiz->methods[i+1];
	}

	return RET_OK;
}

void ftk_input_method_manager_destroy(FtkInputMethodManager* thiz)
{
	size_t i = 0;
	if(thiz != NULL)
	{
		for(i = 0; i < thiz->nr; i++)
		{
			ftk_input_method_destroy(thiz->methods[i]);
			thiz->methods[i] = NULL;
		}
		
		FTK_ZFREE(thiz, sizeof(FtkInputMethodManager));
	}

	return;
}

Ret  ftk_input_method_manager_focus_in(FtkInputMethodManager* thiz, FtkWidget* widget)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(thiz->current < thiz->nr)
	{
		if(ftk_input_method_focus_in(thiz->methods[thiz->current], widget) == RET_OK)
		{
			thiz->widget = widget;
		}
	}

	return RET_OK;
}

Ret  ftk_input_method_manager_focus_out(FtkInputMethodManager* thiz, FtkWidget* widget)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(thiz->current < thiz->nr && thiz->widget == widget)
	{
		thiz->widget = NULL;
		ftk_input_method_focus_out(thiz->methods[thiz->current]);
	}

	return RET_OK;
}

Ret  ftk_input_method_manager_focus_ack_commit(FtkInputMethodManager* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(thiz->current < thiz->nr)
	{
		FtkEvent event;
		ftk_event_init(&event, FTK_EVT_IM_ACT_COMMIT);
		ftk_input_method_handle_event(thiz->methods[thiz->current], &event);
	}

	return RET_OK;
}
