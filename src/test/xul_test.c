#include "ftk.h"
#include "ftk_xul.h"
#include "test_util.c"

const char* t1 = "<window> </window>";

int main(int argc, char* argv[])
{
	FtkWidget* win = NULL;
	ftk_init(argc, argv);

	win = ftk_xul_load(t1, strlen(t1));

	ftk_widget_unref(win);

	return 0;
}
