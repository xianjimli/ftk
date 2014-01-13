#include "ftk.h"

Ret my_action(void* user_data)
{	
	int* p = (int*)user_data;

	(*p)++;
	printf("%d\n", *p);

	return *p == 10 ? RET_REMOVE:RET_OK;
}

int main(int argc, char* argv[])
{
	int n = 0;

	ftk_set_allocator(ftk_allocator_default_create());
	FtkSource* thiz = ftk_source_timer_create(1500, my_action, &n);
	while(1)
	{
		int t = ftk_source_check(thiz);
		printf("t=%d\n", t);
		assert(t > 1000);
		assert(ftk_source_get_fd(thiz) < 0);
		usleep(t*1000);
		if(ftk_source_dispatch(thiz) == RET_REMOVE)
		{
			break;
		}
	}
	ftk_source_unref(thiz);

	return 0;
}
