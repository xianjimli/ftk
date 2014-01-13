/*
 * File: ftk_all_shell.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   shell
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
 * 2010-08-29 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include <pthread.h>
#include "ftk_app_shell.h"
#include "ftk_source_shell.h"

typedef struct _PrivInfo
{
	FtkBitmap* icon;
	FtkWidget* input_entry;
	FtkWidget* output_text_view;

	int read_fd;
	int write_fd;

	FtkSource* shell_source;
}PrivInfo;

static FtkBitmap* ftk_app_shell_get_icon(FtkApp* thiz)
{
	DECL_PRIV(thiz, priv);
	const char* name="shell.png";
	char file_name[FTK_MAX_PATH + 1] = {0};
	return_val_if_fail(priv != NULL, NULL);
	
	if(priv->icon != NULL) return priv->icon;
	
	snprintf(file_name, FTK_MAX_PATH, "%s/icons/%s", APP_DATA_DIR, name);
	priv->icon = ftk_bitmap_factory_load(ftk_default_bitmap_factory(), file_name);
	if(priv->icon != NULL) return priv->icon;

	snprintf(file_name, FTK_MAX_PATH, "%s/icons/%s", APP_LOCAL_DATA_DIR, name);
	priv->icon = ftk_bitmap_factory_load(ftk_default_bitmap_factory(), file_name);

	return priv->icon;
}

static const char* ftk_app_shell_get_name(FtkApp* thiz)
{
	return _("Shell");
}

static Ret ftk_app_shell_exec(FtkApp* thiz, FtkEvent* event)
{
	DECL_PRIV(thiz, priv);
	const char* cmd = NULL;
	char buffer[1024] = {0};
	ssize_t write_ret = 0;
	return_val_if_fail(thiz != NULL && event != NULL, RET_FAIL);

	if(event->type != FTK_EVT_KEY_UP || event->u.key.code != FTK_KEY_ENTER)
	{
		return RET_OK;
	}

	cmd = ftk_entry_get_text(priv->input_entry);

	if(strncmp(cmd, "exit", 4) == 0 || strncmp(cmd, "quit", 4) == 0)
	{
		ftk_widget_unref(ftk_widget_toplevel(priv->input_entry));
	}
	else
	{
		ftk_snprintf(buffer, sizeof(buffer), "%s\n", cmd);
		write_ret = write(priv->write_fd, buffer, strlen(buffer));

		ftk_entry_set_text(priv->input_entry, "");
		ftk_text_view_set_text(priv->output_text_view, buffer, -1);
	}

	return RET_REMOVE;
}

static void ftk_app_shell_reset(FtkApp* thiz)
{
	DECL_PRIV(thiz, priv);
	priv->input_entry = NULL;
	priv->output_text_view = NULL;
	
	close(priv->write_fd);
	priv->read_fd = 0;
	priv->write_fd = 0;

	ftk_source_disable(priv->shell_source);
	ftk_main_loop_remove_source(ftk_default_main_loop(), priv->shell_source);
	priv->shell_source = NULL;

	return;
}

#define SHELL_CMD1 "/bin/bash"
#define SHELL_CMD2 "/bin/sh"

static const char* ftk_app_shell_get_shell(FtkApp* thiz)
{
	if(access(SHELL_CMD1, 0) == 0)
	{
		return SHELL_CMD1;
	}

	if(access(SHELL_CMD2, 0) == 0)
	{
		return SHELL_CMD2;
	}

	return NULL;
}

static Ret ftk_app_shell_create_shell_process(FtkApp* thiz)
{
	int ret = 0;
	DECL_PRIV(thiz, priv);
	int parent_to_child[2] = {0};
	int child_to_parent[2] = {0};
	const char* shell = ftk_app_shell_get_shell(thiz);

	return_val_if_fail(shell != NULL, RET_FAIL);

	if(pipe(parent_to_child) < 0) goto failed;
	if(pipe(child_to_parent) < 0) goto failed;
	if((ret = fork()) < 0) goto failed;
	
	if(ret == 0)
	{
		close(parent_to_child[1]);
		close(child_to_parent[0]);

		dup2(parent_to_child[0], STDIN_FILENO);
		dup2(child_to_parent[1], STDOUT_FILENO);
		dup2(child_to_parent[1], STDERR_FILENO);

		execl(shell, shell, NULL);
		_exit(0);
	}
	else
	{
		close(parent_to_child[0]);
		close(child_to_parent[1]);

		priv->read_fd = child_to_parent[0];
		priv->write_fd = parent_to_child[1];

	}

	return RET_OK;

failed:
	if(parent_to_child[0] > 0) close(parent_to_child[0]);
	if(parent_to_child[1] > 0) close(parent_to_child[1]);
	if(child_to_parent[0] > 0) close(child_to_parent[0]);
	if(child_to_parent[1] > 0) close(child_to_parent[1]);
	
	return 0;
}
	
static Ret ftk_app_shell_run(FtkApp* thiz, int argc, char* argv[])
{
	DECL_PRIV(thiz, priv);
	FtkWidget* entry = NULL;
	FtkWidget* text_view = NULL;
	FtkWidget* win = ftk_app_window_create();
	int width = ftk_widget_width(win);
	int height = ftk_widget_height(win);

	ftk_window_set_animation_hint(win, "app_main_window");
	ftk_widget_set_text(win, _("Shell"));
	text_view = ftk_text_view_create(win, 0, 0, width, height - 30);
	ftk_text_view_set_readonly(text_view, 1);
	entry = ftk_entry_create(win, 0, height - 30, width, 30);
	ftk_entry_set_tips(entry, _("Input command at here."));
	ftk_widget_set_event_listener(entry, (FtkListener)ftk_app_shell_exec, thiz);
	ftk_window_set_focus(win, entry);

	priv->input_entry = entry;
	priv->output_text_view = text_view;

	ftk_widget_set_user_data(win, (FtkDestroy)ftk_app_shell_reset, thiz);

#ifdef HAS_MAIN
	FTK_QUIT_WHEN_WIDGET_CLOSE(win);
#endif
	
	if(ftk_app_shell_create_shell_process(thiz) == RET_OK)
	{
		priv->shell_source = ftk_source_shell_create(priv->read_fd, priv->output_text_view);
		ftk_main_loop_add_source(ftk_default_main_loop(), priv->shell_source);
	}

	ftk_widget_show_all(win, 1);

	return RET_OK;
}

static void ftk_app_shell_destroy(FtkApp* thiz)
{
	FTK_FREE(thiz);

	return;
}

FtkApp* ftk_app_shell_create(void)
{
	FtkApp* thiz = FTK_ZALLOC(sizeof(FtkApp) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		thiz->run = ftk_app_shell_run;
		thiz->get_icon = ftk_app_shell_get_icon;
		thiz->get_name = ftk_app_shell_get_name;
		thiz->destroy = ftk_app_shell_destroy;
	}

	return thiz;
}
