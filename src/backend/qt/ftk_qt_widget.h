
#ifndef FTK_QT_WIDGET_H
#define FTK_QT_WIDGET_H

#include "ftk_event.h"
#include <QtGui/QWidget>

class QtWidget : public QWidget
{
	Q_OBJECT
public:
	QtWidget(QWidget* parent = 0);
	~QtWidget();
protected:
	virtual void paintEvent(QPaintEvent* event);
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void inputMethodEvent(QInputMethodEvent* event);
	virtual void closeEvent(QCloseEvent* event);
private:
	FtkEvent ev;
};

#endif/*FTK_QT_WIDGET_H*/
