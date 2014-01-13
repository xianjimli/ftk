#include "ftk.h"
#include "ftk_source_primary.h"

Ret my_action(void* user_data)
{
	printf("%s:%p\n", __func__, user_data);

	return RET_REMOVE;
}

Ret on_event(void* user_data, FtkEvent* event)
{
	printf("event->type=%d\n", event->type);

	return RET_OK;
}

int main(int argc, char* argv[])
{
	int fd = 0;
	int ret =  0;
	fd_set fdset;
	struct timeval tv = {0};
	ftk_set_allocator(ftk_allocator_default_create());
	FtkSource* thiz = ftk_source_primary_create(on_event, NULL);
	fd = ftk_source_get_fd(thiz);
	assert(fd >= 0);
	FD_ZERO(&fdset);
	FD_SET(fd, &fdset);
	tv.tv_sec = 1000;
	tv.tv_usec =0;
	FtkEvent event = {.type = 1};
	ftk_source_queue_event(thiz, &event);
	event.type = 2;
	ftk_source_queue_event(thiz, &event);
	event.type = 3;
	ftk_source_queue_event(thiz, &event);
	event.type = 4;
	ftk_source_queue_event(thiz, &event);
	while((ret = select(fd+1, &fdset, NULL, NULL, &tv)) > 0)
	{
		ftk_source_dispatch(thiz);
	}
	ftk_source_unref(thiz);
	return 0;
}
