/*
 * File: ftk_animation_translate.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   translate animation
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
#include "ftk_animation_translate.h"

typedef struct _AnimationTransPrivInfo
{
	float end;
	float start;
	int duration;
	const char* dir;
	FtkBitmap* first_win;
	FtkBitmap* second_win;
	FtkRect first_win_rect;
	FtkRect second_win_rect;
}PrivInfo;

static Ret   ftk_animation_translate_step_h(FtkAnimation* thiz, float percent)
{
	FtkRect r = {0};
	FtkRect dst_r = {0};
	DECL_PRIV(thiz, priv);
	int x = (int)((priv->first_win_rect.width + priv->second_win_rect.width) * percent);
	
	r = priv->first_win_rect;
	if(r.width > 0 && r.height > 0)
	{
		r.x = x;
		r.width -= x;
		dst_r = r;
		dst_r.x = 0;
		if(r.width > 0)
		{
			ftk_canvas_draw_bitmap(ftk_shared_canvas(), priv->first_win, &r, &dst_r, 0xff);
		}
	}

	r = priv->second_win_rect;
	if(r.width > 0 && r.height > 0)
	{
		r.width = x;
		dst_r = r;
		if(priv->first_win_rect.width == 0)
		{
			dst_r.x = ftk_display_width(ftk_default_display()) - x;
		}
		else
		{
			dst_r.x = priv->first_win_rect.width - x;
		}

		if(r.width > 0)
		{
			ftk_canvas_draw_bitmap(ftk_shared_canvas(), priv->second_win, &r, &dst_r, 0xff);
		}
	}

	return RET_OK;
}

static Ret   ftk_animation_translate_step_v(FtkAnimation* thiz, float percent)
{
	FtkRect r = {0};
	FtkRect dst_r = {0};
	DECL_PRIV(thiz, priv);
	int y = (int)((priv->first_win_rect.width + priv->second_win_rect.width) * percent);
	
	r = priv->first_win_rect;
	if(r.width > 0 && r.height > 0)
	{
		r.y = y;
		r.height -= y;
		dst_r = r;
		dst_r.y = 0;
		if(r.width > 0)
		{
			ftk_canvas_draw_bitmap(ftk_shared_canvas(), priv->first_win, &r, &dst_r, 0xff);
		}
	}

	r = priv->second_win_rect;
	if(r.width > 0 && r.height > 0)
	{
		r.height = y;
		dst_r = r;
		if(priv->first_win_rect.height== 0)
		{
			dst_r.y = ftk_display_height(ftk_default_display()) - y;
		}
		else
		{
			dst_r.y = priv->first_win_rect.height - y;
		}

		if(r.height > 0)
		{
			ftk_canvas_draw_bitmap(ftk_shared_canvas(), priv->second_win, &r, &dst_r, 0xff);
		}
	}

	return RET_OK;
}

static Ret   ftk_animation_translate_step(FtkAnimation* thiz)
{
	FtkRect rf = {0};
	FtkRect rs = {0};
	float percent = 0;
	DECL_PRIV(thiz, priv);
	const char* dir = ftk_animation_get_param(thiz, "dir");
	return_val_if_fail(dir != NULL, RET_FAIL);
	
	percent = priv->start + (priv->end - priv->start) * ftk_animation_get_percent(thiz);

	if(strcmp(dir, "h") == 0)
	{
		ftk_animation_translate_step_h(thiz, percent);
	}
	else
	{
		ftk_animation_translate_step_v(thiz, percent);
	}

	rf = priv->first_win_rect;
	rs = priv->second_win_rect;
	rf.x = FTK_MIN(rf.x, rs.x);
	rf.y = FTK_MIN(rf.y, rs.y);
	rf.width = FTK_MAX(rf.width, rs.width);
	rf.height = FTK_MAX(rf.height, rs.height);

	ftk_canvas_show(ftk_shared_canvas(), ftk_default_display(), &rf, rf.x, rf.y);

	return RET_OK;
}

static Ret   ftk_animation_translate_reset(FtkAnimation* thiz, FtkBitmap* old_win, FtkBitmap* new_win,
	FtkRect* old_win_rect, FtkRect* new_win_rect)
{
	DECL_PRIV(thiz, priv);
	const char* src = ftk_animation_get_param(thiz, "src");
	return_val_if_fail(src != NULL, RET_FAIL);

	priv->start = ftk_animation_get_param_float(thiz, "from", 1.0);
	priv->end = ftk_animation_get_param_float(thiz, "to", 1.0);

	if(strncmp(src, "new", 3) == 0)
	{
		priv->first_win = new_win;
		priv->first_win_rect = *new_win_rect;
	
		if(strstr(src, "old") != NULL)
		{
			priv->second_win = old_win;
			priv->second_win_rect = *old_win_rect;
		}
		else
		{
			priv->second_win = NULL;
		}
	}
	else if(strncmp(src, "old", 3) == 0)
	{
		priv->first_win = old_win;
		priv->first_win_rect = *old_win_rect;
	
		if(strstr(src, "new") != NULL)
		{
			priv->second_win = new_win;
			priv->second_win_rect = *new_win_rect;
		}
		else
		{
			priv->second_win = NULL;
		}
	}

	ftk_logd("%s: src=%s start=%f end=%f old(%d %d %d %d) new(%d %d %d %d)",
		__func__, src, priv->start, priv->end, 
		old_win_rect->x, old_win_rect->y, old_win_rect->width, old_win_rect->height,
		new_win_rect->x, new_win_rect->y, new_win_rect->width, new_win_rect->height);

	return RET_OK;
}

static void  ftk_animation_translate_destroy(FtkAnimation* thiz)
{
	FTK_FREE(thiz);

	return;
}

FtkAnimation* ftk_animation_translate_create(void)
{
	FtkAnimation* thiz = FTK_NEW_PRIV(FtkAnimation);

	if(thiz != NULL)
	{
		ftk_animation_set_name(thiz,  "translate");
		thiz->step      = ftk_animation_translate_step;
		thiz->reset     = ftk_animation_translate_reset;
		thiz->destroy   = ftk_animation_translate_destroy;
	}

	return thiz;
}

