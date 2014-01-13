/*
 * File: ftk_handwrite_engine_dummy.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   FTK default handwrite engine
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
#include "ftk_lookup_table.h"
#include "ftk_input_method.h"
#include "ftk_handwrite_engine.h"
#include "fhw_primitive_recognizer.h"

struct _FtkHandWriteEngine
{
	FtkRect rect;
	int candidate_size;
	void* on_result_ctx;
	FtkCommitInfo commit_info;
	FtkHandWriteResult on_result;
	FtkLookupTable* lookup_table;
	FhwPrimitiveRecognizer* recognizer;
};

#define FTK_HANDWRITE_DATA "fhw.bin"

static int fhw_compare(const void* obj1, const void* obj2)
{
	/*FIXME: need better algorithm*/
	return strncmp(obj1, obj2, strlen(obj2));
}

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

static Ret ftk_hand_write_engine_init(FtkHandWriteEngine* thiz)
{
	char filename[FTK_MAX_PATH+1] = {0};
	if(thiz->lookup_table != NULL) return RET_OK;
	
	ftk_snprintf(filename, sizeof(filename), "%s/data/%s", DATA_DIR, FTK_HANDWRITE_DATA);
	if((thiz->lookup_table = ftk_lookup_table_create(filename, 1, (FtkCompare)fhw_compare)) == NULL)
	{
		ftk_snprintf(filename, sizeof(filename), "%s/data/%s", LOCAL_DATA_DIR, FTK_HANDWRITE_DATA);
		thiz->lookup_table = ftk_lookup_table_create(filename, 1, (FtkCompare)fhw_compare);
	}

	thiz->recognizer = fhw_primitive_recognizer_create((&thiz->rect));

	return RET_OK;
}

Ret  ftk_hand_write_engine_activate(FtkHandWriteEngine* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	ftk_hand_write_engine_init(thiz);

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
	fhw_primitive_recognizer_reset(thiz->recognizer);
	ftk_logd("%s\n", __func__);

	return RET_OK;
}

Ret  ftk_hand_write_engine_set_rect(FtkHandWriteEngine* thiz, FtkRect* rect)
{
	return_val_if_fail(thiz != NULL && rect != NULL, RET_FAIL);

	thiz->rect = *rect;

	return RET_FAIL;
}

Ret ftk_hand_write_engine_add_stroke(FtkHandWriteEngine* thiz, FtkPoint* points, size_t points_nr)
{
	Ret ret = RET_OK;
	const char* result = NULL;
	const char* matched = NULL;
	return_val_if_fail(thiz->on_result != NULL, RET_FAIL);
	return_val_if_fail(thiz->recognizer != NULL, RET_FAIL);
	return_val_if_fail(thiz != NULL && points != NULL, RET_FAIL);

	ret = fhw_primitive_recognizer_add_stroke(thiz->recognizer, points, points_nr);
	return_val_if_fail(ret == RET_OK, RET_FAIL);

	fhw_primitive_recognizer_get_result(thiz->recognizer, &result);
	return_val_if_fail(result != NULL, RET_FAIL);

	thiz->candidate_size = 0;
	thiz->commit_info.candidate_nr = 0;
	thiz->commit_info.candidates[0] = '\0';
	strcpy(thiz->commit_info.raw_text, result);

	if(result[0] != '\0')
	{
		matched = ftk_lookup_table_lookup(thiz->lookup_table, result);
	}

	if(matched != NULL)
	{
		ftk_im_candidate_info_parse(&(thiz->commit_info), matched, fhw_compare);
	}
	else
	{
		strcpy(thiz->commit_info.raw_text, "");
		ftk_logd("%s: no matched for %s\n", __func__, result);
	}

	thiz->on_result(thiz->on_result_ctx, &(thiz->commit_info));

	return RET_OK;
}

Ret ftk_hand_write_engine_flush(FtkHandWriteEngine* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);
	
	return RET_OK;
}

void ftk_hand_write_engine_destroy(FtkHandWriteEngine* thiz)
{
	if(thiz != NULL)
	{
		ftk_lookup_table_destroy(thiz->lookup_table);
		fhw_primitive_recognizer_destroy(thiz->recognizer);

		FTK_ZFREE(thiz, sizeof(FtkHandWriteEngine));
	}

	return;
}

