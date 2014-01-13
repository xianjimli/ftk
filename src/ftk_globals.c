/*
 * File: ftk_globals.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: some global variables.  
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

#include "ftk_globals.h"

struct _FtkGlobals
{
	FtkCanvas* canvas;
	FtkTheme* theme;
	FtkDisplay* display;
	FtkMainLoop* main_loop;
	FtkLogLevel  log_level;
	FtkWidget*   status_panel;
	FtkConfig*   config;
	FtkSource*   primary_source;
	FtkAllocator* allocator;
	FtkWndManager* wnd_manager;
	FtkTextLayout* text_layout;
	FtkBitmapFactory* bitmap_factory;
	FtkSourcesManager* sources_manager;
	FtkImPreeditor* input_method_preeditor;
	FtkAnimationTrigger* animation_trigger;
	FtkInputMethodManager* input_manager_manager;
}g_globals;

FtkAnimationTrigger* ftk_default_animation_trigger(void)
{
	return g_globals.animation_trigger;
}

FtkDisplay*       ftk_default_display(void)
{
	return g_globals.display;
}

FtkCanvas*         ftk_shared_canvas(void)
{
	return g_globals.canvas;
}

FtkTheme*          ftk_default_theme(void)
{
	return g_globals.theme;
}

FtkSource*         ftk_primary_source(void)
{
	return g_globals.primary_source;
}

FtkConfig*         ftk_default_config(void)
{
	return g_globals.config;
}

FtkAllocator*      ftk_default_allocator(void)
{
	return g_globals.allocator;
}

FtkTextLayout*     ftk_default_text_layout(void)
{
	return g_globals.text_layout;
}

FtkImPreeditor* ftk_default_input_method_preeditor(void)
{
	return g_globals.input_method_preeditor;
}

FtkInputMethodManager* ftk_default_input_method_manager(void)
{
	return g_globals.input_manager_manager;
}

FtkMainLoop*      ftk_default_main_loop(void)
{
	return g_globals.main_loop;
}

FtkLogLevel       ftk_default_log_level(void)
{
	return g_globals.log_level;
}

FtkWndManager*    ftk_default_wnd_manager(void)
{
	return g_globals.wnd_manager;
}

FtkWidget*        ftk_default_status_panel(void)
{
	return g_globals.status_panel;
}

FtkBitmapFactory* ftk_default_bitmap_factory(void)
{
	return g_globals.bitmap_factory;
}

FtkSourcesManager* ftk_default_sources_manager(void)
{
	return g_globals.sources_manager;
}

void ftk_set_animation_trigger(FtkAnimationTrigger* animation_trigger)
{
	g_globals.animation_trigger = animation_trigger;

	return;
}

void ftk_set_display(FtkDisplay* display)
{
	g_globals.display = display;

	return;
}

void ftk_set_main_loop(FtkMainLoop* main_loop)
{
	g_globals.main_loop = main_loop;

	return;
}

void ftk_set_log_level(FtkLogLevel level)
{
	g_globals.log_level = level;

	return;
}

void ftk_set_status_panel(FtkWidget* status_panel)
{
	g_globals.status_panel = status_panel;

	return;
}

void ftk_set_wnd_manager(FtkWndManager* wnd_manager)
{
	g_globals.wnd_manager = wnd_manager;

	return;
}

void ftk_set_bitmap_factory(FtkBitmapFactory* bitmap_factory)
{
	g_globals.bitmap_factory = bitmap_factory;

	return;
}

void ftk_set_sources_manager(FtkSourcesManager* sources_manager)
{
	g_globals.sources_manager = sources_manager;

	return;
}

void ftk_set_shared_canvas(FtkCanvas* canvas)
{
	g_globals.canvas = canvas;

	return;
}

void ftk_set_theme(FtkTheme* theme)
{
	g_globals.theme = theme;

	return;
}

void ftk_set_primary_source(FtkSource* source)
{
	g_globals.primary_source = source;

	return;
}

void ftk_set_config(FtkConfig* config)
{
	g_globals.config = config;

	return;
}

void ftk_set_allocator(FtkAllocator* allocator)
{
	g_globals.allocator = allocator;

	return;
}

void ftk_set_text_layout(FtkTextLayout* text_layout)
{
	g_globals.text_layout = text_layout;

	return;
}

void ftk_set_input_method_preeditor(FtkImPreeditor* input_method_preeditor)
{
	g_globals.input_method_preeditor = input_method_preeditor;

	return;
}

void ftk_set_input_method_manager(FtkInputMethodManager* input_manager_manager)
{
	g_globals.input_manager_manager = input_manager_manager;

	return;
}

void ftk_clear_globals(void)
{
	memset(&g_globals, 0, sizeof(g_globals));

	return;
}
