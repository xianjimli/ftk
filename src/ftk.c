/*
 * File: ftk.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: ftk global init, mainloop and deinit functions.  
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
 * 2009-10-03 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk.h"
#include "ftk_util.h"
#include "ftk_backend.h"
#include "ftk_globals.h"
#include "ftk_main_loop.h"
#include "ftk_status_item.h"
#include "ftk_status_panel.h"
#include "ftk_bitmap_factory.h"
#include "ftk_display_rotate.h"
#include "ftk_allocator_default.h"
#include "ftk_wnd_manager_default.h"
#include "ftk_input_method_preeditor_default.h"

#ifdef FTK_MEMORY_PROFILE
#include "ftk_allocator_profile.h"
#define FTK_ALLOC_PROFILE(a) ftk_allocator_profile_create(a)
#else
#define FTK_ALLOC_PROFILE(a) a
#endif

#define quit_if_fail(p, msg) \
	if(!(p))\
	{\
		ftk_deinit();\
		ftk_loge(msg);\
		exit(0);\
	}

static void ftk_init_panel(void);

static Ret ftk_init_bitmap_factory(void)
{
	ftk_set_bitmap_factory(ftk_bitmap_factory_create());
	quit_if_fail(ftk_default_bitmap_factory(), "Init bitmap factory failed.\n");

	return RET_OK;
}

static Ret ftk_init_theme(const char* theme)
{
	char filename[FTK_MAX_PATH + 1] = {0};

	ftk_set_theme(ftk_theme_create(theme == NULL));

	if(theme != NULL)
	{
#if !defined(ANDROID) && !defined(ANDROID_NDK)
		ftk_strs_cat(filename, FTK_MAX_PATH, 
			ftk_config_get_data_dir(ftk_default_config()), "/theme/", theme, "/theme.xml", NULL);
#else
		ftk_strs_cat(filename, FTK_MAX_PATH, 
			ftk_config_get_data_dir(ftk_default_config()), "/theme/", theme, "/theme.wav", NULL);
#endif
		ftk_normalize_path(filename);
		ftk_theme_parse_file(ftk_default_theme(), filename);
	}
	
	quit_if_fail(ftk_default_theme(), "Init theme failed.\n");
	ftk_set_animation_trigger(ftk_theme_get_animation_trigger(ftk_default_theme()));

	return RET_OK;
}

void ftk_deinit(void)
{
	if(ftk_default_input_method_preeditor() != NULL)
	{
		ftk_input_method_preeditor_destroy(ftk_default_input_method_preeditor());
	}

	if(ftk_default_wnd_manager() != NULL)
	{
		ftk_wnd_manager_destroy(ftk_default_wnd_manager());
	}
	
	if(ftk_default_main_loop() != NULL)
	{
		ftk_main_loop_destroy(ftk_default_main_loop());
	}

	if(ftk_default_sources_manager() != NULL)
	{
		ftk_sources_manager_destroy(ftk_default_sources_manager());
	}

	if(ftk_default_bitmap_factory() != NULL)
	{
		ftk_bitmap_factory_destroy(ftk_default_bitmap_factory());
	}

	if(ftk_default_text_layout() != NULL)
	{
		ftk_text_layout_destroy(ftk_default_text_layout());
	}

	if(ftk_default_display() != NULL)
	{
		ftk_display_destroy(ftk_default_display());
	}

	if(ftk_default_theme() != NULL)
	{
		ftk_theme_destroy(ftk_default_theme());
	}

	if(ftk_shared_canvas() != NULL)
	{
		ftk_canvas_destroy(ftk_shared_canvas());
	}

	if(ftk_default_input_method_manager() != NULL)
	{
		ftk_input_method_manager_destroy(ftk_default_input_method_manager());
	}

	if(ftk_default_config() != NULL)
	{
		ftk_config_destroy(ftk_default_config());
	}

	ftk_platform_deinit();

#ifndef USE_STD_MALLOC
	if(ftk_default_allocator() != NULL)
	{
		ftk_allocator_destroy(ftk_default_allocator());
	}
#endif

	ftk_logd("%s: ftk exit.\n", __func__);

	ftk_clear_globals();

	return;
}

#ifndef USE_LINUX_DFB
static Ret ftk_move_cursor(void* ctx, void* obj)
{
	FtkEvent* event = (FtkEvent*)obj;

	if(event->type == FTK_EVT_MOUSE_MOVE)
	{
		ftk_sprite_move((FtkSprite*)ctx, event->u.mouse.x, event->u.mouse.y);
	}
	else if(event->type == FTK_EVT_DISABLE_CURSOR)
	{
		ftk_sprite_show((FtkSprite*)ctx, 0);
	}
	else if(event->type == FTK_EVT_ENABLE_CURSOR)
	{
		ftk_sprite_show((FtkSprite*)ctx, 1);
	}

	return RET_OK;
}

static Ret ftk_enable_curosr(void)
{
	FtkSprite* sprite = ftk_sprite_create();
	FtkBitmap* icon = ftk_theme_load_image(ftk_default_theme(), "cursor"FTK_STOCK_IMG_SUFFIX);
	
	ftk_sprite_set_icon(sprite, icon);
	ftk_sprite_show(sprite, 1);
	ftk_wnd_manager_add_global_listener(ftk_default_wnd_manager(), ftk_move_cursor, sprite);
	
	return RET_OK;
}
#endif

Ret ftk_init(int argc, char* argv[])
{
	FtkColor bg = {0};
	FtkConfig* config = NULL;
	FtkDisplay* display = NULL;
	static int ftk_inited = 0;

	if(ftk_inited)
	{
		return RET_OK;
	}
	else
	{
		ftk_inited = 1;
	}

	ftk_clear_globals();
	PROFILE_START();
#ifndef USE_STD_MALLOC
	ftk_set_allocator(FTK_ALLOC_PROFILE(ftk_allocator_default_create()));
	quit_if_fail(ftk_default_allocator(), "Init allocator failed.\n");
#endif

	ftk_platform_init(argc, argv);
	config = ftk_config_create();
	ftk_set_config(config);
	ftk_config_init(config, argc, argv);
	quit_if_fail(ftk_default_config(), "Init config failed.\n");

	PROFILE_END("config init");

	ftk_set_text_layout(ftk_text_layout_create());
	PROFILE_START();
	ftk_set_sources_manager(ftk_sources_manager_create(64));
	ftk_set_main_loop(ftk_main_loop_create(ftk_default_sources_manager()));
	ftk_set_wnd_manager(ftk_wnd_manager_default_create(ftk_default_main_loop()));
	quit_if_fail(ftk_default_wnd_manager(), "Init windows manager failed.\n");
	PROFILE_END("source main loop wnd manager init");

	ftk_init_bitmap_factory();
	
	PROFILE_START();
	ftk_init_theme(ftk_config_get_theme(config));
	ftk_backend_init(argc, argv);
	PROFILE_END("theme and backend init");

	display = ftk_display_rotate_create(ftk_default_display(), ftk_config_get_rotate(ftk_default_config()));
	ftk_set_display(display);
	quit_if_fail(ftk_default_display(), "Init display failed.\n");

	PROFILE_START();
	bg.a = 0xff;
	ftk_set_shared_canvas(ftk_canvas_create(ftk_display_width(display), ftk_display_height(display), &bg));
	ftk_logd("canvas init: %d %d\n", ftk_display_width(display), ftk_display_height(display));
	PROFILE_END("canvas init");

	PROFILE_START();
	ftk_set_input_method_manager(ftk_input_method_manager_create());
	ftk_set_input_method_preeditor(ftk_input_method_preeditor_default_create());
	quit_if_fail(ftk_default_input_method_manager(), "Init input method failed.\n");
	PROFILE_END("input method init");

	PROFILE_START();
	if(ftk_config_get_enable_status_bar(config))
	{
		ftk_init_panel();
	}
	PROFILE_END("panel init");

	if(ftk_config_get_enable_cursor(config))
	{
#ifndef USE_LINUX_DFB
		ftk_enable_curosr();
#endif
	}

	return RET_OK;
}

Ret ftk_run(void)
{
	Ret ret = ftk_main_loop_run(ftk_default_main_loop());

	ftk_deinit();

	return ret;
}

void ftk_quit(void)
{
	if(ftk_default_main_loop() != NULL)
	{
		ftk_main_loop_quit(ftk_default_main_loop());
		ftk_logd("%s\n", __func__);
	}

	return;
}

#define IDC_TITLE_ITEM 1000
#define IDC_ICON_ITEM  1001
#define IDC_CLOSE_ITEM 1002

static Ret on_wnd_manager_global_event(void* ctx, void* obj)
{
	Ret ret = RET_OK;
	FtkEvent* event = (FtkEvent*)obj;
	FtkWidget* panel = NULL;
	FtkWidget* top_window = NULL;
	FtkWidget* close_widget = NULL;
	FtkWidget* title_widget = NULL;

	if(event->type != FTK_EVT_TOP_WND_CHANGED
		&& event->type != FTK_EVT_WND_CONFIG_CHANGED)
	{
		return RET_OK;
	}

	if(ftk_widget_type((FtkWidget*)event->widget) != FTK_WINDOW)
	{
		return RET_OK;
	}

	panel = ftk_default_status_panel();
	close_widget = ftk_widget_lookup(panel, IDC_CLOSE_ITEM);
	title_widget = ftk_widget_lookup(panel, IDC_TITLE_ITEM);
	switch(event->type)
	{
		case FTK_EVT_TOP_WND_CHANGED:
		{
			top_window = (FtkWidget*)event->widget;

			if(top_window != NULL)
			{
				ftk_widget_set_text(title_widget, ftk_widget_get_text(top_window));
				ftk_logd("top_window changed: %s\n", ftk_widget_get_text(top_window));
			}
			else
			{
				ftk_widget_set_text(title_widget, NULL);
			}
			ftk_widget_set_user_data(title_widget, NULL, top_window);
			ret = RET_REMOVE;

			break;
		}
		case FTK_EVT_WND_CONFIG_CHANGED:
		{
			top_window = (FtkWidget*)ftk_widget_user_data(title_widget);
			if(top_window == event->widget)
			{
				ftk_widget_set_text(title_widget, ftk_widget_get_text(top_window));
			}
			
			ftk_logd("%s: config changed: %p %p\n", __func__, top_window, event->widget);
			ret = RET_REMOVE;
			break;
		}
		default:break;
	}

	if(top_window != NULL && close_widget != NULL)
	{
		ftk_widget_show(close_widget, !ftk_widget_has_attr(top_window, FTK_ATTR_IGNORE_CLOSE));
	}

	return ret;
}

static Ret button_close_top_clicked(void* ctx, void* obj)
{
	FtkWidget* panel = ftk_default_status_panel();
	FtkWidget* title_widget = ftk_widget_lookup(panel, IDC_TITLE_ITEM);
	FtkWidget* top_window = (FtkWidget*)ftk_widget_user_data(title_widget);

	if(top_window != NULL && ftk_widget_type(top_window) == FTK_WINDOW)
	{
		if(!ftk_widget_has_attr(top_window, FTK_ATTR_IGNORE_CLOSE))
		{
			ftk_widget_set_user_data(title_widget, NULL, NULL);
			ftk_logd("%s: close window %s\n", __func__, ftk_widget_get_text(top_window));
			ftk_widget_unref(top_window);
		}
		else
		{
			ftk_logd("%s: the top window has attribute FTK_ATTR_IGNORE_CLOSE\n", __func__);
		}
	}
	else
	{
		ftk_logd("%s: not normal window.\n", __func__);
	}

	return RET_OK;
}

#define min(a,b) ((a) < (b) ? (a) : (b))

static void ftk_init_panel(void)
{
	FtkGc gc;
	FtkWidget* item = NULL;	
	FtkWidget* panel = ftk_status_panel_create(FTK_STATUS_PANEL_HEIGHT);
	size_t width = ftk_widget_width(panel);

	ftk_set_status_panel(panel);
	quit_if_fail(ftk_default_status_panel(), "Init status panel failed.\n");

	memset(&gc, 0x00, sizeof(gc));
	gc.mask   = FTK_GC_BITMAP;
	gc.bitmap = ftk_theme_load_image(ftk_default_theme(), "status-bg"FTK_STOCK_IMG_SUFFIX);
	ftk_widget_set_gc(panel, FTK_WIDGET_NORMAL, &gc);
	ftk_widget_set_gc(panel, FTK_WIDGET_ACTIVE, &gc);
	ftk_widget_set_gc(panel, FTK_WIDGET_FOCUSED, &gc);
	ftk_gc_reset(&gc);
	
	item = ftk_status_item_create(panel, -100, 32);
	ftk_widget_set_id(item, IDC_CLOSE_ITEM);
	gc.bitmap = ftk_theme_load_image(ftk_default_theme(), "close-32"FTK_STOCK_IMG_SUFFIX);
	if(gc.bitmap != NULL)
	{
		gc.mask = FTK_GC_BITMAP;
		ftk_widget_set_gc(item, FTK_WIDGET_NORMAL, &gc);
		ftk_widget_set_gc(item, FTK_WIDGET_FOCUSED, &gc);
		ftk_gc_reset(&gc);
		gc.mask = FTK_GC_BITMAP;
		gc.bitmap = ftk_theme_load_image(ftk_default_theme(), "close-pressed-32"FTK_STOCK_IMG_SUFFIX);
		ftk_widget_set_gc(item, FTK_WIDGET_ACTIVE, &gc);
		ftk_gc_reset(&gc);
	}
	ftk_status_item_set_clicked_listener(item, button_close_top_clicked, NULL);

	gc.bitmap = ftk_theme_load_image(ftk_default_theme(), "flag-32"FTK_STOCK_IMG_SUFFIX);
	item = ftk_status_item_create(panel, 1, gc.bitmap ? min(ftk_bitmap_width(gc.bitmap), 100) : 32);
	ftk_widget_set_id(item, IDC_ICON_ITEM);
	if(gc.bitmap != NULL)
	{
		gc.mask = FTK_GC_BITMAP;
		ftk_widget_set_gc(item, FTK_WIDGET_NORMAL, &gc);
		ftk_widget_set_gc(item, FTK_WIDGET_ACTIVE, &gc);
		ftk_widget_set_gc(item, FTK_WIDGET_FOCUSED, &gc);
		ftk_gc_reset(&gc);
	}
	ftk_widget_show(item, 1);

	item = ftk_status_item_create(panel, 2, width/2);
	ftk_widget_set_id(item, IDC_TITLE_ITEM);
	ftk_widget_show(item, 1);

	ftk_wnd_manager_add_global_listener(ftk_default_wnd_manager(), on_wnd_manager_global_event, NULL);
	ftk_widget_show(panel, 1);
	
	return;
}

