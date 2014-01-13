/*
 * File: ftk_pairs.h    
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
 * 2011-03-23 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_PAIRS_H
#define FTK_PAIRS_H

#include "ftk_typedef.h"

FTK_BEGIN_DECLS

typedef struct _FtkKeyValue
{
	char key[FTK_KEY_LEN+1];
	char value[FTK_VALUE_LEN+1];
}FtkKeyValue;

struct _FtkPairs;
typedef struct _FtkPairs FtkPairs;

FtkPairs* ftk_pairs_create(size_t ax_pairs_nr, FtkCompare compare);

Ret ftk_pairs_remove(FtkPairs* thiz, const char* key);
const char*  ftk_pairs_find(FtkPairs* thiz, const char* key);
Ret ftk_pairs_set(FtkPairs* thiz, const char* key, const char* value);
Ret ftk_pairs_add(FtkPairs* thiz, const char* key, const char* value);

void ftk_pairs_dump(FtkPairs* thiz);

void ftk_pairs_destroy(FtkPairs* thiz);

FTK_END_DECLS

#endif/*FTK_PAIRS_H*/

