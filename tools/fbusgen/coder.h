/*
 * File:    coder.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   interface for code generator.
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
 * 2010-07-31 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef CODER_H
#define CODER_H

#include "glib.h"
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <libIDL/IDL.h>

struct _Coder;
typedef struct _Coder Coder;

typedef gboolean (*CoderOnInterface)(Coder* thiz, const char* name, const char* parent);
typedef gboolean (*CoderOnFunction)(Coder* thiz, struct _IDL_OP_DCL f);
typedef gboolean (*CoderOnConst)(Coder* thiz, struct _IDL_CONST_DCL c);
typedef gboolean (*CoderOnStruct)(Coder* thiz, struct _IDL_TYPE_STRUCT s);
typedef gboolean (*CoderOnEnum)(Coder* thiz, struct _IDL_TYPE_ENUM e);
typedef gboolean (*CoderOnUnion)(Coder* thiz, struct _IDL_TYPE_UNION u);
typedef void     (*CoderDestroy)(Coder* thiz);

struct _Coder
{
	
	CoderOnInterface  on_interface;
	CoderOnFunction   on_function;
	CoderOnStruct     on_struct;
	CoderOnConst      on_const;
	CoderOnUnion      on_union;
	CoderOnEnum       on_enum;
	CoderDestroy      destroy;

	char priv[1];
};

static inline gboolean coder_on_interface(Coder* thiz, const char* name, const char* parent)
{
	g_return_val_if_fail(thiz != NULL && name != NULL, FALSE);

	if(thiz->on_interface != NULL)
	{
		return thiz->on_interface(thiz, name, parent);
	}
	else
	{
		return FALSE;
	}
}

static inline gboolean coder_on_function(Coder* thiz, struct _IDL_OP_DCL f)
{
	g_return_val_if_fail(thiz != NULL, FALSE);

	if(thiz->on_function != NULL)
	{
		return thiz->on_function(thiz, f);
	}
	else
	{
		return FALSE;
	}
}

static inline gboolean coder_on_const(Coder* thiz, struct _IDL_CONST_DCL c)
{
	g_return_val_if_fail(thiz != NULL, FALSE);
	
	if(thiz->on_const != NULL)
	{
		return thiz->on_const(thiz, c);
	}
	else
	{
		return FALSE;
	}
}

static inline gboolean coder_on_struct(Coder* thiz, struct _IDL_TYPE_STRUCT s)
{
	g_return_val_if_fail(thiz != NULL, FALSE);
	
	if(thiz->on_struct != NULL)
	{
		return thiz->on_struct(thiz, s);
	}
	else
	{
		return FALSE;
	}
}

static inline gboolean coder_on_enum(Coder* thiz, struct _IDL_TYPE_ENUM e)
{
	g_return_val_if_fail(thiz != NULL, FALSE);
	
	if(thiz->on_enum != NULL)
	{
		return thiz->on_enum(thiz, e);
	}
	else
	{
		return FALSE;
	}
}

static inline gboolean coder_on_union(Coder* thiz, struct _IDL_TYPE_UNION u)
{
	g_return_val_if_fail(thiz != NULL, FALSE);
	
	if(thiz->on_union != NULL)
	{
		return thiz->on_union(thiz, u);
	}
	else
	{
		return FALSE;
	}
}

static inline void     coder_destroy(Coder* thiz)
{
	if(thiz != NULL && thiz->destroy != NULL)
	{
		thiz->destroy(thiz);
	}

	return;
}

typedef Coder* (*CoderCreate)(const char* name);

char* coder_name_to_upper(const char* src, char* dst, int skip);
char* coder_name_to_lower(const char* src, char* dst);
const char* coder_to_upper(char* src);
const char* coder_to_lower(char* src);
void coder_write_header(FILE* fp);

#endif/*CODER_H*/
