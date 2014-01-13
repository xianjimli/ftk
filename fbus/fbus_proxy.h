/*
 * File: fbus_proxy.h
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

#ifndef FBUS_PROXY_H
#define FBUS_PROXY_H

#include "fbus_parcel.h"
#include "fbus_typedef.h"

FTK_BEGIN_DECLS

struct _FBusProxy;
typedef struct _FBusProxy FBusProxy;

typedef Ret (*FBusProxyListener)(void* ctx, FBusPushTrigger trigger, FBusParcel* notify);

FBusProxy*  fbus_proxy_create(const char* service);
FBusParcel* fbus_proxy_get_parcel(FBusProxy* thiz);
Ret  fbus_proxy_request(FBusProxy* thiz, FBusParcel* req_resp);
Ret  fbus_proxy_set_notify_listener(FBusProxy* thiz, FBusProxyListener listener, void* ctx);
void fbus_proxy_destroy(FBusProxy* thiz);

FTK_END_DECLS

#endif/*FBUS_PROXY_H*/

