/*
 * File: fconf_service.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   FConf Service.
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
 * 2010-08-01 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "fconf.h"
#include "fconf_xml.h"
#include "fconf_share.h"
#include "ftk_globals.h"
#include "ftk_main_loop.h"
#include "fconf_service.h"
#include "ftk_source_timer.h"

typedef struct _PrivInfo
{
	FConf* impl;
	int client_id;
	int peer_id; /*the port in client side*/
	FBusParcel* parcel;
	FtkSource* timer_save;
}PrivInfo;

static Ret fconf_marshal_lock(FBusService* thiz, FBusParcel* req_resp)
{
	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && priv->impl != NULL && req_resp != NULL, RET_FAIL);

	if((ret = fconf_lock(priv->impl)) == RET_OK)
	{
		fbus_parcel_reset(req_resp);
		fbus_parcel_write_int(req_resp, ret);
	}

	return ret;
}

static Ret fconf_marshal_unlock(FBusService* thiz, FBusParcel* req_resp)
{
	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && priv->impl != NULL && req_resp != NULL, RET_FAIL);

	if((ret = fconf_unlock(priv->impl)) == RET_OK)
	{
		fbus_parcel_reset(req_resp);
		fbus_parcel_write_int(req_resp, ret);
	}

	return ret;
}

static Ret fconf_marshal_remove(FBusService* thiz, FBusParcel* req_resp)
{
	const char* xpath = {0};

	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && priv->impl != NULL && req_resp != NULL, RET_FAIL);

	xpath = fbus_parcel_get_string(req_resp);

	if((ret = fconf_remove(priv->impl, xpath)) == RET_OK)
	{
		fbus_parcel_reset(req_resp);
		fbus_parcel_write_int(req_resp, ret);
	}

	return ret;
}

static Ret fconf_marshal_set(FBusService* thiz, FBusParcel* req_resp)
{
	const char* xpath = {0};
	const char* value = {0};

	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && priv->impl != NULL && req_resp != NULL, RET_FAIL);

	xpath = fbus_parcel_get_string(req_resp);
	value = fbus_parcel_get_string(req_resp);

	if((ret = fconf_set(priv->impl, xpath, value)) == RET_OK)
	{
		fbus_parcel_reset(req_resp);
		fbus_parcel_write_int(req_resp, ret);
	}

	return ret;
}

static Ret fconf_marshal_get(FBusService* thiz, FBusParcel* req_resp)
{
	const char* xpath = {0};
	char* value = {0};

	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && priv->impl != NULL && req_resp != NULL, RET_FAIL);

	xpath = fbus_parcel_get_string(req_resp);

	if((ret = fconf_get(priv->impl, xpath, &value)) == RET_OK)
	{
		fbus_parcel_reset(req_resp);
		fbus_parcel_write_int(req_resp, ret);
		if(ret == RET_OK)
		{
			fbus_parcel_write_string(req_resp, value);
		}
	}

	return ret;
}

static Ret fconf_marshal_get_child_count(FBusService* thiz, FBusParcel* req_resp)
{
	const char* xpath = {0};
	int count = {0};

	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && priv->impl != NULL && req_resp != NULL, RET_FAIL);

	xpath = fbus_parcel_get_string(req_resp);

	if((ret = fconf_get_child_count(priv->impl, xpath, &count)) == RET_OK)
	{
		fbus_parcel_reset(req_resp);
		fbus_parcel_write_int(req_resp, ret);
		if(ret == RET_OK)
		{
			fbus_parcel_write_int(req_resp, count);
		}
	}

	return ret;
}

static Ret fconf_marshal_get_child(FBusService* thiz, FBusParcel* req_resp)
{
	const char* xpath = {0};
	int index = {0};
	char* child = {0};

	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && priv->impl != NULL && req_resp != NULL, RET_FAIL);

	xpath = fbus_parcel_get_string(req_resp);
	index = fbus_parcel_get_int(req_resp);

	if((ret = fconf_get_child(priv->impl, xpath, index, &child)) == RET_OK)
	{
		fbus_parcel_reset(req_resp);
		fbus_parcel_write_int(req_resp, ret);
		if(ret == RET_OK)
		{
			fbus_parcel_write_string(req_resp, child);
		}
	}

	return ret;
}

static const char* fconf_service_get_name(FBusService* thiz)
{
	return FBUS_SERVICE_FCONF;
}

static Ret fconf_service_on_client_connect(FBusService* thiz, int client_id)
{
	return RET_OK;
}

static Ret fconf_service_on_client_disconnect(FBusService* thiz, int client_id)
{
	return RET_OK;
}

static Ret fconf_service_handle_request(FBusService* thiz, int client_id, FBusParcel* req_resp)
{
	DECL_PRIV(thiz, priv);
	int req_code = fbus_parcel_get_int(req_resp);
	
	priv->client_id = client_id;
	switch(req_code)
	{
		case FCONF_LOCK:
		{
			fconf_marshal_lock(thiz, req_resp);
			break;
		}
		case FCONF_UNLOCK:
		{
			fconf_marshal_unlock(thiz, req_resp);
			break;
		}
		case FCONF_REMOVE:
		{
			fconf_marshal_remove(thiz, req_resp);
			break;
		}
		case FCONF_SET:
		{
			fconf_marshal_set(thiz, req_resp);
			break;
		}
		case FCONF_GET:
		{
			fconf_marshal_get(thiz, req_resp);
			break;
		}
		case FCONF_GET_CHILD_COUNT:
		{
			fconf_marshal_get_child_count(thiz, req_resp);
			break;
		}
		case FCONF_GET_CHILD:
		{
			fconf_marshal_get_child(thiz, req_resp);
			break;
		}
		default:break;
	}

	if(fconf_xml_is_dirty(priv->impl) && priv->timer_save->ref < 2)
	{
		ftk_logd("%s: timer save %d\n", __func__, priv->timer_save->ref);
		ftk_source_ref(priv->timer_save);
		ftk_main_loop_add_source(ftk_default_main_loop(), priv->timer_save);	
	}

	return RET_OK;
}

static void fconf_service_destroy(FBusService* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		ftk_source_disable(priv->timer_save);
		ftk_source_unref(priv->timer_save);
		fbus_parcel_destroy(priv->parcel);
		fconf_destroy(priv->impl);
		FTK_FREE(thiz);
	}
	return;
}

static Ret on_changed(void* ctx, int change_by_self, FConfChangeType type, const char* xpath, const char* value)
{
	FBusService* thiz = ctx;
	DECL_PRIV(thiz, priv);

	fbus_parcel_reset(priv->parcel);
	fbus_parcel_write_int(priv->parcel, type);
	fbus_parcel_write_string(priv->parcel, xpath);

	if(value != NULL)
	{
		fbus_parcel_write_string(priv->parcel, value);
	}
	fbus_service_notify_all(thiz, priv->client_id, priv->parcel);
	ftk_logd("%s: client_id=%d %d %d %s %s\n", __func__, priv->client_id, change_by_self, type, xpath, value);

	return RET_OK;
}

static Ret timer_save_func(void* ctx)
{
	FBusService* thiz = ctx;
	DECL_PRIV(thiz, priv);

	fconf_xml_save(priv->impl);

	return RET_REMOVE;
}

FBusService* fconf_service_create(void)
{
	FBusService* thiz = FTK_ZALLOC(sizeof(FBusService)+sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		priv->parcel = fbus_parcel_create(1024);
		priv->timer_save = ftk_source_timer_create(10000, timer_save_func, thiz);
		priv->impl = fconf_xml_create(DATA_DIR"/config");
		thiz->get_name = fconf_service_get_name;
		thiz->on_client_connect = fconf_service_on_client_connect;
		thiz->on_client_disconnect = fconf_service_on_client_disconnect;
		thiz->handle_request = fconf_service_handle_request;
		thiz->destroy = fconf_service_destroy;
		fbus_service_register(thiz);
		fconf_reg_changed_notify(priv->impl, on_changed, thiz);
	}

	return thiz;
}


