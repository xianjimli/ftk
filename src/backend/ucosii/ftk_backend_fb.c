#include "ftk_log.h"
#include "ftk_globals.h"
#include "ftk_display_fb.h"
#include "ftk_source_input.h"
#include "ftk_wnd_manager.h"

static Ret ftk_init_input(void)
{
	const char* tsdev = NULL;
	FtkSource* source = NULL;
	
	source = ftk_source_input_create((FtkOnEvent)ftk_wnd_manager_queue_event_auto_rotate, ftk_default_wnd_manager());

	if(source != NULL)
	{
		ftk_sources_manager_add(ftk_default_sources_manager(), source);
	}

	return RET_OK;
}

Ret ftk_backend_init(int argc, char* argv[])
{
	ftk_init_input();
	ftk_set_display(ftk_display_fb_create());
	if(ftk_default_display() == NULL)
	{
		ftk_loge("open display failed.\n");
		exit(0);
	}

	return RET_OK;
}

