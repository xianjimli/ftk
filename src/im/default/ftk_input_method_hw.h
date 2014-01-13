/*
 * File: ftk_input_method_hw.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   handwrite input method.
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
 * 2010-02-08 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_INPUT_METHOD_HW_H
#define FTK_INPUT_METHOD_HW_H

#include "ftk_input_method.h"

FTK_BEGIN_DECLS

FtkInputMethod* ftk_input_method_hw_create(void);

Ret ftk_input_method_hw_set_rect(FtkInputMethod* thiz, FtkRect* rect);
Ret ftk_input_method_hw_set_pen_width(FtkInputMethod* thiz, int width);
Ret ftk_input_method_hw_set_pen_color(FtkInputMethod* thiz, FtkColor);
Ret ftk_input_method_hw_set_click_timeout(FtkInputMethod* thiz, int ms);
Ret ftk_input_method_hw_set_commit_timeout(FtkInputMethod* thiz, int ms);

FTK_END_DECLS

#endif/*FTK_INPUT_METHOD_HW_H*/

