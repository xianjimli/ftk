/*
 * File: ftk_input_pattern.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   input pattern to format some special input, such as date, 
 *  time and ip address.
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
 * 2011-07-28 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#ifndef FTK_INPUT_PATTERN_H
#define FTK_INPUT_PATTERN_H

#include "ftk_key.h"

FTK_BEGIN_DECLS

struct _FtkInputPattern;
typedef struct _FtkInputPattern FtkInputPattern;

FtkInputPattern* ftk_input_pattern_create(const char* pattern, const char* init);
Ret    ftk_input_pattern_input(FtkInputPattern* thiz, FtkKey key);
Ret    ftk_input_pattern_set_caret(FtkInputPattern* thiz, size_t caret);
Ret    ftk_input_pattern_set_text(FtkInputPattern* thiz, const char* text);
size_t ftk_input_pattern_get_caret(FtkInputPattern* thiz);
const char* ftk_input_pattern_get_text(FtkInputPattern* thiz);
void ftk_input_pattern_destroy(FtkInputPattern* thiz);

FTK_END_DECLS

#endif/*FTK_INPUT_PATTERN_H*/


