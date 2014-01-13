/*
 * File:    fbus_stream.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   stream interface.
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
 * 2010-07-25 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FBUS_STREAM_H
#define FBUS_STREAM_H

#include "ftk_log.h"

FTK_BEGIN_DECLS

struct _FBusStream;
typedef struct _FBusStream FBusStream;

typedef int  (*FBusStreamRead)(FBusStream* thiz, char* buffer, size_t len);
typedef int  (*FBusStreamWrite)(FBusStream* thiz, const char* buffer, size_t len);
typedef int  (*FBusStreamGetFd)(FBusStream* thiz);
typedef void (*FBusStreamDestroy)(FBusStream* thiz);

struct _FBusStream
{
	FBusStreamRead read;
	FBusStreamWrite write;
	FBusStreamGetFd get_fd;
	FBusStreamDestroy destroy;

	char priv[ZERO_LEN_ARRAY];
};

inline static int fbus_stream_read(FBusStream* thiz, char* buffer, size_t len)
{
	return_val_if_fail(thiz != NULL && thiz->read != NULL &&  buffer != NULL, -1);

	return thiz->read(thiz, buffer, len);
}

inline static int fbus_stream_write(FBusStream* thiz, const char* buffer, size_t len)
{
	return_val_if_fail(thiz != NULL && thiz->write != NULL && buffer != NULL, -1);

	return thiz->write(thiz, buffer, len);
}

inline static int fbus_stream_get_fd(FBusStream* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->get_fd != NULL, -1);

	return thiz->get_fd(thiz);
}

inline static void fbus_stream_destroy(FBusStream* thiz)
{
	if(thiz != NULL && thiz->destroy != NULL)
	{
		thiz->destroy(thiz);
	}

	return;
}

int fbus_stream_read_n(FBusStream* thiz, char* buffer, size_t len);
int fbus_stream_write_n(FBusStream* thiz, const char* buffer, size_t len);

FTK_END_DECLS

#endif/*FBUS_STREAM_H*/

