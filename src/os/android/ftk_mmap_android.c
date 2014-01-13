
#include "ftk_jni.h"
#include "ftk_log.h"
#include "ftk_mmap.h"
#include "ftk_allocator.h"

struct _FtkMmap
{
	void* data;
	size_t length;
};

int      ftk_mmap_exist(const char* filename)
{
	return_val_if_fail(filename != NULL, 0);

	ftk_logd("ftk_mmap_exist: %s\n", filename);
	filename++;
	ftk_logd("ftk_mmap_exist: %s\n", filename);

	/* TODO: xxx */

	if(strncmp(filename, "assets", strlen(filename)) == 0)
	{
		return 1;
	}

	return 0;
}

FtkMmap* ftk_mmap_create(const char* filename, size_t offset, size_t size)
{
	FtkMmap* thiz = NULL;
	int fd;
	size_t asset_size;
	return_val_if_fail(filename != NULL, NULL);

	thiz = (FtkMmap*)FTK_ZALLOC(sizeof(FtkMmap));
	return_val_if_fail(thiz != NULL, NULL);

	ftk_logd("ftk_mmap_create: %s\n", filename);
	filename += 8;
	ftk_logd("ftk_mmap_create: %s\n", filename);

	fd = Android_OpenAsset(filename, &asset_size);

	size = (offset + size) <= asset_size ? size : asset_size - offset;

	if(offset < 0 || offset >= asset_size)
	{
		FTK_FREE(thiz);
		close(fd);
		return NULL;
	}

	thiz->length = size;
	thiz->data = FTK_ZALLOC(size);

	read(fd, thiz->data, size);
	close(fd);

	return thiz;
}

void*    ftk_mmap_data(FtkMmap* thiz)
{
	return_val_if_fail(thiz != NULL, NULL);
	return (unsigned char*)thiz->data;
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
		FTK_FREE(thiz->data);
		FTK_ZFREE(thiz, sizeof(*thiz));
	}
}
