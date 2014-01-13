
extern "C" {
#include "ftk_typedef.h"
}
#include "ftk_display_qt.h"
#include <QtGui/QApplication>
#include <QtGui/QWidget>

extern "C" int FTK_MAIN(int argc, char* argv[]);

static pthread_t thread;

static void* thread_func(void* data)
{
	int argc = 1;
	char* argv[] = { (char*)"ftk" };
	FTK_MAIN(argc, argv);

	QApplication::exit(0);
	return NULL;
}

int main(int argc, char* argv[])
{
	int rc;
	QApplication app(argc, argv);
	QWidget* widget = (QWidget*)ftk_display_qt_create_win();
	widget->show();

	pthread_create(&thread, NULL, thread_func, NULL);
	rc = app.exec();
	pthread_join(thread, NULL);

	return rc;
}
