/*
 * File:    fbus_service_maanger.h
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

#ifndef FBUS_SERVICE_MANAGER_H
#define FBUS_SERVICE_MANAGER_H

#include "fbus_proxy.h"
#include "fbus_service_infos.h"
#include "fbus_service_manager_common.h"
typedef struct _FBusProxy FBusServiceManager;

FTK_BEGIN_DECLS

FBusServiceManager* fbus_service_manager_create(void);
Ret fbus_service_manager_get_nr(FBusServiceManager* thiz, int* nr);
Ret fbus_service_manager_get(FBusServiceManager* thiz, int i, FBusServiceInfo* info);
Ret fbus_service_manager_stop(FBusServiceManager* thiz, const char* name);
Ret fbus_service_manager_start(FBusServiceManager* thiz, const char* name);
Ret fbus_service_manager_query(FBusServiceManager* thiz, const char* name, FBusServiceInfo* info);
Ret fbus_service_manager_register(FBusServiceManager* thiz, const char* name, FBusServiceInfo* info/*out*/);
Ret fbus_service_manager_unregister(FBusServiceManager* thiz, const char* name);
void fbus_service_manager_destroy(FBusServiceManager* thiz);

FTK_END_DECLS

#endif/*FBUS_SERVICE_MANAGER_H*/

