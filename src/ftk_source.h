/*
 * File: ftk_source.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   event source interface
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
 * 2009-10-03 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#ifndef FTK_SOURCE_H
#define FTK_SOURCE_H

#include "ftk_log.h"
#include "ftk_allocator.h"

FTK_BEGIN_DECLS

struct _FtkSource;
typedef struct _FtkSource FtkSource;

typedef int  (*FtkSourceGetFd)(FtkSource* thiz);
typedef int  (*FtkSourceCheck)(FtkSource* thiz);
typedef Ret  (*FtkSourceDispatch)(FtkSource* thiz);
typedef void (*FtkSourceDestroy)(FtkSource* thiz);

struct _FtkSource
{
	FtkSourceGetFd    get_fd;
	FtkSourceCheck    check;
	FtkSourceDispatch dispatch;
	FtkSourceDestroy  destroy;
	
	int  ref;
	int  active;/*If this source is managed by sources_manager.*/
	int  disable;
	char priv[ZERO_LEN_ARRAY];
};

static inline Ret ftk_source_disable(FtkSource* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	thiz->disable++;

	return RET_OK;
}

static inline Ret ftk_source_enable(FtkSource* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(thiz->disable > 0)
	{
		thiz->disable--;
	}
	else
	{
		thiz->disable = 0;
	}

	return RET_OK;
}

static inline int ftk_source_get_fd(FtkSource* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->get_fd != NULL, -1);

	return thiz->get_fd(thiz);
}

static inline int ftk_source_check(FtkSource* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->check != NULL, -1);

	return thiz->check(thiz);
}

static inline Ret ftk_source_dispatch(FtkSource* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->dispatch != NULL, RET_FAIL);

	return thiz->dispatch(thiz);
}

static inline void ftk_source_destroy(FtkSource* thiz)
{
	if(thiz != NULL && thiz->destroy != NULL)
	{
		thiz->destroy(thiz);
	}

	return;
}

static inline void ftk_source_ref(FtkSource* thiz)
{
	return_if_fail(thiz != NULL);
	thiz->ref++;

	return;
}

static inline void ftk_source_unref(FtkSource* thiz)
{
	return_if_fail(thiz != NULL);

	thiz->ref--;
	if(thiz->ref == 0)
	{
		ftk_source_destroy(thiz);
	}

	return;
}

FTK_END_DECLS

#endif/*FTK_SOURCE_H*/

