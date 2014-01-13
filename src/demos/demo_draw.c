#include <math.h>
#include "ftk.h"

#define IDC_PAINT 100
static int g_width = 10;

static Ret on_paint(void* ctx, void* obj)
{
	FtkGc gc;
	int i = 0;
	int w = 0;
	int h = 0;
	FtkPoint p = {0};
	FtkWidget* thiz = obj;
	FTK_BEGIN_PAINT(x, y, width, height, canvas);

	ftk_logd("%s:%d\n", __func__, __LINE__);
	gc.fg.a = 0xff;
	gc.fg.r = 0xff;
	gc.fg.g = 0;
	gc.fg.b = 0;
	gc.mask = FTK_GC_FG;
	
	w = g_width % width;
	ftk_canvas_set_gc(canvas, &gc);
	ftk_canvas_draw_line(canvas, x, y + height/2, x + w, y + height/2);
	for(i = 0; i < height; i+=10)
	{
		ftk_canvas_draw_line(canvas, x, y, x + width, y + i);
	}

	for(i = width; i >= 0; i-=10)
	{
		ftk_canvas_draw_line(canvas, x, y, x + i, y + height);
	}

	gc.fg.r = 0;
	gc.fg.g = 0xff;
	ftk_canvas_set_gc(canvas, &gc);

	for(i = 0; i < w; i++)
	{
		double r =  2 * 3.14 * i / width;
		h = height/3 * sin(r);
		p.x = x + i;
		p.y = y + height/2 + h;
		ftk_canvas_draw_pixels(canvas, &p, 1);
	}
	
	ftk_canvas_reset_gc(canvas, ftk_widget_get_gc(thiz)); 
	ftk_canvas_draw_string(canvas, x + 100, y + height/2, "Hello World!", strlen("Hello World!"), 1);

	ftk_logd("%s:%d\n", __func__, __LINE__);
	FTK_END_PAINT();

	return RET_OK;
}

static Ret button_step_clicked(void* ctx, void* obj)
{

	g_width += 30;
	ftk_widget_invalidate(ftk_widget_lookup(ctx, IDC_PAINT));

	return RET_OK;
}

static Ret button_quit_clicked(void* ctx, void* obj)
{
	ftk_widget_unref(ctx);

	return RET_OK;
}

#ifdef FTK_AS_PLUGIN
#include "ftk_app_demo.h"
FTK_HIDE int FTK_MAIN(int argc, char* argv[]);
FtkApp* ftk_app_demo_draw_create()
{
	return ftk_app_demo_create(_("draw"), ftk_main);
}
#else
#define FTK_HIDE extern
#endif /*FTK_AS_PLUGIN*/

FTK_HIDE int FTK_MAIN(int argc, char* argv[])
{
	int width = 0;
	int height = 0;
	FtkWidget* win = NULL;
	FtkWidget* button = NULL;
	FtkWidget* painter = NULL;
	FTK_INIT(argc, argv);

	win = ftk_app_window_create();
	ftk_window_set_animation_hint(win, "app_main_window");
	width = ftk_widget_width(win);
	height = ftk_widget_height(win);

	button = ftk_button_create(win, 0, 10, width/3, 60);
	ftk_widget_set_text(button, "step");
	ftk_button_set_clicked_listener(button, button_step_clicked, win);

	button = ftk_button_create(win, width*2/3, 10, width/3, 60);
	ftk_widget_set_text(button, "quit");
	ftk_button_set_clicked_listener(button, button_quit_clicked, win);

	painter = ftk_painter_create(win, 0, 70, width, height-70);
	ftk_widget_set_id(painter, IDC_PAINT);
	ftk_painter_set_paint_listener(painter, on_paint, NULL);

	ftk_widget_set_text(win, "drawking kit demo");
	ftk_widget_show_all(win, 1);
	FTK_QUIT_WHEN_WIDGET_CLOSE(win);

	FTK_RUN();

	return 0;
}
