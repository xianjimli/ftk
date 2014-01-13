/*
 * File:    fbus_service_manager_daemon.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   service manager daemon.
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
#include <sys/wait.h>
#include <sys/types.h>
#include "ftk_globals.h"
#include "ftk_main_loop.h"
#include "fbus_service.h"
#include "ftk_source_timer.h"
#include "fbus_service_infos.h"
#include "fbus_service_manager.h"

typedef struct _PrivInfo
{
	int quiting;
	FBusServiceInfos* service_infos;
}PrivInfo;

static int process_start(const char* exec)
{
	int pid = fork();

	if(pid == 0)
	{
		execl(exec, exec, NULL);
	}

	return pid;
}

static Ret process_stop(int pid)
{
	int ret = kill(pid, SIGTERM);

	return ret == 0 ? RET_OK : RET_FAIL;
}

static int process_wait_child(void)
{
	int status = 0;

	return waitpid(-1, &status, WNOHANG);
}

static const char* fbus_service_service_manager_get_name(FBusService* thiz)
{
	return FBUS_SERVICE_MANAGER_NAME;
}

static Ret fbus_service_service_manager_handle_get_nr(FBusService* thiz, int* nr)
{
	DECL_PRIV(thiz, priv);
	*nr = fbus_service_infos_get_nr(priv->service_infos);

	return RET_OK;
}

static Ret fbus_service_service_manager_handle_get(FBusService* thiz, int i, FBusServiceInfo* info)
{
	DECL_PRIV(thiz, priv);

	FBusServiceInfo* found = fbus_service_infos_get(priv->service_infos, i);
	return_val_if_fail(found != NULL, RET_FAIL);

	memcpy(info, found, sizeof(FBusServiceInfo));

	return RET_OK;
}

static Ret fbus_service_service_manager_handle_stop(FBusService* thiz, const char* name)
{
	DECL_PRIV(thiz, priv);
	FBusServiceInfo* info = fbus_service_infos_find(priv->service_infos, name);
	return_val_if_fail(info != NULL, RET_NOT_FOUND);

	if(info->status == FBUS_SERVICE_STARTED)
	{
		process_stop(info->pid);
		info->status = FBUS_SERVICE_STOPING;
	}

	return RET_OK;
}

static Ret fbus_service_service_manager_handle_start(FBusService* thiz, const char* name)
{
	DECL_PRIV(thiz, priv);
	FBusServiceInfo* info = fbus_service_infos_find(priv->service_infos, name);
	return_val_if_fail(info != NULL, RET_NOT_FOUND);

	if(info->status == FBUS_SERVICE_STOPED)
	{
		process_start(info->exec);
		info->status = FBUS_SERVICE_STARTING;
	}

	return RET_OK;
}

static Ret fbus_service_service_manager_handle_query(FBusService* thiz, const char* name, FBusServiceInfo* info)
{
	DECL_PRIV(thiz, priv);
	FBusServiceInfo* found = fbus_service_infos_find(priv->service_infos, name);
	return_val_if_fail(found != NULL, RET_NOT_FOUND);

	memcpy(info, found, sizeof(FBusServiceInfo));

	return RET_OK;
}

static Ret fbus_service_service_manager_handle_register(FBusService* thiz, const char* name, 
	FBusServiceInfo* info, const char* host, int pid)
{
	DECL_PRIV(thiz, priv);
	FBusServiceInfo* found = fbus_service_infos_find(priv->service_infos, name);
	return_val_if_fail(found != NULL, RET_NOT_FOUND);
	return_val_if_fail(host != NULL, RET_FAIL);

	found->pid = pid;
	ftk_strncpy(found->host, host, sizeof(found->host)-1);

	found->status = FBUS_SERVICE_STARTED;
	memcpy(info, found, sizeof(FBusServiceInfo));

	return RET_OK;
}

static Ret fbus_service_service_manager_handle_unregister(FBusService* thiz, const char* name,
	const char* host, int pid)
{
	DECL_PRIV(thiz, priv);
	FBusServiceInfo* found = fbus_service_infos_find(priv->service_infos, name);
	return_val_if_fail(found != NULL, RET_NOT_FOUND);
	return_val_if_fail(found->pid == pid && strcmp(found->host, host) == 0, RET_FAIL);
	return_val_if_fail(host != NULL, RET_FAIL);

	found->status = FBUS_SERVICE_STOPED;

	return RET_OK;
}

static Ret fbus_service_service_manager_check_children(FBusService* thiz)
{
	int pid = 0;
	DECL_PRIV(thiz, priv);

	do
	{
		int i = 0;
		int nr = 0;
		
		if((pid = process_wait_child()) <= 0) break;
		
		nr = fbus_service_infos_get_nr(priv->service_infos);
		for(i = 0; i < nr; i++)
		{
			FBusServiceInfo* info = fbus_service_infos_get(priv->service_infos, i);
			if(info->pid == pid)
			{
				info->status = FBUS_SERVICE_STOPED;
				ftk_logd("%s: service %s %d stoped.\n", __func__, info->name, info->pid);

				if(!priv->quiting)
				{
					if(info->start_type == FBUS_SERVICE_RUN_FOREVER)
					{
						process_start(info->exec);
						info->status = FBUS_SERVICE_STARTING;
						ftk_logd("%s: restart service %s\n", __func__, info->name);
					}
				}
				break;
			}
		}
	}while(pid > 0);

	return RET_OK;
}

static Ret fbus_service_service_manager_handle_request(FBusService* thiz, int client_id, FBusParcel* req_resp)
{
	int pid = 0;
	Ret ret = RET_OK;
	int req_code = fbus_parcel_get_int(req_resp);

	switch(req_code)
	{
		case FBUS_SERVICE_MANAGER_REQ_GET:
		{
			FBusServiceInfo info = {0};
			int index = fbus_parcel_get_int(req_resp);
			
			ret = fbus_service_service_manager_handle_get(thiz, index, &info);
			
			fbus_parcel_reset(req_resp);
			fbus_parcel_write_int(req_resp, ret);
			if(ret == RET_OK)
			{
				fbus_parcel_write_data(req_resp, &info, sizeof(info));
			}
			break;
		}
		case FBUS_SERVICE_MANAGER_REQ_GET_NR:
		{
			int nr = 0;
			
			ret = fbus_service_service_manager_handle_get_nr(thiz, &nr);
			
			fbus_parcel_reset(req_resp);
			fbus_parcel_write_int(req_resp, ret);
			fbus_parcel_write_int(req_resp, nr);
			break;
		}
		case FBUS_SERVICE_MANAGER_REQ_STOP:
		{
			const char* name = fbus_parcel_get_string(req_resp);
			
			ret = fbus_service_service_manager_handle_stop(thiz, name);
			
			fbus_parcel_reset(req_resp);
			fbus_parcel_write_int(req_resp, ret);
			break;
		}
		case FBUS_SERVICE_MANAGER_REQ_START:
		{
			const char* name = fbus_parcel_get_string(req_resp);
			
			ret = fbus_service_service_manager_handle_start(thiz, name);
			
			fbus_parcel_reset(req_resp);
			fbus_parcel_write_int(req_resp, ret);
			break;
		}
		case FBUS_SERVICE_MANAGER_REQ_QUERY:
		{
			FBusServiceInfo info = {0};
			const char* name = fbus_parcel_get_string(req_resp);
			
			ret = fbus_service_service_manager_handle_query(thiz, name, &info);
			
			fbus_parcel_reset(req_resp);
			fbus_parcel_write_int(req_resp, ret);
			if(ret == RET_OK)
			{
				fbus_parcel_write_data(req_resp, &info, sizeof(info));
			}
			break;
		}
		case FBUS_SERVICE_MANAGER_REQ_REGISTER:
		{
			FBusServiceInfo info = {0};
			const char* name = fbus_parcel_get_string(req_resp);
			const char* host = fbus_parcel_get_string(req_resp);
			pid = fbus_parcel_get_int(req_resp);

			ret = fbus_service_service_manager_handle_register(thiz, name, &info, host, pid);

			fbus_parcel_reset(req_resp);
			fbus_parcel_write_int(req_resp, ret);
			if(ret == RET_OK)
			{
				fbus_parcel_write_data(req_resp, &info, sizeof(info));
			}
			break;
		}
		case FBUS_SERVICE_MANAGER_REQ_UNREGISTER:
		{
			const char* name = fbus_parcel_get_string(req_resp);
			const char* host = fbus_parcel_get_string(req_resp);
			pid = fbus_parcel_get_int(req_resp);
			
			ret = fbus_service_service_manager_handle_unregister(thiz, name, host, pid);
			
			fbus_parcel_reset(req_resp);
			fbus_parcel_write_int(req_resp, ret);
			break;
		}
		default:
		{
			ftk_logd("%s: not supported: %d\n", __func__, req_code);
			break;
		}
	}

	fbus_service_service_manager_check_children(thiz);

	return RET_OK;
}

static void fbus_service_service_manager_destroy(FBusService* thiz)
{
	if(thiz != NULL)
	{
		int i = 0;
		int nr = 0;
		DECL_PRIV(thiz, priv);

		priv->quiting = 1;
		nr = fbus_service_infos_get_nr(priv->service_infos);
		for(i = 0; i < nr; i++)
		{
			FBusServiceInfo* info = fbus_service_infos_get(priv->service_infos, i);
			if(info->status == FBUS_SERVICE_STARTED)
			{
				process_stop(info->pid);
			}
		}
		fbus_service_service_manager_check_children(thiz);
		fbus_service_infos_destroy(priv->service_infos);
		FTK_FREE(thiz);
	}

	return;
}

static FBusService* fbus_service_service_manager_create(void)
{
	FBusService* thiz = FTK_ZALLOC(sizeof(FBusService) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		int i = 0;
		int nr = 0;
		DECL_PRIV(thiz, priv);
		FtkSource* timer = NULL;
		char path[FTK_MAX_PATH + 1] = {0};

		thiz->get_name = fbus_service_service_manager_get_name;
		thiz->handle_request = fbus_service_service_manager_handle_request;
		thiz->destroy = fbus_service_service_manager_destroy;

		priv->service_infos = fbus_service_infos_create(16);
		ftk_snprintf(path, sizeof(path)-1, DATA_DIR"/services");
		fbus_service_infos_load_dir(priv->service_infos, path);
	
		fbus_service_register(thiz);
		timer = ftk_source_timer_create(5000, (FtkTimer)fbus_service_service_manager_check_children, thiz);
		ftk_main_loop_add_source(ftk_default_main_loop(), timer);

		nr = fbus_service_infos_get_nr(priv->service_infos);
		for(i = 0; i < nr; i++)
		{
			FBusServiceInfo* info = fbus_service_infos_get(priv->service_infos, i);
			if(info->start_type == FBUS_SERVICE_START_ON_LOAD)
			{
				process_start(info->exec);
				info->status = FBUS_SERVICE_STARTING;
				ftk_logd("%s: launch %s:%s\n", __func__, info->name, info->exec);
			}
		}
	}

	return thiz;
}

int main(int argc, char* argv[])
{
	fbus_service_init(argc, argv);

	if(fbus_service_service_manager_create() != NULL)
	{
		fbus_service_run();
	}
	else
	{
		ftk_loge("Create service manager failed.\n");
	}

	return 0;
}
