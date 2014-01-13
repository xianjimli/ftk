#include "ftk.h"
#include "ftk_lookup_table.h"
#include "ftk_allocator_default.h"

int compare(const void* obj1, const void* obj2)
{
	return strncmp(obj1, obj2, strlen(obj2));
}

int main(int argc, char* argv[])
{
	const char* start = NULL;
	char value[FTK_VALUE_LEN + 1] = {0};
	FtkLookupTable* thiz = NULL;
	
	ftk_set_allocator((ftk_allocator_default_create()));
	if(argc != 3)
	{
		ftk_logd("usage: %s file key\n", argv[0]);

		return 0;
	}

	thiz = ftk_lookup_table_create(argv[1], 0, compare);

	assert(ftk_lookup_table_load(thiz) == RET_OK);

	start = ftk_lookup_table_lookup(thiz, argv[2]);
	strncpy(value, start, FTK_VALUE_LEN);
	ftk_logd("%s=%s\n", argv[2], value);
	ftk_lookup_table_destroy(thiz);

	return 0;
}
