/*
 * File: ftk_handwrite_engine_dummy.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   dummy handwrite engine
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
 * 2010-02-10 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_event.h"
#include "ftk_allocator.h"
#include "ftk_handwrite_engine.h"

struct _FtkHandWriteEngine
{
	void* on_result_ctx;
	FtkCommitInfo commit_info;
	FtkHandWriteResult on_result;
};

FtkHandWriteEngine* ftk_hand_write_engine_create(FtkHandWriteResult on_result, void* ctx)
{
	FtkHandWriteEngine* thiz = NULL;
	return_val_if_fail(on_result != NULL, NULL);

	thiz = FTK_ZALLOC(sizeof(FtkHandWriteEngine));

	if(thiz != NULL)
	{
		thiz->on_result = on_result;
		thiz->on_result_ctx = ctx;
	}

	return thiz;
}

Ret  ftk_hand_write_engine_activate(FtkHandWriteEngine* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);
	ftk_logd("%s\n", __func__);

	return RET_OK;
}

Ret  ftk_hand_write_engine_deactivate(FtkHandWriteEngine* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);
	ftk_logd("%s\n", __func__);

	return RET_OK;
}

Ret  ftk_hand_write_engine_reset(FtkHandWriteEngine* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);
	ftk_logd("%s\n", __func__);

	return RET_OK;
}

Ret  ftk_hand_write_engine_set_rect(FtkHandWriteEngine* thiz, FtkRect* rect)
{
	return_val_if_fail(thiz != NULL && rect != NULL, RET_FAIL);

	return RET_FAIL;
}

Ret ftk_hand_write_engine_add_stroke(FtkHandWriteEngine* thiz, FtkPoint* points, size_t points_nr)
{
	return_val_if_fail(thiz != NULL && points != NULL, RET_FAIL);

	ftk_logd("%s\n", __func__);

	return RET_OK;
}

Ret ftk_hand_write_engine_flush(FtkHandWriteEngine* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);
	
	thiz->commit_info.candidate_nr = 1;
	strcpy(thiz->commit_info.candidates, "中国");

	ftk_logd("%s\n", __func__);
	thiz->on_result(thiz->on_result_ctx, &(thiz->commit_info)); 

	return RET_OK;
}

void ftk_hand_write_engine_destroy(FtkHandWriteEngine* thiz)
{
	if(thiz != NULL)
	{
		ftk_logd("%s\n", __func__);
		FTK_ZFREE(thiz, sizeof(FtkHandWriteEngine));
	}

	return;
}
