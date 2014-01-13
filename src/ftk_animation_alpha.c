/*
 * File: ftk_animation_alpha.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   alpha animation
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
 * 2011-03-26 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_globals.h"
#include "ftk_animation_alpha.h"

typedef struct _AnimationAlphaPrivInfo
{
	float end;
	float start;
	int duration;
	FtkBitmap* back_win;
	FtkBitmap* front_win;
	FtkRect back_win_rect;
	FtkRect front_win_rect;
}PrivInfo;

static Ret   ftk_animation_alpha_step(FtkAnimation* thiz)
{
	FtkRect r = {0};
	float percent = 0;
	DECL_PRIV(thiz, priv);

	r = priv->back_win_rect;
	if(r.width > 0 && r.height > 0)
	{
		ftk_canvas_draw_bitmap(ftk_shared_canvas(), priv->back_win, &r, &r, 0xff);
	}

	percent = priv->start + (priv->end - priv->start) * ftk_animation_get_percent(thiz);

	r = priv->front_win_rect;
	if(r.width > 0 && r.height > 0)
	{
		int alpha = (int)(0xff * percent) & 0xff;
		ftk_canvas_draw_bitmap(ftk_shared_canvas(), priv->front_win, &r, &r, alpha);
	}

	ftk_canvas_show(ftk_shared_canvas(), ftk_default_display(), &r, r.x, r.y);

	return RET_OK;
}

static Ret   ftk_animation_alpha_reset(FtkAnimation* thiz, FtkBitmap* old_win, FtkBitmap* new_win,
	FtkRect* old_win_rect, FtkRect* new_win_rect)
{
	DECL_PRIV(thiz, priv);
	const char* src = ftk_animation_get_param(thiz, "src");
	return_val_if_fail(src != NULL, RET_FAIL);

	priv->start = ftk_animation_get_param_float(thiz, "from", 1.0);
	priv->end = ftk_animation_get_param_float(thiz, "to", 1.0);

	if(strstr(src, "new_window") != NULL)
	{
		priv->back_win = old_win;
		priv->back_win_rect = *old_win_rect;
		
		priv->front_win = new_win;
		priv->front_win_rect = *new_win_rect;
	}
	else
	{
		priv->back_win = new_win;
		priv->back_win_rect = *new_win_rect;
		
		priv->front_win = old_win;
		priv->front_win_rect = *old_win_rect;
	}

	ftk_logd("%s: src=%s start=%f end=%f old(%d %d %d %d) new(%d %d %d %d)",
		__func__, src, priv->start, priv->end, 
		old_win_rect->x, old_win_rect->y, old_win_rect->width, old_win_rect->height,
		new_win_rect->x, new_win_rect->y, new_win_rect->width, new_win_rect->height);

	return RET_OK;
}

static void  ftk_animation_alpha_destroy(FtkAnimation* thiz)
{
	FTK_FREE(thiz);

	return;
}

FtkAnimation* ftk_animation_alpha_create(void)
{
	FtkAnimation* thiz = FTK_NEW_PRIV(FtkAnimation);

	if(thiz != NULL)
	{
		ftk_animation_set_name(thiz,  "alpha");
		thiz->step      = ftk_animation_alpha_step;
		thiz->reset     = ftk_animation_alpha_reset;
		thiz->destroy   = ftk_animation_alpha_destroy;
	}

	return thiz;
}

