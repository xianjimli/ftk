/*
 * File:    fbus_source_listen.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   a source to listen on a socket port.
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

#include "ftk_globals.h"
#include "ftk_main_loop.h"
#include "fbus_stream_socket.h"
#include "fbus_source_listen.h"
#include "fbus_source_service.h"

typedef struct _PrivInfo
{
	int sock_no;
	FBusService* service;
}PrivInfo;

static int fbus_source_listen_get_fd(FtkSource* thiz)
{
	DECL_PRIV(thiz, priv);
	
	return priv->sock_no;
}

static int fbus_source_listen_check(FtkSource* thiz)
{
	return -1;
}

Ret fbus_source_listen_dispatch(FtkSource* thiz)
{
	FtkSource* source = NULL;
	FBusStream* stream = NULL;
	DECL_PRIV(thiz, priv);

	stream = fbus_socket_accept(priv->sock_no);
	return_val_if_fail(stream != NULL, RET_REMOVE);

	source = fbus_source_service_create(priv->service, stream);

	if(source != NULL)
	{
		ftk_main_loop_add_source(ftk_default_main_loop(), source);
	}
	else
	{
		fbus_stream_destroy(stream);
	}

	return RET_OK;
}

static void fbus_source_listen_destroy(FtkSource* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		ftk_socket_close(priv->sock_no);
		fbus_service_destroy(priv->service);
		FTK_FREE(thiz);
	}

	return;
}

FtkSource* fbus_source_listen_create(FBusService* service, int port)
{
	int sock_no = 0;
	FtkSource* thiz = NULL;
	return_val_if_fail(service != NULL && port > 0, NULL);
	sock_no = fbus_stream_listen(port);
	return_val_if_fail(sock_no > 0, NULL);

	thiz = FTK_ZALLOC(sizeof(FtkSource) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		priv->sock_no = sock_no;
		priv->service = service;

		thiz->ref = 1;
		thiz->check = fbus_source_listen_check;
		thiz->get_fd = fbus_source_listen_get_fd;
		thiz->dispatch = fbus_source_listen_dispatch;
		thiz->destroy = fbus_source_listen_destroy;
	}
	else
	{
		ftk_socket_close(sock_no);
	}

	return thiz;
}

