#include "ftk_display_win32.h"
#include "ftk_source_win32.h"
#include "ftk_wnd_manager.h"
#include "ftk_globals.h"

Ret ftk_backend_init(void)
{
	FtkSource* source = NULL;
	FtkDisplay* display = NULL;

	display = ftk_display_win32_create();
	ftk_set_display(display);

	source = ftk_source_win32_create();
	ftk_sources_manager_add(ftk_default_sources_manager(), source);

	return display != NULL ? RET_OK : RET_FAIL;
}
