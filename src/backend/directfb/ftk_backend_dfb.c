#include "ftk_globals.h"
#include "ftk_display_dfb.h"
#include "ftk_source_dfb.h"

#define DFBCHECK(x) x
	
static IDirectFB* s_dfb = NULL;

IDirectFB* directfb_get(void)
{
	return s_dfb;
}

Ret ftk_backend_init(int argc, char* argv[])
{
	DFBCHECK(DirectFBInit( &argc, &argv ));
	DFBCHECK(DirectFBCreate( &s_dfb ));

	ftk_set_display(ftk_display_dfb_create(s_dfb));

	return RET_OK;
}
