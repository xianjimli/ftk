/*
 * File: ftk_file_browser.c
 * Author: Li XianJing <xianjimli@hotmail.com>
 * Brief:  file browser window for file manager and choose.
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
 * 2010-08-15 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_util.h"
#include "ftk_theme.h"
#include "ftk_entry.h"
#include "ftk_button.h"
#include "ftk_dialog.h"
#include "ftk_globals.h"
#include "ftk_menu_item.h"
#include "ftk_list_view.h"
#include "ftk_menu_panel.h"
#include "ftk_app_window.h"
#include "ftk_message_box.h"
#include "ftk_file_system.h"
#include "ftk_file_browser.h"
#include "ftk_list_render_default.h"
#include "ftk_list_model_default.h"

typedef struct _FileBrowserPrivInfo
{
	FtkListModel* model;
	FtkWidget* list_view;
	void* on_choosed_ctx;
	char* filter_mime_type;
	FtkFileBrowserType type;
	char path[FTK_MAX_PATH+1];
	FtkFileBrowserOnChoosed on_choosed;
}PrivInfo;

static void priv_info_destroy(void* obj)
{
	PrivInfo* priv = (PrivInfo*)obj;

	FTK_FREE(priv);

	return;
}

static Ret ftk_file_browser_on_quit(void* ctx, void* obj)
{
	ftk_widget_unref((FtkWidget*)ctx);
	
	return RET_OK;
}

static Ret ftk_file_browser_on_ok(void* ctx, void* obj)
{
	int i = 0;
	FtkWidget* win = (FtkWidget*)ctx;
	FtkListItemInfo* info = NULL;
	char path[FTK_MAX_PATH+1] = {0};
	PrivInfo* priv = (PrivInfo*)ftk_widget_user_data(win);
	return_val_if_fail(priv != NULL && priv->on_choosed != NULL, RET_FAIL);

	if(priv->type == FTK_FILE_BROWER_SINGLE_CHOOSER)
	{
		i = ftk_list_view_get_selected(priv->list_view);
		ftk_list_model_get_data(priv->model, i, (void**)&info);
		if(info != NULL)
		{
			ftk_strs_cat(path, FTK_MAX_PATH, priv->path, "/", info->text, NULL);
			priv->on_choosed(priv->on_choosed_ctx, 0, path);
		}
	}
	else
	{
		int index = 0;
		int n = ftk_list_model_get_total(priv->model);
		for(i = 0; i < n; i++)
		{
			ftk_list_model_get_data(priv->model, i, (void**)&info);
			if(info != NULL && info->state)
			{
				ftk_strs_cat(path, FTK_MAX_PATH, priv->path, "/", info->text, NULL);
				priv->on_choosed(priv->on_choosed_ctx, index++, path);
			}
		}
		priv->on_choosed(priv->on_choosed_ctx, -1, NULL);
	}
	ftk_widget_unref((FtkWidget*)ctx);
	
	return RET_OK;
}

static Ret ftk_file_browser_on_cancel(void* ctx, void* obj)
{
	ftk_widget_unref((FtkWidget*)ctx);
	
	return RET_OK;
}

static Ret ftk_file_browser_on_prepare_options_menu_for_choose(void* ctx, FtkWidget* menu_panel)
{
	FtkWidget* item = NULL;
	
	item = ftk_menu_item_create(menu_panel);
	ftk_widget_set_text(item, _("OK"));
	ftk_menu_item_set_clicked_listener(item, ftk_file_browser_on_ok, ctx);
	ftk_widget_show(item, 1);
	
	item = ftk_menu_item_create(menu_panel);
	ftk_widget_set_text(item, _("Cancel"));
	ftk_menu_item_set_clicked_listener(item, ftk_file_browser_on_cancel, ctx);
	ftk_widget_show(item, 1);

	return	RET_OK;
}

static Ret ftk_file_browser_on_remove(void* ctx, void* obj)
{
	int i = 0;
	int ret = 0;
	const char* buttons[3];
	FtkWidget* win = (FtkWidget*)ctx;
	FtkListItemInfo* info = NULL;
	PrivInfo* priv = (PrivInfo*)ftk_widget_user_data(win);
	char message[FTK_MAX_PATH + 30] = {0};
	buttons[0] = _("Yes");
	buttons[1] = _("No");
	buttons[2] = NULL;
		
	i = ftk_list_view_get_selected(priv->list_view);
	ftk_list_model_get_data(priv->model, i, (void**)&info);
	return_val_if_fail(info != NULL, RET_FAIL);
	
	if(strcmp(info->text, _("..")) == 0)
	{
		return RET_FAIL;
	}

	ftk_strs_cat(message, sizeof(message), _("Are you sure to remove:\n "), info->text, NULL);

	if((ret = ftk_question(_("Remove"), message, buttons)) == 1)
	{
		char path[FTK_MAX_PATH+1] = {0};
		ftk_strs_cat(path, FTK_MAX_PATH, priv->path, "/", info->text, NULL);
		if(ftk_fs_delete(path) == RET_OK)
		{
			ftk_list_model_remove(priv->model, i);
		}
		else
		{
			ftk_strs_cat(message, sizeof(message), _("Remove failed:\n"), info->text, NULL);
			ftk_tips(message);
		}
	}
	
	return RET_OK;
}

static Ret ftk_file_browser_input_dialog_button_clicked(void* ctx, void* obj)
{
	return RET_QUIT;
}

enum _WidgetId
{
	ID_OK = 1,
	ID_CANCEL = 2,
	ID_ENTRY = 3
};

static FtkWidget* ftk_file_browser_input_dialog(FtkWidget* thiz, const char* text)
{
	int dialog_width = 0;
	FtkWidget* win = thiz;
	FtkWidget* entry = NULL;
	FtkWidget* button = NULL;
	FtkWidget* dialog = NULL;

	dialog = ftk_dialog_create(0, 0, ftk_widget_width(win) - 20, 150);
	dialog_width = ftk_widget_width(dialog);
	entry = ftk_entry_create(dialog, 10, 20, dialog_width - 20, 20);
	ftk_widget_set_id(entry, ID_ENTRY);
	
	button = ftk_button_create(dialog, dialog_width/2-70, 60, 80, 50);
	ftk_widget_set_text(button, _("OK"));
	ftk_widget_set_id(button, ID_OK);
	ftk_button_set_clicked_listener(button, ftk_file_browser_input_dialog_button_clicked, NULL);

	button = ftk_button_create(dialog, dialog_width/2+10, 60, 80, 50);
	ftk_widget_set_text(button, _("Cancel"));
	ftk_widget_set_id(button, ID_CANCEL);
	ftk_button_set_clicked_listener(button, ftk_file_browser_input_dialog_button_clicked, NULL);
	
	ftk_window_set_focus(dialog, entry);
	ftk_widget_set_text(dialog, text);
	ftk_widget_show_all(dialog, 1);

	return dialog;
}

static Ret ftk_file_browser_on_rename(void* ctx, void* obj)
{
	int i = 0;
	int ret = 0;
	const char* name = NULL;
	FtkWidget* win = (FtkWidget*)ctx;
	FtkWidget* dialog = NULL;
	FtkListItemInfo* info = NULL;
	PrivInfo* priv = (PrivInfo*)ftk_widget_user_data(win);
	i = ftk_list_view_get_selected(priv->list_view);
	ftk_list_model_get_data(priv->model, i, (void**)&info);
	return_val_if_fail(info != NULL, RET_FAIL);
	
	if(strcmp(info->text, _("..")) == 0)
	{
		return RET_FAIL;
	}

	dialog = ftk_file_browser_input_dialog(win, _("Please input new name:"));
	ret = ftk_dialog_run(dialog);
	if(ret == ID_OK)
	{
		name = ftk_entry_get_text(ftk_widget_lookup(dialog, ID_ENTRY));
		if(name != NULL)
		{
			char new_name[FTK_MAX_PATH+1] = {0};
			char old_name[FTK_MAX_PATH+1] = {0};
			ftk_strs_cat(old_name, FTK_MAX_PATH, priv->path, "/", info->text, NULL);
			ftk_strs_cat(new_name, FTK_MAX_PATH, priv->path, "/", name, NULL);

			if(ftk_fs_move(old_name, new_name) == RET_OK)
			{
				ftk_file_browser_load(win);
			}
			else
			{
				ftk_tips(_("Rename failed."));
			}
			ftk_logd("%s: %s --> %s\n", __func__, old_name, new_name);
		}
	}
	ftk_widget_unref(dialog);

	ftk_logd("%s: ret=%d\n", __func__, ret);

	return RET_OK;
}

static Ret ftk_file_browser_on_create_folder(void* ctx, void* obj)
{
	int ret = 0;
	const char* name = NULL;
	FtkWidget* win = (FtkWidget*)ctx;
	FtkWidget* dialog = NULL;
	PrivInfo* priv = (PrivInfo*)ftk_widget_user_data(win);
		
	dialog = ftk_file_browser_input_dialog(win, _("Please input folder name:"));
	ret = ftk_dialog_run(dialog);
	if(ret == ID_OK)
	{
		name = ftk_entry_get_text(ftk_widget_lookup(dialog, ID_ENTRY));
		if(name != NULL)
		{
			char folder_name[FTK_MAX_PATH+1] = {0};
			ftk_strs_cat(folder_name, FTK_MAX_PATH, priv->path, "/", name, NULL);
			
			if(ftk_fs_create_dir(folder_name) == RET_OK)
			{
				ftk_file_browser_load(win);
			}
			else
			{
				ftk_tips(_("Create folder failed."));
			}
			ftk_logd("%s: create %s\n", __func__, folder_name);
		}
	}
	ftk_widget_unref(dialog);

	ftk_logd("%s: ret=%d\n", __func__, ret);

	return RET_OK;
}

static Ret ftk_file_browser_on_detail(void* ctx, void* obj)
{
	/*TODO*/
	return RET_OK;
}

static Ret ftk_file_browser_on_more(void* ctx, void* obj)
{
	/*TODO*/
	return RET_OK;
}

static Ret ftk_file_browser_on_prepare_options_menu(void* ctx, FtkWidget* menu_panel)
{
	FtkWidget* item = ftk_menu_item_create(menu_panel);
	ftk_widget_set_text(item, _("Remove"));
	ftk_menu_item_set_clicked_listener(item, ftk_file_browser_on_remove, ctx);
	ftk_widget_show(item, 1);
	
	item = ftk_menu_item_create(menu_panel);
	ftk_widget_set_text(item, _("Rename"));
	ftk_menu_item_set_clicked_listener(item, ftk_file_browser_on_rename, ctx);
	ftk_widget_show(item, 1);
	
	item = ftk_menu_item_create(menu_panel);
	ftk_widget_set_text(item, _("Create Folder"));
	ftk_menu_item_set_clicked_listener(item, ftk_file_browser_on_create_folder, ctx);
	ftk_widget_show(item, 1);
	
	item = ftk_menu_item_create(menu_panel);
	ftk_widget_set_text(item, _("Detail"));
	ftk_menu_item_set_clicked_listener(item, ftk_file_browser_on_detail, ctx);
	ftk_widget_show(item, 1);
	
	item = ftk_menu_item_create(menu_panel);
	ftk_widget_set_text(item, _("Quit"));
	ftk_menu_item_set_clicked_listener(item, ftk_file_browser_on_quit, ctx);
	ftk_widget_show(item, 1);
	
	item = ftk_menu_item_create(menu_panel);
	ftk_widget_set_text(item, _("More"));
	ftk_menu_item_set_clicked_listener(item, ftk_file_browser_on_more, ctx);
	ftk_widget_show(item, 1);

	return	RET_OK;
}

static Ret ftk_file_browser_on_item_clicked(void* ctx, void* list)
{
	FtkWidget* win = (FtkWidget*)ctx;
	FtkListItemInfo* info = NULL;
	int i = ftk_list_view_get_selected((FtkWidget*)list);
	PrivInfo* priv = (PrivInfo*)ftk_widget_user_data(win);
	FtkListModel* model = ftk_list_view_get_model((FtkWidget*)list);
 
 	return_val_if_fail(priv != NULL, RET_FAIL);

	ftk_list_model_get_data(model, i, (void**)&info);
	if(info != NULL && info->text != NULL)
	{
		char* p = NULL;
		char  path[FTK_MAX_PATH+1] = {0};
		char* file_name = info->text;

		if(strcmp(file_name, _("..")) == 0)
		{
			ftk_strcpy(path, priv->path);
			if((p = strrchr(path, FTK_PATH_DELIM)) != NULL)
			{
				if(p == path)
				{
					p[1] = '\0';
				}
				else
				{
					*p = '\0';
				}
			}
			ftk_file_browser_set_path(win, path);
			ftk_file_browser_load(win);
		}
		else if(info->value) /*enter selected folder*/
		{
			ftk_strs_cat(path, FTK_MAX_PATH, priv->path, "/", file_name, NULL);
			ftk_file_browser_set_path(win, path);
			ftk_file_browser_load(win);
		}
		else if(priv->type == FTK_FILE_BROWER_SINGLE_CHOOSER)
		{
			if(priv->on_choosed != NULL)
			{
				ftk_strs_cat(path, FTK_MAX_PATH, priv->path, "/", file_name, NULL);
				priv->on_choosed(priv->on_choosed_ctx, 0, path);
				ftk_widget_unref(win);
			}
		}
		else if(priv->type == FTK_FILE_BROWER_MULTI_CHOOSER)
		{
			info->state = !info->state;
		}
	}

	return RET_OK;
}

FtkWidget* ftk_file_browser_create(FtkFileBrowserType type)
{
	FtkListModel* model = NULL;
	FtkListRender* render = NULL;	
	FtkPrepareOptionsMenu option_menu = NULL;
	FtkWidget* win  = ftk_app_window_create();
	PrivInfo* priv  = FTK_NEW(PrivInfo);
	FtkWidget* list = ftk_list_view_create(win, 0, 0, ftk_widget_width(win), ftk_widget_height(win));

	if(type == FTK_FILE_BROWER_APP)
	{
		option_menu = ftk_file_browser_on_prepare_options_menu;
	}
	else
	{
		option_menu = ftk_file_browser_on_prepare_options_menu_for_choose;
	}
	ftk_app_window_set_on_prepare_options_menu(win, option_menu, win);

	model = ftk_list_model_default_create(10);
	render = ftk_list_render_default_create();

	priv->type = type;
	priv->model = model;
	priv->list_view = list;
	ftk_list_view_init(list, model, render, 40);
	ftk_widget_set_text(win, _("File Browser"));
	ftk_widget_set_user_data(win, priv_info_destroy, priv);
	ftk_list_view_set_clicked_listener(list, ftk_file_browser_on_item_clicked, win);
	ftk_list_render_default_set_marquee_attr(render, 
		FTK_MARQUEE_AUTO | FTK_MARQUEE_RIGHT2LEFT | FTK_MARQUEE_FOREVER);
	ftk_list_model_unref(model);

	return win;
}

Ret		   ftk_file_browser_set_path(FtkWidget* thiz, const char* path)
{
	PrivInfo* priv = (PrivInfo*)ftk_widget_user_data(thiz);
	return_val_if_fail(priv != NULL && path != NULL, RET_FAIL);	

	ftk_strncpy(priv->path, path, FTK_MAX_PATH);
	ftk_normalize_path(priv->path);

	return RET_OK;
}

Ret		   ftk_file_browser_set_filter(FtkWidget* thiz, const char* mime_type)
{
	PrivInfo* priv = (PrivInfo*)ftk_widget_user_data(thiz);
	return_val_if_fail(priv != NULL, RET_FAIL);	

	FTK_FREE(priv->filter_mime_type);
	priv->filter_mime_type = FTK_STRDUP(mime_type);

	return RET_OK;
}

Ret		   ftk_file_browser_set_choosed_handler(FtkWidget* thiz, FtkFileBrowserOnChoosed on_choosed, void* ctx)
{
	PrivInfo* priv = (PrivInfo*)ftk_widget_user_data(thiz);
	return_val_if_fail(priv != NULL, RET_FAIL);	

	priv->on_choosed = on_choosed;
	priv->on_choosed_ctx = ctx;

	return RET_OK;
}

static FtkBitmap* ftk_file_browser_load_mime_icon(const char* file_name)
{
	char* p = NULL;
	char icon_name[FTK_MAX_PATH+1] = {0};
	char mime_type[FTK_MIME_TYPE_LEN + 1] = {0};
	
	ftk_strcpy(mime_type, ftk_file_get_mime_type(file_name));

	p = strrchr(mime_type, '/');
	return_val_if_fail(p != NULL, NULL);

	*p = '\0';
	ftk_strs_cat(icon_name, FTK_MAX_PATH, "mime_", mime_type, FTK_STOCK_IMG_SUFFIX, NULL);

	return ftk_theme_load_image(ftk_default_theme(), icon_name);
}

static int ftk_file_browser_get_display_style(PrivInfo* priv, int is_dir)
{
	if(priv->type == FTK_FILE_BROWER_APP || priv->type == FTK_FILE_BROWER_SINGLE_CHOOSER)
	{
		return FTK_LIST_ITEM_NORMAL;
	}

	if(priv->filter_mime_type == NULL)
	{
		return is_dir ? FTK_LIST_ITEM_NORMAL : FTK_LIST_ITEM_CHECK;	
	}
	else
	{
		if(strcmp(priv->filter_mime_type, FTK_MIME_DIR) == 0)
		{
			return is_dir ? FTK_LIST_ITEM_CHECK: FTK_LIST_ITEM_NORMAL;	
		}
		else
		{
			return !is_dir ? FTK_LIST_ITEM_CHECK: FTK_LIST_ITEM_NORMAL;	
		}
	}
}

Ret		   ftk_file_browser_load(FtkWidget* thiz)
{
	FtkFileInfo info = {0};
	FtkFsHandle handle = NULL;
	FtkListItemInfo item = {0};
	const char* mime_type = NULL;
	char path[FTK_MAX_PATH+1] = {0};
	PrivInfo* priv = ftk_widget_user_data(thiz);
	return_val_if_fail(priv != NULL && priv->path != NULL, RET_FAIL);	

	handle = ftk_dir_open(priv->path);
	if(handle == NULL)
	{
		ftk_logd("%s: open %s\n", __func__, priv->path);
	}
	return_val_if_fail(handle != NULL, RET_FAIL);

	ftk_list_model_reset(priv->model);
	ftk_list_model_disable_notify(priv->model);

	if(!ftk_fs_is_root(priv->path))
	{
		item.value = 1;
		item.text = _("..");
		item.type = FTK_LIST_ITEM_NORMAL;
		item.left_icon = ftk_theme_load_image(ftk_default_theme(), "up"FTK_STOCK_IMG_SUFFIX);
		ftk_list_model_add(priv->model, &item);
	}

	/*directory go first.*/
	while(ftk_dir_read(handle, &info) == RET_OK)
	{
		if(info.name[0] == '.') continue;

		if(info.is_dir)
		{
			item.value = 1;
			item.text = info.name;
			item.type = ftk_file_browser_get_display_style(priv, 1);
			item.left_icon = ftk_theme_load_image(ftk_default_theme(), "folder"FTK_STOCK_IMG_SUFFIX);
			ftk_list_model_add(priv->model, &item);
		}
	}
	ftk_dir_close(handle);

	if(priv->filter_mime_type == NULL || strcmp(priv->filter_mime_type, FTK_MIME_DIR) != 0)
	{
		handle = ftk_dir_open(priv->path);
		while(ftk_dir_read(handle, &info) == RET_OK)
		{
			if(info.name[0] == '.') continue;
			if(info.is_dir) continue;

			if(priv->filter_mime_type != NULL)
			{
				ftk_strs_cat(path, FTK_MAX_PATH, priv->path, "/", info.name, NULL);
				mime_type = ftk_file_get_mime_type(path);
				if(strstr(priv->filter_mime_type, mime_type) != NULL)
				{
					continue;
				}
			}
				
			item.value = 0;
			item.type = ftk_file_browser_get_display_style(priv, 0);
			item.text = info.name;
			item.left_icon = ftk_file_browser_load_mime_icon(info.name);
			ftk_list_model_add(priv->model, &item);
		}
		ftk_dir_close(handle);
	}

	ftk_window_set_focus(thiz, priv->list_view);
	ftk_list_view_set_cursor(priv->list_view, 0);
	ftk_list_model_enable_notify(priv->model);
	ftk_list_model_notify(priv->model);
	ftk_widget_show_all(thiz, 1);

	return RET_OK;
}

