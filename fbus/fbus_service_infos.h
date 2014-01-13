/*
 * File:    fbus_service_infos.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   FtkServiceInfo Manager.
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

#ifndef FBUS_SERVICE_INFOS_H
#define FBUS_SERVICE_INFOS_H

#include "fbus_service_manager_common.h"

FTK_BEGIN_DECLS

struct _FBusServiceInfos;
typedef struct _FBusServiceInfos FBusServiceInfos;

FBusServiceInfos* fbus_service_infos_create(int capacity);
FBusServiceInfo*  fbus_service_infos_find(FBusServiceInfos* thiz, const char* name);
Ret fbus_service_infos_get_nr(FBusServiceInfos* thiz);
Ret fbus_service_infos_add(FBusServiceInfos* thiz, FBusServiceInfo* service);
FBusServiceInfo* fbus_service_infos_get(FBusServiceInfos* thiz, size_t index);
Ret  fbus_service_infos_load_dir(FBusServiceInfos* services, const char* path);
Ret  fbus_service_infos_load_file(FBusServiceInfos* services, const char* filename);
void fbus_service_infos_destroy(FBusServiceInfos* thiz);

FTK_END_DECLS

#endif/*FBUS_SERVICE_INFOS_H*/

