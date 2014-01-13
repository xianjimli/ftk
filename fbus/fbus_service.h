/*
 * File:    fbus_service.h
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

#ifndef FBUS_SERVICE_H
#define FBUS_SERVICE_H

#include "fbus_parcel.h"
#include "fbus_stream.h"

FTK_BEGIN_DECLS

#define FBUS_SERVICE_MAX_CLIENTS 16

struct _FBusService;
typedef struct _FBusService FBusService;

typedef const char* (*FBusServiceGetName)(FBusService* thiz);
typedef Ret         (*FBusServiceOnClientConnect)(FBusService* thiz, int client_id);
typedef Ret         (*FBusServiceOnClientDisconnect)(FBusService* thiz, int client_id);
typedef Ret         (*FBusServiceHandleRequest)(FBusService* thiz, int client_id, FBusParcel* req_resp);
typedef void        (*FBusServiceDestroy)(FBusService* thiz);

struct _FBusService
{
	FBusServiceGetName            get_name;
	FBusServiceOnClientConnect    on_client_connect;
	FBusServiceOnClientDisconnect on_client_disconnect;
	FBusServiceHandleRequest      handle_request;
	FBusServiceDestroy            destroy;

	void* data;
	char priv[ZERO_LEN_ARRAY];
};

inline static const char* fbus_service_get_name(FBusService* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->get_name != NULL, NULL);

	return thiz->get_name(thiz);
}

inline static Ret fbus_service_handle_request(FBusService* thiz, int client_id, FBusParcel* req_resp)
{
	return_val_if_fail(thiz != NULL && thiz->handle_request != NULL && req_resp != NULL, RET_FAIL);

	return thiz->handle_request(thiz, client_id, req_resp);
}

Ret  fbus_service_on_client_connect(FBusService* thiz, FBusStream* client);
Ret  fbus_service_on_client_disconnect(FBusService* thiz, FBusStream* client);
void fbus_service_destroy(FBusService* thiz);

/*XXX:The following functions are for service implementer use.*/
Ret  fbus_service_register(FBusService* thiz);
Ret  fbus_service_unregister(FBusService* thiz);

Ret  fbus_service_notify(FBusService* thiz, int target_id, int trigger_id, FBusParcel* notify);
Ret  fbus_service_notify_all(FBusService* thiz, int trigger_id, FBusParcel* notify);

Ret  fbus_service_init(int argc, char* argv[]);
Ret  fbus_service_run(void);
void fbus_service_quit(void);

FTK_END_DECLS

#endif/*FBUS_SERVICE_H*/

