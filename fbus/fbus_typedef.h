/*
 * File:    fbus_typedef.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   common used stuff.
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


#ifndef FBUS_TYPEDEF_H
#define FBUS_TYPEDEF_H
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include "ftk_log.h"
#include "ftk_globals.h"
#include "ftk_allocator.h"

FTK_BEGIN_DECLS

typedef enum _FBusRespType
{
	FBUS_RESP_NORMAL,
	FBUS_RESP_PUSH
}FBusRespType;

typedef enum _FBusPushTrigger
{
	FBUS_TRIGGER_BY_SELF = 1,
	FBUS_TRIGGER_BY_OTHER,
	FBUS_TRIGGER_BY_SERVICE,
}FBusPushTrigger;

FTK_END_DECLS

#endif/*FBUS_TYPEDEF_H*/

