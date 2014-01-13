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

#include "ftk_log.h"
#include "ftk_util.h"
#include "ftk_allocator.h"

void* ftk_allocator_alloc(FtkAllocator* thiz, size_t size)
{
	return_val_if_fail(thiz != NULL, NULL);

	return thiz->alloc(thiz, size);
}

void* ftk_allocator_realloc(FtkAllocator* thiz, void* ptr, size_t new_size)
{
	return_val_if_fail(thiz != NULL, NULL);

	return thiz->realloc(thiz, ptr, new_size);
}

void  ftk_allocator_free(FtkAllocator* thiz, void* ptr)
{
	return_if_fail(thiz != NULL);
	if(ptr == NULL) return;

	thiz->free(thiz, ptr);

	return;
}

void  ftk_allocator_destroy(FtkAllocator* thiz)
{
	if(thiz != NULL && thiz->destroy != NULL)
	{
		thiz->destroy(thiz);
	}

	return;
}

void* ftk_allocator_zalloc(FtkAllocator* thiz, size_t size)
{
	void* ptr = ftk_allocator_alloc(thiz, size);
	return_val_if_fail(ptr != NULL, NULL);

	memset(ptr, 0x00, size);

	return ptr;
}

void  ftk_allocator_zfree(FtkAllocator* thiz, void* ptr, size_t size)
{
	return_if_fail(thiz != NULL && ptr != NULL);

	memset(ptr, 0x00, size);

	ftk_allocator_free(thiz, ptr);

	return;
}

char* ftk_strdup(const char* str)
{
	char* new_str = NULL;
	return_val_if_fail(str != NULL, NULL);
	new_str = (char*)ftk_allocator_alloc(ftk_default_allocator(), strlen(str) + 1);

	if(new_str != NULL)
	{
		ftk_strcpy(new_str, str);
	}

	return new_str;
}
