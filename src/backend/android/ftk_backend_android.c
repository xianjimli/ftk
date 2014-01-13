
#include "ftk_display_android.h"
#include "ftk_source_android.h"
#include "ftk_globals.h"

Ret ftk_backend_init(void)
{
	FtkSource* source = NULL;
	FtkDisplay* display = NULL;

	display = ftk_display_android_create();
	ftk_set_display(display);

	source = ftk_source_android_create();
	ftk_sources_manager_add(ftk_default_sources_manager(), source);

	return display != NULL ? RET_OK : RET_FAIL;
}
