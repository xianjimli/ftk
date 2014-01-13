/*
 * File: ftk_clipboard.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   clip board
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
 * 2010-01-28 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_clipboard.h"

static char g_clipboard[FTK_CLIPBOARD_SIZE+1];

int ftk_clipboard_has_data(void)
{
	return g_clipboard[0] != '\0';
}

Ret ftk_clipboard_set_text(const char* text)
{
	return_val_if_fail(text != NULL, RET_FAIL);

	ftk_strncpy(g_clipboard, text, FTK_CLIPBOARD_SIZE);

	return RET_OK;
}

Ret ftk_clipboard_get_text(const char** text)
{
	return_val_if_fail(text != NULL, RET_FAIL);

	*text = g_clipboard;

	return RET_OK;
}



