/*
 * File: ftk_path.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   path.
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
 * 2010-08-01 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#include "ftk_log.h"
#include "ftk_path.h"
#include "ftk_allocator.h"

#define FTK_PATH_LEVELS 16

struct _FtkPath
{
	size_t nr;
	size_t cur;
	char* path;
	char  full[FTK_MAX_PATH+1];
	char* subs[FTK_PATH_LEVELS];
};

FtkPath* ftk_path_create(const char* path)
{
	FtkPath* thiz = FTK_NEW(FtkPath);

	if(thiz != NULL)
	{
		ftk_path_set_path(thiz, path);
	}

	return thiz;
}

size_t ftk_path_get_levels(FtkPath* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->nr;
}

typedef void (*OnTokenFunc)(void* ctx, int index, const char* token);

#define IS_DELIM(c) (strchr(delims, c) != NULL)
static int parse_token(const char* text, const char* delims, OnTokenFunc on_token, void* ctx, char** ret_copy_text)
{
	enum _State
	{
		STAT_INIT,
		STAT_IN,
		STAT_OUT,
	}state = STAT_INIT;

	int   count		= 0;
	char* copy_text = ftk_strdup(text);
	char* p			= copy_text;
	char* token		= copy_text;

	for(p = copy_text; *p != '\0'; p++)
	{
		switch(state)
		{
			case STAT_INIT:
			case STAT_OUT:
			{
				if(!IS_DELIM(*p))
				{
					token = p;
					state = STAT_IN;
				}
				break;
			}
			case STAT_IN:
			{
				if(IS_DELIM(*p))
				{
					*p = '\0';
					on_token(ctx, count++, token);
					state = STAT_OUT;
				}
				break;
			}
			default:break;
		}
	}

	if(state == STAT_IN)
	{
		on_token(ctx, count++, token);
	}

	on_token(ctx, -1, NULL);

	if(ret_copy_text != NULL)
	{
		*ret_copy_text = copy_text;
	}
	else
	{
		FTK_FREE(copy_text);
	}

	return count;
}

static void on_sub_path(void* ctx, int index, const char* token)
{
	FtkPath* thiz = (FtkPath*)ctx;

	if(index >= 0)
	{
		if(strcmp(token, ".") == 0)
		{
			return;
		}
		else if(strcmp(token, "..") == 0)
		{
			if(thiz->nr > 0)
			{
				thiz->nr--;
			}
		}
		else
		{
			thiz->subs[thiz->nr++] = (char*)token;
			strcat(thiz->full, "/");
			strcat(thiz->full, token);
		}
	}

	return;
}

Ret ftk_path_set_path(FtkPath* thiz, const char* path)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(path != NULL)
	{
		thiz->nr = 0;
		thiz->cur = 0;
		thiz->full[0] = '\0';
		FTK_FREE(thiz->path);
		parse_token(path, "/\\", on_sub_path, thiz, &thiz->path);
		thiz->subs[thiz->nr] = NULL;
	}

	return RET_OK;
}

const char* ftk_path_full(FtkPath* thiz)
{
	return_val_if_fail(thiz != NULL, NULL);

	return thiz->full;
}

Ret ftk_path_up(FtkPath* thiz)
{
	Ret ret = RET_OK;
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(thiz->cur > 0)
	{
		thiz->cur--;
	}
	else
	{
		ret = RET_FAIL;
	}

	return ret;
}

Ret ftk_path_down(FtkPath* thiz)
{
	Ret ret = RET_OK;
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if((thiz->cur + 1) < thiz->nr)
	{
		thiz->cur++;
	}
	else
	{
		ret = RET_FAIL;
	}

	return ret;
}

Ret ftk_path_root(FtkPath* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	thiz->cur = 0;

	return RET_OK;
}

int ftk_path_is_leaf(FtkPath* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return (thiz->cur + 1) == thiz->nr;
}

int ftk_path_current_level(FtkPath* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->cur;
}

const char* ftk_path_current(FtkPath* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->nr > 0, NULL);

	return thiz->subs[thiz->cur];
}

const char* ftk_path_get_sub(FtkPath* thiz, size_t level)
{
	return_val_if_fail(thiz != NULL && level < thiz->nr, NULL);

	return thiz->subs[level];
}

void ftk_path_destroy(FtkPath* thiz)
{
	if(thiz != NULL)
	{
		FTK_FREE(thiz->path);
		FTK_FREE(thiz);
	}

	return;
}

#ifdef FTK_PATH_TEST
#include "assert.h"
#include "ftk_globals.h"
#include "ftk_allocator_default.h"

static void test1(FtkPath* thiz, const char** expected)
{
	size_t i = 0;
	assert(ftk_path_get_levels(thiz) == 6);
	for(i = 0; i < ftk_path_get_levels(thiz); i++)
	{
		assert(strcmp(ftk_path_get_sub(thiz, i), expected[i]) == 0);
	}

	assert(ftk_path_root(thiz) == RET_OK);
	for(i = 0; i < ftk_path_get_levels(thiz); i++)
	{
		assert(strcmp(ftk_path_current(thiz), expected[i]) == 0);

		if((i + 1) < ftk_path_get_levels(thiz))
		{
			assert(ftk_path_down(thiz) == RET_OK);
		}
	}
	assert(ftk_path_is_leaf(thiz) == 1);
	assert(ftk_path_current_level(thiz) == (ftk_path_get_levels(thiz) - 1));
	i--;
	for(; i > 0; i --)
	{
		assert(strcmp(ftk_path_current(thiz), expected[i]) == 0);
		if(i > 0)
		{
			assert(ftk_path_up(thiz) == RET_OK);
		}
	}

	return;
}

int main(int argc, char* argv[])
{
	FtkPath* thiz = NULL;
	const char* expected[] = {"1234", "abcd", "gg", "mm", "g", "m", NULL};
#ifndef USE_STD_MALLOC
	ftk_set_allocator((ftk_allocator_default_create()));
#endif
	
	thiz = ftk_path_create("/1234/abcd///gg/mm/g/m");
	test1(thiz, expected);
	ftk_path_destroy(thiz);
	
	thiz = ftk_path_create("/1234/abcd///gg/mm/g/m/");
	test1(thiz, expected);
	ftk_path_destroy(thiz);
	
	thiz = ftk_path_create("/1234/abcd/././gg/mm/g/m/");
	test1(thiz, expected);
	ftk_path_destroy(thiz);
	
	thiz = ftk_path_create("/1234/abcd/aa/../gg/mm/g/m/");
	test1(thiz, expected);
	ftk_path_destroy(thiz);

	return 0;
}
#endif/*FTK_PATH_TEST*/

