/*
 * File:    fbus_proxy.c 
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   the proxy object in client side, help you to access remote 
 *          object in server side.
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

#include "ftk_source.h"
#include "fbus_parcel.h"
#include "fbus_proxy.h"
#include "ftk_globals.h"
#include "ftk_main_loop.h"
#include "fbus_stream_socket.h"
#include "fbus_source_proxy.h"
#include "fbus_service_manager.h"

struct _FBusProxy
{
	int busy;
	FBusParcel* parcel;
	FBusStream* stream;
	FtkSource* source;
	void* listener_ctx;
	FBusProxyListener listener;
};

FBusProxy*  fbus_proxy_create(const char* service)
{
	Ret ret = RET_FAIL;
	FBusProxy* thiz = NULL;
	FBusStream* stream = NULL;
	FBusServiceInfo info = {0};
	
	return_val_if_fail(service != NULL, NULL);

	if(strcmp(service, FBUS_SERVICE_MANAGER_NAME) != 0)
	{
		FBusServiceManager* service_manager = fbus_proxy_create(FBUS_SERVICE_MANAGER_NAME);
		if(service_manager != NULL)
		{
			ret = fbus_service_manager_start(service_manager, service);
			if(ret == RET_OK)
			{
				int i = 0;
				for(i = 0; i < 5; i++)
				{
					ret = fbus_service_manager_query(service_manager, service, &info);
					if(ret ==  RET_OK && info.status == FBUS_SERVICE_STARTED)
					{
						break;
					}

					if(ret != RET_OK)
					{
						usleep(200000);
					}
				}
			}
			fbus_proxy_destroy(service_manager);
		}
	}
	else
	{
		strcpy(info.host, FBUS_LOCALHOST);
		info.port = FBUS_SERVICE_MANAGER_PORT;
	}

	return_val_if_fail(info.port > 0, NULL);

	stream = fbus_stream_socket_connect(info.host, info.port);
	return_val_if_fail(stream != NULL, NULL);

	thiz = FTK_ZALLOC(sizeof(FBusProxy));
	if(thiz != NULL)
	{
		thiz->stream = stream;
	}
	else
	{
		fbus_stream_destroy(stream);
	}

	return thiz;
}

FBusParcel* fbus_proxy_get_parcel(FBusProxy* thiz)
{
	return_val_if_fail(thiz != NULL, NULL);

	if(thiz->parcel == NULL)
	{
		thiz->parcel = fbus_parcel_create(256);
	}
	else
	{
		fbus_parcel_reset(thiz->parcel);
	}

	return thiz->parcel;
}

static Ret  fbus_proxy_send_request(FBusProxy* thiz, FBusParcel* parcel)
{
	int size = 0;
	int ret = RET_FAIL;
	return_val_if_fail(thiz != NULL && thiz->stream != NULL && parcel != NULL, RET_FAIL);

	size = fbus_parcel_size(parcel);
	ret = fbus_stream_write_n(thiz->stream, (const char*)&size, sizeof(size));
	return_val_if_fail(ret == sizeof(size), RET_FAIL);

	ret = fbus_stream_write_n(thiz->stream, fbus_parcel_data(parcel), size);
	return_val_if_fail(ret == size, RET_FAIL);

	return RET_OK;
}

static Ret  fbus_proxy_recv_response(FBusProxy* thiz, FBusParcel* parcel)
{
	int type = 0;
	int trigger = 0;
	int size = 0;
	int ret = RET_FAIL;
	return_val_if_fail(thiz != NULL && thiz->stream != NULL && parcel != NULL, RET_FAIL);

	do
	{
		fbus_parcel_reset(parcel);
		ret = fbus_stream_read_n(thiz->stream, (char*)&type, sizeof(type));
		return_val_if_fail(ret == sizeof(type), RET_FAIL);
		
		if(type == FBUS_RESP_PUSH)
		{
			ret = fbus_stream_read_n(thiz->stream, (char*)&trigger, sizeof(trigger));
			return_val_if_fail(ret == sizeof(trigger), RET_FAIL);
		}
		ret = fbus_stream_read_n(thiz->stream, (char*)&size, sizeof(size));
		return_val_if_fail(ret == sizeof(size), RET_FAIL);
		
		fbus_parcel_extend(parcel, size);
		fbus_parcel_set_size(parcel, size);
		ret = fbus_stream_read_n(thiz->stream, fbus_parcel_data(parcel), size);
		return_val_if_fail(ret == size, RET_FAIL);

		if(type == FBUS_RESP_PUSH)
		{
			if(thiz->listener != NULL)
			{
				thiz->listener(thiz->listener_ctx, trigger, parcel);
			}
		}
	}while(type != FBUS_RESP_NORMAL);
	
	return RET_OK;
}

Ret  fbus_proxy_set_notify_listener(FBusProxy* thiz, FBusProxyListener listener, void* ctx)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	thiz->listener = listener;
	thiz->listener_ctx = ctx;
	
	if(thiz->source != NULL)
	{
		ftk_main_loop_remove_source(ftk_default_main_loop(), thiz->source);
		thiz->source = NULL;
	}

	if(listener != NULL)
	{
		thiz->source = fbus_source_proxy_create(thiz, thiz->stream, listener, ctx);

		if(thiz->source != NULL)
		{
			ftk_main_loop_add_source(ftk_default_main_loop(), thiz->source);
		}
	}

	return RET_OK;
}

void fbus_proxy_destroy(FBusProxy* thiz)
{
	if(thiz != NULL)
	{
		if(thiz->source != NULL)
		{
			fbus_proxy_set_notify_listener(thiz, NULL, NULL);
		}
		fbus_stream_destroy(thiz->stream);
		if(thiz->parcel != NULL)
		{
			fbus_parcel_destroy(thiz->parcel);
		}

		FTK_FREE(thiz);
	}

	return;
}

Ret fbus_proxy_request(FBusProxy* thiz, FBusParcel* req_resp)
{
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL && req_resp != NULL, RET_FAIL);
	return_val_if_fail(thiz->busy == 0, RET_FAIL);

	thiz->busy = 1;
	if((ret = fbus_proxy_send_request(thiz, req_resp)) == RET_OK)
	{
		ret = fbus_proxy_recv_response(thiz, req_resp);
	}
	thiz->busy = 0;

	return ret;
}

