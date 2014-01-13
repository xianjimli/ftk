
#include "lua_ftk_typedef.h"
#include "lua_ftk_structs.h"

static void tolua_reg_types (lua_State* L)
{
	tolua_usertype(L, "FtkGc");
	tolua_usertype(L, "FtkEvent");
}

int lua_ftk_gc_create(lua_State* L)
{
	FtkGc* retv = calloc(1, sizeof(FtkGc));
	tolua_pushusertype_and_takeownership(L, (void*)retv, "FtkGc");

	return 1;
}

static int tolua_get_FtkGc_ref(lua_State* L)
{
	lua_Number retv;
	FtkGc* thiz = (FtkGc*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	retv = (lua_Number)thiz->ref;
	tolua_pushnumber(L, (lua_Number)retv);

	return 1;
}

static int tolua_set_FtkGc_ref(lua_State* L)
{
	FtkGc* thiz = (FtkGc*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	thiz->ref = (int) (tolua_tonumber(L, 2, 0));

	return 1;
}

static int tolua_get_FtkGc_mask(lua_State* L)
{
	lua_Number retv;
	FtkGc* thiz = (FtkGc*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	retv = (lua_Number)thiz->mask;
	tolua_pushnumber(L, (lua_Number)retv);

	return 1;
}

static int tolua_set_FtkGc_mask(lua_State* L)
{
	FtkGc* thiz = (FtkGc*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	thiz->mask = (int) (tolua_tonumber(L, 2, 0));

	return 1;
}

static int tolua_get_FtkGc_bg(lua_State* L)
{
	void* retv;
	FtkGc* thiz = (FtkGc*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	retv = (void*)&(thiz->bg);
	tolua_pushusertype(L, retv, "FtkColor");

	return 1;
}

static int tolua_set_FtkGc_bg(lua_State* L)
{
	FtkGc* thiz = (FtkGc*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	thiz->bg = (*(FtkColor*)tolua_tousertype(L, 2, 0));

	return 1;
}

static int tolua_get_FtkGc_fg(lua_State* L)
{
	void* retv;
	FtkGc* thiz = (FtkGc*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	retv = (void*)&(thiz->fg);
	tolua_pushusertype(L, retv, "FtkColor");

	return 1;
}

static int tolua_set_FtkGc_fg(lua_State* L)
{
	FtkGc* thiz = (FtkGc*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	thiz->fg = (*(FtkColor*)tolua_tousertype(L, 2, 0));

	return 1;
}

static int tolua_get_FtkGc_bitmap(lua_State* L)
{
	void* retv;
	FtkGc* thiz = (FtkGc*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	retv = (void*)thiz->bitmap;
	tolua_pushusertype(L, (FtkBitmap*)retv, "FtkBitmap");

	return 1;
}

static int tolua_set_FtkGc_bitmap(lua_State* L)
{
	FtkGc* thiz = (FtkGc*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	thiz->bitmap = (FtkBitmap*) (tolua_tousertype(L, 2, 0));

	return 1;
}

static int tolua_get_FtkGc_alpha(lua_State* L)
{
	lua_Number retv;
	FtkGc* thiz = (FtkGc*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	retv = thiz->alpha;
	tolua_pushnumber(L, (lua_Number)retv);

	return 1;
}

static int tolua_set_FtkGc_alpha(lua_State* L)
{
	FtkGc* thiz = (FtkGc*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	thiz->alpha = (char)tolua_tonumber(L, 2, 0);

	return 1;
}

static int tolua_get_FtkGc_line_mask(lua_State* L)
{
	lua_Number retv;
	FtkGc* thiz = (FtkGc*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	retv = (lua_Number)thiz->line_mask;
	tolua_pushnumber(L, (lua_Number)retv);

	return 1;
}

static int tolua_set_FtkGc_line_mask(lua_State* L)
{
	FtkGc* thiz = (FtkGc*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	thiz->line_mask = (int) (tolua_tonumber(L, 2, 0));

	return 1;
}

int lua_ftk_event_create(lua_State* L)
{
	FtkEvent* retv = calloc(1, sizeof(FtkEvent));
	tolua_pushusertype_and_takeownership(L, (void*)retv, "FtkEvent");

	return 1;
}

static int tolua_get_FtkEvent_type(lua_State* L)
{
	lua_Number retv;
	FtkEvent* thiz = (FtkEvent*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	retv = (lua_Number)thiz->type;
	tolua_pushnumber(L, (lua_Number)retv);

	return 1;
}

static int tolua_set_FtkEvent_type(lua_State* L)
{
	FtkEvent* thiz = (FtkEvent*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	thiz->type = tolua_tonumber(L, 2, 0);

	return 1;
}

static int tolua_get_FtkEvent_widget(lua_State* L)
{
	void* retv;
	FtkEvent* thiz = (FtkEvent*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	retv = (void*)thiz->widget;
	tolua_pushusertype(L, (FtkWidget*)retv, "FtkWidget");

	return 1;
}

static int tolua_set_FtkEvent_widget(lua_State* L)
{
	FtkEvent* thiz = (FtkEvent*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	thiz->widget = (FtkWidget*) (tolua_tousertype(L, 2, 0));

	return 1;
}

static int tolua_get_FtkEvent_time(lua_State* L)
{
	lua_Number retv;
	FtkEvent* thiz = (FtkEvent*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	retv = (lua_Number)thiz->time;
	tolua_pushnumber(L, (lua_Number)retv);

	return 1;
}

static int tolua_set_FtkEvent_time(lua_State* L)
{
	FtkEvent* thiz = (FtkEvent*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	thiz->time = (int) (tolua_tonumber(L, 2, 0));

	return 1;
}

static int tolua_get_FtkEvent_u_key_code(lua_State* L)
{
	lua_Number retv;
	FtkEvent* thiz = (FtkEvent*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	retv = (lua_Number)thiz->u.key.code;
	tolua_pushnumber(L, (lua_Number)retv);

	return 1;
}

static int tolua_set_FtkEvent_u_key_code(lua_State* L)
{
	FtkEvent* thiz = (FtkEvent*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	thiz->u.key.code = (int) (tolua_tonumber(L, 2, 0));

	return 1;
}

static int tolua_get_FtkEvent_u_mouse_press(lua_State* L)
{
	lua_Number retv;
	FtkEvent* thiz = (FtkEvent*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	retv = (lua_Number)thiz->u.mouse.press;
	tolua_pushnumber(L, (lua_Number)retv);

	return 1;
}

static int tolua_set_FtkEvent_u_mouse_press(lua_State* L)
{
	FtkEvent* thiz = (FtkEvent*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	thiz->u.mouse.press = (int) (tolua_tonumber(L, 2, 0));

	return 1;
}

static int tolua_get_FtkEvent_u_mouse_button(lua_State* L)
{
	lua_Number retv;
	FtkEvent* thiz = (FtkEvent*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	retv = (lua_Number)thiz->u.mouse.button;
	tolua_pushnumber(L, (lua_Number)retv);

	return 1;
}

static int tolua_set_FtkEvent_u_mouse_button(lua_State* L)
{
	FtkEvent* thiz = (FtkEvent*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	thiz->u.mouse.button = (int) (tolua_tonumber(L, 2, 0));

	return 1;
}

static int tolua_get_FtkEvent_u_mouse_x(lua_State* L)
{
	lua_Number retv;
	FtkEvent* thiz = (FtkEvent*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	retv = (lua_Number)thiz->u.mouse.x;
	tolua_pushnumber(L, (lua_Number)retv);

	return 1;
}

static int tolua_set_FtkEvent_u_mouse_x(lua_State* L)
{
	FtkEvent* thiz = (FtkEvent*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	thiz->u.mouse.x = (int) (tolua_tonumber(L, 2, 0));

	return 1;
}

static int tolua_get_FtkEvent_u_mouse_y(lua_State* L)
{
	lua_Number retv;
	FtkEvent* thiz = (FtkEvent*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	retv = (lua_Number)thiz->u.mouse.y;
	tolua_pushnumber(L, (lua_Number)retv);

	return 1;
}

static int tolua_set_FtkEvent_u_mouse_y(lua_State* L)
{
	FtkEvent* thiz = (FtkEvent*)  tolua_tousertype(L, 1, 0);
	return_val_if_fail(thiz != NULL, 0);
	thiz->u.mouse.y = (int) (tolua_tonumber(L, 2, 0));

	return 1;
}

int tolua_ftk_structs_init(lua_State* L)
{
	tolua_open(L);
	tolua_reg_types(L);
	tolua_module(L, NULL, 0);
	tolua_beginmodule(L, NULL);

	tolua_cclass(L, "FtkEvent", "FtkEvent", "", NULL);
	tolua_beginmodule(L, "FtkEvent");
	tolua_function(L, "Create", lua_ftk_event_create);
	tolua_variable(L, "type",tolua_get_FtkEvent_type, tolua_set_FtkEvent_type);
	tolua_variable(L, "widget",tolua_get_FtkEvent_widget, tolua_set_FtkEvent_widget);
	tolua_variable(L, "time",tolua_get_FtkEvent_time, tolua_set_FtkEvent_time);
	tolua_variable(L, "u_key_code",tolua_get_FtkEvent_u_key_code, tolua_set_FtkEvent_u_key_code);
	tolua_variable(L, "u_mouse_press",tolua_get_FtkEvent_u_mouse_press, tolua_set_FtkEvent_u_mouse_press);
	tolua_variable(L, "u_mouse_button",tolua_get_FtkEvent_u_mouse_button, tolua_set_FtkEvent_u_mouse_button);
	tolua_variable(L, "u_mouse_x",tolua_get_FtkEvent_u_mouse_x, tolua_set_FtkEvent_u_mouse_x);
	tolua_variable(L, "u_mouse_y",tolua_get_FtkEvent_u_mouse_y, tolua_set_FtkEvent_u_mouse_y);
	tolua_endmodule(L);
	
	tolua_cclass(L, "FtkGc", "FtkGc", "", NULL);
	tolua_beginmodule(L, "FtkGc");
	tolua_function(L, "Create", lua_ftk_gc_create);
	tolua_variable(L, "ref",tolua_get_FtkGc_ref, tolua_set_FtkGc_ref);
	tolua_variable(L, "mask",tolua_get_FtkGc_mask, tolua_set_FtkGc_mask);
	tolua_variable(L, "bg",tolua_get_FtkGc_bg, tolua_set_FtkGc_bg);
	tolua_variable(L, "fg",tolua_get_FtkGc_fg, tolua_set_FtkGc_fg);
	tolua_variable(L, "bitmap",tolua_get_FtkGc_bitmap, tolua_set_FtkGc_bitmap);
	tolua_variable(L, "alpha",tolua_get_FtkGc_alpha, tolua_set_FtkGc_alpha);
	tolua_variable(L, "line_mask",tolua_get_FtkGc_line_mask, tolua_set_FtkGc_line_mask);
	tolua_endmodule(L);
	
	tolua_endmodule(L);

	return 1;
}
