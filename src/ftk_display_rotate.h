/*
 * File: ftk_display_rotate.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   decorate display to support screen rotation.
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
 * 2010-07-10 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#ifndef FTK_DISPLAY_ROTATE_H
#define FTK_DISPLAY_ROTATE_H
#include "ftk_display.h"

FTK_BEGIN_DECLS

FtkDisplay* ftk_display_rotate_create(FtkDisplay* display, FtkRotate rotate);

FtkRotate ftk_display_get_rotate(FtkDisplay* thiz);
Ret ftk_display_set_rotate(FtkDisplay* thiz, FtkRotate rotate);
FtkDisplay* ftk_display_get_real_display(FtkDisplay* thiz);

FTK_END_DECLS

#endif/*FTK_DISPLAY_ROTATE_H*/

