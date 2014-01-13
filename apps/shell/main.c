#include "ftk.h"
#include "ftk_app_shell.h"

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

	app = ftk_app_shell_create();
	ftk_app_run(ftk_app_shell_create(), argc, argv);

#ifdef HAS_MAIN
	ftk_run();
#endif

	return RET_OK;
}

