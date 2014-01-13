
#include "ftk_typedef.h"
#include "ftk.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_display_rotate.h"
#include "ftk_display_gtk.h"
#include <gtk/gtk.h>

typedef struct _PrivInfo
{
	GdkGC* gc;
	GdkPixbuf* pixbuf;
	int width;
	int height;
	FtkEvent event;
}PrivInfo;

static GtkWidget* widget = NULL;

static Ret ftk_display_gtk_update(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
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
	FtkColor* dst = (FtkColor*)gdk_pixbuf_get_pixels(priv->pixbuf);
	GdkRectangle paint_rect;
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

	paint_rect.x = xoffset;
	paint_rect.y = yoffset;
	paint_rect.width = width;
	paint_rect.height= height;

	gdk_window_begin_paint_rect(widget->window, &paint_rect);
	gdk_draw_pixbuf(widget->window, priv->gc, priv->pixbuf, 0, 0, 0, 0, priv->width, priv->height, GDK_RGB_DITHER_NONE, 1, 1);
	gdk_window_end_paint(widget->window);
#if 0
	gdk_window_invalidate_rect(widget->window, &paint_rect, FALSE);
#endif

	return ret;
}

static Ret ftk_display_gtk_snap(FtkDisplay* thiz, FtkRect* r, FtkBitmap* bitmap)
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

static int ftk_display_gtk_width(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);
	return priv->width;
}

static int ftk_display_gtk_height(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);
	return priv->height;
}

static void ftk_display_gtk_destroy(FtkDisplay* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		g_object_unref(priv->pixbuf);
		g_object_unref(priv->gc);
		FTK_ZFREE(thiz, sizeof(FtkDisplay) + sizeof(PrivInfo));
	}
}

FtkDisplay* ftk_display_gtk_create(void)
{
	FtkDisplay* thiz = NULL;

	thiz = (FtkDisplay*)FTK_ZALLOC(sizeof(FtkDisplay) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->update = ftk_display_gtk_update;
		thiz->width = ftk_display_gtk_width;
		thiz->height = ftk_display_gtk_height;
		thiz->snap = ftk_display_gtk_snap;
		thiz->destroy = ftk_display_gtk_destroy;

		priv->width = 480;
		priv->height = 320;

		gtk_window_resize(GTK_WINDOW(widget), priv->width, priv->height);

		ftk_logd("%d-%d\n", priv->width, priv->height);

		priv->pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, priv->width, priv->height);
		if(priv->pixbuf == NULL)
		{
			FTK_ZFREE(thiz, sizeof(FtkDisplay) + sizeof(PrivInfo));
			return NULL;
		}

		priv->gc = gdk_gc_new(widget->window);
		if(priv->gc == NULL)
		{
			g_object_unref(priv->pixbuf);
			FTK_ZFREE(thiz, sizeof(FtkDisplay) + sizeof(PrivInfo));
			return NULL;
		}

		ftk_logd("gdk_pixbuf_get_rowstride() %d\n", gdk_pixbuf_get_rowstride(priv->pixbuf));
	}

	return thiz;
}

static gboolean window_button_press(GtkWidget* widget, GdkEventButton* event, gpointer data)
{
	FtkDisplay* display;
	PrivInfo* priv;

	display = ftk_display_get_real_display(ftk_default_display());
	priv = (PrivInfo*)display->priv;

	priv->event.type = FTK_EVT_MOUSE_DOWN;
	priv->event.u.mouse.x = event->x;
	priv->event.u.mouse.y = event->y;
	ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &priv->event);

	ftk_logd("window_button_press()\n");

	return TRUE;
}

static gboolean window_button_release(GtkWidget* widget, GdkEventButton* event, gpointer data)
{
	FtkDisplay* display;
	PrivInfo* priv;

	display = ftk_display_get_real_display(ftk_default_display());
	priv = (PrivInfo*)display->priv;

	priv->event.type = FTK_EVT_MOUSE_UP;
	priv->event.u.mouse.x = event->x;
	priv->event.u.mouse.y = event->y;
	ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &priv->event);

	ftk_logd("window_button_release()\n");

	return TRUE;
}

static gboolean window_motion_notify(GtkWidget* widget, GdkEventMotion* event, gpointer data)
{
	int x, y;
	GdkModifierType state;
	FtkDisplay* display;
	PrivInfo* priv;

	if(event->is_hint)
	{
		gdk_window_get_pointer(event->window, &x, &y, &state);
	}
	else
	{
		x = event->x;
		y = event->y;
		state = event->state;
	}

	if(!(state & GDK_BUTTON1_MASK))
	{
		return TRUE;
	}

	display = ftk_display_get_real_display(ftk_default_display());
	priv = (PrivInfo*)display->priv;

	priv->event.type = FTK_EVT_MOUSE_MOVE;
	priv->event.u.mouse.x = x;
	priv->event.u.mouse.y = y;
	ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &priv->event);

	/* ftk_logd("window_motion_notify()\n"); */

	return TRUE;
}

static gboolean window_key_press(GtkWidget* widget, GdkEventKey* event, gpointer data)
{
	ftk_logd("window_key_press()\n");
	return TRUE;
}

static gboolean window_key_release(GtkWidget* widget, GdkEventKey* event, gpointer data)
{
	ftk_logd("window_key_release()\n");
	return TRUE;
}

static void window_destroy(GtkWidget* widget, gpointer data)
{
	FTK_QUIT();

	gtk_main_quit();
}

void* ftk_display_gtk_create_win(void)
{
	widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_container_set_border_width(GTK_CONTAINER(widget), 20);
	gtk_window_set_title(GTK_WINDOW(widget), "FTK");

	gtk_widget_set_events(widget, GDK_EXPOSURE_MASK|GDK_POINTER_MOTION_MASK|GDK_POINTER_MOTION_HINT_MASK|GDK_BUTTON_MOTION_MASK|GDK_BUTTON_PRESS_MASK|GDK_BUTTON_RELEASE_MASK|GDK_KEY_PRESS_MASK|GDK_KEY_RELEASE_MASK|GDK_ENTER_NOTIFY_MASK|GDK_LEAVE_NOTIFY_MASK|GDK_FOCUS_CHANGE_MASK);

	g_signal_connect(G_OBJECT(widget), "button-press-event", G_CALLBACK(window_button_press), NULL);
	g_signal_connect(G_OBJECT(widget), "button-release-event", G_CALLBACK(window_button_release), NULL);
	g_signal_connect(G_OBJECT(widget), "motion-notify-event", G_CALLBACK(window_motion_notify), NULL);

	g_signal_connect(G_OBJECT(widget), "key-press-event", G_CALLBACK(window_key_press), NULL);
	g_signal_connect(G_OBJECT(widget), "key-release-event", G_CALLBACK(window_key_release), NULL);

	g_signal_connect(G_OBJECT(widget), "destroy", G_CALLBACK(window_destroy), NULL);

	gtk_widget_show_all(widget);

	return widget;
}
