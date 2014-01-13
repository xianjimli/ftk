/*
 * File: ftk_display.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   display
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
 * 2009-11-22 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_display.h"

Ret ftk_display_reg_update_listener(FtkDisplay* thiz, FtkDisplayOnUpdate on_update, void* ctx)
{
	int i = 0;
	return_val_if_fail(thiz != NULL && on_update != NULL, RET_FAIL);

	for(i = 0; i < FTK_DISPLAY_LISTENER_NR; i++)
	{
		if(thiz->on_update[i] == NULL)
		{
			thiz->on_update[i] = on_update;
			thiz->on_update_ctx[i] = ctx;

			return RET_OK;
		}
	}

	return RET_FAIL;
}

Ret ftk_display_unreg_update_listener(FtkDisplay* thiz, FtkDisplayOnUpdate on_update, void* ctx)
{
	int i = 0;
	return_val_if_fail(thiz != NULL && on_update != NULL, RET_FAIL);

	for(i = 0; i < FTK_DISPLAY_LISTENER_NR; i++)
	{
		if(thiz->on_update[i] == on_update && thiz->on_update_ctx[i] == ctx)
		{
			thiz->on_update[i] = NULL;
			thiz->on_update_ctx[i] = NULL;

			return RET_OK;
		}
	}

	return RET_FAIL;
}

Ret ftk_display_notify(FtkDisplay* thiz, int before, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	int i = 0;
	return_val_if_fail(thiz != NULL, RET_FAIL);

	for(i = 0; i < FTK_DISPLAY_LISTENER_NR; i++)
	{
		if(thiz->on_update[i] != NULL)
		{
			thiz->on_update[i](thiz->on_update_ctx[i], thiz, before, bitmap, rect, xoffset, yoffset);
		}
	}
			
	return RET_OK;
}

