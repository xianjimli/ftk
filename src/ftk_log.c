/*
 * File: ftk_log.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   some log functions.
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
 * 2009-10-03 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_globals.h"

#ifdef IPHONE
static FILE* log_file = NULL;
#elif defined(ANDROID) && defined(ANDROID_NDK)
#include "ftk_jni.h"
#endif

Ret ftk_log(const char* format, va_list ap)
{
	char buffer[1024] = {0};
	ftk_vsnprintf(buffer, sizeof(buffer), format, ap);

	printf("%s", buffer);

#ifdef IPHONE
	if(log_file == NULL)
	{
		char* filename = "log_file.txt";
		log_file = fopen(filename, "a+");
		if(log_file == NULL)
		{
			return RET_OK;
		}
	}
	fprintf(log_file, buffer);
	fflush(log_file);
#elif defined(ANDROID) && defined(ANDROID_NDK)
	Android_Log(buffer);
#endif

	return RET_OK;
}

Ret ftk_logv(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	if(ftk_default_log_level() <= FTK_LOG_V)
	{
		ftk_log(format, ap);
	}
	va_end(ap);
	
	return RET_OK;
}

Ret ftk_logd(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	if(ftk_default_log_level() <= FTK_LOG_D)
	{
		ftk_log(format, ap);
	}
	va_end(ap);

	return RET_OK;
}

Ret ftk_logi(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	if(ftk_default_log_level() <= FTK_LOG_I)
	{
		ftk_log(format, ap);
	}
	va_end(ap);
	
	return RET_OK;
}

Ret ftk_logw(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	if(ftk_default_log_level() <= FTK_LOG_W)
	{
		ftk_log(format, ap);
	}
	va_end(ap);
	
	return RET_OK;
}

Ret ftk_loge(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	if(ftk_default_log_level() <= FTK_LOG_E)
	{
		ftk_log(format, ap);
	}
	va_end(ap);
	
	return RET_OK;
}


