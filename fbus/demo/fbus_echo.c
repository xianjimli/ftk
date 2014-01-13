/*
 * File:    fbus_echo.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   echo client api.
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

#include "fbus_echo.h"
#include "fbus_echo_common.h"

FBusEcho* fbus_echo_create()
{
	return fbus_proxy_create(FBUS_SERVICE_ECHO);	
}

Ret fbus_echo_char(FBusEcho* thiz, char in, char* out)
{
	Ret ret = RET_FAIL;
	FBusParcel* parcel = fbus_proxy_get_parcel(thiz);
	return_val_if_fail(parcel != NULL && out != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FBUS_ECHO_CHAR);
	fbus_parcel_write_char(parcel, in);
	
	if((ret = fbus_proxy_request(thiz, parcel)) == RET_OK)
	{
		if((ret = fbus_parcel_get_int(parcel)) == RET_OK)
		{
			*out = fbus_parcel_get_char(parcel);
		}
	}

	return ret;
}

Ret fbus_echo_short(FBusEcho* thiz, short in, short* out)
{
	Ret ret = RET_FAIL;
	FBusParcel* parcel = fbus_proxy_get_parcel(thiz);
	return_val_if_fail(parcel != NULL && out != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FBUS_ECHO_SHORT);
	fbus_parcel_write_short(parcel, in);
	
	if((ret = fbus_proxy_request(thiz, parcel)) == RET_OK)
	{
		if((ret = fbus_parcel_get_int(parcel)) == RET_OK)
		{
			*out = fbus_parcel_get_short(parcel);
		}
	}

	return ret;
}

Ret fbus_echo_int(FBusEcho* thiz, int in, int* out)
{
	Ret ret = RET_FAIL;
	FBusParcel* parcel = fbus_proxy_get_parcel(thiz);
	return_val_if_fail(parcel != NULL && out != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FBUS_ECHO_INT);
	fbus_parcel_write_int(parcel, in);
	
	if((ret = fbus_proxy_request(thiz, parcel)) == RET_OK)
	{
		if((ret = fbus_parcel_get_int(parcel)) == RET_OK)
		{
			*out = fbus_parcel_get_int(parcel);
		}
	}

	return ret;
}

Ret fbus_echo_string(FBusEcho* thiz, const char* in, const char** out)
{
	Ret ret = RET_FAIL;
	FBusParcel* parcel = fbus_proxy_get_parcel(thiz);
	return_val_if_fail(parcel != NULL && out != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FBUS_ECHO_STRING);
	fbus_parcel_write_string(parcel, in);
	
	if((ret = fbus_proxy_request(thiz, parcel)) == RET_OK)
	{
		if((ret = fbus_parcel_get_int(parcel)) == RET_OK)
		{
			*out = fbus_parcel_get_string(parcel);
		}
	}

	return ret;
}

Ret fbus_echo_data(FBusEcho* thiz, const char* in, size_t in_len, const char** out, size_t* out_len)
{
	Ret ret = RET_FAIL;
	FBusParcel* parcel = fbus_proxy_get_parcel(thiz);
	return_val_if_fail(parcel != NULL && out != NULL, RET_FAIL);

	fbus_parcel_write_int(parcel, FBUS_ECHO_BINARY);
	fbus_parcel_write_int(parcel, in_len);
	fbus_parcel_write_data(parcel, in, in_len);
	
	if((ret = fbus_proxy_request(thiz, parcel)) == RET_OK)
	{
		if((ret = fbus_parcel_get_int(parcel)) == RET_OK)
		{
			*out_len = fbus_parcel_get_data_size(parcel);
			*out = fbus_parcel_get_data(parcel, in_len);
		}
	}

	return ret;
}

void fbus_echo_destroy(FBusEcho* thiz)
{
	fbus_proxy_destroy(thiz);

	return;
}

