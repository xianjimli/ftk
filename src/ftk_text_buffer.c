/*
 * File: ftk_text_buffer.c
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

#include "ftk_util.h"
#include "ftk_text_buffer.h"

FtkTextBuffer* ftk_text_buffer_create(size_t init_buffer_length)
{
	FtkTextBuffer* thiz = (FtkTextBuffer*)FTK_ALLOC(sizeof(FtkTextBuffer));
	if(thiz != NULL)
	{
		thiz->length = 0;
		thiz->buffer = (char*)FTK_ALLOC(init_buffer_length);
		thiz->buffer_length = init_buffer_length;
		thiz->buffer[thiz->length] = '\0';
	}

	return thiz;
}

static Ret ftk_text_buffer_extend(FtkTextBuffer* thiz, size_t length)
{
	char* buffer = NULL;
	size_t buffer_length = 0;
	if((thiz->length + length) < thiz->buffer_length)
	{
		return RET_OK;
	}

	buffer_length = (thiz->buffer_length + length + 10);
	buffer_length = buffer_length + (buffer_length>>1);

	if((buffer = (char*)FTK_REALLOC(thiz->buffer, buffer_length)) != NULL)
	{
		thiz->buffer = buffer;
		thiz->buffer_length = buffer_length;
		thiz->buffer[thiz->length] = '\0';

		return RET_OK;
	}

	return RET_FAIL;
}

Ret  ftk_text_buffer_insert(FtkTextBuffer* thiz, size_t offset, const char* text, int len)
{
	int str_len = len;
	size_t length = 0;
	char* dst = NULL;
	const char* src = NULL;
	return_val_if_fail(thiz != NULL && thiz->buffer != NULL && text != NULL, RET_FAIL);	
	str_len = len < 0 ? (int)strlen(text) : len;
	return_val_if_fail(ftk_text_buffer_extend(thiz, str_len + 1) == RET_OK, RET_FAIL);
	return_val_if_fail(offset <= thiz->length, RET_FAIL);

	if(offset < thiz->length)
	{
		length = thiz->length - offset;
		dst = thiz->buffer + thiz->length + str_len - 1;
		src = thiz->buffer + thiz->length - 1;
		for(; length != 0; length--, dst--, src--)
		{
			*dst = *src;
		}
	}
	dst = thiz->buffer + offset;
	src = text;
	length = str_len;
	for(;length != 0; length--, dst++, src++)
	{
		*dst = *src;
	}

	thiz->length += str_len;
	thiz->buffer[thiz->length] = '\0';

	return RET_OK;
}

Ret  ftk_text_buffer_delete(FtkTextBuffer* thiz, size_t offset, size_t length)
{
	size_t i = 0;
	char* dst = NULL;
	char* src = NULL;
	return_val_if_fail(thiz != NULL && thiz->buffer != NULL, RET_FAIL);

	if(offset >= thiz->length) return RET_OK;

	length = (offset + length) < thiz->length ? length : (thiz->length - offset);
	
	dst = thiz->buffer + offset;
	src = thiz->buffer + offset + length;

	i = thiz->length - length - offset;
	for(; i != 0; i--, dst++, src++)
	{
		*dst = *src;
	}
	thiz->length -= length;
	thiz->buffer[thiz->length] = '\0';

	return RET_OK;
}

Ret  ftk_text_buffer_delete_chars(FtkTextBuffer* thiz, int offset, int count)
{
	int length = 0;
	return_val_if_fail(thiz != NULL && thiz->buffer != NULL, RET_FAIL);
	return_val_if_fail(offset <= (int)thiz->length && offset >= 0, RET_FAIL);

	if(!((offset + count) <= (int)thiz->length && (offset + count) >= 0))
	{
		return RET_FAIL;
	}

	length = ftk_text_buffer_chars_bytes(thiz, offset, count);
	if(length == 0)
	{
		return RET_FAIL;
	}

	if(length < 0)
	{
		return ftk_text_buffer_delete(thiz, offset+length, -length);
	}
	else
	{
		return ftk_text_buffer_delete(thiz, offset, length);
	}
}

int  ftk_text_buffer_char_bytes(FtkTextBuffer* thiz, size_t offset, int backward)
{
	return ftk_text_buffer_chars_bytes(thiz, offset, backward ? -1 : 1);
}

int  ftk_text_buffer_chars_bytes(FtkTextBuffer* thiz, int offset, int count)
{
	const char* iter = NULL;
	const char* offset_p = NULL;
	return_val_if_fail(thiz != NULL && thiz->buffer != NULL, 0);
	return_val_if_fail(offset <= (int)thiz->length && offset >= 0, 0);
	
	if((offset + count) > (int)thiz->length || (offset + count) <0)
	{
		return 0;
	}

	offset_p = thiz->buffer + offset;
	iter = offset_p;
	if(count > 0)
	{
		for(; count > 0; count--)
		{
			 if(utf8_get_char(iter, &iter) == 0)
			 {
			 	break;
			 }
		}
	}
	else
	{
		for(; count < 0; count++)
		{
			utf8_get_prev_char(iter, &iter);
			if(iter <= thiz->buffer)
			{
				break;
			}
		}

	}
	
	return iter - offset_p;
}

Ret   ftk_text_buffer_reset(FtkTextBuffer* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	thiz->length = 0;

	return RET_OK;
}

char* ftk_text_buffer_append_string(FtkTextBuffer* thiz, const char* str)
{
	size_t str_len = 0;
	char*  start = NULL;
	return_val_if_fail(thiz != NULL && str != NULL, NULL);
	
	str_len = strlen(str) + 1;
	return_val_if_fail(ftk_text_buffer_extend(thiz, str_len) == RET_OK, NULL);

	start = thiz->buffer + thiz->length;
	ftk_strncpy(start, str, str_len);

	thiz->length += str_len;

	return start;
}

void ftk_text_buffer_destroy(FtkTextBuffer* thiz)
{
	if(thiz != NULL)
	{
		FTK_FREE(thiz->buffer);
		FTK_ZFREE(thiz, sizeof(*thiz));
	}

	return;
}


