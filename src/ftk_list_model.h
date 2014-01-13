/*
 * File: ftk_list_model.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   list model
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
 * 2009-11-15 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_LIST_MODEL_H
#define FTK_LIST_MODEL_H

#include "ftk_typedef.h"

FTK_BEGIN_DECLS

struct _FtkListModel;
typedef struct _FtkListModel FtkListModel;

typedef int  (*FtkListModelGetTotal)(FtkListModel* thiz);
typedef Ret  (*FtkListModelGetData)(FtkListModel* thiz, size_t index, void** ret);
typedef void (*FtkListModelDestroy)(FtkListModel* thiz);
typedef Ret  (*FtkListModelAdd)(FtkListModel* thiz, void* item);
typedef Ret  (*FtkListModelReset)(FtkListModel* thiz);
typedef Ret  (*FtkListModelRemove)(FtkListModel* thiz, size_t index);

struct _FtkListModel
{
	FtkListModelGetTotal get_total;
	FtkListModelGetData  get_data;
	FtkListModelAdd      add;
	FtkListModelReset    reset;
	FtkListModelRemove   remove;
	FtkListModelDestroy  destroy;

	int ref;
	int disable_notify;
	void* listener_ctx;
	FtkListener listener;
	char priv[ZERO_LEN_ARRAY];
};

static inline Ret ftk_list_model_enable_notify(FtkListModel* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->disable_notify > 0, RET_FAIL);

	thiz->disable_notify--;
	
	return RET_OK;
}

static inline Ret ftk_list_model_disable_notify(FtkListModel* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	thiz->disable_notify++;
	
	return RET_OK;
}

static inline Ret ftk_list_model_set_changed_listener(FtkListModel* thiz, FtkListener listener, void* ctx)
{
	if(thiz != NULL)
	{
		thiz->listener = listener;
		thiz->listener_ctx = ctx;
	}

	return RET_OK;
}

static inline Ret ftk_list_model_notify(FtkListModel* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);
	
	if(thiz->disable_notify) return RET_FAIL;

	return FTK_CALL_LISTENER(thiz->listener, thiz->listener_ctx, thiz);
}

static inline Ret ftk_list_model_add(FtkListModel* thiz, void* item)
{
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL && thiz->add != NULL, RET_FAIL);

	if((ret = thiz->add(thiz, item)) == RET_OK)
	{
		ftk_list_model_notify(thiz);
	}

	return ret;
}

static inline Ret ftk_list_model_remove(FtkListModel* thiz, size_t index)
{
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL && thiz->remove != NULL, RET_FAIL);

	if((ret = thiz->remove(thiz, index)) == RET_OK)
	{
		ftk_list_model_notify(thiz);
	}

	return ret;
}

static inline Ret ftk_list_model_reset(FtkListModel* thiz)
{
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL && thiz->reset != NULL, RET_FAIL);

	if((ret = thiz->reset(thiz)) == RET_OK)
	{
		ftk_list_model_notify(thiz);
	}

	return ret;
}

static inline int ftk_list_model_get_total(FtkListModel* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->get_total != NULL, 0);

	return thiz->get_total(thiz);
}

static inline Ret ftk_list_model_get_data(FtkListModel* thiz, size_t index, void** ret)
{
	return_val_if_fail(thiz != NULL && thiz->get_data != NULL, RET_FAIL);

	return thiz->get_data(thiz, index, ret);
}

static inline void ftk_list_model_destroy(FtkListModel* thiz)
{
	if(thiz != NULL && thiz->destroy != NULL)
	{
		thiz->destroy(thiz);
	}

	return;
}

static inline void ftk_list_model_ref(FtkListModel* thiz)
{
	if(thiz != NULL)
	{
		thiz->ref++;
	}

	return;
}

static inline void ftk_list_model_unref(FtkListModel* thiz)
{
	if(thiz != NULL)
	{
		thiz->ref--;
		if(thiz->ref == 0)
		{
			ftk_list_model_destroy(thiz);
		}
	}

	return;
}

FTK_END_DECLS

#endif/*FTK_LIST_MODEL_H*/

