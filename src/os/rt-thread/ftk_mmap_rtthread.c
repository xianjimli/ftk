/*
 * File: ftk_mmap_rtthread.c
 * Author:  Jiao JinXing <jiaojinxing1987@gmail.com>
 * Brief:   RT-Thread mmap implemention.
 *
 * Copyright (c) 2009 - 2010  Jiao JinXing <jiaojinxing1987@gmail.com>
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
 * 2010-10-2 Jiao JinXing <jiaojinxing1987@gmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_mmap.h"
#include "ftk_allocator.h"

struct _FtkMmap
{
	int fd;
	void* data;
	size_t length;
};

int      ftk_mmap_exist(const char* filename)
{
	struct stat st = {0};
	return_val_if_fail(filename != NULL, 0);

	return stat(filename, &st) == 0;
}

FtkMmap* ftk_mmap_create(const char* filename, size_t offset, size_t size)
{
	FtkMmap* thiz = NULL;
	struct stat st = {0};
	ssize_t ret = 0;
	return_val_if_fail(filename != NULL, NULL);
	return_val_if_fail(stat(filename, &st) == 0, NULL);
	return_val_if_fail(offset < st.st_size, NULL);

	size = (offset + size) <= st.st_size ? size : st.st_size - offset;

	thiz = FTK_ZALLOC(sizeof(FtkMmap));
	return_val_if_fail(thiz != NULL, NULL);

	thiz->fd = open(filename, O_RDONLY, 0);
	if (thiz->fd < 0)
	{
		FTK_ZFREE(thiz, sizeof(*thiz));
		return NULL;
	}

	thiz->length = size;
	thiz->data = FTK_ZALLOC(size);
	if(thiz->data == NULL)
	{
		close(thiz->fd);
		FTK_ZFREE(thiz, sizeof(*thiz));
		return NULL;
	}

	ret = read(thiz->fd, thiz->data, size);
	close(thiz->fd);
	if (ret != size)
	{
		FTK_ZFREE(thiz->data, size);
		FTK_ZFREE(thiz, sizeof(*thiz));
		return NULL;
	}

	return thiz;
}

void*    ftk_mmap_data(FtkMmap* thiz)
{
	return_val_if_fail(thiz != NULL, NULL);

	return thiz->data;
}

size_t   ftk_mmap_length(FtkMmap* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->length;
}

void     ftk_mmap_destroy(FtkMmap* thiz)
{
	if (thiz != NULL)
	{
		FTK_ZFREE(thiz->data, thiz->length);
		FTK_ZFREE(thiz, sizeof(*thiz));
	}
}

