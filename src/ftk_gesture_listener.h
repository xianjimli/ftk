/*
 * File: ftk_gesture_listener.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   gesture listener
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

#ifndef FTK_GESTURE_LISTENER_H
#define FTK_GESTURE_LISTENER_H

#include "ftk_event.h"
#include "ftk_log.h"

struct _FtkGestureListener;
typedef struct _FtkGestureListener FtkGestureListener;

typedef Ret (*FtkGestureListenerOnClicked)(FtkGestureListener* thiz, FtkEvent* event);
typedef Ret (*FtkGestureListenerOnDblClicked)(FtkGestureListener* thiz, FtkEvent* event);
typedef Ret (*FtkGestureListenerOnLongPressed)(FtkGestureListener* thiz, FtkEvent* event);
typedef Ret (*FtkGestureListenerOnFling)(FtkGestureListener* thiz, 
		FtkEvent* e1, FtkEvent* e2, int velocity_x, int velocity_y);
typedef Ret (*FtkGestureListenerOnScroll)(FtkGestureListener* thiz, 
		FtkEvent* e1, FtkEvent* e2, int distance_x, int distance_y);
typedef void (*FtkGestureListenerDestroy)(FtkGestureListener* thiz);

struct _FtkGestureListener
{
	FtkGestureListenerOnFling       on_fling;
	FtkGestureListenerOnScroll      on_scroll;
	FtkGestureListenerOnClicked     on_clicked;
	FtkGestureListenerOnDblClicked  on_dbl_clicked;
	FtkGestureListenerOnLongPressed on_long_pressed;
	FtkGestureListenerDestroy       destroy;

	char priv[ZERO_LEN_ARRAY];
};

static inline Ret ftk_gesture_listener_on_clicked(FtkGestureListener* thiz, FtkEvent* event)
{
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL, ret);
	
	if(thiz->on_clicked != NULL)
	{
		ret = thiz->on_clicked(thiz, event);
	}

	return ret;
}

static inline Ret ftk_gesture_listener_on_dbl_clicked(FtkGestureListener* thiz, FtkEvent* event)
{
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL, ret);
	
	if(thiz->on_dbl_clicked != NULL)
	{
		ret = thiz->on_dbl_clicked(thiz, event);
	}

	return ret;
}

static inline Ret ftk_gesture_listener_on_long_pressed(FtkGestureListener* thiz, FtkEvent* event)
{
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL, ret);
	
	if(thiz->on_long_pressed != NULL)
	{
		ret = thiz->on_long_pressed(thiz, event);
	}

	return ret;
}

static inline Ret ftk_gesture_listener_on_fling(FtkGestureListener* thiz, 
		FtkEvent* e1, FtkEvent* e2, int velocity_x, int velocity_y)
{
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL, ret);
	
	if(thiz->on_fling != NULL)
	{
		ret = thiz->on_fling(thiz, e1, e2, velocity_x, velocity_y);
	}

	return ret;
}

static inline Ret ftk_gesture_listener_on_scroll(FtkGestureListener* thiz, 
		FtkEvent* e1, FtkEvent* e2, int distance_x, int distance_y)
{
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL, ret);
	
	if(thiz->on_scroll != NULL)
	{
		ret = thiz->on_scroll(thiz, e1, e2, distance_x, distance_y);
	}

	return ret;
}

static inline void ftk_gesture_listener_destroy(FtkGestureListener* thiz)
{
	if(thiz != NULL && thiz->destroy != NULL)
	{
		thiz->destroy(thiz);
	}

	return;
}

#endif/*FTK_GESTURE_LISTENER_H*/

