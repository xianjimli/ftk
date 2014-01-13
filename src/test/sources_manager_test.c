#include "ftk.h"
#include "ftk_source_idle.h"
#include "ftk_sources_manager.h"

Ret idle(void* ctx)
{
	return RET_OK;
}

int main(int argc, char* argv[])
{
	int i = 0;
	int n = 64;
	
	FtkSource* source = NULL;
	FtkSourcesManager* thiz = NULL;
	ftk_set_allocator(ftk_allocator_default_create());
	thiz = ftk_sources_manager_create(n);
	for(i = 0; i < n; i++)
	{
		source = ftk_source_idle_create(idle, NULL);
		assert(ftk_sources_manager_add(thiz, source) == RET_OK);
		assert(ftk_sources_manager_get_count(thiz) == (i + 1));
		assert(ftk_sources_manager_get(thiz, i) == source);
	}
	
	for(i = 0; i < n; i++)
	{
		assert(ftk_sources_manager_get_count(thiz) == (n - i));
		assert(ftk_sources_manager_remove(thiz, ftk_sources_manager_get(thiz, 0)) == RET_OK);
	}

	ftk_sources_manager_destroy(thiz);

	return 0;
}
