#include "ftk.h"
#include "ftk_app_designer.h"

#ifdef HAS_MAIN
int main(int argc, char* argv[])
#else
Ret ftk_main(int argc, char* argv[])
#endif
{
	FtkApp* app = NULL;
#ifdef HAS_MAIN
	ftk_init(argc, argv);
#endif

	app = ftk_app_designer_create();

	ftk_app_run(app, argc, argv);

#ifdef HAS_MAIN
	ftk_run();
#endif

	return RET_OK;
}

