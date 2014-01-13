#include "ftk.h"

int main(int argc, char* argv[])
{
	int i = 0;
	FtkListItemInfo info ={0};
	FtkListItemInfo* pinfo = NULL;
	ftk_set_allocator(ftk_allocator_default_create());
	FtkListModel* thiz = ftk_list_model_default_create(0);
	
	for(i = 0; i < 1000; i++)
	{
		info.text ="12346578";
		info.state = i;
		assert(ftk_list_model_get_total(thiz) == i);
		assert(ftk_list_model_add(thiz, &info) == RET_OK);
		assert(ftk_list_model_get_data(thiz, i, (void**)&pinfo) == RET_OK);
		assert(pinfo->state == i);
	}
	
	for(i = 0; i < 1000; i++)
	{
		assert(ftk_list_model_get_total(thiz) == 1000-i);
		assert(ftk_list_model_remove(thiz, 0) == RET_OK);
	}

	ftk_list_model_destroy(thiz);

	return 0;
}
