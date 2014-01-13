/*
 * File: ftk_input_method.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   input method interface.
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

#ifndef FTK_INPUT_METHOD_H
#define FTK_INPUT_METHOD_H

#include "ftk_widget.h"

FTK_BEGIN_DECLS

typedef enum _FtkInputType
{
	FTK_INPUT_NORMAL= 0,
	FTK_INPUT_DIGIT,
	FTK_INPUT_ALPHA,
	FTK_INPUT_URL,
	FTK_INPUT_EMAIL,
	FTK_INPUT_ALL
}FtkInputType;

#define FTK_IM_MAX_CANDIDATES  20

struct _FtkInputMethod;
typedef struct _FtkInputMethod FtkInputMethod;

typedef Ret  (*FtkInputMethodFocusIn)(FtkInputMethod* thiz, FtkWidget* editor);
typedef Ret  (*FtkInputMethodFocusOut)(FtkInputMethod* thiz);
typedef Ret  (*FtkInputMethodSetType)(FtkInputMethod* thiz, FtkInputType type);
typedef Ret  (*FtkInputMethodHandleEvent)(FtkInputMethod* thiz, FtkEvent* event);
typedef void (*FtkInputMethodDestroy)(FtkInputMethod* thiz);

struct _FtkInputMethod
{
	const char* name;

	int ref;

	FtkInputMethodFocusIn     focus_in;
	FtkInputMethodFocusOut    focus_out;
	FtkInputMethodSetType     set_type;
	FtkInputMethodHandleEvent handle_event;
	FtkInputMethodDestroy     destroy;

	char priv[ZERO_LEN_ARRAY];
};

static inline Ret  ftk_input_method_focus_in(FtkInputMethod* thiz, FtkWidget* editor)
{
	return_val_if_fail(thiz != NULL && thiz->focus_in != NULL, RET_FAIL);

	return thiz->focus_in(thiz, editor);
}

static inline Ret  ftk_input_method_focus_out(FtkInputMethod* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->focus_out != NULL, RET_FAIL);

	return thiz->focus_out(thiz);
}

static inline Ret  ftk_input_method_set_type(FtkInputMethod* thiz, FtkInputType type)
{
	return_val_if_fail(thiz != NULL && thiz->set_type != NULL, RET_FAIL);

	return thiz->set_type(thiz, type);
}

static inline Ret  ftk_input_method_handle_event(FtkInputMethod* thiz, FtkEvent* event)
{
	return_val_if_fail(thiz != NULL && thiz->handle_event != NULL, RET_FAIL);

	return thiz->handle_event(thiz, event);
}

static inline void ftk_input_method_destroy(FtkInputMethod* thiz)
{
	if(thiz != NULL && thiz->destroy != NULL)
	{
		thiz->destroy(thiz);
	}

	return;
}

static inline void ftk_input_method_ref(FtkInputMethod* thiz)
{
	if(thiz != NULL)
	{
		thiz->ref++;
	}

	return;
}

static inline void ftk_input_method_unref(FtkInputMethod* thiz)
{
	if(thiz != NULL)
	{
		thiz->ref--;
		if(thiz->ref == 0)
		{
			ftk_input_method_destroy(thiz);	
		}
		else
		{
			assert(thiz->ref > 0);
		}
	}

	return;
}

FTK_END_DECLS

#endif/*FTK_INPUT_METHOD_H*/

