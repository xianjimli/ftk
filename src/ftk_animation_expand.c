/*
 * File: ftk_animation_expand.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   expand animation
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
#include "ftk_animation_expand.h"

typedef struct _AnimationExpandPrivInfo
{
	int push;
	float end;
	float start;
	int duration;
	int alpha_enable;
	const char* base;
	FtkBitmap* back_win;
	FtkBitmap* front_win;
	FtkRect back_win_rect;
	FtkRect front_win_rect;
}PrivInfo;

static Ret   ftk_animation_expand_step_base_left(FtkAnimation* thiz, float percent)
{
	FtkRect r = {0};
	FtkRect dst_r = {0};
	DECL_PRIV(thiz, priv);
	
	r = priv->back_win_rect;
	if(r.width > 0 && r.height > 0)
	{
		dst_r = r;
		if(priv->push)
		{
			int offset = (int)(priv->front_win_rect.width * percent);
			dst_r.x = dst_r.x + offset;
			dst_r.width = r.width = r.width - offset;
		}

		ftk_canvas_draw_bitmap(ftk_shared_canvas(), priv->back_win, &r, &dst_r, 0xff);
	}

	r = priv->front_win_rect;
	r.width = (int)(r.width * percent);
	if(r.width > 0 && r.height > 0)
	{
		int alpha = priv->alpha_enable ? (int)(0xff * percent) & 0xff : 0xff;
		dst_r = r;
		ftk_canvas_draw_bitmap(ftk_shared_canvas(), priv->front_win, &r, &dst_r, alpha);
	}

	return RET_OK;
}

static Ret   ftk_animation_expand_step_base_top(FtkAnimation* thiz, float percent)
{
	FtkRect r = {0};
	FtkRect dst_r = {0};
	DECL_PRIV(thiz, priv);
	
	r = priv->back_win_rect;
	if(r.width > 0 && r.height > 0)
	{
		dst_r = r;
		if(priv->push)
		{
			int offset = (int)(priv->front_win_rect.height * percent);
			dst_r.y = dst_r.y + offset;
			dst_r.height = r.height = r.height - offset;
		}

		ftk_canvas_draw_bitmap(ftk_shared_canvas(), priv->back_win, &r, &dst_r, 0xff);
	}

	r = priv->front_win_rect;
	r.height = (int)(r.height * percent);
	if(r.width > 0 && r.height > 0)
	{
		int alpha = priv->alpha_enable ? (int)(0xff * percent) & 0xff : 0xff;
		dst_r = r;
		ftk_canvas_draw_bitmap(ftk_shared_canvas(), priv->front_win, &r, &dst_r, alpha);
	}

	return RET_OK;
}

static Ret   ftk_animation_expand_step_base_right(FtkAnimation* thiz, float percent)
{
	FtkRect r = {0};
	FtkRect dst_r = {0};
	DECL_PRIV(thiz, priv);
	
	r = priv->back_win_rect;
	if(r.width > 0 && r.height > 0)
	{
		dst_r = r;
		if(priv->push)
		{
			int offset = (int)(priv->front_win_rect.width * percent);
			r.x = r.x + offset;
			dst_r.width = r.width = r.width - offset;
		}

		ftk_canvas_draw_bitmap(ftk_shared_canvas(), priv->back_win, &r, &dst_r, 0xff);
	}

	r = priv->front_win_rect;
	r.width = (int)(r.width * percent);
	r.x = priv->front_win_rect.width - r.width;
	if(r.width > 0 && r.height > 0)
	{
		int alpha = priv->alpha_enable ? (int)(0xff * percent) & 0xff : 0xff;
		dst_r = r;
		ftk_canvas_draw_bitmap(ftk_shared_canvas(), priv->front_win, &r, &dst_r, alpha);
	}

	return RET_OK;
}

static Ret   ftk_animation_expand_step_base_bottom(FtkAnimation* thiz, float percent)
{
	FtkRect r = {0};
	FtkRect dst_r = {0};
	DECL_PRIV(thiz, priv);
	
	r = priv->back_win_rect;
	if(r.width > 0 && r.height > 0)
	{
		dst_r = r;
		if(priv->push)
		{
			int offset = (int)(priv->front_win_rect.height * percent);
			r.y = r.y + offset;
			dst_r.height = r.height = r.height - offset;
		}

		ftk_canvas_draw_bitmap(ftk_shared_canvas(), priv->back_win, &r, &dst_r, 0xff);
	}

	r = priv->front_win_rect;
	r.height = (int)(r.height * percent);
	r.y = r.y + priv->front_win_rect.height - r.height;
	if(r.width > 0 && r.height > 0)
	{
		int alpha = priv->alpha_enable ? (int)(0xff * percent) & 0xff : 0xff;
		dst_r = r;
		ftk_canvas_draw_bitmap(ftk_shared_canvas(), priv->front_win, &r, &dst_r, alpha);
	}

	return RET_OK;
}

static Ret   ftk_animation_expand_step_base_center(FtkAnimation* thiz, float percent)
{
	FtkRect r = {0};
	FtkRect dst_r = {0};
	DECL_PRIV(thiz, priv);
	
	r = priv->back_win_rect;
	if(r.width > 0 && r.height > 0)
	{
		ftk_canvas_draw_bitmap(ftk_shared_canvas(), priv->back_win, &r, &r, 0xff);
	}

	r = priv->front_win_rect;
	r.width = (int)(r.width * percent);
	r.height = (int)(r.height * percent);

	if(r.width > 0 && r.height > 0)
	{
		int alpha = priv->alpha_enable ? (int)(0xff * percent) & 0xff : 0xff;
		dst_r = r;
		dst_r.x = r.x + (priv->front_win_rect.width - r.width)/2;
		dst_r.y = r.y + (priv->front_win_rect.height - r.height)/2;
		r.x = dst_r.x;
		r.y = dst_r.y;

		ftk_canvas_draw_bitmap(ftk_shared_canvas(), priv->front_win, &r, &dst_r, alpha);
	}

	return RET_OK;
}

static Ret   ftk_animation_expand_step(FtkAnimation* thiz)
{
	FtkRect rf = {0};
	FtkRect rs = {0};
	float percent = 0;
	DECL_PRIV(thiz, priv);
	const char* base = ftk_animation_get_param(thiz, "base");
	return_val_if_fail(base != NULL, RET_FAIL);
	
	percent = priv->start + (priv->end - priv->start) * ftk_animation_get_percent(thiz);

	if(strcmp(base, "left") == 0)
	{
		ftk_animation_expand_step_base_left(thiz, percent);
	}
	else if(strcmp(base, "top") == 0)
	{
		ftk_animation_expand_step_base_top(thiz, percent);
	}
	else if(strcmp(base, "right") == 0)
	{
		ftk_animation_expand_step_base_right(thiz, percent);
	}
	else if(strcmp(base, "bottom") == 0)
	{
		ftk_animation_expand_step_base_bottom(thiz, percent);
	}
	else
	{
		ftk_animation_expand_step_base_center(thiz, percent);
	}

	rf = priv->back_win_rect;
	rs = priv->front_win_rect;
	rf.x = FTK_MIN(rf.x, rs.x);
	rf.y = FTK_MIN(rf.y, rs.y);
	rf.width = FTK_MAX(rf.width, rs.width);
	rf.height = FTK_MAX(rf.height, rs.height);

	ftk_canvas_show(ftk_shared_canvas(), ftk_default_display(), &rf, rf.x, rf.y);

	return RET_OK;
}

static Ret   ftk_animation_expand_reset(FtkAnimation* thiz, FtkBitmap* old_win, FtkBitmap* new_win,
	FtkRect* old_win_rect, FtkRect* new_win_rect)
{
	DECL_PRIV(thiz, priv);
	const char* src = ftk_animation_get_param(thiz, "src");
	const char* push = ftk_animation_get_param(thiz, "push");
	const char* alpha_enable = ftk_animation_get_param(thiz, "alpha");
	return_val_if_fail(src != NULL, RET_FAIL);

	priv->start = ftk_animation_get_param_float(thiz, "from", 1.0);
	priv->end = ftk_animation_get_param_float(thiz, "to", 1.0);
	priv->push = push != NULL && strcmp(push, "true") == 0;
	priv->alpha_enable = alpha_enable != NULL && strcmp(alpha_enable, "enable") == 0;

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

static void  ftk_animation_expand_destroy(FtkAnimation* thiz)
{
	FTK_FREE(thiz);

	return;
}

FtkAnimation* ftk_animation_expand_create(void)
{
	FtkAnimation* thiz = FTK_NEW_PRIV(FtkAnimation);

	if(thiz != NULL)
	{
		ftk_animation_set_name(thiz,  "expand");
		thiz->step      = ftk_animation_expand_step;
		thiz->reset     = ftk_animation_expand_reset;
		thiz->destroy   = ftk_animation_expand_destroy;
	}

	return thiz;
}

