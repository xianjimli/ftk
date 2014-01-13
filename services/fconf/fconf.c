/*
 * File: fconf.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   common used functions for FConf interface.
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
#include "fconf.h"

static Ret on_changed(void* ctx, int change_by_self, FConfChangeType type, const char* xpath, const char* value)
{
	ftk_logd("%s: %d %x %s %s\n", __func__, change_by_self, type, xpath, value);

	return RET_OK;
}

Ret fconf_test(FConf* thiz)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int n = 0;
	char* value = NULL;
	char* child = NULL;
	char name[32] = {0};

	fconf_reg_changed_notify(thiz, on_changed, NULL);
	assert(fconf_set(thiz, "/a/b/cc/name1", "1") == RET_OK);
	assert(fconf_set(thiz, "/a/b/cc/name2", "2") == RET_OK);
	assert(fconf_set(thiz, "/a/b/cc/name3", "2") == RET_OK);
	assert(fconf_get(thiz, "/a/b/cc/name3", &value) == RET_OK);
	assert(strcmp(value, "2") == 0);
	assert(fconf_set(thiz, "/a/b/cc/name3", "3") == RET_OK);
	assert(fconf_get(thiz, "/a/b/cc/name1", &value) == RET_OK);
	assert(strcmp(value, "1") == 0);
	assert(fconf_get(thiz, "/a/b/cc/name2", &value) == RET_OK);
	assert(strcmp(value, "2") == 0);
	assert(fconf_get(thiz, "/a/b/cc/name3", &value) == RET_OK);
	assert(strcmp(value, "3") == 0);
	
	assert(fconf_set(thiz, "/a/b/dd/name1", "1") == RET_OK);
	assert(fconf_set(thiz, "/a/b/dd/name2", "2") == RET_OK);
	assert(fconf_set(thiz, "/a/b/dd/name3", "3") == RET_OK);
	assert(fconf_get(thiz, "/a/b/dd/name1", &value) == RET_OK);
	assert(strcmp(value, "1") == 0);
	assert(fconf_get(thiz, "/a/b/dd/name2", &value) == RET_OK);
	assert(strcmp(value, "2") == 0);
	assert(fconf_get(thiz, "/a/b/dd/name3", &value) == RET_OK);
	assert(strcmp(value, "3") == 0);
	
	assert(fconf_set(thiz, "/b/b/dd/name1", "1") == RET_OK);
	assert(fconf_set(thiz, "/b/b/dd/name2", "2") == RET_OK);
	assert(fconf_set(thiz, "/b/b/dd/name3", "3") == RET_OK);

	assert(fconf_get_child_count(thiz, "/a/", &n) == RET_OK && n == 1);
	assert(fconf_get_child_count(thiz, "/a/b", &n) == RET_OK && n == 2);
	assert(fconf_get_child_count(thiz, "/a/b/cc", &n) == RET_OK && n == 3);

	//fconf_reg_changed_notify(thiz, NULL, NULL);
	for(i = 0; i < n; i++)
	{
		snprintf(name, sizeof(name)-1, "name%d", i+1);
		assert(fconf_get_child(thiz, "/a/b/cc", i, &child) == RET_OK);
		assert(strcmp(name, child) == 0);
	}
	assert(fconf_get_child_count(thiz, "/a/b/dd", &n) == RET_OK && n == 3);
	for(i = 0; i < n; i++)
	{
		snprintf(name, sizeof(name)-1, "name%d", i+1);
		assert(fconf_get_child(thiz, "/a/b/dd", i, &child) == RET_OK);
		assert(strcmp(name, child) == 0);
	}
	assert(fconf_remove(thiz, "/a/b/dd/name") != RET_OK);
	assert(fconf_remove(thiz, "/a/b/dd/name1") == RET_OK);
	assert(fconf_get_child_count(thiz, "/a/b/dd", &n) == RET_OK && n == 2);
	assert(fconf_remove(thiz, "/a/b/dd/name2") == RET_OK);
	assert(fconf_get_child_count(thiz, "/a/b/dd", &n) == RET_OK && n == 1);
	assert(fconf_remove(thiz, "/a/b/dd") == RET_OK);
	assert(fconf_get_child_count(thiz, "/a/b/dd", &n) != RET_OK);
	
	assert(fconf_remove(thiz, "/a/b") == RET_OK);
	assert(fconf_get_child_count(thiz, "/a/b", &n) != RET_OK);

	assert(fconf_remove(thiz, "/a") == RET_OK);
	assert(fconf_get_child_count(thiz, "/a", &n) != RET_OK);

	int times = 10;
	for(i = 0; i < times; i++)
	{
		for(j = 0; j < times; j++)
		{
			for(k = 0; k < times; k++)
			{
				snprintf(name, sizeof(name)-1, "/%d/%04d/%08d", i, j, k);
				assert(fconf_set(thiz, name, name) == RET_OK);
			}
			snprintf(name, sizeof(name)-1, "/%d/%04d", i, j);
			assert(fconf_get_child_count(thiz, name, &n) == RET_OK);
			assert(n == times);
			for(k = 0; k < n; k++)
			{
				snprintf(name, sizeof(name)-1, "/%d/%04d", i, j);
				assert(fconf_get_child(thiz, name, k, &child) == RET_OK);
			}
		}
	}
	//fconf_reg_changed_notify(thiz, on_changed, NULL);

	return RET_OK;
}
