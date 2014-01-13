/*
 * File: ftk_gesture.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   gesture recognizer 
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
 * 2009-11-19 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_GESTURE_H
#define FTK_GESTURE_H

#include "ftk_gesture_listener.h"

struct _FtkGesture;
typedef struct _FtkGesture FtkGesture;

FtkGesture* ftk_gesture_create(FtkGestureListener* listener);
Ret  ftk_gesture_dispatch(FtkGesture* thiz, FtkEvent* event);
void ftk_gesture_destroy(FtkGesture* thiz);

#endif/*FTK_GESTURE_H*/

