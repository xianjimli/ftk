/*
 * File:    fbus_service_manager_common.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   some stuff used by both the service and client of service manager.
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


#ifndef FBUS_SERVICE_MANAGER_COMMON_H
#define FBUS_SERVICE_MANAGER_COMMON_H

#include "fbus_typedef.h"

FTK_BEGIN_DECLS

typedef enum _FBusServiceManagerReqCode
{
	FBUS_SERVICE_MANAGER_REQ_NONE,
	FBUS_SERVICE_MANAGER_REQ_GET,
	FBUS_SERVICE_MANAGER_REQ_GET_NR,
	FBUS_SERVICE_MANAGER_REQ_STOP,
	FBUS_SERVICE_MANAGER_REQ_START,
	FBUS_SERVICE_MANAGER_REQ_QUERY,
	FBUS_SERVICE_MANAGER_REQ_REGISTER,
	FBUS_SERVICE_MANAGER_REQ_UNREGISTER,
	FBUS_SERVICE_MANAGER_REQ_NR
}FBusServiceManagerReqCode;

typedef enum _FBusServiceStatus
{
	FBUS_SERVICE_STOPED,
	FBUS_SERVICE_STOPING,
	FBUS_SERVICE_STARTED,
	FBUS_SERVICE_STARTING,
	FBUS_SERVICE_NOT_AVAIL
}FBusServiceStatus;

typedef enum _FBusServiceStartType
{
	FBUS_SERVICE_START_NONE,
	FBUS_SERVICE_START_ON_LOAD,
	FBUS_SERVICE_START_ON_REQUEST
}FBusServiceStartType;

typedef enum _FBusServiceLifeCycle
{
	FBUS_SERVICE_RUN_ONCE,
	FBUS_SERVICE_RUN_FOREVER
}FBusServiceLifeCycle;

typedef struct _FBusServiceInfo
{
	int    pid;
	int    port;
	int    status;
	char   host[32];
	char   name[32];
	int    start_type;
	int    life_cycle;
	time_t start_time;
	char   exec[FTK_MAX_PATH + 1];
}FBusServiceInfo;

#define FBUS_LOCALHOST            "127.0.0.1"
#define FTK_MAX_SERVICE_NR        16
#define FBUS_SERVICE_MANAGER_PORT 1978
#define FBUS_SERVICE_MANAGER_NAME "fbus.service.manager"

FTK_END_DECLS

#endif/*FBUS_SERVICE_MANAGER_COMMON_H*/

