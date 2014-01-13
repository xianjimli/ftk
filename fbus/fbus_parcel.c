/*
 * File: fbus_parcel.c
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

#include "fbus_parcel.h"

enum _FBusParcelDataType
{
	FTK_TYPE_NONE,
	FTK_TYPE_INT    = 0x49494949, /*IIII*/
	FTK_TYPE_CHAR   = 0x43434343, /*CCCC*/
	FTK_TYPE_SHORT  = 0x53535353, /*SSSS*/
	FTK_TYPE_STRING = 0x53545249, /*STRI*/
	FTK_TYPE_BINARY = 0x42494e41, /*BINA*/
	FTK_TYPE_NR
};

struct _FBusParcel
{
	char*  data;
	size_t size;
	size_t cursor;
	size_t capacity;
};

FBusParcel* fbus_parcel_create(size_t capacity)
{
	FBusParcel* thiz = FTK_ZALLOC(sizeof(FBusParcel));

	if(thiz != NULL)
	{
		if(fbus_parcel_extend(thiz, capacity) != RET_OK)
		{
			FTK_FREE(thiz);
		}
		else
		{
			memset(thiz->data, 0x00, thiz->capacity);
		}
	}

	return thiz;
}

size_t fbus_parcel_capacity(FBusParcel* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->capacity;
}

size_t fbus_parcel_size(FBusParcel* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->size;
}

size_t fbus_parcel_cursor(FBusParcel* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->cursor;
}

void*  fbus_parcel_data(FBusParcel* thiz)
{
	return_val_if_fail(thiz != NULL, NULL);

	return thiz->data;
}

Ret    fbus_parcel_extend(FBusParcel* thiz, size_t capacity)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(capacity > thiz->capacity)
	{
		char* data = NULL;
		
		capacity = capacity + (capacity >> 1);
		data = FTK_REALLOC(thiz->data, capacity);
		if(data != NULL)
		{
			thiz->data = data;
			thiz->capacity = capacity;
		}
	}

	return RET_OK;
}

Ret    fbus_parcel_set_size(FBusParcel* thiz, size_t size)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(size <= thiz->capacity)
	{
		thiz->size = size;
	}

	return thiz->size == size ? RET_OK : RET_FAIL;
}

Ret    fbus_parcel_set_cursor(FBusParcel* thiz, size_t cursor)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(cursor <= thiz->size)
	{
		thiz->cursor = cursor;
	}

	return thiz->cursor == cursor ? RET_OK : RET_FAIL;
}

static int fbus_parcel_get_raw_int(FBusParcel* thiz, int peek)
{
	int data = 0;

	if((thiz->cursor + sizeof(int)) <= thiz->size)
	{
		memcpy(&data, thiz->data+thiz->cursor, sizeof(int));
		if(!peek)
		{
			thiz->cursor += sizeof(int);
		}
	}
	else
	{
		assert(!"no enough data.");
	}

	return data;
}

static Ret fbus_parcel_check(FBusParcel* thiz, int want_type)
{
	int type = 0;
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL && thiz->data != NULL, RET_FAIL);
	return_val_if_fail((thiz->cursor + 2 * sizeof(int)) <= thiz->size, RET_FAIL);

	memcpy(&type, thiz->data+thiz->cursor, sizeof(int));
	if(type == want_type)
	{
		ret = RET_OK;
		thiz->cursor += sizeof(int);
	}

	return ret;
}

char  fbus_parcel_get_char(FBusParcel* thiz)
{
	return_val_if_fail(fbus_parcel_check(thiz, FTK_TYPE_CHAR) == RET_OK, '\0');

	return (char)fbus_parcel_get_raw_int(thiz, 0);
}

short fbus_parcel_get_short(FBusParcel* thiz)
{
	return_val_if_fail(fbus_parcel_check(thiz, FTK_TYPE_SHORT) == RET_OK, 0);

	return (short)fbus_parcel_get_raw_int(thiz, 0);
}

int   fbus_parcel_get_int(FBusParcel* thiz)
{
	return_val_if_fail(fbus_parcel_check(thiz, FTK_TYPE_INT) == RET_OK, 0);

	return fbus_parcel_get_raw_int(thiz, 0);
}

#define ROUND4(len) ((len + 3) & 0xfffffffc)

static const char* fbus_parcel_get_raw_string(FBusParcel* thiz)
{
	const char* str = thiz->data+thiz->cursor;
	int len = strlen(str) + 1;
	
	thiz->cursor += ROUND4(len);

	return str;
}

const char* fbus_parcel_get_string(FBusParcel* thiz)
{
	return_val_if_fail(fbus_parcel_check(thiz, FTK_TYPE_STRING) == RET_OK, NULL);

	return fbus_parcel_get_raw_string(thiz);
}

const void* fbus_parcel_get_data(FBusParcel* thiz, size_t size)
{
	void* data = NULL;
	int data_size = 0;
	return_val_if_fail(fbus_parcel_check(thiz, FTK_TYPE_BINARY) == RET_OK, NULL);

	data_size = fbus_parcel_get_raw_int(thiz, 0);
	assert(data_size == size);
	
	data = thiz->data + thiz->cursor;
	thiz->cursor += ROUND4(data_size);

	return data;
}

int fbus_parcel_get_data_size(FBusParcel* thiz)
{
	int size = 0;
	return_val_if_fail(thiz != NULL, -1);
	return_val_if_fail(fbus_parcel_get_raw_int(thiz, 1) == FTK_TYPE_BINARY, -1);

	memcpy(&size, thiz->data+thiz->cursor+sizeof(int), sizeof(int));

	return size;
}

static Ret fbus_parcel_write_raw_data(FBusParcel* thiz, int type, const void* data, int len)
{
	int size = 2 * sizeof(int) + ROUND4(len);
	return_val_if_fail(fbus_parcel_extend(thiz, thiz->size + size) == RET_OK, RET_FAIL);

	memcpy(thiz->data + thiz->cursor, &type, sizeof(type));
	thiz->cursor += sizeof(type);
	if(type == FTK_TYPE_BINARY)
	{
		memcpy(thiz->data + thiz->cursor, &len, sizeof(len));
		thiz->cursor += sizeof(len);
	}
	memcpy(thiz->data + thiz->cursor, data, len);
	thiz->cursor += ROUND4(len);
	thiz->size = thiz->cursor;

	return RET_OK;
}

Ret fbus_parcel_write_char(FBusParcel* thiz, char data)
{
	int value = data;
	
	return fbus_parcel_write_raw_data(thiz, FTK_TYPE_CHAR, &value, sizeof(int));
}

Ret fbus_parcel_write_short(FBusParcel* thiz, short data)
{
	int value = data;
	
	return fbus_parcel_write_raw_data(thiz, FTK_TYPE_SHORT, &value, sizeof(int));
}

Ret fbus_parcel_write_int(FBusParcel* thiz, int data)
{
	int value = data;
	
	return fbus_parcel_write_raw_data(thiz, FTK_TYPE_INT, &value, sizeof(int));
}

Ret fbus_parcel_write_string(FBusParcel* thiz, const char* data)
{
	return_val_if_fail(thiz != NULL && data != NULL, RET_FAIL);

	return fbus_parcel_write_raw_data(thiz, FTK_TYPE_STRING, data, strlen(data) + 1);
}

Ret fbus_parcel_write_data(FBusParcel* thiz, const void* data, size_t len)
{
	return_val_if_fail(thiz != NULL && data != NULL, RET_FAIL);

	return fbus_parcel_write_raw_data(thiz, FTK_TYPE_BINARY, data, len);
}

Ret fbus_parcel_reset(FBusParcel* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	thiz->size = 0;
	thiz->cursor = 0;

	return RET_OK;
}

void fbus_parcel_destroy(FBusParcel* thiz)
{
	if(thiz != NULL)
	{
		FTK_FREE(thiz->data);
		FTK_FREE(thiz);
	}

	return;
}

#ifdef FBUS_PARCEL_TEST
#include "ftk_globals.h"
#include "ftk_allocator_default.h"
int main(int argc, char* argv[])
{
	int i = 0;
	int capacity = 0;
	char str[128] = {0};
	FBusParcel* thiz = NULL;
#ifndef USE_STD_MALLOC
	ftk_set_allocator((ftk_allocator_default_create()));
#endif
	thiz = fbus_parcel_create(32);

	for(i = 0; i < 100; i++)
	{
		snprintf(str, sizeof(str)-1, "%08d", i);
		assert(RET_OK == fbus_parcel_write_char(thiz, i));
		assert(RET_OK == fbus_parcel_write_short(thiz, i));
		assert(RET_OK == fbus_parcel_write_int(thiz, i));
		assert(RET_OK == fbus_parcel_write_string(thiz, str));
		assert(RET_OK == fbus_parcel_write_data(thiz, str, strlen(str)+1));
	}
	
	fbus_parcel_set_cursor(thiz, 0);

	for(i = 0; i < 100; i++)
	{
		snprintf(str, sizeof(str)-1, "%08d", i);
		assert(i == fbus_parcel_get_char(thiz));
		assert(i == fbus_parcel_get_short(thiz));
		assert(i == fbus_parcel_get_int(thiz));
		assert(strcmp(str, fbus_parcel_get_string(thiz)) == 0);
		assert(strcmp(str, fbus_parcel_get_data(thiz, fbus_parcel_get_data_size(thiz))) == 0);
	}
	
	capacity = fbus_parcel_capacity(thiz);
	assert(thiz->cursor == thiz->size);
	fbus_parcel_reset(thiz);
	assert(fbus_parcel_size(thiz) == 0);
	assert(fbus_parcel_cursor(thiz) == 0);
	assert(capacity == fbus_parcel_capacity(thiz));

	fbus_parcel_destroy(thiz);

	return 0;
}
#endif/*FBUS_PARCEL_TEST*/

