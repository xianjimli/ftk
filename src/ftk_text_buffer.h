/*
 * File: ftk_text_buffer.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   text buffer
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
 * 2009-11-06 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_TEXT_BUFFER_H
#define FTK_TEXT_BUFFER_H

#include "ftk_typedef.h"

FTK_BEGIN_DECLS

typedef struct _FtkTextBuffer
{
	char* buffer;
	size_t length;
	size_t buffer_length;
}FtkTextBuffer;

FtkTextBuffer* ftk_text_buffer_create(size_t init_buffer_length);

Ret   ftk_text_buffer_insert(FtkTextBuffer* thiz, size_t offset, const char* text, int len);
Ret   ftk_text_buffer_delete(FtkTextBuffer* thiz, size_t offset, size_t length);
Ret   ftk_text_buffer_delete_chars(FtkTextBuffer* thiz, int offset, int count);
int   ftk_text_buffer_chars_bytes(FtkTextBuffer* thiz, int offset, int count);

Ret   ftk_text_buffer_reset(FtkTextBuffer* thiz);
char* ftk_text_buffer_append_string(FtkTextBuffer* thiz, const char* str);

void ftk_text_buffer_destroy(FtkTextBuffer* thiz);

FTK_END_DECLS

#endif/*FTK_TEXT_BUFFER_H*/

