/*
 * File: ftk_allocator.h    
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

#ifndef FTK_ALLOCATOR_H
#define FTK_ALLOCATOR_H

#include "ftk_typedef.h"

FTK_BEGIN_DECLS

struct _FtkAllocator;
typedef struct _FtkAllocator FtkAllocator;

typedef void* (*FtkAllocatorAlloc)(FtkAllocator* thiz, size_t size);
typedef void* (*FtkAllocatorRealloc)(FtkAllocator* thiz, void* ptr, size_t new_size);
typedef void  (*FtkAllocatorFree)(FtkAllocator* thiz, void* ptr);
typedef void  (*FtkAllocatorDestroy)(FtkAllocator* thiz);

struct _FtkAllocator
{
	FtkAllocatorAlloc   alloc;
	FtkAllocatorRealloc realloc;
	FtkAllocatorFree    free;
	FtkAllocatorDestroy destroy;

	char priv[ZERO_LEN_ARRAY];
};

void* ftk_allocator_alloc(FtkAllocator* thiz, size_t size);
void* ftk_allocator_realloc(FtkAllocator* thiz, void* ptr, size_t new_size);
void  ftk_allocator_free(FtkAllocator* thiz, void* ptr);
void  ftk_allocator_destroy(FtkAllocator* thiz);

char* ftk_strdup(const char* str);
void* ftk_allocator_zalloc(FtkAllocator* thiz, size_t size);
void  ftk_allocator_zfree(FtkAllocator* thiz, void* ptr, size_t size);

extern FtkAllocator*      ftk_default_allocator(void);

FTK_END_DECLS

#endif/*FTK_ALLOCATOR_H*/

