/*
 * File:    typedef.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   common types definition.
 *
 * Copyright (c) Li XianJing
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
 * 2008-12-10 Li XianJing <xianjimli@hotmail.com> created.
 *
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifndef TYPEDEF_H
#define TYPEDEF_H

typedef enum _Ret
{
	RET_OK,
	RET_OOM,
	RET_STOP,
	RET_FOUND,
	RET_INVALID_PARAMS,
	RET_FAIL
}Ret;

typedef void     (*DataDestroyFunc)(void* ctx, void* data);
typedef int      (*DataCompareFunc)(void* ctx, void* data);
typedef Ret      (*DataVisitFunc)(void* ctx, void* data);
typedef int       (*DataHashFunc)(void* data);

#ifdef __cplusplus
#define DECLS_BEGIN extern "C" {
#define DECLS_END   }
#else
#define DECLS_BEGIN
#define DECLS_END
#endif/*__cplusplus*/

#define return_if_fail(p) if(!(p)) \
	{printf("%s:%d Warning: "#p" failed.\n", \
		__func__, __LINE__); return;}
#define return_val_if_fail(p, ret) if(!(p)) \
	{printf("%s:%d Warning: "#p" failed.\n",\
	__func__, __LINE__); return (ret);}

#define SAFE_FREE(p) if(p != NULL) {free(p); p = NULL;}

typedef Ret (*SortFunc)(void** array, size_t nr, DataCompareFunc cmp);
#define ftk_strncpy strncpy
#endif/*TYPEDEF_H*/

