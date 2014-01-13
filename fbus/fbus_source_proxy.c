/*
 * File:    fbus_source_proxy.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   a source to handle service push event.
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

#include "fbus_source_proxy.h"

typedef struct _PrivInfo
{
	FBusProxy*   proxy;
	FBusStream*  stream;
	FBusProxyListener listener;
	void* listener_ctx;
}PrivInfo;

static int fbus_source_proxy_get_fd(FtkSource* thiz)
{
	DECL_PRIV(thiz, priv);
	
	return fbus_stream_get_fd(priv->stream);
}

static int fbus_source_proxy_check(FtkSource* thiz)
{
	return -1;
}

static Ret fbus_source_proxy_dispatch(FtkSource* thiz)
{
	int type = 0;
	int size = 0;
	int trigger = 0;
	int ret = RET_FAIL;
	FBusParcel* parcel = NULL;
	DECL_PRIV(thiz, priv);
	
	parcel = fbus_proxy_get_parcel(priv->proxy);
	return_val_if_fail(parcel != NULL, RET_REMOVE);

	ret = fbus_stream_read_n(priv->stream, (char*)&type, sizeof(type));
	return_val_if_fail(ret == sizeof(type) && type == FBUS_RESP_PUSH, RET_FAIL);
	
	ret = fbus_stream_read_n(priv->stream, (char*)&trigger, sizeof(trigger));
	return_val_if_fail(ret == sizeof(trigger), RET_FAIL);
	
	ret = fbus_stream_read_n(priv->stream, (char*)&size, sizeof(size));
	return_val_if_fail(ret == sizeof(size), RET_REMOVE);

	ret = fbus_parcel_extend(parcel, size);
	return_val_if_fail(ret == RET_OK, RET_REMOVE);
	
	fbus_parcel_set_size(parcel, size);
	ret = fbus_stream_read_n(priv->stream, fbus_parcel_data(parcel), size);

	priv->listener(priv->listener_ctx, trigger, parcel);

	return RET_OK;
}

static void fbus_source_proxy_destroy(FtkSource* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		fbus_proxy_set_notify_listener(priv->proxy, NULL, NULL);
		FTK_FREE(thiz);
	}

	return;
}

FtkSource* fbus_source_proxy_create(FBusProxy* proxy, FBusStream* stream, FBusProxyListener listener, void* ctx)
{
	FtkSource* thiz = NULL;
	return_val_if_fail(proxy != NULL && stream != NULL && listener != NULL, NULL);

	thiz = FTK_ZALLOC(sizeof(FtkSource) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		thiz->check    = fbus_source_proxy_check;
		thiz->get_fd   = fbus_source_proxy_get_fd;
		thiz->destroy  = fbus_source_proxy_destroy;
		thiz->dispatch = fbus_source_proxy_dispatch;

		thiz->ref = 1;
		priv->proxy = proxy;
		priv->stream = stream;
		priv->listener = listener;
		priv->listener_ctx = ctx;
	}

	return thiz;
}

