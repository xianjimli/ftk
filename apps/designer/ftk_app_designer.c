/*
 * File: ftk_app_designer.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: ui designer app.
 *
 * Copyright (c) 2009 - 2011  Li XianJing <xianjimli@hotmail.com>
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
 * 2011-09-27 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "open.h"
#include "save.h"
#include "widgets_info.h"
#include "widget_editor.h"
#include "ftk_popup_menu.h"
#include "ftk_app_designer.h"

typedef struct _PrivInfo
{
	FtkBitmap* icon;
}PrivInfo;

typedef struct _Info
{
	int last_x;
	int last_y;
	int ctrl_down;
	int alt_down;
	int mouse_down;
	FtkWidget* selected_widget;
}Info;

static Ret designer_on_event(void* ctx, void* data);
static Ret designer_on_prepare_options_menu(void* ctx, FtkWidget* menu_panel);

static Info* info_create(void)
{
	Info* info = FTK_NEW(Info);
	
	return info;
}

static void info_destroy(void* data)
{
	FTK_FREE(data);
	
	return;
}

static FtkBitmap* ftk_app_designer_get_icon(FtkApp* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, NULL);
	
	if(priv->icon != NULL) return priv->icon;

	priv->icon = ftk_app_load_bitmap(thiz, "designer", "designer");

	return priv->icon;
}

static const char* ftk_app_designer_get_name(FtkApp* thiz)
{
	return _("Designer");
}

static Ret designer_init(FtkWidget* win)
{
	ftk_widget_set_user_data(win, info_destroy, info_create());
	ftk_widget_set_event_listener(win, designer_on_event, win);
	ftk_app_window_set_on_prepare_options_menu(win, designer_on_prepare_options_menu, win);
	
	ftk_widget_show(win, 1);

	return RET_OK;
}

static Ret designer_on_new(void* ctx, void* item)
{
	FtkWidget* win = NULL;
	FtkWidget* old_win = (FtkWidget*)ctx;
	
	win = ftk_app_window_create();
	ftk_widget_set_text(win, _("FTK UI Designer"));
	ftk_window_set_animation_hint(win, "app_main_window");
	designer_init(win);

	if(old_win != NULL)
	{
		ftk_widget_unref(old_win);
	}

	return RET_OK;
}

static Ret designer_on_open_ok(void* ctx, FtkWidget* win)
{
	FtkWidget* old_win = (FtkWidget*)ctx;
	
	if(win != NULL)
	{
		designer_init(win);
	}

	ftk_widget_unref(old_win);

	return RET_OK;
}

static Ret designer_on_open(void* ctx, void* item)
{
	ftk_widget_editor_open(ctx, designer_on_open_ok);

	return RET_OK;
}

static Ret designer_on_save(void* ctx, void* item)
{
	FtkWidget* win = (FtkWidget*)ctx;

	ftk_widget_editor_save(win);

	return RET_OK;
}

static Ret designer_on_help(void* ctx, void* item)
{
	ftk_tips("Ctrl + UP|DOWN|LEFT|RIGHT: move widget.\n Alt + UP|DOWN|LEFT|RIGHT: resize widget\n <F5>: popup menu.\n");

	return RET_OK;
}

static Ret designer_on_quit(void* ctx, void* item)
{
	FtkWidget* win = (FtkWidget*)ctx;

	ftk_widget_unref(win);

	return RET_OK;
}

static int designer_has_selected_widget(FtkWidget* win)
{
	Info* info = (Info*)ftk_widget_user_data(win);

	return (info->selected_widget != NULL && info->selected_widget != win);
}

static Ret designer_on_duplicate(void* ctx, void* item)
{
	FtkWidget* widget = NULL;
	FtkWidget* win = (FtkWidget*)ctx;
	const WidgetInfo* widget_info = NULL; 
	Info* info = (Info*)ftk_widget_user_data(win);
	
	if(!designer_has_selected_widget(win))
	{
		return RET_OK;
	}

	widget_info = widgets_info_find_by_type(ftk_widget_type(info->selected_widget));
	return_val_if_fail(widget_info != NULL, RET_FAIL);

	if(widget_info->is_leaf_widget)
	{
		widget = widget_info->create(
			ftk_widget_parent(info->selected_widget),
			ftk_widget_left(info->selected_widget),
			ftk_widget_top(info->selected_widget) + ftk_widget_height(info->selected_widget),
			ftk_widget_width(info->selected_widget),
			ftk_widget_height(info->selected_widget));
		ftk_widget_set_text(widget, ftk_widget_get_text(info->selected_widget));
		ftk_widget_show(widget, 1);
		ftk_widget_ref(widget);
		/*TODO: duplicate listview/combobox/iconview*/
	}

	return RET_OK;
}

static Ret designer_on_insert(void* ctx, void* item)
{
	int x = 0;
	int y = 0;
	FtkWidget* parent = NULL;
	FtkWidget* win = (FtkWidget*)ctx;
	const WidgetInfo* widget_info = NULL; 
	Info* info = (Info*)ftk_widget_user_data(win);
	
	parent = win;
	if(designer_has_selected_widget(win))
	{
		widget_info = widgets_info_find_by_type(ftk_widget_type(info->selected_widget));
		parent = widget_info->is_leaf_widget ? ftk_widget_parent(info->selected_widget) : info->selected_widget;
	}

	x = info->last_x - ftk_widget_left_abs(parent);
	y = info->last_y - ftk_widget_top_abs(parent);
	ftk_widget_editor_new(parent, x, y);
	
	return RET_OK;
}

static Ret designer_on_delete(void* ctx, void* item)
{
	FtkWidget* win = (FtkWidget*)ctx;
	Info* info = (Info*)ftk_widget_user_data(win);
	
	if(designer_has_selected_widget(win))
	{
		ftk_widget_remove_child(ftk_widget_parent(info->selected_widget), info->selected_widget);
		info->selected_widget = NULL;
		ftk_widget_invalidate(win);
	}

	return RET_OK;
}

static Ret designer_on_prop(void* ctx, void* item)
{
	FtkWidget* parent = NULL;
	FtkWidget* widget = NULL;
	FtkWidget* win = (FtkWidget*)ctx;
	Info* info = (Info*)ftk_widget_user_data(win);
	
	if(designer_has_selected_widget(win))
	{
		widget = info->selected_widget;
		parent = ftk_widget_parent(info->selected_widget);
	}
	else
	{
		widget = win;
		parent = win;
	}

	ftk_widget_editor_edit(parent, widget);

	return RET_OK;
}

static Ret designer_on_special_prop(void* ctx, void* item)
{
	const WidgetInfo* widget_info = NULL; 
	FtkWidget* win = (FtkWidget*)ctx;
	Info* info = (Info*)ftk_widget_user_data(win);

	if(designer_has_selected_widget(win))
	{
		widget_info = widgets_info_find_by_type(ftk_widget_type(info->selected_widget));
		if(widget_info != NULL && widget_info->edit != NULL)
		{
			widget_info->edit(info->selected_widget);
		}
	}

	return RET_OK;
}

typedef struct _MenuItem
{
	int need_selected_widget;
	char* name;
	const char* icon_file_name;
	FtkListener on_clicked;
}MenuItem;

static const MenuItem s_menu_items[] =
{
	{0, "New", NULL,  designer_on_new},
	{0, "Open", NULL, designer_on_open},
	{0, "Save", NULL, designer_on_save},
	{0, "Help", NULL, designer_on_help},
	{0, "Quit", NULL, designer_on_quit}
};

static const MenuItem s_popup_menu_items[] =
{
	{0, "Insert",           NULL, designer_on_insert},
	{1, "Duplicate",        NULL, designer_on_duplicate},
	{1, "Delete",           NULL, designer_on_delete},
	{0, "General property", NULL, designer_on_prop},
	{1, "Special property", NULL, designer_on_special_prop},
	{0, "Cancel",           NULL, NULL}
};

static Ret designer_on_popup_menu_item_clicked(void* ctx, void* data)
{
	FtkListItemInfo* info = data;
	FtkListener on_clicked = (FtkListener)info->extra_user_data;

	if(on_clicked != NULL)
	{
		on_clicked(ctx, NULL);
	}

	return RET_OK;
}

static Ret designer_on_popup_menu(void* ctx, void* obj)
{
	size_t i = 0;
	int nr = 0;
	int height = 0;
	FtkBitmap* icon = NULL; 
	FtkWidget* popup = NULL;
	FtkListItemInfo infos;
	FtkWidget* win = (FtkWidget*)ctx;

	memset(&infos, 0x00, sizeof(infos));
	icon = ftk_theme_load_image(ftk_default_theme(), "info"FTK_STOCK_IMG_SUFFIX); 

	for(i = 0; i < FTK_ARRAY_SIZE(s_popup_menu_items); i++)
	{
		if(s_popup_menu_items[i].need_selected_widget && !designer_has_selected_widget(win))
		{
			continue;
		}
		nr++;
	}

	height = (nr + 1) * FTK_POPUP_MENU_ITEM_HEIGHT;
	height = height < ftk_widget_height(win) ? height : ftk_widget_height(win);

	popup = ftk_popup_menu_create(0, 0, 0, height, icon, _("Edit"));	

	infos.state = 0;
	infos.type = FTK_LIST_ITEM_NORMAL;
	for(i = 0; i < FTK_ARRAY_SIZE(s_popup_menu_items); i++)
	{
		if(s_popup_menu_items[i].need_selected_widget && !designer_has_selected_widget(win))
		{
			continue;
		}

		infos.value = i;
		infos.text = s_popup_menu_items[i].name;
		infos.extra_user_data = s_popup_menu_items[i].on_clicked;
		
		ftk_popup_menu_add(popup, &infos);
	}
	ftk_bitmap_unref(icon);
	
	ftk_popup_menu_set_clicked_listener(popup, designer_on_popup_menu_item_clicked, ctx);
	ftk_widget_show_all(popup, 1);

	return RET_OK;
}

static Ret designer_on_prepare_options_menu(void* ctx, FtkWidget* menu_panel)
{
	size_t i = 0;
	FtkWidget* item = NULL;
	FtkWidget* win = (FtkWidget*)ctx;

	for(i = 0; i < FTK_ARRAY_SIZE(s_menu_items); i++)	
	{
		item = ftk_menu_item_create(menu_panel);
		ftk_widget_set_text(item, _(s_menu_items[i].name));
		ftk_menu_item_set_clicked_listener(item, s_menu_items[i].on_clicked, win);
		ftk_widget_show(item, 1);
	}
	
	return RET_OK;
}

static Ret designer_move_widget(FtkWidget* widget, int x, int y, int w, int h)
{
	int parent_w = ftk_widget_width(ftk_widget_parent(widget));
	int parent_h = ftk_widget_height(ftk_widget_parent(widget));
	const WidgetInfo* widget_info = widgets_info_find_by_type(ftk_widget_type(widget));

	return_val_if_fail(widget_info != NULL, RET_OK);

	x = x >= 0 ? x : 0;
	y = y >= 0 ? y : 0;

	w = FTK_MAX(w, widget_info->min_width);
	h = FTK_MAX(h, widget_info->min_height);

	if((x + w) < parent_w && (y + h) < parent_h)
	{
		ftk_widget_move_resize(widget, x, y, w, h);
	}

	return RET_OK;
}

static  Ret designer_handle_direction_key(FtkWidget* win, int press, int code)
{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	Ret ret = RET_OK;
	Info* info = (Info*)ftk_widget_user_data(win);

	x = ftk_widget_left(info->selected_widget);
	y = ftk_widget_top(info->selected_widget);
	w = ftk_widget_width(info->selected_widget);
	h = ftk_widget_height(info->selected_widget);
	
	switch(code)
	{
		case FTK_KEY_UP:
		{
			if(info->ctrl_down)
			{
				y--;
			}

			if(info->alt_down)
			{
				h--;
			}

			break;
		}
		case FTK_KEY_DOWN:
		{
			if(info->ctrl_down)
			{
				y++;
			}

			if(info->alt_down)
			{
				h++;
			}

			break;
		}
		case FTK_KEY_LEFT:
		{
			if(info->ctrl_down)
			{
				x--;
			}

			if(info->alt_down)
			{
				w--;
			}

			break;
		}
		case FTK_KEY_RIGHT:
		{
			if(info->ctrl_down)
			{
				x++;
			}

			if(info->alt_down)
			{
				w++;
			}

			break;
		}
		default:break; 
	}

	if(info->ctrl_down || info->alt_down)
	{
		designer_move_widget(info->selected_widget, x, y, w, h);
		ret = RET_REMOVE;
	}

	return ret;
}

static Ret designer_on_key_event(FtkWidget* win, int press, int code)
{
	Ret ret = RET_OK;
	Info* info = (Info*)ftk_widget_user_data(win);

	/*XXX: Li XianJing 37 is the key code of left ctrl on my thinkpad, I dont know why.*/
	if(code == FTK_KEY_LEFTCTRL || code == FTK_KEY_RIGHTCTRL || code == 37)
	{
		info->ctrl_down = press;

		return RET_OK;
	}

	if(code == FTK_KEY_LEFTALT || code == FTK_KEY_RIGHTALT 
		|| code == FTK_KEY_LEFTSHIFT || code == FTK_KEY_RIGHTSHIFT)
	{
		info->alt_down = press;

		return RET_OK;
	}

	if(press)
	{
		if((info->ctrl_down || info->alt_down))
		{
			return RET_REMOVE;
		}
		else
		{
			return RET_OK;
		}
	}
	else
	{
		if(code == FTK_KEY_F5)
		{
			designer_on_popup_menu(win, NULL);
		}
		else if(code == FTK_KEY_INSERT)
		{
			designer_on_insert(win, NULL);
		}
		else if(code == FTK_KEY_DELETE)
		{
			designer_on_delete(win, NULL);
		}

		if(!info->ctrl_down && !info->alt_down)
		{
			if(code == FTK_KEY_TAB || code == FTK_KEY_LEFT || code == FTK_KEY_UP || code == FTK_KEY_DOWN || code == FTK_KEY_RIGHT)
			{
				info->selected_widget = ftk_window_get_focus(win);
			}
			return RET_OK;
		}
	}
	
	if(!designer_has_selected_widget(win))
	{
		if(info->ctrl_down || info->alt_down)
		{
			return RET_REMOVE;
		}
		else
		{
			return RET_OK;
		}
	}

	ret = designer_handle_direction_key(win, press, code);

	return ret;
}

static Ret designer_on_mouse_event(FtkWidget* win, int press, int x, int y)
{
	Info* info = (Info*)ftk_widget_user_data(win);

	if(press == 1)
	{
		info->last_x = x;
		info->last_y = y;
		info->mouse_down = 1;
		info->selected_widget = ftk_widget_find_target(win, info->last_x, info->last_y, 0);
	}
	else if(press == 0)
	{
		info->mouse_down = 0;
	}
	else
	{
		int w = 0;
		int h = 0;
		int x_offset = x - info->last_x;
		int y_offset = y - info->last_y;
		
		if(!designer_has_selected_widget(win) || !info->mouse_down)
		{
			return RET_OK;
		}
	
		info->last_x = x;
		info->last_y = y;
		x = ftk_widget_left(info->selected_widget) + x_offset;
		y = ftk_widget_top(info->selected_widget) + y_offset;
		w = ftk_widget_width(info->selected_widget);
		h = ftk_widget_height(info->selected_widget);

		designer_move_widget(info->selected_widget, x, y, w, h);
	}

	return RET_OK;
}

static Ret designer_on_event(void* ctx, void* data)
{
	Ret ret = RET_OK;
	FtkWidget* win = (FtkWidget*)ctx;
	FtkEvent* event = (FtkEvent*)data;

	if(event->type == FTK_EVT_MOUSE_LONG_PRESS)
	{
		ret = RET_REMOVE;
		designer_on_popup_menu(win, NULL);
	}
	else if(event->type == FTK_EVT_KEY_DOWN)
	{
		ret = designer_on_key_event(win, 1, event->u.key.code);
	}
	else if(event->type == FTK_EVT_KEY_UP)
	{
		ret = designer_on_key_event(win, 0, event->u.key.code);
	}
	else if(event->type == FTK_EVT_MOUSE_DOWN)
	{
		ret = designer_on_mouse_event(win, 1, event->u.mouse.x, event->u.mouse.y);
	}
	else if(event->type == FTK_EVT_MOUSE_MOVE)
	{
		ret = designer_on_mouse_event(win, -1, event->u.mouse.x, event->u.mouse.y);
	}
	else if(event->type == FTK_EVT_MOUSE_UP)
	{
		ret = designer_on_mouse_event(win, 0, event->u.mouse.x, event->u.mouse.y);
	}
	
	return ret;
}

static Ret ftk_app_designer_run(FtkApp* thiz, int argc, char* argv[])
{
	designer_on_new(NULL, NULL);

	return RET_OK;
}

static void ftk_app_designer_destroy(FtkApp* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		ftk_bitmap_unref(priv->icon);
		FTK_FREE(thiz);
	}

	return;
}

FtkApp* ftk_app_designer_create(void)
{
	FtkApp* thiz = FTK_ZALLOC(sizeof(FtkApp) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		thiz->run  = ftk_app_designer_run;
		thiz->get_icon = ftk_app_designer_get_icon;
		thiz->get_name = ftk_app_designer_get_name;
		thiz->destroy = ftk_app_designer_destroy;
	}

	return thiz;
}

