/*
 * File: ftk_animation.h    
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

#ifndef FTK_ANIMATION_H
#define FTK_ANIMATION_H

#include "ftk_params.h"
#include "ftk_bitmap.h"
#include "ftk_display.h"
#include "ftk_interpolator.h"

FTK_BEGIN_DECLS

struct _FtkAnimation;
typedef struct _FtkAnimation FtkAnimation;

typedef FtkAnimation* (*FtkAnimationCreate)();
typedef Ret   (*FtkAnimationStep)(FtkAnimation* thiz);
typedef Ret   (*FtkAnimationReset)(FtkAnimation* thiz, FtkBitmap* old_win, FtkBitmap* new_win,
				FtkRect* old_win_rect, FtkRect* new_win_rect);
typedef void  (*FtkAnimationDestroy)(FtkAnimation* thiz);

struct _FtkAnimation
{
	FtkAnimationStep     step;
	FtkAnimationReset    reset;
	FtkAnimationDestroy  destroy;

	char name[32];
	long start_time;
	FtkParams* params;
	FtkInterpolator* interpolator;
	char priv[ZERO_LEN_ARRAY];
};

Ret   ftk_animation_step(FtkAnimation* thiz);
float ftk_animation_get_percent(FtkAnimation* thiz);
Ret   ftk_animation_set_name(FtkAnimation* thiz, const char* name);
Ret   ftk_animation_set_var(FtkAnimation* thiz, const char* name, const char* value);
Ret   ftk_animation_set_param(FtkAnimation* thiz, const char* name, const char* value);
const char* ftk_animation_get_param(FtkAnimation* thiz, const char* name);
int   ftk_animation_get_param_int(FtkAnimation* thiz, const char* name, int defval);
float ftk_animation_get_param_float(FtkAnimation* thiz, const char* name, float defval);
Ret   ftk_animation_run(FtkAnimation* thiz, FtkBitmap* old_win, FtkBitmap* new_win,
		FtkRect* old_win_rect, FtkRect* new_win_rect);
Ret   ftk_animation_reset(FtkAnimation* thiz, FtkBitmap* old_win, FtkBitmap* new_win,
		FtkRect* old_win_rect, FtkRect* new_win_rect);
void ftk_animation_dump(FtkAnimation* thiz);
void ftk_animation_destroy(FtkAnimation* thiz);

FTK_END_DECLS

#endif/*FTK_ANIMATION_H*/

