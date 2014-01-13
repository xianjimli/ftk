/*
 * File: ftk_translator.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   translate English to local text.
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
 * 2010-01-31 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_allocator.h"
#include "ftk_translator.h"

struct _FtkTranslator
{
	const char* package;
	const char* locale;
};

FtkTranslator* ftk_translator_create(const char* package, const char* locale)
{
	FtkTranslator* thiz = NULL;
	
	return_val_if_fail(package != NULL, NULL);

	thiz = FTK_ZALLOC(sizeof(FtkTranslator));
	if(thiz != NULL)
	{
		thiz->package = package;
		thiz->locale = NULL;
	}

	return thiz;
}

const char* ftk_translator_get_text(FtkTranslator* thiz, const char* text)
{
	return_val_if_fail(thiz != NULL && text != NULL, NULL);

	return text;
}

void ftk_translator_destroy(FtkTranslator* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(FtkTranslator));
	}

	return;
}


