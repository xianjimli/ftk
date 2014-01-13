/*
 * File:    fbus_service_infos.c
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

#include "ftk_mmap.h"
#include "ftk_allocator.h"
#include "ftk_xml_parser.h"
#include "fbus_service_infos.h"

#define FBUS_SERVICE_PORT_START FBUS_SERVICE_MANAGER_PORT + 1

struct _FBusServiceInfos
{
	int port;
	size_t size;
	size_t capacity;
	FBusServiceInfo* service_infos;
};

FBusServiceInfos* fbus_service_infos_create(int capacity)
{
	FBusServiceInfos* thiz = FTK_ZALLOC(sizeof(FBusServiceInfos));
	
	if(thiz != NULL)
	{
		capacity = capacity < 5 ? 5 : capacity;
		thiz->service_infos = FTK_ZALLOC(sizeof(FBusServiceInfo) * capacity);
		if(thiz->service_infos != NULL)
		{
			thiz->port = FBUS_SERVICE_PORT_START;
			thiz->capacity = capacity;
		}
		else
		{
			FTK_FREE(thiz);
		}
	}

	return thiz;
}

FBusServiceInfo*  fbus_service_infos_find(FBusServiceInfos* thiz, const char* name)
{
	size_t i = 0;
	return_val_if_fail(thiz != NULL && name != NULL, NULL);

	for(i = 0; i < thiz->size; i++)
	{
		if(strcmp(name, thiz->service_infos[i].name) == 0)
		{
			return thiz->service_infos+i;
		}
	}

	return NULL;
}

Ret fbus_service_infos_get_nr(FBusServiceInfos* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->size;
}

static Ret fbus_service_infos_extend(FBusServiceInfos* thiz, size_t capacity)
{
	return_val_if_fail(thiz != NULL && capacity > 0, RET_FAIL);

	if(capacity > thiz->capacity)
	{
		capacity = capacity + (capacity >> 1);
		FBusServiceInfo* service_infos = FTK_REALLOC(thiz->service_infos, capacity*sizeof(FBusServiceInfo));
		if(service_infos != NULL)
		{
			thiz->capacity = capacity;
			thiz->service_infos = service_infos;
			
			return RET_OK;
		}
		else
		{
			return RET_FAIL;
		}
	}

	return RET_OK;
}

Ret fbus_service_infos_add(FBusServiceInfos* thiz, FBusServiceInfo* service)
{
	return_val_if_fail(thiz != NULL && service != NULL, RET_FAIL);
	return_val_if_fail(fbus_service_infos_extend(thiz, thiz->size+1) == RET_OK, RET_FAIL);

	if(service->host[0] == '\0')
	{
		ftk_strncpy(service->host, FBUS_LOCALHOST, sizeof(service->host)-1);
	}
	service->port = thiz->port + thiz->size;
	memcpy(thiz->service_infos+thiz->size, service, sizeof(FBusServiceInfo));
	thiz->size++;

	ftk_logd("add: port=%d name=%s exec=%s\n", service->port, service->name, service->exec);

	return RET_OK;
}

FBusServiceInfo* fbus_service_infos_get(FBusServiceInfos* thiz, size_t index)
{
	return_val_if_fail(thiz != NULL && index < thiz->size, NULL);

	return thiz->service_infos+index;
}

void fbus_service_infos_destroy(FBusServiceInfos* thiz)
{
	if(thiz != NULL)
	{
		FTK_FREE(thiz->service_infos);
		FTK_FREE(thiz);
	}
}

static int fbus_service_info_is_valid(FBusServiceInfo* service)
{
	return_val_if_fail(service != NULL, 0);

	return (service->name[0] != '\0' && service->exec[0] != '\0');
}

typedef struct _BuilderInfo
{
	FBusServiceInfos* services;
}BuilderInfo;

static void fbus_service_info_builder_on_start(FtkXmlBuilder* thiz, const char* tag, const char** attrs)
{
	int i = 0;
	FBusServiceInfo service = {0};
	BuilderInfo* info = (BuilderInfo*)thiz->priv;
	return_if_fail(thiz != NULL && tag != NULL && attrs != NULL);
	
	memset(&service, 0x00, sizeof(service));
	for(i = 0; attrs[i] != NULL; i+=2)
	{
		const char* name = attrs[i];
		const char* value = attrs[i+1];
		if(strcmp(name, "name") == 0)
		{
			ftk_strncpy(service.name, value, sizeof(service.name)-1);
		}
		else if(strcmp(name, "exec") == 0)
		{
			ftk_strncpy(service.exec, value, sizeof(service.exec)-1);
		}
		else if(strcmp(name, "start_type") == 0)
		{
			if(strcmp(value, "on_load") == 0)
			{
				service.start_type = FBUS_SERVICE_START_ON_LOAD;
			}
			else if(strcmp(value, "on_request") == 0)
			{
				service.start_type = FBUS_SERVICE_START_ON_REQUEST;
			}
			else
			{
				service.start_type = FBUS_SERVICE_START_NONE;
			}
		}
		else if(strcmp(name, "life_cycle") == 0)
		{
			if(strcmp(value, "once") == 0)
			{
				service.life_cycle = FBUS_SERVICE_RUN_ONCE;
			}
			else
			{
				service.life_cycle = FBUS_SERVICE_RUN_FOREVER;
			}
		}
	}

	return_if_fail(fbus_service_info_is_valid(&service));

	fbus_service_infos_add(info->services, &service);

	return;
}

static void fbus_service_info_builder_on_end(FtkXmlBuilder* thiz, const char* tag)
{
	return;
}

static void fbus_service_info_builder_on_text(FtkXmlBuilder* thiz, const char* text, size_t length)
{
	return;
}

static void fbus_service_info_builder_destroy(FtkXmlBuilder* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(FtkXmlBuilder) + sizeof(BuilderInfo));
	}

	return;
}

static FtkXmlBuilder* fbus_service_info_builder_create(FBusServiceInfos* services)
{
	FtkXmlBuilder* thiz = FTK_ZALLOC(sizeof(FtkXmlBuilder) + sizeof(BuilderInfo));

	if(thiz != NULL)
	{
		BuilderInfo* priv      = (BuilderInfo* )thiz->priv;
		priv->services		   = services;
		thiz->on_start_element = fbus_service_info_builder_on_start;
		thiz->on_end_element   = fbus_service_info_builder_on_end;
		thiz->on_text		   = fbus_service_info_builder_on_text;
		thiz->destroy		   = fbus_service_info_builder_destroy;
	}

	return thiz;
}

static Ret  fbus_service_info_parse(FBusServiceInfos* services, const char* xml, size_t length)
{
    FtkXmlParser* parser = NULL;
    FtkXmlBuilder* builder = NULL;
    return_val_if_fail(xml != NULL, RET_FAIL);

    parser  = ftk_xml_parser_create();
    return_val_if_fail(parser != NULL, RET_FAIL);

    builder = fbus_service_info_builder_create(services);
    if(builder != NULL)
    {
        ftk_xml_parser_set_builder(parser, builder);
        ftk_xml_parser_parse(parser, xml, length);
    }
    ftk_xml_builder_destroy(builder);
    ftk_xml_parser_destroy(parser);

    return RET_OK;
}

Ret  fbus_service_infos_load_file(FBusServiceInfos* services, const char* filename)
{
    FtkMmap* m = NULL;
    Ret ret = RET_FAIL;
    return_val_if_fail(services != NULL && filename != NULL, RET_FAIL);

    m = ftk_mmap_create(filename, 0, -1);
    return_val_if_fail(m != NULL, RET_FAIL);

    ret = fbus_service_info_parse(services, ftk_mmap_data(m), ftk_mmap_length(m));
    ftk_mmap_destroy(m);

    return ret;
}

Ret  fbus_service_infos_load_dir(FBusServiceInfos* services, const char* path)
{
    DIR* dir = NULL;
    struct dirent* iter = NULL;
    char filename[FTK_MAX_PATH+1] = {0};
    return_val_if_fail(services != NULL && path != NULL, RET_FAIL);
    
	dir = opendir(path);
    return_val_if_fail(dir != NULL, RET_FAIL);

    while((iter = readdir(dir)) != NULL)
    {
        if(iter->d_name[0] == '.') continue;
        if(strstr(iter->d_name, ".service") == NULL) continue;

        ftk_snprintf(filename, sizeof(filename)-1, "%s/%s", path, iter->d_name);
        fbus_service_infos_load_file(services, filename);
    }
    closedir(dir);

    return RET_OK;
}


#ifdef FBUS_SERVICE_INFOS_TEST
#include <assert.h>
#include "ftk_allocator_default.h"

int main(int argc, char* argv[])
{
	int i = 0;
	FBusServiceInfo service = {0};
	FBusServiceInfos* thiz = NULL;
#ifndef USE_STD_MALLOC
	ftk_set_allocator((ftk_allocator_default_create()));
#endif
	thiz = fbus_service_infos_create(0);
	assert(fbus_service_infos_get_nr(thiz) == 0);

	for(i = 0; i < 32; i++)
	{
		ftk_snprintf(service.name, sizeof(service.name)-1, "name%4d", i);
		assert(fbus_service_infos_add(thiz, &service) == RET_OK);
		assert(strcmp(fbus_service_infos_find(thiz, service.name)->name, service.name) == 0);
		assert(strcmp(fbus_service_infos_get(thiz, i)->name, service.name) == 0);
		assert(fbus_service_infos_get_nr(thiz) == (i+1));
	}
	fbus_service_infos_destroy(thiz);

	return 0;
}
#endif
