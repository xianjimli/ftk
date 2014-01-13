
extern "C" {
#include "ftk_typedef.h"
}
#include "ftk.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_display_rotate.h"
#include "ftk_display_qt.h"
#include "ftk_qt_widget.h"
#include <QtGui/QApplication>
#include <QtGui/QCloseEvent>
#include <QtGui/QDesktopWidget>
#include <QtGui/QImage>
#include <QtGui/QPainter>

typedef struct _PrivInfo
{
	QImage* img;
	unsigned char* bits;
	int width;
	int height;
}PrivInfo;

static QtWidget* widget = NULL;

QtWidget::QtWidget(QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_PaintOutsidePaintEvent);
	setAttribute(Qt::WA_InputMethodEnabled);
	//setAttribute(Qt::WA_PaintOnScreen);

	setUpdatesEnabled(false);

	//showMaximized();

	//setWindowFlags(Qt::Dialog);
	//showFullScreen();

	//setWindowFlags(Qt::subwindow);
	//showNormal();

	memset(&ev, 0, sizeof(FtkEvent));
}

QtWidget::~QtWidget()
{
}

void QtWidget::paintEvent(QPaintEvent* event)
{
	FtkDisplay* display;

	display = ftk_default_display();
	if(display == NULL)
	{
		return;
	}

	display = ftk_display_get_real_display(display);
	DECL_PRIV(display, priv);

	QPainter painter(this);
	QRect rect(0, 0, priv->width, priv->height);
	painter.drawImage(rect, *priv->img, rect/*event->rect()*/);
}

void QtWidget::keyPressEvent(QKeyEvent* event)
{
	ftk_logd("keyPressEvent\n");
}

void QtWidget::keyReleaseEvent(QKeyEvent* event)
{
	ftk_logd("keyReleaseEvent\n");
}

void QtWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	ftk_logd("mouseDoubleClickEvent\n");
}

void QtWidget::mouseMoveEvent(QMouseEvent* event)
{
	ev.type = FTK_EVT_MOUSE_MOVE;
	ev.u.mouse.x = event->x();
	ev.u.mouse.y = event->y();
	ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &ev);

	//ftk_logd("mouseMoveEvent\n");
}

void QtWidget::mousePressEvent(QMouseEvent* event)
{
	ev.type = FTK_EVT_MOUSE_DOWN;
	ev.u.mouse.x = event->x();
	ev.u.mouse.y = event->y();
	ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &ev);

	//ftk_logd("mousePressEvent\n");
}

void QtWidget::mouseReleaseEvent(QMouseEvent* event)
{
	ev.type = FTK_EVT_MOUSE_UP;
	ev.u.mouse.x = event->x();
	ev.u.mouse.y = event->y();
	ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &ev);

	//ftk_logd("mouseReleaseEvent\n");
}

void QtWidget::inputMethodEvent(QInputMethodEvent* event)
{
	ftk_logd("inputMethodEvent\n");
}

void QtWidget::closeEvent(QCloseEvent* event)
{
	ftk_logd("closeEvent\n");
	FTK_QUIT();
	event->accept();
}

static Ret ftk_display_qt_update(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	int i = 0;
	int j = 0;
	Ret ret = RET_OK;
	DECL_PRIV(thiz, priv);
	int width = rect->width;
	int height = rect->height;
	int src_width = ftk_bitmap_width(bitmap);
	int src_height = ftk_bitmap_height(bitmap);
	int dst_width = priv->width;
	int dst_height = priv->height;
	FtkColor* src = ftk_bitmap_lock(bitmap);
	FtkColor* dst = (FtkColor*)priv->bits;
	return_val_if_fail(rect->x < src_width && rect->y < src_height
		&& xoffset < dst_width && yoffset < dst_height, RET_FAIL);

	width = (xoffset + width) >= dst_width ? dst_width - xoffset : width;
	height = (yoffset + height) >= dst_height ? dst_height - yoffset : height;

	src += rect->y * src_width + rect->x;
	dst += yoffset * dst_width + xoffset;

	for(i = 0; i < height; i++)
	{
		memcpy(dst, src, sizeof(FtkColor) * width);
		dst += dst_width;
		src += src_width;
	}

	QApplication::postEvent(widget, new QPaintEvent(QRect(xoffset, yoffset, width, height)));

	return ret;
}

static Ret ftk_display_qt_snap(FtkDisplay* thiz, FtkRect* r, FtkBitmap* bitmap)
{
#if 0
	int i = 0;
	int xoffset = r->x;
	int yoffset = r->y;
	int width = r->width;
	int height = r->height;
	DECL_PRIV(thiz, priv);
	FtkColor* dst = ftk_bitmap_lock(bitmap);
	FtkColor* src = ftk_bitmap_lock(priv->bitmap);

	/*TODO*/
#endif

	return RET_OK;
}

static int ftk_display_qt_width(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);
	return priv->width;
}

static int ftk_display_qt_height(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);
	return priv->height;
}

static void ftk_display_qt_destroy(FtkDisplay* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		FTK_FREE(priv->bits);
		FTK_ZFREE(thiz, sizeof(FtkDisplay) + sizeof(PrivInfo));
	}
}

FtkDisplay* ftk_display_qt_create(void)
{
	FtkDisplay* thiz = NULL;

	thiz = (FtkDisplay*)FTK_ZALLOC(sizeof(FtkDisplay) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->update = ftk_display_qt_update;
		thiz->width = ftk_display_qt_width;
		thiz->height = ftk_display_qt_height;
		thiz->snap = ftk_display_qt_snap;
		thiz->destroy = ftk_display_qt_destroy;

		ftk_logd("%d-%d\n", QApplication::desktop()->width(), QApplication::desktop()->height());

		priv->width = /*widget->width();*/480;
		priv->height = /*widget->height();*/320;

		widget->setFixedSize(priv->width, priv->height);

		ftk_logd("%d-%d\n", priv->width, priv->height);
		priv->bits = (unsigned char*)FTK_ZALLOC(priv->width * priv->height * 4);

		priv->img = new QImage(priv->bits, priv->width, priv->height, priv->width * 4, QImage::Format_ARGB32);
		if(priv->img == NULL)
		{
			FTK_FREE(priv->bits);
			FTK_ZFREE(thiz, sizeof(FtkDisplay) + sizeof(PrivInfo));
		}
	}

	return thiz;
}

void* ftk_display_qt_create_win(void)
{
	widget = new QtWidget();
	return widget;
}
