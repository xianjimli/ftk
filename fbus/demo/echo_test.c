/*
 * File: echo_test.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   echo tester
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

#include "fbus_echo.h"
#include "ftk_globals.h"
#include "ftk_allocator_default.h"

int main(int argc, char* argv[])
{
	int i = 0;
	char in  = 'a';
	char out = 0;
	short in_short = 0;
	short out_short = 0;
	int in_int = 0;
	int out_int = 0;
	FBusEcho* thiz = NULL;
	size_t out_len = 0;
	const char* out_str = NULL;
	const char* in_str = "========================it is ok?==========================";
#ifndef USE_STD_MALLOC
	ftk_set_allocator((ftk_allocator_default_create()));
#endif
	thiz = fbus_echo_create();

	for(i = 0; i < 1000; i++)
	{
		assert(fbus_echo_char(thiz, in, &out) == RET_OK);
		assert(in == out);
		
		assert(fbus_echo_short(thiz, in_short, &out_short) == RET_OK);
		assert(in_short == out_short);

		assert(fbus_echo_int(thiz, in_int, &out_int) == RET_OK);
		assert(in_int == out_int);

		assert(fbus_echo_string(thiz, in_str, &out_str) == RET_OK);
		assert(strcmp(in_str, out_str) == 0);
	
		assert(fbus_echo_data(thiz, in_str, strlen(in_str), &out_str, &out_len) == RET_OK);
		assert(memcmp(in_str, out_str, strlen(in_str)) == 0);
		assert(out_len == strlen(in_str));
	}

	fbus_echo_destroy(thiz);

	return 0;
}
