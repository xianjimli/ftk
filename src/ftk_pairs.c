/*
 * File: ftk_pairs.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:  key-value manager
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
 * 2011-03-25 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_allocator.h"
#include "ftk_log.h"
#include "ftk_pairs.h"

struct _FtkPairs
{
	size_t nr;
	FtkCompare compare;
	FtkKeyValue pairs[ZERO_LEN_ARRAY];
};

FtkPairs* ftk_pairs_create(size_t nr, FtkCompare compare)
{
	FtkPairs* thiz = NULL;
	return_val_if_fail(nr > 0 && compare != NULL, NULL);

	thiz = (FtkPairs*)FTK_ZALLOC(sizeof(FtkPairs) + nr * sizeof(FtkKeyValue));
	if(thiz != NULL)
	{
		thiz->nr = nr;
		thiz->compare = compare;
	}

	return thiz;
}

Ret ftk_pairs_remove(FtkPairs* thiz, const char* key)
{
	size_t i = 0;
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL && key != NULL, RET_FAIL);

	for(i = 0; i < thiz->nr; i++)
	{
		if(thiz->compare(key, thiz->pairs[i].key) == 0)
		{
			memset(thiz->pairs+i, 0x00, sizeof(FtkKeyValue));
			ret = RET_OK;
			break;
		}
	}

	return ret;
}

Ret ftk_pairs_set(FtkPairs* thiz, const char* key, const char* value)
{
	size_t i = 0;
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL && key != NULL, RET_FAIL);

	for(i = 0; i < thiz->nr; i++)
	{
		if(thiz->compare(key, thiz->pairs[i].key) == 0)
		{
			ftk_strncpy(thiz->pairs[i].value, value, FTK_VALUE_LEN);
			thiz->pairs[i].value[FTK_VALUE_LEN] = '\0';
			ret = RET_OK;
			break;
		}
	}

	if(ret != RET_OK)
	{
		ret = ftk_pairs_add(thiz, key, value);
	}

	return ret;
}

Ret ftk_pairs_add(FtkPairs* thiz, const char* key, const char* value)
{
	size_t i = 0;
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL && key != NULL && value != NULL, RET_FAIL);

	for(i = 0; i < thiz->nr; i++)
	{
		if(thiz->pairs[i].key[0] == '\0')
		{
			ret = RET_OK;
			ftk_strncpy(thiz->pairs[i].key, key, FTK_KEY_LEN);
			ftk_strncpy(thiz->pairs[i].value, value, FTK_VALUE_LEN);
			thiz->pairs[i].key[FTK_KEY_LEN] = '\0';
			thiz->pairs[i].value[FTK_VALUE_LEN] = '\0';
			break;
		}
	}

	return ret;
}

const char*  ftk_pairs_find(FtkPairs* thiz, const char* key)
{
	size_t i = 0;
	const char* value = NULL;
	return_val_if_fail(thiz != NULL && key != NULL, NULL);

	for(i = 0; i < thiz->nr; i++)
	{
		if(thiz->compare(key, thiz->pairs[i].key) == 0)
		{
			value = thiz->pairs[i].value;
			break;
		}
	}

	return value;
}

void ftk_pairs_dump(FtkPairs* thiz)
{
	size_t i = 0;
	if(thiz != NULL)
	{
		for(i = 0; i < thiz->nr; i++)
		{
			if(thiz->pairs[i].key[0])
			{
				ftk_logd("%s=%s\n", thiz->pairs[i].key, thiz->pairs[i].value);
			}
		}
	}
}

void ftk_pairs_destroy(FtkPairs* thiz)
{
	if(thiz != NULL)
	{
		FTK_FREE(thiz);
	}

	return;
}


