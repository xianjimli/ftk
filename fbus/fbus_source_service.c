/*
 * File:   fbus_source_service.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   a source to handle service request.
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

#include "fbus_typedef.h"
#include "fbus_source_service.h"

typedef struct _PrivInfo
{
	FBusStream*  stream;
	FBusParcel*  parcel;
	FBusService* service;
}PrivInfo;

static int fbus_source_service_get_fd(FtkSource* thiz)
{
	DECL_PRIV(thiz, priv);
	
	return fbus_stream_get_fd(priv->stream);
}

static int fbus_source_service_check(FtkSource* thiz)
{
	return -1;
}

Ret fbus_source_service_dispatch(FtkSource* thiz)
{
	int ret = 0;
	int size = 0;
	int type = FBUS_RESP_NORMAL;
	DECL_PRIV(thiz, priv);

	fbus_parcel_reset(priv->parcel);
	ret = fbus_stream_read_n(priv->stream, (char*)&size, sizeof(size));
	if(size <= 0)
	{
		return RET_REMOVE;
	}
	return_val_if_fail(ret == sizeof(size), RET_REMOVE);

	fbus_parcel_extend(priv->parcel, size);
	fbus_parcel_set_size(priv->parcel, size);
	ret = fbus_stream_read_n(priv->stream, fbus_parcel_data(priv->parcel), size);

	ret = fbus_service_handle_request(priv->service, fbus_stream_get_fd(priv->stream), priv->parcel);
	if(ret != RET_OK)
	{
		fbus_parcel_reset(priv->parcel);
		ret = fbus_parcel_write_int(priv->parcel, ret);
	}
	
	ret = fbus_stream_write_n(priv->stream, (const char*)&type, sizeof(type));
	size = fbus_parcel_size(priv->parcel);
	ret = fbus_stream_write_n(priv->stream, (const char*)&size, sizeof(size));
	ret = fbus_stream_write_n(priv->stream, fbus_parcel_data(priv->parcel), size);

	return ret == size ? RET_OK : RET_FAIL;
}

static void fbus_source_service_destroy(FtkSource* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		fbus_service_on_client_disconnect(priv->service, priv->stream);
		fbus_stream_destroy(priv->stream);
		FTK_FREE(thiz);
	}

	return;
}

FtkSource* fbus_source_service_create(FBusService* service, FBusStream* stream)
{
	FtkSource* thiz = NULL;
	return_val_if_fail(service != NULL && stream != NULL, NULL);

	thiz = FTK_ZALLOC(sizeof(FtkSource) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		thiz->ref = 1;
		priv->stream   = stream;
		priv->service  = service;
		priv->parcel   = fbus_parcel_create(256); 
		thiz->get_fd   = fbus_source_service_get_fd;
		thiz->check    = fbus_source_service_check;
		thiz->dispatch = fbus_source_service_dispatch;
		thiz->destroy  = fbus_source_service_destroy;

		fbus_service_on_client_connect(service, stream);
	}

	return thiz;
}

