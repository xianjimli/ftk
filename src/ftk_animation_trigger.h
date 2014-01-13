/*
 * File: ftk_animation_trigger.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   animation trigger interface
 *
 * Copyright (c) 2009 - 2011  Li XianJing <xianjimli@hotmail.com>
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
 * 2011-03-21 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_ANIMATION_TRIGGER_H
#define FTK_ANIMATION_TRIGGER_H

#include "ftk_widget.h"

FTK_BEGIN_DECLS

struct _FtkAnimationTrigger;
typedef struct _FtkAnimationTrigger FtkAnimationTrigger;

typedef struct _FtkAnimationEvent
{
	int type;
	FtkWidget* old_window;
	FtkWidget* new_window;
}FtkAnimationEvent;

typedef Ret (*FtkAnimationTriggerOnEvent)(FtkAnimationTrigger* thiz, FtkAnimationEvent* evt);
typedef void (*FtkAnimationTriggerDestroy)(FtkAnimationTrigger* thiz);

struct _FtkAnimationTrigger
{
	FtkAnimationTriggerOnEvent  on_event;
	FtkAnimationTriggerDestroy  destroy;

	char priv[ZERO_LEN_ARRAY];
};

static inline Ret ftk_animation_trigger_on_event(FtkAnimationTrigger* thiz, FtkAnimationEvent* evt)
{
	return_val_if_fail(thiz != NULL && thiz->on_event != NULL, RET_FAIL);

	return thiz->on_event(thiz, evt);
}

static inline void ftk_animation_trigger_destroy(FtkAnimationTrigger* thiz)
{
	if(thiz != NULL && thiz->destroy != NULL)
	{
		thiz->destroy(thiz);
	}

	return;
}

FTK_END_DECLS

#endif/*FTK_ANIMATION_TRIGGER_H*/

