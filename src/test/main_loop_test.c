#include "ftk.h"
#include "ftk_globals.h"
#include "ftk_main_loop.h"
#include "ftk_source_idle.h"
#include "ftk_source_timer.h"

Ret timeout_quit(void* user_data)
{
	ftk_quit();

	printf("%s:%d\n", __func__, __LINE__);
	return RET_REMOVE;
}

Ret timeout_repeat(void* user_data)
{
	printf("%s:%d\n", __func__, __LINE__);
	return RET_OK;
}

Ret idle(void* user_data)
{
	printf("%s:%d\n", __func__, __LINE__);
	return RET_REMOVE;
}

int main(int argc, char* argv[])
{
	FtkSource* source = NULL;
	ftk_init(argc, argv);

	source = ftk_source_idle_create(idle, NULL);
	ftk_main_loop_add_source(ftk_default_main_loop(), source);

	source = ftk_source_timer_create(1000, timeout_repeat, NULL);
	ftk_main_loop_add_source(ftk_default_main_loop(), source);

	source = ftk_source_timer_create(5000, timeout_quit, NULL);
	ftk_main_loop_add_source(ftk_default_main_loop(), source);

	ftk_run();

	return 0;
}

