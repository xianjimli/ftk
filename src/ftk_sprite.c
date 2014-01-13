/*
 * File:    ftk_sprite.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   screen sprite
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
 * 2009-11-21 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_sprite.h"
#include "ftk_globals.h"

struct _FtkSprite
{
	int x;
	int y;
	int visible;
	FtkBitmap* icon;
	FtkBitmap* snap;
	void* listener_ctx;
	FtkListener listener;
};

FtkSprite* ftk_sprite_create(void)
{
	FtkSprite* thiz = FTK_NEW(FtkSprite);
	
	return thiz;
}

static Ret  ftk_sprite_backup(FtkSprite* thiz)
{
	FtkRect rect = {0};
	FtkDisplay* display = ftk_default_display();
	return_val_if_fail(thiz != NULL, RET_FAIL);
	return_val_if_fail(thiz->snap != NULL, RET_FAIL);
	
	rect.x = thiz->x;
	rect.y = thiz->y;
	rect.width = ftk_bitmap_width(thiz->snap);
	rect.height = ftk_bitmap_height(thiz->snap);

	return ftk_display_snap(display, &rect, thiz->snap);
}

static Ret  ftk_sprite_restore(FtkSprite* thiz)
{
	FtkRect rect = {0};
	FtkDisplay* display = ftk_default_display();
	return_val_if_fail(thiz != NULL, RET_FAIL);
	return_val_if_fail(thiz->snap != NULL, RET_FAIL);
	
	rect.width = ftk_bitmap_width(thiz->snap);
	rect.height = ftk_bitmap_height(thiz->snap);

	return ftk_display_update(display, thiz->snap, &rect, thiz->x, thiz->y);
}

static Ret  ftk_sprite_paint(FtkSprite* thiz, int notify)
{
	Ret ret = RET_OK;
	FtkRect rect = {0};
	FtkDisplay* display = ftk_default_display();
	return_val_if_fail(thiz != NULL, RET_FAIL);
	return_val_if_fail(thiz->icon != NULL, RET_FAIL);
	
	rect.width = ftk_bitmap_width(thiz->icon);
	rect.height = ftk_bitmap_height(thiz->icon);

	if(notify)
	{
		ret = ftk_display_update_and_notify(display, thiz->icon, &rect, thiz->x, thiz->y);
	}
	else
	{
		ret = ftk_display_update(display, thiz->icon, &rect, thiz->x, thiz->y);
	}

	return ret;
}

Ret  ftk_sprite_set_icon(FtkSprite* thiz, FtkBitmap* icon)
{
	Ret ret = RET_OK;
	FtkColor bg = {0};
	return_val_if_fail(thiz != NULL && icon != NULL, RET_FAIL);
	
	bg.a = 0xff;
	thiz->icon = icon;
	if(thiz->snap != NULL)
	{
		ftk_bitmap_unref(thiz->snap);
	}
	thiz->snap = ftk_bitmap_create(ftk_bitmap_width(icon), ftk_bitmap_height(icon), bg);

	if(thiz->visible)
	{
		ftk_sprite_restore(thiz);
		ret = ftk_sprite_paint(thiz, 1);
	}

	return ret;
}

Ret ftk_sprite_on_display_update(void* ctx, FtkDisplay* display, int before,
	FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	FtkSprite* thiz = (FtkSprite*)ctx;
	return_val_if_fail(thiz != NULL && bitmap != NULL, RET_FAIL);

	if(bitmap == thiz->icon || bitmap == thiz->snap)
	{
		return RET_OK;
	}

	if(before)
	{
		ftk_sprite_restore(thiz);
	}
	else
	{
		ftk_sprite_backup(thiz);
		ftk_sprite_paint(thiz, 0);
	}

	return RET_OK;
}

Ret  ftk_sprite_show(FtkSprite* thiz, int show)
{
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL, RET_FAIL);
	return_val_if_fail(thiz->visible != show, RET_FAIL);

	thiz->visible = show;
	
	if(thiz->visible)
	{
		ftk_sprite_backup(thiz);
		ret = ftk_sprite_paint(thiz, 1);
		ftk_display_reg_update_listener(ftk_default_display(), ftk_sprite_on_display_update, thiz);
	}
	else
	{
		ftk_display_unreg_update_listener(ftk_default_display(), ftk_sprite_on_display_update, thiz);
		ret = ftk_sprite_restore(thiz);
	}

	return ret;
}

int  ftk_sprite_is_visible(FtkSprite* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->visible;
}

Ret  ftk_sprite_move(FtkSprite* thiz, int x, int y)
{
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL, RET_FAIL);
	
	if(thiz->x == x && thiz->y == y)
	{
		return RET_OK;
	}

	if(thiz->visible)
	{
		ftk_sprite_restore(thiz);	
	}

	thiz->x = x;
	thiz->y = y;
	
	if(thiz->visible)
	{
		ftk_sprite_backup(thiz);
		ret = ftk_sprite_paint(thiz, 1);
	}

	FTK_CALL_LISTENER(thiz->listener, thiz->listener_ctx, thiz);

	return ret;
}

int  ftk_sprite_get_x(FtkSprite* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->x;
}

int  ftk_sprite_get_y(FtkSprite* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->y;
}

Ret  ftk_sprite_set_move_listener(FtkSprite* thiz, FtkListener listener, void* ctx)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	thiz->listener = listener;
	thiz->listener_ctx = ctx;

	return RET_OK;
}

void ftk_sprite_destroy(FtkSprite* thiz)
{
	if(thiz != NULL)
	{
		ftk_bitmap_unref(thiz->icon);
		ftk_bitmap_unref(thiz->snap);
		FTK_ZFREE(thiz, sizeof(*thiz));
	}

	return;
}


