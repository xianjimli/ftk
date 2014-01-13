/*
 * File:    fbus_service_echo.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   echo service.
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

#include "ftk_log.h"
#include "fbus_echo_common.h"
#include "fbus_service.h"

static const char* fbus_service_echo_get_name(FBusService* thiz)
{
	return FBUS_SERVICE_ECHO;
}

static Ret fbus_service_echo_on_client_connect(FBusService* thiz, int client_id)
{
	ftk_logd("%s: client_id=%d\n", __func__, client_id);
	
	return RET_OK;
}

static Ret fbus_service_echo_on_client_disconnect(FBusService* thiz, int client_id)
{
	ftk_logd("%s: client_id=%d\n", __func__, client_id);
	
	return RET_OK;
}

static Ret fbus_service_echo_handle_request(FBusService* thiz, int client_id, FBusParcel* req_resp)
{
	int req_code = fbus_parcel_get_int(req_resp);

	switch(req_code)
	{
		case FBUS_ECHO_CHAR:
		{
			char data = fbus_parcel_get_char(req_resp);
			fbus_parcel_reset(req_resp);
			fbus_parcel_write_int(req_resp, RET_OK);
			fbus_parcel_write_char(req_resp, data);
			break;
		}
		case FBUS_ECHO_SHORT:
		{
			short data = fbus_parcel_get_short(req_resp);
			fbus_parcel_reset(req_resp);
			fbus_parcel_write_int(req_resp, RET_OK);
			fbus_parcel_write_short(req_resp, data);
			break;
		}
		case FBUS_ECHO_INT:
		{
			int data = fbus_parcel_get_int(req_resp);
			fbus_parcel_reset(req_resp);
			fbus_parcel_write_int(req_resp, RET_OK);
			fbus_parcel_write_int(req_resp, data);
			break;
		}
		case FBUS_ECHO_STRING:
		{
			const char* data = fbus_parcel_get_string(req_resp);
			fbus_parcel_reset(req_resp);
			fbus_parcel_write_int(req_resp, RET_OK);
			fbus_parcel_write_string(req_resp, data);

			break;
		}
		case FBUS_ECHO_BINARY:
		{
			int len = fbus_parcel_get_int(req_resp);
			const char* data = fbus_parcel_get_data(req_resp, len);
			fbus_parcel_reset(req_resp);
			fbus_parcel_write_int(req_resp, RET_OK);
			fbus_parcel_write_data(req_resp, data, len);
			break;
		}
	}
	//ftk_logd("%s: client_id=%d\n", __func__, client_id);

	return RET_OK;	
}

static void fbus_service_echo_destroy(FBusService* thiz)
{
	if(thiz != NULL)
	{
		FTK_FREE(thiz);
	}
}

static FBusService* fbus_service_echo_create(void)
{
	FBusService* thiz = FTK_ZALLOC(sizeof(FBusService));

	if(thiz != NULL)
	{
		thiz->get_name = fbus_service_echo_get_name;
		thiz->on_client_connect = fbus_service_echo_on_client_connect;
		thiz->on_client_disconnect = fbus_service_echo_on_client_disconnect;
		thiz->handle_request = fbus_service_echo_handle_request;
		thiz->destroy = fbus_service_echo_destroy;

		fbus_service_register(thiz);
	}

	return thiz;
}

int main(int argc, char* argv[])
{
	fbus_service_init(argc, argv);
	
	if(fbus_service_echo_create() != NULL)
	{
		fbus_service_run();
	}
	else
	{
		ftk_loge("Create service echo failed.\n");
	}

	return 0;
}
