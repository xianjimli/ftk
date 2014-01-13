/*
 * File:   ftk_app_lua.c
 * Author: Li XianJing <xianjimli@hotmail.com>
 * Brief:  wrap lua script to ftk_app interface.
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
 * 2011-04-04 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_app.h"
#include "lualib.h"
#include "ftk_lua.h"

typedef struct _PrivInfo
{
	char* name;
	char* exec;
	FtkBitmap* icon;
}PrivInfo;

static FtkBitmap* ftk_app_lua_get_icon(FtkApp* thiz)
{
	DECL_PRIV(thiz, priv);
	char file_name[FTK_MAX_PATH + 1] = {0};
	return_val_if_fail(priv != NULL, NULL);
	
	if(priv->icon != NULL) return priv->icon;
	
	ftk_snprintf(file_name, FTK_MAX_PATH, "%s/%s/icons/icon.png", FTK_DATA_ROOT, priv->name);
	priv->icon = ftk_bitmap_factory_load(ftk_default_bitmap_factory(), file_name);
	if(priv->icon != NULL) return priv->icon;

	return priv->icon;
}

static const char* ftk_app_lua_get_name(FtkApp* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, NULL);

	return priv->name;
}

static int g_lua_ref = 0;
static lua_State* g_lua = NULL;
static lua_State* lua_get(void)
{
	if(g_lua == NULL)
	{
		g_lua = lua_open();
		luaL_openlibs(g_lua);
		ftk_lua_init(g_lua);
	}
	g_lua_ref++;

	return g_lua;
}

static void lua_put(void)
{
	g_lua_ref--;

	if(g_lua_ref == 0)
	{
		lua_close(g_lua);
	}

	return;
}

static Ret ftk_app_lua_run(FtkApp* thiz, int argc, char* argv[])
{
	int ret = 0;
	lua_State *L = g_lua;
	DECL_PRIV(thiz, priv);
	ret = luaL_dofile(L, priv->exec);
	//report(L, ret);

	return RET_OK;
}

static void ftk_app_lua_destroy(FtkApp* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		ftk_bitmap_unref(priv->icon);
		FTK_FREE(priv->name);
		FTK_FREE(priv->exec);
		FTK_FREE(thiz);
		lua_put();
	}
	
	return;
}

FtkApp* ftk_app_lua_create(const char*name, const char* exec)
{
	FtkApp* thiz = FTK_ZALLOC(sizeof(FtkApp) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		char file_name[FTK_MAX_PATH + 1] = {0};

		priv->name = FTK_STRDUP(name);
		ftk_snprintf(file_name, FTK_MAX_PATH, "%s/%s/lua/main.lua", FTK_DATA_ROOT, priv->name);

		priv->exec = FTK_STRDUP(file_name);
		thiz->get_icon = ftk_app_lua_get_icon;
		thiz->get_name = ftk_app_lua_get_name;
		thiz->run = ftk_app_lua_run;
		thiz->destroy = ftk_app_lua_destroy;
		lua_get();
	}

	return thiz;
}
