#include "ftk.h"
#include "ftk_util.h"
#include "ftk_key_board.h"

#ifdef FTK_AS_PLUGIN
#include "ftk_app_demo.h"
FTK_HIDE int FTK_MAIN(int argc, char* argv[]);
FtkApp* ftk_app_demo_keyboard_create()
{
	return ftk_app_demo_create(_("keyboard"), ftk_main);
}
#else
#define FTK_HIDE extern
#endif /*FTK_AS_PLUGIN*/

FTK_HIDE int FTK_MAIN(int argc, char* argv[])
{
	size_t w = 0;
	size_t h = 0;
	FtkWidget* win = NULL;
	FtkWidget* entry = NULL;
	FtkWidget* key_board = NULL;
	char filename[FTK_MAX_PATH+1] = {0};

	FTK_INIT(argc, argv);
	win = ftk_app_window_create();
	ftk_window_set_animation_hint(win, "app_main_window");
	ftk_widget_set_text(win, "KeyBoard");

	w = ftk_widget_width(win);
	h = ftk_widget_height(win);

	ftk_input_method_manager_set_current(ftk_default_input_method_manager(), -1);
	entry = ftk_entry_create(win, 10, 30, w - 20, 30);

	ftk_strs_cat(filename, FTK_MAX_PATH, 
		ftk_config_get_data_dir(ftk_default_config()), "/theme/", "default", "/keyboard.xml", NULL);
	key_board = ftk_key_board_create(win, 0, h-180, w, 180);
	ftk_key_board_load(key_board, filename);
	ftk_key_board_reset_candidates(key_board);
	ftk_key_board_add_candidate(key_board, "我");
	ftk_key_board_add_candidate(key_board, "是");
	ftk_key_board_add_candidate(key_board, "中国");
	ftk_key_board_add_candidate(key_board, "人");
	ftk_key_board_add_candidate(key_board, "计算机");
	ftk_key_board_add_candidate(key_board, "程序员");
	ftk_key_board_set_editor(key_board, entry);

	ftk_widget_show_all(win, 1);
	FTK_QUIT_WHEN_WIDGET_CLOSE(win);

	FTK_RUN();

	return 0;
}

