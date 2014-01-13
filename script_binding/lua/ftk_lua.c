/*
 * File: ftk_lua.h    
 * Author: Li XianJing <xianjimli@hotmail.com>
 * Brief:  lua binding. 
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
 * 2009-11-22 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "ftk.h"
#include "ftk_xul.h"
#include "ftk_lua.h"

#include "lua_ftk_xul.h"
#include "lua_ftk_structs.h"
#include "lua_ftk_widget.h"
#include "lua_ftk_window.h"
#include "lua_ftk_label.h"
#include "lua_ftk_entry.h"
#include "lua_ftk_button.h"
#include "lua_ftk_typedef.h"
#include "lua_ftk_app_window.h"
#include "lua_ftk_source.h"
#include "lua_ftk_source_idle.h"
#include "lua_ftk_source_timer.h"
#include "lua_ftk_main_loop.h"
#include "lua_ftk_combo_box.h"
#include "lua_ftk_dialog.h"
#include "lua_ftk.h"
#include "lua_ftk_callbacks.h"
#include "lua_ftk_theme.h"
#include "lua_ftk_bitmap_factory.h"
#include "lua_ftk_bitmap.h"

#include "lua_ftk_file_browser.h"
#include "lua_ftk_fs.h"
#include "lua_ftk_dir.h"
#include "lua_ftk_file.h"
#include "lua_ftk_check_button.h"
#include "lua_ftk_icon_view.h"
#include "lua_ftk_menu_item.h"
#include "lua_ftk_menu_panel.h"
#include "lua_ftk_painter.h"
#include "lua_ftk_progress_bar.h"
#include "lua_ftk_group_box.h"
#include "lua_ftk_scroll_bar.h"
#include "lua_ftk_text_view.h"
#include "lua_ftk_status_item.h"
#include "lua_ftk_status_panel.h"
#include "lua_ftk_wait_box.h"
#include "lua_ftk_image.h"
#include "lua_ftk_popup_menu.h"
#include "lua_ftk_mmap.h"
#include "lua_ftk_clipboard.h"
#include "lua_ftk_display.h"
#include "lua_ftk_icon_cache.h"
#include "lua_ftk_list_view.h"
#include "lua_ftk_list_model_default.h"
#include "lua_ftk_list_model.h"
#include "lua_ftk_list_render_default.h"
#include "lua_ftk_list_render.h"
#include "lua_ftk_sprite.h"
#include "lua_ftk_config.h"
#include "lua_ftk_wnd_manager.h"

int ftk_lua_init(lua_State *L) 
{
	tolua_ftk_init(L);
	tolua_ftk_fs_init(L);
	tolua_ftk_dir_init(L);
	tolua_ftk_file_init(L);
	tolua_ftk_file_browser_init(L);
	tolua_ftk_widget_init(L);
	tolua_ftk_window_init(L);
	tolua_ftk_xul_init(L);
	tolua_ftk_app_window_init(L);
	tolua_ftk_label_init(L);
	tolua_ftk_button_init(L);
	tolua_ftk_entry_init(L);
	tolua_ftk_source_init(L);
	tolua_ftk_source_idle_init(L);
	tolua_ftk_source_timer_init(L);
	tolua_ftk_main_loop_init(L);
	tolua_ftk_typedef_init(L);
	tolua_ftk_combo_box_init(L);
	tolua_ftk_dialog_init(L);
	tolua_ftk_theme_init(L);
	tolua_ftk_bitmap_factory_init(L);
	tolua_ftk_bitmap_init(L);
	
	tolua_ftk_check_button_init(L);
	tolua_ftk_icon_view_init(L);
	tolua_ftk_image_init(L);
	tolua_ftk_menu_item_init(L);
	tolua_ftk_menu_panel_init(L);
	tolua_ftk_painter_init(L);
	tolua_ftk_progress_bar_init(L);
	tolua_ftk_group_box_init(L);
	tolua_ftk_scroll_bar_init(L);
	tolua_ftk_status_item_init(L);
	tolua_ftk_status_panel_init(L);
	tolua_ftk_text_view_init(L);
	tolua_ftk_wait_box_init(L);
	tolua_ftk_popup_menu_init(L);
	tolua_ftk_clipboard_init(L);
	tolua_ftk_display_init(L);
	tolua_ftk_icon_cache_init(L);

	tolua_ftk_list_model_default_init(L);
	tolua_ftk_list_model_init(L);
	tolua_ftk_list_render_default_init(L);
	tolua_ftk_list_render_init(L);
	tolua_ftk_list_view_init(L);
	tolua_ftk_mmap_init(L);
	tolua_ftk_sprite_init(L);
	tolua_ftk_config_init(L);
	tolua_ftk_wnd_manager_init(L);
	tolua_ftk_structs_init(L);

	lua_callbacks_init(L);

	return 1;
}


