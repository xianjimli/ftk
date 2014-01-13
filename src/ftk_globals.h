/*
 * File: ftk_globals.h    
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

#ifndef FTK_GLOBALS_H
#define FTK_GLOBALS_H

#include "ftk_theme.h"
#include "ftk_config.h"
#include "ftk_source.h"
#include "ftk_display.h"
#include "ftk_allocator.h"
#include "ftk_text_layout.h"
#include "ftk_wnd_manager.h"
#include "ftk_bitmap_factory.h"
#include "ftk_sources_manager.h"
#include "ftk_input_method_manager.h"
#include "ftk_input_method_preeditor.h"
#include "ftk_animation_trigger.h"

FTK_BEGIN_DECLS

FtkDisplay*       ftk_default_display(void);
FtkMainLoop*      ftk_default_main_loop(void);
FtkLogLevel       ftk_default_log_level(void);
FtkWndManager*    ftk_default_wnd_manager(void);
FtkWidget*        ftk_default_status_panel(void);
FtkBitmapFactory* ftk_default_bitmap_factory(void);
FtkSourcesManager* ftk_default_sources_manager(void);
FtkCanvas*         ftk_shared_canvas(void);
FtkTheme*          ftk_default_theme(void);
FtkSource*         ftk_primary_source(void);
FtkConfig*         ftk_default_config(void);
FtkAllocator*      ftk_default_allocator(void);
FtkTextLayout*     ftk_default_text_layout(void);
FtkInputMethodManager* ftk_default_input_method_manager(void);
FtkImPreeditor*        ftk_default_input_method_preeditor(void);
FtkAnimationTrigger*   ftk_default_animation_trigger(void);

void ftk_set_display(FtkDisplay* display);
void ftk_set_main_loop(FtkMainLoop* main_loop);
void ftk_set_log_level(FtkLogLevel level);
void ftk_set_status_panel(FtkWidget* status_panel);
void ftk_set_wnd_manager(FtkWndManager* wnd_manager);
void ftk_set_bitmap_factory(FtkBitmapFactory* bitmap_factory);
void ftk_set_sources_manager(FtkSourcesManager* sources_manager);
void ftk_set_shared_canvas(FtkCanvas* canvas);
void ftk_set_theme(FtkTheme* theme);
void ftk_set_primary_source(FtkSource* source);
void ftk_set_config(FtkConfig* config);
void ftk_set_text_layout(FtkTextLayout* text_layout);
void ftk_set_allocator(FtkAllocator* allocator);
void ftk_set_input_method_manager(FtkInputMethodManager* input_manager_manager);
void ftk_set_input_method_preeditor(FtkImPreeditor* input_method_preeditor);
void ftk_set_animation_trigger(FtkAnimationTrigger* animation_trigger);

void ftk_clear_globals(void);

FtkFontDesc* ftk_default_font();

FTK_END_DECLS

#endif/*FTK_GLOBALS_H*/

