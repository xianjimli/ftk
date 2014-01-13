/*
 * File: ftk_animation_trigger_silence.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   this animation trigger just keep silence.
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

#include "ftk_log.h"
#include "ftk_animation_trigger.h"

static Ret ftk_animation_trigger_silence_on_event(FtkAnimationTrigger* thiz, FtkAnimationEvent* evt)
{
	ftk_logd("%s: type=%d %s %s\n", __func__, evt->type, 
		evt->old_window != NULL ? ftk_widget_get_text(evt->old_window) : "null",
		evt->new_window != NULL ? ftk_widget_get_text(evt->new_window) : "null");

	return RET_OK;
}

static void ftk_animation_trigger_silence_destroy(FtkAnimationTrigger* thiz)
{
	return;
}

static FtkAnimationTrigger s_animation_trigger_silence =
{
	ftk_animation_trigger_silence_on_event,
	ftk_animation_trigger_silence_destroy	
};

FtkAnimationTrigger* ftk_animation_trigger_silence_create(void)
{
	return &s_animation_trigger_silence;	
}

