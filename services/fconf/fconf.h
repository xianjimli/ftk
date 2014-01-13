/*
 * File: fconf.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   config interface
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

#ifndef FCONF_H
#define FCONF_H

#include "fbus_typedef.h"

FTK_BEGIN_DECLS

struct _FConf;
typedef struct _FConf FConf;

typedef enum _FConfChangeType
{
	FCONF_CHANGED_BY_SET = 0x74657300,
	FCONF_CHANGED_BY_ADD,
	FCONF_CHANGED_BY_REMOVE
}FConfChangeType;

typedef Ret(*FConfOnChanged)(void* ctx, int changed_by_self, FConfChangeType type, const char* xpath, const char* value);

/**
 * xpath: root path is the filename(remove the dir and ext name), the sub path are the nodes name in xml.
 * for example:
 * globals.xml contains:
 * <service_manager><port value=\"1978\" /></service_manager>
 * The value of xpath /globals/service_manager/port are 1978.
 *
 */
typedef Ret  (*FConfLock)(FConf* thiz);
typedef Ret  (*FConfUnlock)(FConf* thiz);
typedef Ret  (*FConfRemove)(FConf* thiz, const char* xpath);
typedef Ret  (*FConfSet)(FConf* thiz, const char* xpath, const char* value);
typedef Ret  (*FConfGet)(FConf* thiz, const char* xpath, char** value);
typedef Ret  (*FConfGetChildCount)(FConf* thiz, const char* xpath, int* count);
typedef Ret  (*FConfGetChild)(FConf* thiz, const char* xpath, int index, char** child);
typedef Ret  (*FConfRegChangedNotify)(FConf* thiz, FConfOnChanged on_changed, void* ctx);
typedef void (*FConfDestroy)(FConf* thiz);

struct _FConf
{
	FConfLock lock;
	FConfUnlock unlock;
	FConfSet set;
	FConfGet get;
	FConfRemove remove;
	FConfGetChild get_child;
	FConfGetChildCount get_child_count;
	FConfRegChangedNotify reg_changed_notify;
	FConfDestroy destroy;

	char priv[1];
};

static inline Ret fconf_lock(FConf* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->lock != NULL, RET_FAIL);

	return thiz->lock(thiz);
}

static inline Ret fconf_unlock(FConf* thiz)
{
	return_val_if_fail(thiz != NULL && thiz->unlock != NULL, RET_FAIL);

	return thiz->unlock(thiz);
}

static inline Ret fconf_remove(FConf* thiz, const char* xpath)
{
	return_val_if_fail(thiz != NULL && thiz->remove != NULL, RET_FAIL);

	return thiz->remove(thiz, xpath);
}

static inline Ret fconf_set(FConf* thiz, const char* xpath, const char* value)
{
	return_val_if_fail(thiz != NULL && thiz->set != NULL, RET_FAIL);

	return thiz->set(thiz, xpath, value);
}

static inline Ret fconf_get(FConf* thiz, const char* xpath, char** value)
{
	return_val_if_fail(thiz != NULL && thiz->get != NULL, RET_FAIL);

	return thiz->get(thiz, xpath, value);
}

static inline Ret fconf_get_child_count(FConf* thiz, const char* xpath, int* count)
{
	return_val_if_fail(thiz != NULL && thiz->get_child_count != NULL, RET_FAIL);

	return thiz->get_child_count(thiz, xpath, count);
}

static inline Ret fconf_get_child(FConf* thiz, const char* xpath, int index, char** child)
{
	return_val_if_fail(thiz != NULL && thiz->get_child != NULL, RET_FAIL);

	return thiz->get_child(thiz, xpath, index, child);
}

static inline Ret fconf_reg_changed_notify(FConf* thiz, FConfOnChanged on_changed, void* ctx)
{
	return_val_if_fail(thiz != NULL && thiz->reg_changed_notify != NULL, RET_FAIL);

	return thiz->reg_changed_notify(thiz, on_changed, ctx);
}

static inline void fconf_destroy(FConf* thiz)
{
	if(thiz != NULL && thiz->destroy != NULL)
	{
		thiz->destroy(thiz);
	}

	return;
}

FTK_END_DECLS

#endif/*FCONF_H*/
