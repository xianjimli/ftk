/*
 * File:    fbus_service_manager.c 
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   client api used to access service manager.
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

#include "fbus_service_manager.h"

FBusServiceManager* fbus_service_manager_create(void)
{
	return fbus_proxy_create(FBUS_SERVICE_MANAGER_NAME);
}

Ret fbus_service_manager_get_nr(FBusServiceManager* thiz, int* nr)
{
	Ret ret = RET_FAIL;
	FBusParcel* parcel = NULL;
	return_val_if_fail(thiz != NULL && nr != NULL, RET_FAIL);
	parcel = fbus_proxy_get_parcel(thiz);
	return_val_if_fail(parcel != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FBUS_SERVICE_MANAGER_REQ_GET_NR);
	
	if((ret = fbus_proxy_request(thiz, parcel)) == RET_OK)
	{
		ret = fbus_parcel_get_int(parcel);
		if(ret == RET_OK)
		{
			*nr = fbus_parcel_get_int(parcel);	
		}
	}

	return ret;
}

Ret fbus_service_manager_get(FBusServiceManager* thiz, int i, FBusServiceInfo* info)
{
	Ret ret = RET_FAIL;
	FBusParcel* parcel = NULL;
	return_val_if_fail(thiz != NULL && info != NULL, RET_FAIL);
	parcel = fbus_proxy_get_parcel(thiz);
	return_val_if_fail(parcel != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FBUS_SERVICE_MANAGER_REQ_GET);
	fbus_parcel_write_int(parcel, i);
	
	if((ret = fbus_proxy_request(thiz, parcel)) == RET_OK)
	{
		ret = fbus_parcel_get_int(parcel);
		if(ret == RET_OK)
		{
			*info = *(FBusServiceInfo*)fbus_parcel_get_data(parcel, sizeof(FBusServiceInfo));
		}
	}

	return ret;
}

Ret fbus_service_manager_stop(FBusServiceManager* thiz, const char* name)
{
	Ret ret = RET_FAIL;
	FBusParcel* parcel = NULL;
	return_val_if_fail(thiz != NULL && name != NULL, RET_FAIL);
	parcel = fbus_proxy_get_parcel(thiz);
	return_val_if_fail(parcel != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FBUS_SERVICE_MANAGER_REQ_STOP);
	fbus_parcel_write_string(parcel, name);
	
	if((ret = fbus_proxy_request(thiz, parcel)) == RET_OK)
	{
		ret = fbus_parcel_get_int(parcel);
	}

	return ret;
}

Ret fbus_service_manager_start(FBusServiceManager* thiz, const char* name)
{
	Ret ret = RET_FAIL;
	FBusParcel* parcel = NULL;
	return_val_if_fail(thiz != NULL && name != NULL, RET_FAIL);
	parcel = fbus_proxy_get_parcel(thiz);
	return_val_if_fail(parcel != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FBUS_SERVICE_MANAGER_REQ_START);
	fbus_parcel_write_string(parcel, name);
	
	if((ret = fbus_proxy_request(thiz, parcel)) == RET_OK)
	{
		ret = fbus_parcel_get_int(parcel);
	}

	return ret;
}

Ret fbus_service_manager_query(FBusServiceManager* thiz, const char* name, FBusServiceInfo* info)
{
	Ret ret = RET_FAIL;
	FBusParcel* parcel = NULL;
	return_val_if_fail(thiz != NULL && name != NULL && info != NULL, RET_FAIL);
	parcel = fbus_proxy_get_parcel(thiz);
	return_val_if_fail(parcel != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FBUS_SERVICE_MANAGER_REQ_QUERY);
	fbus_parcel_write_string(parcel, name);
	
	if((ret = fbus_proxy_request(thiz, parcel)) == RET_OK)
	{
		ret = fbus_parcel_get_int(parcel);
		if(ret == RET_OK)
		{
			*info = *(FBusServiceInfo*)fbus_parcel_get_data(parcel, sizeof(FBusServiceInfo));
		}
	}

	return ret;
}

Ret fbus_service_manager_register(FBusServiceManager* thiz, const char* name, FBusServiceInfo* info)
{
	Ret ret = RET_FAIL;
	FBusParcel* parcel = NULL;
	return_val_if_fail(thiz != NULL && name != NULL && info != NULL, RET_FAIL);
	parcel = fbus_proxy_get_parcel(thiz);
	return_val_if_fail(parcel != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FBUS_SERVICE_MANAGER_REQ_REGISTER);
	fbus_parcel_write_string(parcel, name);
	fbus_parcel_write_string(parcel, FBUS_LOCALHOST);
	fbus_parcel_write_int(parcel, getpid());
	
	if((ret = fbus_proxy_request(thiz, parcel)) == RET_OK)
	{
		ret = fbus_parcel_get_int(parcel);
		if(ret == RET_OK)
		{
			*info = *(FBusServiceInfo*)fbus_parcel_get_data(parcel, sizeof(FBusServiceInfo));
		}
	}

	return ret;
}

Ret fbus_service_manager_unregister(FBusServiceManager* thiz, const char* name)
{
	Ret ret = RET_FAIL;
	FBusParcel* parcel = NULL;
	return_val_if_fail(thiz != NULL && name != NULL, RET_FAIL);
	parcel = fbus_proxy_get_parcel(thiz);
	return_val_if_fail(parcel != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FBUS_SERVICE_MANAGER_REQ_UNREGISTER);
	fbus_parcel_write_string(parcel, name);
	fbus_parcel_write_string(parcel, FBUS_LOCALHOST);
	fbus_parcel_write_int(parcel, getpid());

	if((ret = fbus_proxy_request(thiz, parcel)) == RET_OK)
	{
		ret = fbus_parcel_get_int(parcel);
	}

	return ret;
}

void fbus_service_manager_destroy(FBusServiceManager* thiz)
{
	fbus_proxy_destroy(thiz);

	return;
}

