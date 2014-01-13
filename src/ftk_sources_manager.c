/*
 * File: ftk_source_manager.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   sources manager
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
 * 2009-10-15 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_sources_manager.h"

struct _FtkSourcesManager
{
	int source_nr;
	int need_refresh;
	int max_source_nr;
	FtkSource* sources[1];
};

#define FTK_DEFAULT_SOURCE_NR 32
FtkSourcesManager* ftk_sources_manager_create(int max_source_nr)
{
	FtkSourcesManager* thiz = NULL;
	max_source_nr = max_source_nr < FTK_DEFAULT_SOURCE_NR ? FTK_DEFAULT_SOURCE_NR : max_source_nr;

	thiz = (FtkSourcesManager*)FTK_ZALLOC(sizeof(FtkSourcesManager) + sizeof(FtkSource*)*(max_source_nr + 1));

	if(thiz != NULL)
	{
		thiz->max_source_nr = max_source_nr;
	}

	return thiz;
}

Ret  ftk_sources_manager_add(FtkSourcesManager* thiz, FtkSource* source)
{
	return_val_if_fail(thiz != NULL && source != NULL, RET_FAIL);
	return_val_if_fail(thiz->source_nr < thiz->max_source_nr, RET_FAIL);

	source->active = 1;
	thiz->sources[thiz->source_nr++] = source;

	ftk_sources_manager_set_need_refresh(thiz);

	return RET_OK;
}

Ret  ftk_sources_manager_remove(FtkSourcesManager* thiz, FtkSource* source)
{
	int i = 0;
	return_val_if_fail(thiz != NULL && source != NULL, RET_FAIL);

	for(i = 0; i < thiz->source_nr; i++)
	{
		if(thiz->sources[i] == source)
		{
			break;
		}
	}
	
	if(i < thiz->source_nr)
	{
		for(; (i + 1) < thiz->source_nr; i++)
		{
			thiz->sources[i] = thiz->sources[i+1];
		}
		thiz->source_nr--;
		thiz->sources[thiz->source_nr] = NULL;
		source->active = 0;
		ftk_source_unref(source);
	}
	ftk_sources_manager_set_need_refresh(thiz);

	return RET_OK;
}

int  ftk_sources_manager_get_count(FtkSourcesManager* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->source_nr;
}

FtkSource* ftk_sources_manager_get(FtkSourcesManager* thiz, int i)
{
	return_val_if_fail(thiz != NULL && i < thiz->source_nr, NULL);

	return thiz->sources[i];
}

int ftk_sources_manager_need_refresh(FtkSourcesManager* thiz)
{
	int need_refresh = 0;
	return_val_if_fail(thiz != NULL, 0);

	if(thiz->need_refresh > 0)
	{
		need_refresh = thiz->need_refresh;
		thiz->need_refresh--;
	}

	return need_refresh;
}

Ret  ftk_sources_manager_set_need_refresh(FtkSourcesManager* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	thiz->need_refresh++;

	return RET_OK;
}

void ftk_sources_manager_destroy(FtkSourcesManager* thiz)
{
	int i = 0;

	if(thiz != NULL)
	{
		for(i = 0; i < thiz->source_nr; i++)
		{
			ftk_source_unref(thiz->sources[i]);
			thiz->sources[i] = NULL;
		}

		FTK_ZFREE(thiz, sizeof(FtkSourcesManager) + sizeof(FtkSource*)*(thiz->max_source_nr + 1));
	}

	return;
}


