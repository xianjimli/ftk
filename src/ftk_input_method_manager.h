/*
 * File: ftk_input_method_manager.h    
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

#ifndef FTK_INPUT_METHOD_MANAGER_H
#define FTK_INPUT_METHOD_MANAGER_H 

#include "ftk_input_method.h"
#include "ftk_input_method_chooser.h"

FTK_BEGIN_DECLS

struct _FtkInputMethodManager;
typedef struct _FtkInputMethodManager FtkInputMethodManager;

FtkInputMethodManager* ftk_input_method_manager_create(void);
size_t  ftk_input_method_manager_count(FtkInputMethodManager* thiz);
Ret  ftk_input_method_manager_get(FtkInputMethodManager* thiz, size_t index, FtkInputMethod** im);
Ret  ftk_input_method_manager_get_current(FtkInputMethodManager* thiz, FtkInputMethod** im);
Ret  ftk_input_method_manager_set_current(FtkInputMethodManager* thiz, size_t index);
Ret  ftk_input_method_manager_set_current_type(FtkInputMethodManager* thiz, FtkInputType type);

Ret  ftk_input_method_manager_register(FtkInputMethodManager* thiz, FtkInputMethod* im);
Ret  ftk_input_method_manager_unregister(FtkInputMethodManager* thiz, FtkInputMethod* im);
void ftk_input_method_manager_destroy(FtkInputMethodManager* thiz);

Ret  ftk_input_method_manager_focus_out(FtkInputMethodManager* thiz, FtkWidget* widget);
Ret  ftk_input_method_manager_focus_ack_commit(FtkInputMethodManager* thiz);
Ret  ftk_input_method_manager_focus_in(FtkInputMethodManager* thiz, FtkWidget* widget);

FTK_END_DECLS

#endif/*FTK_INPUT_METHOD_MANAGER_H*/

