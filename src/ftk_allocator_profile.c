/*
 * File: ftk_allocator.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   memory allocator interface.
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
 * 2010-01-31 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk.h"
#include "ftk_allocator_profile.h"

typedef struct _AllocPrivInfo
{
	int free_times;
	int allocated_times;
	int allocated;
	int max_allocated;
	int total_allocated;
	FtkAllocator* allocator;
}PrivInfo;

void* ftk_allocator_profile_alloc(FtkAllocator* thiz, size_t size)
{
	DECL_PRIV(thiz, priv);
	void* ptr = ftk_allocator_alloc(priv->allocator, size + sizeof(size_t));

	if(ptr != NULL)
	{
		priv->allocated_times++;
		priv->allocated += size;
		priv->total_allocated += size;
		priv->max_allocated = priv->allocated > priv->max_allocated? priv->allocated : priv->max_allocated;

		*(size_t*)ptr = size;
		ptr = (char*)ptr + sizeof(size_t);
	}

	return ptr;
}

void* ftk_allocator_profile_realloc(FtkAllocator* thiz, void* ptr, size_t new_size)
{
	size_t old_size = 0;
	DECL_PRIV(thiz, priv);

	ptr = ptr != NULL ? (char*)ptr - sizeof(size_t) : NULL;
	old_size = ptr != NULL ? *(size_t*)ptr : 0;

	ptr = ftk_allocator_realloc(priv->allocator, ptr, new_size + sizeof(size_t));
	if(ptr != NULL)
	{
		priv->allocated += new_size - old_size;
		priv->total_allocated += new_size;
		priv->max_allocated = priv->allocated > priv->max_allocated? priv->allocated : priv->max_allocated;
		*(size_t*)ptr = new_size;
		ptr = (char*)ptr + sizeof(size_t);
	}

	return ptr;
}

void  ftk_allocator_profile_free(FtkAllocator* thiz, void* ptr)
{
	size_t size = 0;
	DECL_PRIV(thiz, priv);

	ptr = ptr != NULL ? (char*)ptr - sizeof(size_t) : NULL;
	size = ptr != NULL ? *(size_t*)ptr : 0;
	if(ptr != NULL)
	{
		priv->free_times++;
		priv->allocated -= size;
	}
	ftk_allocator_free(priv->allocator, ptr);

	return;
}

void  ftk_allocator_profile_destroy(FtkAllocator* thiz)
{
	DECL_PRIV(thiz, priv);
	FtkAllocator* allocator = priv->allocator;
	
	ftk_logd("%s: allocated_times=%d\n", __func__, priv->allocated_times);
	ftk_logd("%s: free_times=%d\n", __func__, priv->free_times);
	ftk_logd("%s: allocated=%d\n", __func__, priv->allocated);
	ftk_logd("%s: max_allocated=%d\n", __func__, priv->max_allocated);
	ftk_logd("%s: total_allocated=%d\n", __func__, priv->total_allocated);
	ftk_allocator_free(allocator, thiz);
	ftk_allocator_destroy(allocator);

	return;
}

FtkAllocator* ftk_allocator_profile_create(FtkAllocator* allocator)
{
	FtkAllocator* thiz = (FtkAllocator*)ftk_allocator_zalloc(allocator, sizeof(FtkAllocator) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		priv->allocator = allocator;

		thiz->alloc   = ftk_allocator_profile_alloc;
		thiz->realloc = ftk_allocator_profile_realloc;
		thiz->free    = ftk_allocator_profile_free;
		thiz->destroy = ftk_allocator_profile_destroy;
	}

	return thiz;
}
