#include "ftk.h"

static Ret button_clicked(void* ctx, void* obj)
{
	return RET_OK;
}

#ifdef FTK_AS_PLUGIN
#include "ftk_app_demo.h"
FTK_HIDE int FTK_MAIN(int argc, char* argv[]);
FtkApp* ftk_app_demo_check_button_create()
{
	return ftk_app_demo_create(_("check_button"), ftk_main);
}
#else
#define FTK_HIDE extern
#endif /*FTK_AS_PLUGIN*/

FTK_HIDE int FTK_MAIN(int argc, char* argv[])
{
	int win_width = 0;
	int win_height = 0;
	int group_height = 0;
	int button_width = 0;
	int button_height = 0;
	FtkWidget* win = NULL;
	FtkWidget* button = NULL;
	FtkWidget* group = NULL;
	FTK_INIT(argc, argv);
	
	win = ftk_app_window_create();
	ftk_window_set_animation_hint(win, "app_main_window");
	win_width = ftk_widget_width(win);
	win_height = ftk_widget_height(win);
	
	button_width = win_width/2 - 20;
	button_height = 50;
	group_height = (win_height - 6)/3;

	group = ftk_group_box_create(win, 0, 0, win_width, group_height);
	ftk_widget_set_text(group, "Favorites");
	button = ftk_check_button_create(group, 20, 
		(group_height - button_height) / 2, button_width, button_height);
	ftk_widget_set_text(button, "Sports");
	ftk_check_button_set_clicked_listener(button, button_clicked, win);

	button = ftk_check_button_create(group, button_width + 20, 
		(group_height - button_height) / 2, button_width, button_height);
	ftk_widget_set_text(button, "Reading");
	ftk_check_button_set_clicked_listener(button, button_clicked, win);

	group = ftk_group_box_create(win, 0, group_height + 2, win_width, group_height);
	ftk_widget_set_text(group, "Gender");
	
	button = ftk_check_button_create_radio(group, 20,
		(group_height - button_height) / 2, button_width, button_height);
	ftk_widget_set_text(button, "Male");
	ftk_check_button_set_clicked_listener(button, button_clicked, win);

	button = ftk_check_button_create_radio(group, button_width + 20, 
		(group_height - button_height) / 2, button_width, button_height);
	ftk_widget_set_text(button, "Female");
	ftk_check_button_set_clicked_listener(button, button_clicked, win);
	
	group = ftk_group_box_create(win, 0, 2 * group_height + 2, win_width, group_height);
	ftk_widget_set_text(group, "Gender(Right Icon)");
	
	button = ftk_check_button_create_radio(group, 20,
		(group_height - button_height) / 2, button_width, button_height);
	ftk_widget_set_text(button, "Male");
	ftk_check_button_set_icon_position(button, 1);
	ftk_check_button_set_clicked_listener(button, button_clicked, win);

	button = ftk_check_button_create_radio(group, button_width + 20, 
		(group_height - button_height) / 2, button_width, button_height);
	ftk_widget_set_text(button, "Female");
	ftk_check_button_set_icon_position(button, 1);
	ftk_check_button_set_clicked_listener(button, button_clicked, win);
	
	ftk_widget_set_text(win, "check button demo");
	ftk_widget_show_all(win, 1);
	FTK_QUIT_WHEN_WIDGET_CLOSE(win);

	FTK_RUN();

	return 0;
}

