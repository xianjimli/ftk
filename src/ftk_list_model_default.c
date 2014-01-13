/*
 * File: ftk_list_model_default.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   default list model
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
 * 2009-11-28 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_list_model_default.h"

typedef struct _ListModelDefaultPrivInfo
{
	size_t nr;
	size_t alloc_nr;
	FtkListItemInfo* items;
}PrivInfo;

static Ret ftk_list_item_reset(FtkListItemInfo* info);

static int  ftk_list_model_default_get_total(FtkListModel* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);

	return priv->nr;
}

static Ret  ftk_list_model_default_get_data(FtkListModel* thiz, size_t index, void** ret)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && index < priv->nr && ret != NULL, RET_FAIL);
	
	*ret = priv->items+index;
	
	return RET_OK;
}

static void ftk_list_model_default_destroy(FtkListModel* thiz)
{
	DECL_PRIV(thiz, priv);
	
	if(priv != NULL)
	{
		size_t i = 0;

		for(i = 0; i < priv->nr; i++)
		{
			ftk_list_item_reset(priv->items+i);
		}
		FTK_FREE(priv->items);
		FTK_ZFREE(thiz, sizeof(FtkListModel) + sizeof(PrivInfo));
	}

	return;
}

static Ret ftk_list_model_default_extend(FtkListModel* thiz, size_t delta)
{
	int alloc_nr = 0;
	DECL_PRIV(thiz, priv);
	FtkListItemInfo* items = NULL;

	if(priv->items != NULL && (priv->nr + delta) < priv->alloc_nr)
	{
		return RET_OK;
	}

	alloc_nr = (priv->alloc_nr + delta) + FTK_HALF(priv->alloc_nr + delta) + 2; 
	items = (FtkListItemInfo*)FTK_REALLOC(priv->items, sizeof(FtkListItemInfo) * alloc_nr);
	if(items != NULL)
	{
		priv->items = items;
		priv->alloc_nr = alloc_nr;
	}

	return (priv->nr + delta) < priv->alloc_nr ? RET_OK : RET_FAIL;
}

static Ret ftk_list_item_reset(FtkListItemInfo* info)
{
	return_val_if_fail(info != NULL, RET_FAIL);

	if(info->text != NULL)
	{
		FTK_FREE(info->text);
	}

	if(info->left_icon != NULL)
	{
		ftk_bitmap_unref(info->left_icon);
	}

	if(info->right_icon != NULL)
	{
		ftk_bitmap_unref(info->right_icon);
	}

	memset(info, 0x00, sizeof(FtkListItemInfo));

	return RET_OK;
}

static Ret ftk_list_item_copy(FtkListModel* thiz, FtkListItemInfo* dst, FtkListItemInfo* src)
{
	return_val_if_fail(dst != NULL && src != NULL && thiz != NULL, RET_FAIL);
	
	memcpy(dst, src, sizeof(FtkListItemInfo));

	if(src->text != NULL)
	{
		dst->text = ftk_strdup(src->text);
	}

	if(dst->left_icon != NULL)
	{
		ftk_bitmap_ref(dst->left_icon);
	}

	if(dst->right_icon != NULL)
	{
		ftk_bitmap_ref(dst->right_icon);
	}

	return RET_OK;
}

Ret ftk_list_model_default_add(FtkListModel* thiz, void* item)
{
	DECL_PRIV(thiz, priv);
	FtkListItemInfo* info = (FtkListItemInfo*)item;
	return_val_if_fail(thiz != NULL && info != NULL, RET_FAIL);
	return_val_if_fail(ftk_list_model_default_extend(thiz, 1) == RET_OK, RET_FAIL);

	if(ftk_list_item_copy(thiz, priv->items+priv->nr, info) == RET_OK)
	{
		priv->nr++;
	}

	return RET_OK;
}

static Ret ftk_list_model_default_reset(FtkListModel* thiz)
{
	size_t i = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	for(i = 0; i < priv->nr; i++)
	{
		ftk_list_item_reset(priv->items+i);
	}
	priv->nr = 0;

	return RET_OK;
}

Ret ftk_list_model_default_remove(FtkListModel* thiz, size_t index)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && index < priv->nr, RET_FAIL);

	ftk_list_item_reset(priv->items+index);
	if(index < (priv->nr - 1))
	{
		size_t i = index; 
		for(; i < priv->nr; i++)
		{
			memcpy(priv->items+i, priv->items+i+1, sizeof(FtkListItemInfo));
		}
	}

	priv->nr--;

	return RET_OK;
}

FtkListModel* ftk_list_model_default_create(size_t init_nr)
{
	FtkListModel* thiz = FTK_NEW_PRIV(FtkListModel);
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->get_total = ftk_list_model_default_get_total;
		thiz->get_data  = ftk_list_model_default_get_data;
		thiz->add       = ftk_list_model_default_add;
		thiz->reset     = ftk_list_model_default_reset;
		thiz->remove    = ftk_list_model_default_remove;
		thiz->destroy   = ftk_list_model_default_destroy;

		thiz->ref = 1;
		priv->alloc_nr = init_nr;
	}

	return thiz;
}

