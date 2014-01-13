
#ifndef LUA_FTK_CALLBACKS_H
#define LUA_FTK_CALLBACKS_H

Ret lua_ftk_listener_func(void* ctx, void* obj);
int lua_ftk_compare_func(const void* obj1, const void* obj2);
Ret lua_ftk_on_event_func(void* ctx, FtkEvent* event);
Ret  lau_ftk_widget_on_event_func(FtkWidget* thiz, FtkEvent* event);
Ret lua_ftk_prepare_options_menu_func(void* ctx, FtkWidget* menu_panel);
Ret lua_ftk_list_item_listener_func(void* ctx, void* obj);
Ret lua_ftk_icon_view_item_listener_func(void* ctx, void* obj);
Ret lua_ftk_widget_listener_func(void* ctx, void* obj);
Ret lua_ftk_event_listener_func(void* ctx, void* obj);
Ret lua_ftk_file_browser_on_choosed_func(void* ctx, int index, const char* path);

void lua_ftk_destroy_func(void* ctx);
Ret lua_ftk_idle_func(void* ctx);
Ret lua_ftk_timer_func(void* ctx);

void lua_callbacks_init(lua_State* L);

#endif/*LUA_FTK_CALLBACKS_H*/
