#include "ftk_display_x11.h"
#include "ftk_source_x11.h"
#include "ftk_wnd_manager.h"
#include "ftk_globals.h"

Ret ftk_backend_init(void)
{
	FtkSource* source = NULL;
	FtkDisplay* display = NULL;

	display = ftk_display_x11_create(&source, (FtkOnEvent)ftk_wnd_manager_queue_event_auto_rotate, 
		ftk_default_wnd_manager());
	ftk_set_display(display);
	ftk_sources_manager_add(ftk_default_sources_manager(), source);

	return display != NULL ? RET_OK : RET_FAIL;
}
