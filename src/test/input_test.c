#include "ftk.h"
#include "ftk_source_input.h"
#include "ftk_source_ps2mouse.h"

Ret on_event(void* user_data, FtkEvent* event)
{
	switch(event->type)
	{
		case FTK_EVT_MOUSE_DOWN:
		{
			printf("%s: press(%d %d)\n", __func__, 
				event->u.mouse.x, event->u.mouse.y);
			break;
		}
		case FTK_EVT_MOUSE_UP:
		{
			printf("%s: release (%d %d)\n", __func__, 
				event->u.mouse.x, event->u.mouse.y);
			break;
		}
		case FTK_EVT_MOUSE_MOVE:
		{
			printf("%s: move (%d %d)\n", __func__, 
				event->u.mouse.x, event->u.mouse.y);
			break;
		}
		case FTK_EVT_KEY_DOWN:
		case FTK_EVT_KEY_UP:
		{
			printf("%s: %d %d\n", __func__, event->type, event->u.key.code);
		}
		default:break;
	}
	return RET_OK;
}

int main(int argc, char* argv[])
{
	int i = 0;
	FtkSource* thiz = NULL;
	const char* filename = argv[1] != NULL ? argv[1] : "/dev/input/event2";
	
	ftk_set_allocator(ftk_allocator_default_create());
	if(strstr(filename, "mice") != NULL)
	{
		thiz = ftk_source_ps2mouse_create(filename, on_event, NULL, 1024, 768);
	}
	else
	{
		thiz = ftk_source_input_create(filename, on_event, NULL);
	}
	assert(ftk_source_get_fd(thiz) > 0);
	assert(ftk_source_check(thiz) < 0);

	for(i = 0; i < 1000; i++)
	{
		ftk_source_dispatch(thiz);
	}

	ftk_source_unref(thiz);

	return 0;
}
