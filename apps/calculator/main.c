#include "ftk.h"
#include "ftk_app_calc.h"

#ifdef HAS_MAIN
int main(int argc, char* argv[])
#else
Ret ftk_main(int argc, char* argv[])
#endif
{
#ifdef HAS_MAIN
	ftk_init(argc, argv);
#endif
	ftk_app_run(ftk_app_calc_create(), argc, argv);	
#ifdef HAS_MAIN
	ftk_run();
#endif

	return RET_OK;
}
