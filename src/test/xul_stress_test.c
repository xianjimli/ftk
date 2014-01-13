#include "ftk.h"
#include "ftk_xul.h"

#define IDC_QUIT 100

int i = 0;
FtkWidget* win[100] = {0};
const char* t1 = "<window> </window>";

static Ret timeout(void* ctx)
{
	ftk_widget_unref(win[i]);
	i--;
	
	if(i == 0)
	{
		ftk_quit();
	}

	return i == 0 ? RET_REMOVE: RET_OK;
}

int main(int argc, char* argv[])
{
	if(argc > 1)
	{
		FtkSource* timer = NULL;
		ftk_init(argc, argv);

		for(; i < 100; i++)
		{
			win[i] = ftk_xul_load_file(argv[1], NULL);
			ftk_widget_show_all(win[i], 1);
		}

		timer = ftk_source_timer_create(100, timeout, NULL);
		ftk_main_loop_add_source(ftk_default_main_loop(), timer);

		ftk_run();
	}
	else
	{
		ftk_logd("Usage: %s xul\n", argv[0]);

		return 0;
	}

	return 0;
}

