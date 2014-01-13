/*
 * File:    fbus_service.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   service interface and some helper functions.
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

#include "ftk.h"
#include "fbus_service.h"
#include "ftk_source_primary.h"
#include "fbus_source_listen.h"
#include "fbus_service_manager.h"
#include "ftk_allocator_default.h"

typedef struct _ServiceData
{
	FtkSource*  listener;
	FBusStream* clients[FBUS_SERVICE_MAX_CLIENTS];
}ServiceData;

static Ret fbus_service_register_source(FBusService* thiz, int port)
{
	ServiceData* data = NULL;
	return_val_if_fail(thiz != NULL && thiz->data != NULL, RET_FAIL);

	data = thiz->data;
	return_val_if_fail(data->listener == NULL, RET_FAIL);

	if((data->listener = fbus_source_listen_create(thiz, port)) != NULL)
	{
		ftk_main_loop_add_source(ftk_default_main_loop(), data->listener);
	}

	return RET_OK;
}

Ret fbus_service_register(FBusService* thiz)
{
	int port = 0;
	Ret ret = RET_OK;
	ServiceData* data = NULL;
	const char* name = NULL;
	return_val_if_fail(thiz != NULL && thiz->data == NULL, RET_FAIL);

	name = fbus_service_get_name(thiz);
	return_val_if_fail(name != NULL, RET_FAIL);
	
	data = FTK_ZALLOC(sizeof(ServiceData));
	thiz->data = data;
	return_val_if_fail(thiz->data != NULL, RET_FAIL);
	
	if(strcmp(name, FBUS_SERVICE_MANAGER_NAME) != 0)
	{
		FBusServiceInfo info = {0};
		FBusServiceManager* service_manager = fbus_service_manager_create();
		if(service_manager != NULL)
		{
			if((ret = fbus_service_manager_register(service_manager, name, &info)) == RET_OK)
			{
				port = info.port;
			}
			fbus_service_manager_destroy(service_manager);
		}
	}
	else 
	{
		port = FBUS_SERVICE_MANAGER_PORT;
	}

	return_val_if_fail(port > 0, RET_FAIL);

	return fbus_service_register_source(thiz, port);
}

Ret fbus_service_unregister(FBusService* thiz)
{
	int i = 0;
	Ret ret = RET_OK;
	ServiceData* data = NULL;
	const char* name = NULL;

	return_val_if_fail(thiz != NULL && thiz->data != NULL, RET_FAIL);

	data = thiz->data;
	if(strcmp(name, FBUS_SERVICE_MANAGER_NAME) != 0)
	{
		FBusServiceManager* service_manager = fbus_service_manager_create();
		if(service_manager != NULL)
		{
			ret = fbus_service_manager_unregister(service_manager, name);
			fbus_service_manager_destroy(service_manager);
		}
	}

	ret = ftk_main_loop_remove_source(ftk_default_main_loop(), data->listener);
	data->listener = NULL;

	name = fbus_service_get_name(thiz);
	return_val_if_fail(name != NULL, RET_FAIL);

	for(i = 0; i < FBUS_SERVICE_MAX_CLIENTS; i++)
	{
		if(data->clients[i] != NULL)
		{
			fbus_stream_destroy(data->clients[i]);
			data->clients[i] = NULL;
		}
	}

	FTK_ZFREE(data, sizeof(ServiceData));
	thiz->data = NULL;

	return ret;
}

Ret  fbus_service_notify(FBusService* thiz, int target_id, int trigger_id, FBusParcel* notify)
{
	int i = 0;
	int ret = RET_FAIL;
	int type = FBUS_RESP_PUSH;
	ServiceData* data = NULL;
	int trigger = FBUS_TRIGGER_BY_SELF;

	return_val_if_fail(thiz != NULL && thiz->data != NULL && notify != NULL, RET_FAIL);

	data = thiz->data;
	for(i = 0; i < FBUS_SERVICE_MAX_CLIENTS; i++)
	{
		FBusStream* stream = data->clients[i];
		if(stream != NULL && (fbus_stream_get_fd(stream) == target_id || target_id < 0))
		{
			size_t size = fbus_parcel_size(notify);
	
			if(trigger_id <= 0)
			{
				trigger = FBUS_TRIGGER_BY_SERVICE;
			}
			else
			{
				if(trigger_id == fbus_stream_get_fd(stream))
				{
					trigger = FBUS_TRIGGER_BY_SELF;
				}
				else
				{
					trigger = FBUS_TRIGGER_BY_OTHER;
				}
			}

			ret = fbus_stream_write_n(stream, (char*)&type, sizeof(type));
			ret = fbus_stream_write_n(stream, (char*)&trigger, sizeof(trigger));
			ret = fbus_stream_write_n(stream, (char*)&size, sizeof(size));
			ret = fbus_stream_write_n(stream, fbus_parcel_data(notify), size);
		}
	}

	return ret;
}

Ret  fbus_service_notify_all(FBusService* thiz, int trigger_id, FBusParcel* notify)
{
	return fbus_service_notify(thiz, -1, trigger_id, notify);
}

Ret fbus_service_on_client_connect(FBusService* thiz, FBusStream* client)
{
	int i = 0;
	Ret ret = RET_OK;
	ServiceData* data = NULL;
	
	return_val_if_fail(thiz != NULL && thiz->data != NULL, RET_FAIL);

	data = thiz->data;
	if(thiz->on_client_connect != NULL)
	{
		ret = thiz->on_client_connect(thiz, fbus_stream_get_fd(client));
	}

	ftk_logd("%s: client_id=%d\n", __func__, fbus_stream_get_fd(client));
	if(ret == RET_OK)
	{
		for(i = 0; i < FBUS_SERVICE_MAX_CLIENTS; i++)
		{
			if(data->clients[i] == NULL)
			{
				data->clients[i] = client;
				break;
			}
		}

		if(i == FBUS_SERVICE_MAX_CLIENTS)
		{
			ret = RET_OUT_OF_SPACE;
			ftk_logd("%s: %s no space for client.\n", __func__, fbus_service_get_name(thiz));
		}
	}

	return ret;
}

Ret fbus_service_on_client_disconnect(FBusService* thiz, FBusStream* client)
{
	int i = 0;
	Ret ret = RET_OK;
	ServiceData* data = NULL;
	int client_id = fbus_stream_get_fd(client);

	return_val_if_fail(thiz != NULL && thiz->data != NULL, RET_FAIL);

	data = thiz->data;
	if(thiz->on_client_disconnect != NULL)
	{
		ret = thiz->on_client_disconnect(thiz, client_id);
	}
	
	for(i = 0; i < FBUS_SERVICE_MAX_CLIENTS; i++)
	{
		if(data->clients[i] != NULL && fbus_stream_get_fd(data->clients[i]) == client_id)
		{
			data->clients[i] = NULL;
			break;
		}
	}

	ftk_logd("%s: client_id=%d\n", __func__, client_id);

	return ret;
}

void fbus_service_destroy(FBusService* thiz)
{
	if(thiz != NULL && thiz->destroy != NULL)
	{
		if(thiz->data != NULL)
		{
			fbus_service_unregister(thiz);
		}
		thiz->destroy(thiz);
	}

	return;
}

/****************************************************/

Ret fbus_service_init(int argc, char* argv[])
{
	ftk_platform_init(argc, argv);

#ifndef USE_STD_MALLOC
	ftk_set_allocator((ftk_allocator_default_create()));
#endif
	ftk_set_sources_manager(ftk_sources_manager_create(64));
	ftk_set_main_loop(ftk_main_loop_create(ftk_default_sources_manager()));
	ftk_set_primary_source(ftk_source_primary_create(NULL, NULL));
	ftk_sources_manager_add(ftk_default_sources_manager(), ftk_primary_source());

	return RET_OK;
}

Ret fbus_service_run(void)
{
	return ftk_run();
}

void fbus_service_quit(void)
{
	ftk_quit();

	return;
}

