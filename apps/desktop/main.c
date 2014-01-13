/*
 * File: main.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   desktop main
 *
 * Copyright (c) 2009 - 2010  Li XianJing <xianjimli@hotmail.com>
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * History:
 * ================================================================
 * 2009-11-29 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk.h"
#include <time.h>
#include "ftk_xul.h"
#include "ftk_util.h"
#include "app_info.h"
#include "ftk_tester.h"
#include "vnc_service.h"

typedef struct _FtkDesktop
{
	int is_horizonal;
	FtkWidget*      applist_win;
	AppInfoManager* app_manager;
	FtkIconCache*   icon_cache;
}FtkDesktop;

static FtkDesktop g_desktop;

static Ret desktop_on_button_close_applist_clicked(void* ctx, void* obj)
{
	ftk_widget_show(ctx, 0);

	return RET_OK;
}

static const char* desktop_translate_text(void* ctx, const char* text)
{
	return _(text);
}

static const char* desktop_translate_path(const char* path, char out_path[FTK_MAX_PATH+1])
{
	ftk_snprintf(out_path, FTK_MAX_PATH, "%s/desktop/%s", FTK_ROOT_DIR, path);
	ftk_normalize_path(out_path);

	ftk_logd("%s: %s --> %s\n", __func__, path, out_path);

	return out_path;
}

static FtkWidget* desktop_load_xul(const char* filename)
{
	FtkXulCallbacks callbacks = {0};
	char path[FTK_MAX_PATH+1] = {0};
	
	callbacks.ctx = g_desktop.icon_cache;
	callbacks.translate_text = desktop_translate_text;
	callbacks.load_image = (FtkXulLoadImage)ftk_icon_cache_load;

	desktop_translate_path(filename, path);

	return ftk_xul_load_file(path, &callbacks);
}

static Ret applist_on_item_clicked(void* ctx, void* obj)
{
	FtkIconViewItem* item = obj;
	AppInfo* info = item->user_data;

	ftk_app_run(info->app, 0, NULL);

	ftk_logd("%s: %s: user_data=%d\n", __func__, item->text, (int)item->user_data);

	return RET_OK;
}

static Ret desktop_on_button_open_applist_clicked(void* ctx, void* obj)
{
	size_t i = 0;
	size_t n = 0;
	size_t item_size = 100;
	FtkWidget* win = NULL;
	AppInfo* app_info = NULL;
	FtkWidget* button = NULL;
	FtkIconViewItem item;
	FtkWidget* icon_view = NULL;
	
	if(g_desktop.applist_win != NULL)
	{
		ftk_widget_show_all(g_desktop.applist_win, 1);

		return RET_OK;
	}

	win = desktop_load_xul(g_desktop.is_horizonal ? "xul/appview-h.xul" : "xul/appview-v.xul"); 
	
	button = ftk_widget_lookup(win, 100);
	ftk_button_set_clicked_listener(button, desktop_on_button_close_applist_clicked, win);

	icon_view = ftk_widget_lookup(win, 99);

	item_size = 6 * ftk_widget_get_font_size(icon_view);
	if(ftk_widget_width(icon_view) < 2 * item_size)
	{
		item_size = (ftk_widget_width(icon_view) - 10) / 2;
	}

	ftk_icon_view_set_item_size(icon_view, item_size);
	ftk_icon_view_set_clicked_listener(icon_view, applist_on_item_clicked, win);
	n = app_info_manager_get_count(g_desktop.app_manager);
	
	for(i = 0; i < n; i++)
	{
		app_info_manager_get(g_desktop.app_manager, i, &app_info);
		
		item.icon = ftk_app_get_icon(app_info->app);
		item.user_data = app_info;
		item.text = (char*)ftk_app_get_name(app_info->app);
		if(item.text == NULL)
		{
			item.text = app_info->name;
		}
		ftk_icon_view_add(icon_view, &item);
	}

	g_desktop.applist_win = win;

	return RET_OK;
}

#define IDC_TIME_ITEM 2000

static Ret desktop_update_time(void* ctx)
{
	char text[10] = {0};
	time_t now = time(0);
	FtkWidget* item = NULL;
	FtkWidget* panel = NULL;
	FtkWidget* win = ctx;
	FtkWidget* image = NULL;
	FtkBitmap* bitmap = NULL;
	char filename[FTK_MAX_PATH] = {0};
	struct tm* t = localtime(&now);

	panel = ftk_default_status_panel();
	if(panel == NULL)
	{
		return RET_REMOVE;
	}

	ftk_snprintf(text, sizeof(text)-1, "%d:%02d", t->tm_hour, t->tm_min);
	item = ftk_widget_lookup(panel, IDC_TIME_ITEM);
	ftk_widget_set_text(item, text);
	ftk_logd("%s\n", __func__);
	
	image = ftk_widget_lookup(win, 1);
	ftk_snprintf(filename, sizeof(filename)-1, "icons/%d.png", t->tm_hour/10);
	bitmap = ftk_icon_cache_load(g_desktop.icon_cache, filename);
	ftk_image_set_image(image, bitmap);
	
	image = ftk_widget_lookup(win, 2);
	ftk_snprintf(filename, sizeof(filename)-1, "icons/%d.png", t->tm_hour%10);
	bitmap = ftk_icon_cache_load(g_desktop.icon_cache, filename);
	ftk_image_set_image(image, bitmap);

	image = ftk_widget_lookup(win, 4);
	ftk_snprintf(filename, sizeof(filename)-1, "icons/%d.png", t->tm_min/10);
	bitmap = ftk_icon_cache_load(g_desktop.icon_cache, filename);
	ftk_image_set_image(image, bitmap);
	
	image = ftk_widget_lookup(win, 5);
	ftk_snprintf(filename, sizeof(filename)-1, "icons/%d.png", t->tm_min%10);
	bitmap = ftk_icon_cache_load(g_desktop.icon_cache, filename);
	ftk_image_set_image(image, bitmap);

	return RET_OK;
}

static Ret desktop_on_shutdown(void* ctx, void* obj)
{
#ifdef USE_VNC
	if(ftk_display_vnc_is_active())
	{
		ftk_display_vnc_quit();
	}
#endif
	ftk_quit();

	return RET_OK;
}

#ifdef USE_VNC
static Ret desktop_on_vnc(void* ctx, void* obj)
{
	if(ftk_display_vnc_is_active())
	{
		ftk_display_vnc_stop();
	}
	else
	{
		ftk_display_vnc_start();
	}
	
	return RET_OK;
}
#endif

static Ret desktop_on_prepare_options_menu(void* ctx, FtkWidget* menu_panel)
{
	FtkWidget* item = ftk_menu_item_create(menu_panel);
	ftk_widget_set_text(item, _("Shutdown"));
	ftk_menu_item_set_clicked_listener(item, desktop_on_shutdown, ctx);
	ftk_widget_show(item, 1);

#ifdef USE_VNC
	item = ftk_menu_item_create(menu_panel);
	if(ftk_display_vnc_is_active())
	{
		ftk_widget_set_text(item, _("Stop VNC"));
	}
	else
	{
		ftk_widget_set_text(item, _("Start VNC"));
	}
	ftk_menu_item_set_clicked_listener(item, desktop_on_vnc, ctx);
	ftk_widget_show(item, 1);
#endif

	return	RET_OK;
}

static Ret desktop_add_time_item_on_statusbar(void)
{
	FtkWidget* item = NULL;
	FtkWidget* panel = NULL;

	panel = ftk_default_status_panel();
	if(panel != NULL)
	{
		item = ftk_status_item_create(panel, -2, 60);
		ftk_widget_set_id(item, IDC_TIME_ITEM);
		ftk_widget_show(item, 1);
	}

	ftk_logd("%s\n", __func__);
	return RET_OK;
}

static void desktop_destroy(void* data)
{
	ftk_icon_cache_destroy(g_desktop.icon_cache);
	app_info_manager_destroy(g_desktop.app_manager);

	return;
}

int FTK_MAIN(int argc, char* argv[])
{
	int i = 0;
	FtkWidget* win = NULL;
	FtkWidget* button = NULL;
	FtkSource* timer = NULL;
	char path[FTK_MAX_PATH] = {0};
	const char* root_path[FTK_ICON_PATH_NR] = {0};

	ftk_init(argc, argv);

	for(i = 0; i < argc; i++)
	{
		const char* key_play="--event-play=";
		const char* key_record="--event-record=";
#ifdef FTK_HAS_TESTER
		if(strncmp(argv[i], key_play, strlen(key_play)) == 0)
		{
			ftk_tester_start_play(argv[i] + strlen(key_play));	
		}
		
		if(strncmp(argv[i], key_record, strlen(key_record)) == 0)
		{
			ftk_tester_start_record(argv[i] + strlen(key_record));	
		}
#endif
	}

#ifdef ENABLE_NLS
	if(getenv("LANG") == NULL)
	{
		setenv("LANG", "zh_CN.UTF-8", 1);
		setlocale (LC_ALL, "zh_CN.UTF-8");
		ftk_logd("LANG is not set, use zh_CN.UTF-8\n");
	}
	else
	{
		setlocale (LC_ALL, "");
	}

	bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
	textdomain (PACKAGE); 
	ftk_logd("%s: locale=%s\n", _("Hello, GetText"), setlocale(LC_ALL, NULL));
#endif

	desktop_add_time_item_on_statusbar();

	if(argv[1] != NULL && strncmp(argv[1], "--hor", 5) == 0)
	{
		g_desktop.is_horizonal = 1;
	}

	g_desktop.app_manager = app_info_manager_create();

	ftk_snprintf(path, sizeof(path), "%s/base/apps", FTK_ROOT_DIR);
	ftk_normalize_path(path);
	if(app_info_manager_load_dir(g_desktop.app_manager, path) != RET_OK)
	{
		ftk_snprintf(path, sizeof(path), "%s/apps", LOCAL_DATA_DIR);
		app_info_manager_load_dir(g_desktop.app_manager, path);
	}

	ftk_snprintf(path, sizeof(path), "%s/desktop", FTK_ROOT_DIR);
	root_path[0] = path;
	root_path[1] = NULL;

	g_desktop.icon_cache = ftk_icon_cache_create(root_path, NULL);
	win = desktop_load_xul(g_desktop.is_horizonal ? "xul/desktop-h.xul" : "xul/desktop-v.xul"); 
	ftk_app_window_set_on_prepare_options_menu(win, desktop_on_prepare_options_menu, win);
	button = ftk_widget_lookup(win, 100);
	ftk_button_set_clicked_listener(button, desktop_on_button_open_applist_clicked, win);
	ftk_widget_show_all(win, 1);

	desktop_update_time(win);
	timer = ftk_source_timer_create(60000, desktop_update_time, win);
	ftk_main_loop_add_source(ftk_default_main_loop(), timer);
	ftk_widget_set_user_data(win, desktop_destroy, &g_desktop);

	ftk_run();

	return 0;
}
