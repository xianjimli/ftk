/*
 * File: ftk_input_method_preeditor.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   input candidate selector.
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
 * 2010-02-11 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_INPUT_METHOD_PREEDITOR_H
#define FTK_INPUT_METHOD_PREEDITOR_H

#include "ftk_widget.h"

FTK_BEGIN_DECLS

struct _FtkImPreeditor;
typedef struct _FtkImPreeditor FtkImPreeditor;

typedef Ret  (*FtkInputMethodPreeditorHide)(FtkImPreeditor* thiz);
typedef Ret  (*FtkInputMethodPreeditorReset)(FtkImPreeditor* thiz);
typedef Ret  (*FtkInputMethodPreeditorSetEditor)(FtkImPreeditor* thiz, FtkWidget* editor);
typedef Ret  (*FtkInputMethodPreeditorSetRawText)(FtkImPreeditor* thiz, const char* text);
typedef Ret  (*FtkInputMethodPreeditorAddCandidate)(FtkImPreeditor* thiz, const char* text);
typedef Ret  (*FtkInputMethodPreeditorShow)(FtkImPreeditor* thiz, FtkPoint* caret);
typedef void (*FtkInputMethodPreeditorDestroy)(FtkImPreeditor* thiz);

struct _FtkImPreeditor
{
	FtkInputMethodPreeditorHide         hide;
	FtkInputMethodPreeditorShow         show;
	FtkInputMethodPreeditorReset        reset;
	FtkInputMethodPreeditorSetEditor    set_editor;
	FtkInputMethodPreeditorSetRawText   set_raw_text;
	FtkInputMethodPreeditorAddCandidate add_candidate;
	FtkInputMethodPreeditorDestroy      destroy;

	char priv[ZERO_LEN_ARRAY];
};

static inline Ret ftk_input_method_preeditor_reset(FtkImPreeditor* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->reset != NULL, RET_FAIL);

	return thiz->reset(thiz);
}

static inline Ret ftk_input_method_preeditor_set_editor(FtkImPreeditor* thiz, FtkWidget* editor)
{
	return_val_if_fail(thiz != NULL && thiz->set_editor != NULL, RET_FAIL);

	return thiz->set_editor(thiz, editor);
}

static inline Ret ftk_input_method_preeditor_set_raw_text(FtkImPreeditor* thiz, const char* text)
{
	return_val_if_fail(thiz != NULL && thiz->set_raw_text != NULL, RET_FAIL);

	return thiz->set_raw_text(thiz, text);
}

static inline Ret ftk_input_method_preeditor_add_candidate(FtkImPreeditor* thiz, const char* text)
{
	return_val_if_fail(thiz != NULL && thiz->add_candidate != NULL, RET_FAIL);

	return thiz->add_candidate(thiz, text);
}

static inline Ret ftk_input_method_preeditor_hide(FtkImPreeditor* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->hide != NULL, RET_FAIL);

	return thiz->hide(thiz);
}

static inline Ret ftk_input_method_preeditor_show(FtkImPreeditor* thiz, FtkPoint* caret)
{
	return_val_if_fail(thiz != NULL && thiz->show != NULL, RET_FAIL);

	return thiz->show(thiz, caret);
}

static inline void ftk_input_method_preeditor_destroy(FtkImPreeditor* thiz)
{
	if(thiz != NULL && thiz->destroy != NULL)
	{
		thiz->destroy(thiz);
	}

	return;
}

Ret ftk_im_show_preeditor(FtkWidget* editor, FtkPoint* caret_pos, FtkCommitInfo* info);

FTK_END_DECLS

#endif/*FTK_INPUT_METHOD_PREEDITOR_H*/

