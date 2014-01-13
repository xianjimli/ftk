/*
 * File: ftk.h    
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

#ifndef FTK_H
#define FTK_H

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

#include "ftk_log.h"
#include "ftk_mmap.h"
#include "ftk_bitmap.h"
#include "ftk_dialog.h"
#include "ftk_display.h"
#include "ftk_allocator.h"
#include "ftk_app_window.h"
#include  "ftk_icon_cache.h"
#include "ftk_status_item.h"
#include "ftk_progress_bar.h"
#include "ftk_source_timer.h"
#include "ftk_popup_menu.h"
#include "ftk_list_view.h"
#include "ftk_menu_item.h"
#include "ftk_globals.h"
#include "ftk_window.h"
#include "ftk_label.h"
#include "ftk_image.h"
#include "ftk_entry.h"
#include "ftk_button.h"
#include "ftk_painter.h"
#include "ftk_wait_box.h"
#include "ftk_combo_box.h"
#include "ftk_list_view.h"
#include "ftk_text_view.h"
#include "ftk_menu_panel.h"
#include "ftk_source_idle.h"
#include "ftk_status_panel.h"
#include "ftk_allocator_default.h"
#include "ftk_list_model_default.h"
#include "ftk_list_render_default.h"
#include "ftk_check_button.h"
#include "ftk_file_system.h"
#include "ftk_file_browser.h"
#include "ftk_message_box.h"
#include "ftk_group_box.h"
#include "ftk_scroll_bar.h"
#include "ftk_icon_view.h"
#include "ftk_clipboard.h"
#include "ftk_sprite.h"
#include "ftk_tab.h"

FTK_BEGIN_DECLS

Ret  ftk_init(int argc, char* argv[]);
Ret  ftk_run(void);
void ftk_quit(void);
void ftk_deinit(void);

FTK_END_DECLS

#ifdef __cplusplus
}
#endif/*__cplusplus*/

#endif/*FTK_H*/

