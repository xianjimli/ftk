/*
 * File:   lua_runner.c
 * Author: Li XianJing <xianjimli@hotmail.com>
 * Brief:  lua runner
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
 * 2009-11-22 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "lualib.h"
#include "ftk_lua.h"
#include "ftk_mmap.h"

static void l_message (const char *pname, const char *msg) {
  if (pname) fprintf(stderr, "%s: ", pname);
  fprintf(stderr, "%s\n", msg);
  fflush(stderr);
}


static int report (lua_State *L, int status) {
  if (status && !lua_isnil(L, -1)) {
    const char *msg = lua_tostring(L, -1);
    if (msg == NULL) msg = "(error object is not a string)";
    l_message("ftk_run", msg);
    lua_pop(L, 1);
  }
  return status;
}

int FTK_MAIN(int argc, char* argv[])
{
	int ret = 0;
	lua_State *L = NULL;

	if(argc != 2)
	{
		printf("Usage: %s lua\n", argv[0]);

		return 0;
	}

	L = lua_open();
	luaL_openlibs(L);
	ftk_lua_init(L);
	ret = luaL_dofile(L, argv[1]);
	report(L, ret);
	lua_close(L);

	return 0;
}
