include $(abs_top_builddir)/src/rules.mk

SUBDIRS=lua
lib_LTLIBRARIES=libftklua.la
libftklua_la_SOURCES=ftk_lua.c tolua_event.c  tolua_is.c  tolua_map.c  tolua_push.c  tolua_to.c \
	lua_ftk.c lua_ftk_typedef.c lua_ftk_widget.c lua_ftk_window.c lua_ftk_xul.c lua_ftk_callbacks.c \
	lua_ftk_app_window.c lua_ftk_label.c lua_ftk_button.c lua_ftk_entry.c \
	lua_ftk_source_timer.c lua_ftk_main_loop.c lua_ftk_source_idle.c lua_ftk_source.c  \
	lua_ftk_combo_box.c lua_ftk_dialog.c lua_ftk_theme.c lua_ftk_bitmap.c \
	lua_ftk_bitmap_factory.c lua_ftk_check_button.c lua_ftk_icon_view.c lua_ftk_image.c \
	lua_ftk_menu_item.c lua_ftk_menu_panel.c lua_ftk_painter.c \
	lua_ftk_progress_bar.c lua_ftk_group_box.c lua_ftk_scroll_bar.c\
	lua_ftk_status_item.c lua_ftk_status_panel.c lua_ftk_text_view.c lua_ftk_wait_box.c \
	lua_ftk_popup_menu.c \
	lua_ftk_mmap.c lua_ftk_clipboard.c lua_ftk_display.c \
	lua_ftk_icon_cache.c lua_ftk_list_view.c lua_ftk_list_model_default.c lua_ftk_list_model.c\
	lua_ftk_list_render_default.c lua_ftk_list_render.c lua_ftk_sprite.c \
	lua_ftk_wnd_manager.c lua_ftk_config.c  lua_ftk_structs.c lua_ftk_file.c lua_ftk_fs.c lua_ftk_dir.c \
	lua_ftk_file_browser.c

libftklua_la_LDFLAGS=$(MYLDFLAGS) $(abs_top_builddir)/script_binding/lua/lua/liblua.la 

if FTK_BACKEND_PSP
libftklua_la_CFLAGS=-DLUA_USE_PSP
else
libftklua_la_CFLAGS=-DLUA_USE_LINUX
libftklua_la_LDFLAGS+=-ldl
endif
libftklua_la_CFLAGS+=-I$(srcdir)/lua

bin_PROGRAMS=ftk_run
ftk_run_SOURCES=lua_runner.c 
ftk_run_LDFLAGS=$(MYLDFLAGS) -L$(abs_top_builddir)/script_binding/lua/ -lftklua \
	 $(abs_top_builddir)/script_binding/lua/lua/liblua.la

if FTK_BACKEND_PSP
ftk_run_CFLAGS=-DLUA_USE_PSP
else
ftk_run_CFLAGS=-DLUA_USE_LINUX
ftk_run_LDFLAGS+=-ldl
endif
ftk_run_CFLAGS+=-I$(srcdir)/lua

EXTRA_DIST=\
	demo1.lua\
	demo2.lua\
	demo3.lua\
	demo4.lua\
	demo_button.lua\
	demo_check_button.lua\
	demo_combo_box.lua\
	demo_dialog.lua\
	demo_entry.lua\
	demo_fullscreen.lua\
	demo_icon_view.lua\
	demo_label.lua\
	demo_list_view.lua\
	demo_menu.lua\
	demo_msgbox.lua\
	demo_popup.lua\
	demo_progressbar.lua\
	demo_scrollbar.lua\
	demo_text_view.lua\
	demo_wait_box.lua\
	ftk_lua.h\
	lua_ftk_app_window.h\
	lua_ftk_bitmap_factory.h\
	lua_ftk_bitmap.h\
	lua_ftk_button.h\
	lua_ftk_callbacks.h\
	lua_ftk_check_button.h\
	lua_ftk_clipboard.h\
	lua_ftk_combo_box.h\
	lua_ftk_dialog.h\
	lua_ftk_display.h\
	lua_ftk_entry.h\
	lua_ftk_font_default.h\
	lua_ftk_font_free_type.h\
	lua_ftk_font.h\
	lua_ftk.h\
	lua_ftk_icon_cache.h\
	lua_ftk_icon_view.h\
	lua_ftk_image.h\
	lua_ftk_label.h\
	lua_ftk_list_model_default.h\
	lua_ftk_list_model.h\
	lua_ftk_list_render_default.h\
	lua_ftk_list_render.h\
	lua_ftk_list_view.h\
	lua_ftk_main_loop.h\
	lua_ftk_menu_item.h\
	lua_ftk_menu_panel.h\
	lua_ftk_mmap.h\
	lua_ftk_painter.h\
	lua_ftk_popup_menu.h\
	lua_ftk_progress_bar.h\
	lua_ftk_group_box.h\
	lua_ftk_scroll_bar.h\
	lua_ftk_source.h\
	lua_ftk_source_idle.h\
	lua_ftk_source_timer.h\
	lua_ftk_sprite.h\
	lua_ftk_status_item.h\
	lua_ftk_status_panel.h\
	lua_ftk_text_view.h\
	lua_ftk_theme.h\
	lua_ftk_translator.h\
	lua_ftk_typedef.h\
	lua_ftk_wait_box.h\
	lua_ftk_widget.h\
	lua_ftk_window.h\
	lua_ftk_wnd_manager.h\
	lua_ftk_xul.h\
	tolua_event.h\
	tolua++.h \
	lua_ftk_config.h \
	lua_ftk_structs.h

