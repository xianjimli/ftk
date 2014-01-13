
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
