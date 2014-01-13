/*
 * File: fconf_client.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   fbus client implementation for interface FConf.
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

#include "fbus_proxy.h"
#include "fconf_share.h"

#include "fconf_client.h"

typedef struct _PrivInfo
{
	FBusProxy* proxy;
	void* on_changed_ctx;
	FConfOnChanged on_changed;
}PrivInfo;

static Ret fconf_client_lock(FConf* thiz)
{
	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	FBusParcel* parcel = fbus_proxy_get_parcel(priv->proxy);;
	return_val_if_fail(priv != NULL && priv->proxy != NULL && parcel != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FCONF_LOCK);

	if((ret = fbus_proxy_request(priv->proxy, parcel)) == RET_OK)
	{
		ret = fbus_parcel_get_int(parcel);
	}

	return ret;
}

static Ret fconf_client_unlock(FConf* thiz)
{
	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	FBusParcel* parcel = fbus_proxy_get_parcel(priv->proxy);;
	return_val_if_fail(priv != NULL && priv->proxy != NULL && parcel != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FCONF_UNLOCK);

	if((ret = fbus_proxy_request(priv->proxy, parcel)) == RET_OK)
	{
		ret = fbus_parcel_get_int(parcel);
	}

	return ret;
}

static Ret fconf_client_remove(FConf* thiz, const char* xpath)
{
	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	FBusParcel* parcel = fbus_proxy_get_parcel(priv->proxy);;
	return_val_if_fail(priv != NULL && priv->proxy != NULL && parcel != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FCONF_REMOVE);
	fbus_parcel_write_string(parcel, xpath);

	if((ret = fbus_proxy_request(priv->proxy, parcel)) == RET_OK)
	{
		ret = fbus_parcel_get_int(parcel);
	}

	return ret;
}

static Ret fconf_client_set(FConf* thiz, const char* xpath, const char* value)
{
	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	FBusParcel* parcel = fbus_proxy_get_parcel(priv->proxy);;
	return_val_if_fail(priv != NULL && priv->proxy != NULL && parcel != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FCONF_SET);
	fbus_parcel_write_string(parcel, xpath);
	fbus_parcel_write_string(parcel, value);

	if((ret = fbus_proxy_request(priv->proxy, parcel)) == RET_OK)
	{
		ret = fbus_parcel_get_int(parcel);
	}

	return ret;
}

static Ret fconf_client_get(FConf* thiz, const char* xpath, char** value)
{
	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	FBusParcel* parcel = fbus_proxy_get_parcel(priv->proxy);;
	return_val_if_fail(priv != NULL && priv->proxy != NULL && parcel != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FCONF_GET);
	fbus_parcel_write_string(parcel, xpath);

	if((ret = fbus_proxy_request(priv->proxy, parcel)) == RET_OK)
	{
		if((ret = fbus_parcel_get_int(parcel)) == RET_OK)
		{
			*value = (char*)fbus_parcel_get_string(parcel);
		}
	}

	return ret;
}

static Ret fconf_client_get_child_count(FConf* thiz, const char* xpath, int* count)
{
	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	FBusParcel* parcel = fbus_proxy_get_parcel(priv->proxy);;
	return_val_if_fail(priv != NULL && priv->proxy != NULL && parcel != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FCONF_GET_CHILD_COUNT);
	fbus_parcel_write_string(parcel, xpath);

	if((ret = fbus_proxy_request(priv->proxy, parcel)) == RET_OK)
	{
		if((ret = fbus_parcel_get_int(parcel)) == RET_OK)
		{
			*count = fbus_parcel_get_int(parcel);
		}
	}

	return ret;
}

static Ret fconf_client_get_child(FConf* thiz, const char* xpath, int index, char** child)
{
	Ret ret = RET_FAIL;
	DECL_PRIV(thiz, priv);
	FBusParcel* parcel = fbus_proxy_get_parcel(priv->proxy);;
	return_val_if_fail(priv != NULL && priv->proxy != NULL && parcel != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FCONF_GET_CHILD);
	fbus_parcel_write_string(parcel, xpath);
	fbus_parcel_write_int(parcel, index);

	if((ret = fbus_proxy_request(priv->proxy, parcel)) == RET_OK)
	{
		if((ret = fbus_parcel_get_int(parcel)) == RET_OK)
		{
			*child = (char*)fbus_parcel_get_string(parcel);
		}
	}

	return ret;
}

static Ret fconf_client_on_server_push(void* ctx, FBusPushTrigger trigger, FBusParcel* parcel)
{
	int type = 0;
	FConf* thiz = ctx;
	DECL_PRIV(thiz, priv);
	const char* path = NULL;
	const char* value = NULL;
	return_val_if_fail(thiz != NULL && parcel != NULL, RET_FAIL);

	if(priv->on_changed != NULL)
	{
		type = fbus_parcel_get_int(parcel);
		path = fbus_parcel_get_string(parcel);
		if(type != FCONF_CHANGED_BY_REMOVE)
		{
			value = fbus_parcel_get_string(parcel);
		}
		priv->on_changed(priv->on_changed_ctx, trigger == FBUS_TRIGGER_BY_SELF, type, path, value);
	}

	return RET_OK;
}

static Ret fconf_client_reg_changed_notify(FConf* thiz, FConfOnChanged on_changed, void* ctx)
{
	DECL_PRIV(thiz, priv);

	priv->on_changed = on_changed;
	priv->on_changed_ctx = ctx;

	if(priv->on_changed != NULL)
	{
		fbus_proxy_set_notify_listener(priv->proxy, fconf_client_on_server_push, thiz);
	}
	else
	{
		fbus_proxy_set_notify_listener(priv->proxy, NULL, NULL);
	}
	return RET_OK;
}


static void fconf_client_destroy(FConf* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		fbus_proxy_destroy(priv->proxy);
		FTK_FREE(thiz);
	}
	return;
}

FConf* fconf_client_create(void)
{
	FConf* thiz = FTK_ZALLOC(sizeof(FConf) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		priv->proxy = fbus_proxy_create(FBUS_SERVICE_FCONF);
		thiz->lock = fconf_client_lock;
		thiz->unlock = fconf_client_unlock;
		thiz->remove = fconf_client_remove;
		thiz->set = fconf_client_set;
		thiz->get = fconf_client_get;
		thiz->get_child_count = fconf_client_get_child_count;
		thiz->get_child = fconf_client_get_child;
		thiz->reg_changed_notify = fconf_client_reg_changed_notify;
		thiz->destroy = fconf_client_destroy;
	}

	return thiz;
}

