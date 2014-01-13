/*
 * File: ftk_animation.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   animation interface for animation.
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
 * 2011-03-20 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_animation.h"
#include "ftk_interpolator_bounce.h"
#include "ftk_interpolator_linear.h"
#include "ftk_interpolator_accelerate.h"
#include "ftk_interpolator_decelerate.h"
#include "ftk_interpolator_acc_decelerate.h"

Ret   ftk_animation_set_name(FtkAnimation* thiz, const char* name)
{
	return_val_if_fail(thiz != NULL && name != NULL, RET_FAIL);

	ftk_strncpy(thiz->name, name, sizeof(thiz->name));

	return RET_OK;
}

Ret   ftk_animation_set_var(FtkAnimation* thiz, const char* name, const char* value)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);
	
	if(thiz->params == NULL)
	{
		thiz->params = ftk_params_create(16, 16);
	}

	return ftk_params_set_var(thiz->params, name, value);
}

Ret   ftk_animation_set_param(FtkAnimation* thiz, const char* name, const char* value)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);
	
	if(thiz->params == NULL)
	{
		thiz->params = ftk_params_create(16, 16);
	}

	return ftk_params_set_param(thiz->params, name, value);
}

const char* ftk_animation_get_param(FtkAnimation* thiz, const char* name)
{
	return_val_if_fail(thiz != NULL, NULL);

	return ftk_params_eval_string(thiz->params, name);
}

int   ftk_animation_get_param_int(FtkAnimation* thiz, const char* name, int defval)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	return ftk_params_eval_int(thiz->params, name, defval);
}

float ftk_animation_get_param_float(FtkAnimation* thiz, const char* name, float defval)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	return ftk_params_eval_float(thiz->params, name, defval);
}


Ret   ftk_animation_step(FtkAnimation* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->step != NULL, RET_FAIL);

	return thiz->step(thiz);
}

float ftk_animation_get_percent(FtkAnimation* thiz)
{
	float time_percent = 0;
	unsigned long pass = 0;
	unsigned long duration = ftk_animation_get_param_int(thiz, "duration", 0);
	return_val_if_fail(thiz != NULL && duration > 0, 1.0);

	pass = ftk_get_relative_time() - thiz->start_time;

	pass = FTK_MIN(pass, duration);
	time_percent = (float)pass/(float)duration;

	if(thiz->interpolator != NULL)
	{
		return ftk_interpolator_get(thiz->interpolator, time_percent); 
	}
	else
	{
		return time_percent;
	}
}

static Ret ftk_animation_ensure_interpolator(FtkAnimation* thiz)
{
	const char* name = NULL;

	if(thiz->interpolator != NULL)
	{
		return RET_OK;
	}

	if((name = ftk_params_eval_string(thiz->params, "interpolator")) != NULL)
	{

		if(strcmp(name, "linear") == 0)
		{
			thiz->interpolator = ftk_interpolator_linear_create();
		}
		else if(strcmp(name, "bounce") == 0)
		{
			thiz->interpolator = ftk_interpolator_bounce_create();
		}
		else if(strcmp(name, "accelerate") == 0)
		{
			thiz->interpolator = ftk_interpolator_accelerate_create();
		}
		else if(strcmp(name, "decelerate") == 0)
		{
			thiz->interpolator = ftk_interpolator_decelerate_create();
		}
		else if(strcmp(name, "acc_decelerate") == 0)
		{
			thiz->interpolator = ftk_interpolator_acc_decelerate_create();
		}
		else
		{
			ftk_logd("%s is not a valid interpolator.\n", name);
		}
	}

	return RET_OK;
}

Ret ftk_animation_run(FtkAnimation* thiz, FtkBitmap* old_win, 
	FtkBitmap* new_win, FtkRect* old_win_rect, FtkRect* new_win_rect)
{
	int i = 0;
	return_val_if_fail(thiz != NULL, RET_FAIL);

	ftk_animation_ensure_interpolator(thiz);
	ftk_animation_reset(thiz, old_win, new_win, old_win_rect, new_win_rect);

	while(ftk_animation_step(thiz) == RET_OK)
	{
		i++;
		if(ftk_animation_get_percent(thiz) >= 1.0)
		{
			break;
		}
	}
	ftk_logd("%s: %s show %d frames\n", __func__, thiz->name, i);

	return RET_OK;
}

Ret ftk_animation_reset(FtkAnimation* thiz, FtkBitmap* old_win, 
	FtkBitmap* new_win,	FtkRect* old_win_rect, FtkRect* new_win_rect)
{
	return_val_if_fail(thiz != NULL && thiz->reset != NULL, RET_FAIL);

	thiz->start_time = ftk_get_relative_time();

	return thiz->reset(thiz, old_win, new_win, old_win_rect, new_win_rect);
}

void ftk_animation_dump(FtkAnimation* thiz)
{
	if(thiz != NULL)
	{
		ftk_logd("%s: %s\n", __func__, thiz->name);
		ftk_params_dump(thiz->params);
	}

	return;
}

void ftk_animation_destroy(FtkAnimation* thiz)
{
	if(thiz != NULL)
	{
		ftk_interpolator_destroy(thiz->interpolator);
		ftk_params_destroy(thiz->params);

		if(thiz->destroy != NULL)
		{
			thiz->destroy(thiz);
		}
	}

	return;
}

