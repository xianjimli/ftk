
#include "ftk_typedef.h"
#include "ftk_display_gtk.h"
#include <gtk/gtk.h>

extern int FTK_MAIN(int argc, char* argv[]);

static pthread_t thread;

static void* thread_func(void* data)
{
	int argc = 1;
	char* argv[] = { (char*)"ftk" };

	FTK_MAIN(argc, argv);
	gtk_main_quit();

	return NULL;
}

int main(int argc, char* argv[])
{
	GtkWidget* window = NULL;

	gtk_init(&argc, &argv);

	window = (GtkWidget*)ftk_display_gtk_create_win();

	pthread_create(&thread, NULL, thread_func, NULL);
	gtk_main();
	pthread_join(thread, NULL);

	return 0;
}
