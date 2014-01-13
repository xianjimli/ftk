#include "ftk.h"
#include "ftk_label.h"

int main(int argc, char* argv[])
{
	ftk_init(argc, argv);
	FtkWidget* thiz = ftk_label_create(NULL, 0, 0, 60, 20);
	ftk_widget_set_text(thiz, "ok");
	assert(strcmp(ftk_widget_get_text(thiz), "ok") == 0);
	ftk_widget_destroy(thiz);

	return 0;
}
