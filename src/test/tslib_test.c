#include "ftk_source_tslib.h"

Ret on_event(void* user_data, FtkEvent* event)
{
	printf("%s: %d(%d %d)\n", __func__, event->type, event->u.mouse.x, event->u.mouse.y);

	return RET_OK;
}

int main(int argc, char* argv[])
{
	int i = 0;
	FtkSource* thiz = NULL;
	const char* filename = argv[1] != NULL ? argv[1] : "/dev/input/event0";
	ftk_set_allocator(ftk_allocator_default_create());
	thiz = ftk_source_tslib_create(filename, on_event, NULL);
	assert(ftk_source_get_fd(thiz) > 0);
	assert(ftk_source_check(thiz) < 0);

	for(i = 0; i < 100; i++)
	{
		if(ftk_source_dispatch(thiz) <= 0)
		{
			sleep(1);
		}
	}

	ftk_source_unref(thiz);

	return 0;
}
