/*
 * File: app_info.c 
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   application infomation.
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
 * 2009-11-29 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#include "ftk_log.h"
#include "ftk_util.h"
#include  "ftk_dlfcn.h"
#include "ftk_mmap.h"
#include "app_info.h"
#include "ftk_app_lua.h"

struct _AppInfoManager
{
	size_t nr;
	size_t alloc_nr;
	AppInfo* infos;
};

AppInfoManager* app_info_manager_create(void)
{
	AppInfoManager* thiz = FTK_ZALLOC(sizeof(AppInfoManager));

	return thiz;
}

typedef struct _BuilderInfo
{
	AppInfoManager* app_info_manager;
}BuilderInfo;

static void ftk_app_info_builder_on_start(FtkXmlBuilder* thiz, const char* tag, const char** attrs)
{
	int i = 0;
	AppInfo app_info;
	const char* name = NULL;
	const char* value = NULL;
	BuilderInfo* info = (BuilderInfo*)thiz->priv;
	AppInfoManager* app_info_manager = info->app_info_manager;

	if(strcmp(tag, "application") != 0)
	{
		return;
	}

	memset(&app_info, 0, sizeof(AppInfo));
	for(i = 0; attrs[i] != NULL; i+=2)
	{
		name = attrs[i];
		value = attrs[i+1];
		switch(name[0])
		{
			case 'n':/*name*/
			{
				strncpy(app_info.name, value, sizeof(app_info.name) - 1);
				break;
			}
			case 'e': /*exec*/
			{
				strncpy(app_info.exec, value, sizeof(app_info.exec) - 1);
				break;
			}
			case 'i':
			{
				if(strcmp(name, "init") == 0)
				{
					strncpy(app_info.init, value, sizeof(app_info.init) - 1);
				}
				break;
			}
			default:break;
		}
	}

	if(app_info.init[0] && app_info.exec[0])
	{
		app_info_manager_add(app_info_manager, &app_info);
	}
	else
	{
		ftk_logd("%s: invalid application info.\n", __func__);
	}

	return;
}

static void ftk_app_info_builder_on_end(FtkXmlBuilder* thiz, const char* tag)
{
	return;
}

static void ftk_app_info_builder_on_text(FtkXmlBuilder* thiz, const char* text, size_t length)
{
	return;
}

static void ftk_app_info_builder_on_comment(FtkXmlBuilder* thiz, const char* text, size_t length)
{
	return;
}

static void ftk_app_info_builder_on_pi(FtkXmlBuilder* thiz, const char* tag, const char** attrs)
{
	return;
}

static void ftk_app_info_builder_on_error(FtkXmlBuilder* thiz, int line, int row, const char* message)
{
	return;
}

static void ftk_app_info_builder_destroy(FtkXmlBuilder* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(FtkXmlBuilder) + sizeof(BuilderInfo));
	}

	return;
}

static FtkXmlBuilder* ftk_app_info_builder_create(void)
{
	FtkXmlBuilder* thiz = FTK_ZALLOC(sizeof(FtkXmlBuilder) + sizeof(BuilderInfo));

	if(thiz != NULL)
	{
		thiz->on_start_element = ftk_app_info_builder_on_start;
		thiz->on_end_element   = ftk_app_info_builder_on_end;
		thiz->on_text          = ftk_app_info_builder_on_text;
		thiz->on_comment       = ftk_app_info_builder_on_comment;
		thiz->on_pi_element    = ftk_app_info_builder_on_pi;
		thiz->on_error         = ftk_app_info_builder_on_error;
		thiz->destroy          = ftk_app_info_builder_destroy;
	}

	return thiz;
}

static Ret  app_info_manager_parse(AppInfoManager* thiz, const char* xml, size_t length)
{
	FtkXmlParser* parser = NULL;
	FtkXmlBuilder* builder = NULL;
	return_val_if_fail(xml != NULL, RET_FAIL);

	parser  = ftk_xml_parser_create();
	return_val_if_fail(parser != NULL, RET_FAIL);

	builder = ftk_app_info_builder_create();

	if(builder != NULL)
	{
		BuilderInfo* info = (BuilderInfo*)builder->priv;
		info->app_info_manager = thiz;
		ftk_xml_parser_set_builder(parser, builder);
		ftk_xml_parser_parse(parser, xml, length);
	}
	ftk_xml_builder_destroy(builder);
	ftk_xml_parser_destroy(parser);

	return RET_OK;
}

Ret  app_info_manager_load_file(AppInfoManager* thiz, const char* filename)
{
	FtkMmap* m = NULL;
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL && filename != NULL, RET_FAIL);

	m = ftk_mmap_create(filename, 0, -1);
	return_val_if_fail(m != NULL, RET_FAIL);

	ret = app_info_manager_parse(thiz, ftk_mmap_data(m), ftk_mmap_length(m));
	ftk_mmap_destroy(m);

	return ret;
}

Ret  app_info_manager_load_dir(AppInfoManager* thiz, const char* path)
{
	DIR* dir = NULL;
	char filename[260] = {0};
	struct dirent* iter = NULL;
	return_val_if_fail(thiz != NULL && path != NULL, RET_FAIL);
	dir = opendir(path);
	return_val_if_fail(dir != NULL, RET_FAIL);

	while((iter = readdir(dir)) != NULL)
	{
		if(iter->d_name[0] == '.') continue;
		if(strstr(iter->d_name, ".desktop") == NULL) continue;

		ftk_snprintf(filename, sizeof(filename), "%s/%s", path, iter->d_name);
		ftk_normalize_path(filename);
		app_info_manager_load_file(thiz, filename);
	}
	closedir(dir);

	return RET_OK;
}

int  app_info_manager_get_count(AppInfoManager* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->nr;
}

Ret  app_info_manager_init_app(AppInfoManager* thiz, AppInfo* info)
{
	FtkLoadApp load = NULL;
	char filename[FTK_MAX_PATH+1] = {0};
	return_val_if_fail(thiz != NULL && info != NULL, RET_FAIL);

	if(info->app != NULL)
	{
		return RET_OK;
	}
#ifdef FTK_HAS_LUA
	if(strstr(info->exec, ".lua") != NULL)
	{
		info->app = ftk_app_lua_create(info->name, info->exec);
	}
	else
	{
#endif
		ftk_dl_file_name(info->exec, filename);
		if((info->handle = ftk_dlopen(filename)) == NULL)
		{
		}
		return_val_if_fail(info->handle != NULL, RET_FAIL);
		
		load = (FtkLoadApp)ftk_dlsym(info->handle, info->init);
		if(load == NULL)
		{

		}
		return_val_if_fail(load != NULL, RET_FAIL);

		info->app = load();
		return_val_if_fail(info->app != NULL, RET_FAIL);
#ifdef FTK_HAS_LUA
	}
#endif
	return RET_OK;
}

Ret  app_info_manager_get(AppInfoManager* thiz, size_t index, AppInfo** info)
{
	return_val_if_fail(thiz != NULL && index < thiz->nr && info != NULL, RET_FAIL);

	*info = thiz->infos+index;

	return app_info_manager_init_app(thiz, *info);
}

static Ret  app_info_manager_extend(AppInfoManager* thiz, size_t delta)
{
	size_t alloc_nr = 0;
	AppInfo* infos = NULL;
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if((thiz->nr + delta) < thiz->alloc_nr) return RET_OK;

	alloc_nr = thiz->alloc_nr + delta + 10;
	infos = FTK_REALLOC(thiz->infos, sizeof(AppInfo) * alloc_nr);
	if(infos != NULL)
	{
		thiz->infos = infos;
		thiz->alloc_nr = alloc_nr;
	}

	return (thiz->nr + delta) < thiz->alloc_nr ? RET_OK : RET_FAIL;
}

Ret  app_info_manager_add(AppInfoManager* thiz, AppInfo* info)
{
	return_val_if_fail(thiz != NULL && info != NULL, RET_FAIL);
	return_val_if_fail(app_info_manager_extend(thiz, 1) == RET_OK, RET_FAIL);

	memcpy(thiz->infos+thiz->nr, info, sizeof(AppInfo));
	thiz->nr++;

	return RET_OK;
}

Ret  app_info_manager_remove(AppInfoManager* thiz, size_t index)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);
	return_val_if_fail(index < thiz->nr, RET_FAIL);

	if((index + 1) < thiz->nr)
	{
		for(; (index + 1) < thiz->nr; index++)
		{
			thiz->infos[index] = thiz->infos[index+1];
		}
	}
	thiz->nr--;

	return RET_OK;
}

void app_info_manager_destroy(AppInfoManager* thiz)
{
	if(thiz != NULL)
	{
		size_t i = 0;
		for(i = 0; i < thiz->nr; i++)
		{
			ftk_dlclose(thiz->infos[i].handle);
			thiz->infos[i].handle = NULL;
			ftk_app_destroy(thiz->infos[i].app);
		}
		FTK_FREE(thiz->infos);
		FTK_FREE(thiz);
	}

	return;
}

#ifdef APP_INFO_TEST
const char* xml = "<application name=\"Contacts\" icon=\"icons/contacts.png\" shortcut=\"0\" exec=\"libapp-contacts.so\" main=\"ftk_main\" tr_path=\"ftk_translate_path\" tr_text=\"ftk_translate_text\" /><application name=\"Calculator\" icon=\"icons/calc.png\" shortcut=\"1\" exec=\"libapp-calc.so\" main=\"ftk_main\" tr_path=\"ftk_translate_path\" tr_text=\"ftk_translate_text\" />";

int main(int argc, char* argv[])
{
	size_t i = 0;
	AppInfo* info = NULL;
	AppInfoManager* thiz = app_info_manager_create();
	assert(app_info_manager_parse(thiz, xml, strlen(xml)) == RET_OK);
	assert(app_info_manager_get_count(thiz) == 2);
	app_info_manager_load_dir(thiz, DATA_DIR"/apps");
	assert(app_info_manager_get(thiz, 0, &info) == RET_OK);
	assert(strcmp(info->name, "Contacts") == 0);	

	for(i = 0; i < app_info_manager_get_count(thiz); i++)
	{
		assert(app_info_manager_get(thiz, i, &info) == RET_OK);
		assert(info != NULL);
		ftk_logd("%s\n%s\n%s\n\n", 
			info->name, info->exec, info->init);
	}
	app_info_manager_destroy(thiz);

	return 0;
}
#endif/*APP_INFO_TEST*/
