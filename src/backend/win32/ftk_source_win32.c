/*
 * File: ftk_source_win32.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   win32 source 
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
 * 2009-10-03 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#include "ftk_source_win32.h"

typedef struct _PrivInfo
{
	void* user_data;
}PrivInfo;

static int ftk_source_win32_get_fd(FtkSource* thiz)
{
	return -1;
}

static int ftk_source_win32_check(FtkSource* thiz)
{
	return 0;
}

static Ret ftk_source_win32_dispatch(FtkSource* thiz)
{
	MSG msg;
	DECL_PRIV(thiz, priv);

	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	if(!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		Sleep(10);
	}

	return RET_OK;
}

static void ftk_source_win32_destroy(FtkSource* thiz)
{
	FTK_ZFREE(thiz, sizeof(FtkSource) + sizeof(PrivInfo));

	return;
}

FtkSource* ftk_source_win32_create(void)
{
	FtkSource* thiz = NULL;

	thiz = (FtkSource*)FTK_ZALLOC(sizeof(FtkSource) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->get_fd   = ftk_source_win32_get_fd;
		thiz->check    = ftk_source_win32_check;
		thiz->dispatch = ftk_source_win32_dispatch;
		thiz->destroy  = ftk_source_win32_destroy;

		thiz->ref = 1;
	}

	return thiz;
}
