#include "ftk.h"
#include "ftk_pairs.h"

int main(int argc, char* argv[])
{
	int i = 0;
	int nr = 100;
	char key[FTK_KEY_LEN+1];
	char value[FTK_VALUE_LEN+1];
	FtkPairs* thiz = NULL;
	ftk_set_allocator(ftk_allocator_default_create());
	
	thiz = ftk_pairs_create(nr, (FtkCompare)strcmp);

	for(i = 0; i < nr; i++)
	{
		snprintf(key, FTK_KEY_LEN, "key%d", i);
		snprintf(value, FTK_VALUE_LEN, "value%d", i);
		assert(ftk_pairs_set(thiz, key, value) == RET_OK);
		assert(strcmp(ftk_pairs_find(thiz, key), value) == 0);
	}
	
	ftk_pairs_dump(thiz);
	for(i = 0; i < nr; i++)
	{
		snprintf(key, FTK_KEY_LEN, "key%d", i);
		assert(ftk_pairs_remove(thiz, key) == RET_OK);
		assert(ftk_pairs_find(thiz, key) == NULL);
	}

	ftk_pairs_destroy(thiz);

	return 0;
}
