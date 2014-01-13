/*
 * File: ftk_mmap_linux.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   linux mmap implemention.
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
 * 2009-11-20 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_mmap.h"
#include "ftk_allocator.h"
#include <sys/mman.h>

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
	return_val_if_fail(filename != NULL, NULL);
	if(stat(filename, &st) != 0)
	{
		return NULL;
	}
	return_val_if_fail(offset < st.st_size, NULL);

	size = (offset + size) <= st.st_size ? size : st.st_size - offset;

	thiz = FTK_ZALLOC(sizeof(FtkMmap));
	return_val_if_fail(thiz != NULL, NULL);

	thiz->fd = open(filename, O_RDONLY);
	if(thiz->fd > 0)
	{
		thiz->length = size;
		thiz->data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, thiz->fd, offset);
	}

	if(thiz->data == NULL || thiz->data == MAP_FAILED)
	{
		close(thiz->fd);
		FTK_ZFREE(thiz, sizeof(*thiz));
		ftk_logd("%s mmap %s failed.\n", __func__, filename);
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
	if(thiz != NULL)
	{
		close(thiz->fd);
		munmap(thiz->data, thiz->length);

		FTK_ZFREE(thiz, sizeof(*thiz));
	}

	return;
}


