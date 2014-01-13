
#include "ftk_log.h"
#include "ftk_mmap.h"
#include "ftk_allocator.h"

struct _FtkMmap
{
	FILE* fp;
	void* data;
	size_t length;
};

int ftk_mmap_exist(const char* filename)
{
	wchar_t buf[MAX_PATH];
	HANDLE handle;
	WIN32_FIND_DATAW find_data;
	if (MultiByteToWideChar(CP_ACP, 0, filename, -1, buf, MAX_PATH) == 0)
	{
		return 0;
	}
	handle = FindFirstFileW(buf, &find_data);
	if (handle == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	FindClose(handle);
	return 1;
}

FtkMmap* ftk_mmap_create(const char* filename, size_t offset, size_t size)
{
	size_t n;
	wchar_t buf[MAX_PATH];
	FtkMmap* thiz = NULL;
	LARGE_INTEGER li;
	WIN32_FILE_ATTRIBUTE_DATA attr;
	return_val_if_fail(filename != NULL, NULL);

	if (MultiByteToWideChar(CP_ACP, 0, filename, -1, buf, MAX_PATH) == 0)
	{
		return NULL;
	}
	if (!GetFileAttributesExW(buf, GetFileExInfoStandard, &attr))
	{
		return NULL;
	}
	li.LowPart  = attr.nFileSizeLow;
	li.HighPart = attr.nFileSizeHigh;
	n = (size_t) li.QuadPart;
	/*ftk_logd("%s: %d\n", filename, n);*/
	return_val_if_fail(offset < n, NULL);

	size = (offset + size) <= n ? size : n - offset;

	thiz = FTK_ZALLOC(sizeof(FtkMmap));
	return_val_if_fail(thiz != NULL, NULL);

	thiz->fp = fopen(filename, "rb");
	if(thiz->fp != NULL)
	{
		thiz->length = size;
		thiz->data = FTK_ZALLOC(size+1);
		fseek(thiz->fp, offset, SEEK_SET);
		fread(thiz->data, thiz->length, 1, thiz->fp);
		fclose(thiz->fp);
	}

	if(thiz->data == NULL || thiz->data == NULL)
	{
		FTK_ZFREE(thiz, sizeof(*thiz));
		ftk_logd("%s mmap %s failed.\n", __func__, filename);
	}

	return thiz;
}

void* ftk_mmap_data(FtkMmap* thiz)
{
	return_val_if_fail(thiz != NULL, NULL);

	return thiz->data;
}

size_t ftk_mmap_length(FtkMmap* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->length;
}

void ftk_mmap_destroy(FtkMmap* thiz)
{
	if(thiz != NULL)
	{
		FTK_FREE(thiz->data);
		FTK_ZFREE(thiz, sizeof(*thiz));
	}

	return;
}
