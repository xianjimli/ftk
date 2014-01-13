/*
 * File: fbus_parcel.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   a buffer used to pack RPC parameters.
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

#ifndef FBUS_PARCEL_H
#define FBUS_PARCEL_H

#include "fbus_typedef.h"

FTK_BEGIN_DECLS

struct _FBusParcel;
typedef struct _FBusParcel FBusParcel;

FBusParcel* fbus_parcel_create(size_t capacity);

size_t fbus_parcel_capacity(FBusParcel* thiz);
size_t fbus_parcel_size(FBusParcel* thiz);
size_t fbus_parcel_cursor(FBusParcel* thiz);
void*  fbus_parcel_data(FBusParcel* thiz);
Ret    fbus_parcel_extend(FBusParcel* thiz, size_t capacity);
Ret    fbus_parcel_set_size(FBusParcel* thiz, size_t size);
Ret    fbus_parcel_set_cursor(FBusParcel* thiz, size_t cursor);

char  fbus_parcel_get_char(FBusParcel* thiz);
short fbus_parcel_get_short(FBusParcel* thiz);
int   fbus_parcel_get_int(FBusParcel* thiz);
const char* fbus_parcel_get_string(FBusParcel* thiz);
int   fbus_parcel_get_data_size(FBusParcel* thiz);
const void* fbus_parcel_get_data(FBusParcel* thiz, size_t size);

Ret fbus_parcel_write_char(FBusParcel* thiz, char data);
Ret fbus_parcel_write_short(FBusParcel* thiz, short data);
Ret fbus_parcel_write_int(FBusParcel* thiz, int data);
Ret fbus_parcel_write_string(FBusParcel* thiz, const char* data);
Ret fbus_parcel_write_data(FBusParcel* thiz, const void* data, size_t len);
Ret fbus_parcel_reset(FBusParcel* thiz);

void fbus_parcel_destroy(FBusParcel* thiz);

FTK_END_DECLS

#endif/*FBUS_PARCEL_H*/

