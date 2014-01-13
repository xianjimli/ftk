/*
 * File: ftk_gesture.c
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

#include "ftk_gesture.h"
#include "ftk_allocator.h"

struct _FtkGesture
{
	FtkGestureListener* listener;
};

FtkGesture* ftk_gesture_create(FtkGestureListener* listener)
{
	FtkGesture* thiz = FTK_NEW(FtkGesture);

	if(thiz != NULL)
	{
		thiz->listener = listener;
	}

	return thiz;
}

Ret  ftk_gesture_dispatch(FtkGesture* thiz, FtkEvent* event)
{
	/*TODO*/

	return RET_OK;
}

void ftk_gesture_destroy(FtkGesture* thiz)
{
	if(thiz != NULL)
	{
		ftk_gesture_listener_destroy(thiz->listener);
		FTK_ZFREE(thiz, sizeof(FtkGesture));
	}

	return;
}



