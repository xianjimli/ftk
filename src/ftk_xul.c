/*
 * File: ftk_xul.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: create ui from xml.
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
 * 2009-11-23 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk.h"
#include "ftk_xul.h"
#include "ftk_expr.h"
#include "ftk_util.h"
#include "ftk_mmap.h"
#include "ftk_xml_parser.h"

typedef struct _XulPrivInfo
{
	FtkWidget* root;
	FtkWidget* current;
	int  meet_start_tag;
	int  prev_is_widget;
	char processed_value[128];
	char translated_path[FTK_MAX_PATH+1];
	FtkXulCallbacks* callbacks;
}PrivInfo;

typedef struct _FtkWidgetCreateInfo
{
	int id;
	int x;
	int y;
	int w;
	int h;
	int attr;
	int visible;
	char anim_hint[32];
	const char* value;
	FtkWidget* parent;
	FtkGc gc[FTK_WIDGET_STATE_NR];
	
	/*check button*/
	int checked;
	int icon_position;

	/* dialog */
	int hide_title;

	/* entry */
	const char *tips;

	/* file_browser */
	const char *filter;

    /* label */
    int alignment;

	/*scroll bar*/
	int max_value;
	int page_delta;

	/*text_view*/
	int readonly;

    int type;
    FtkBitmap* icon;
    const char *font;

	PrivInfo* priv;
}FtkWidgetCreateInfo;

typedef FtkWidget* (*FtkXulWidgetCreate)(FtkWidgetCreateInfo* info);

typedef struct _WidgetCreator
{
	const char* name;
	FtkXulWidgetCreate create;
	int is_widget;
}WidgetCreator;

static inline const char*  ftk_xul_translate_text(FtkXulCallbacks* thiz, const char* text)
{
	if(thiz == NULL || thiz->translate_text == NULL)
	{
		return text;
	}
	else
	{
		return thiz->translate_text(thiz->ctx, text);
	}
}

static inline  FtkBitmap*  ftk_xul_load_image(FtkXulCallbacks* thiz, const char* filename)
{
	return_val_if_fail(thiz != NULL && thiz->load_image != NULL, NULL);

	return thiz->load_image(thiz->ctx, filename);
}

static FtkWidget* ftk_xul_label_create(FtkWidgetCreateInfo* info)
{
	FtkWidget* widget = NULL;

	widget = ftk_label_create(info->parent, info->x, info->y, info->w, info->h);
	if(info->value != NULL)
	{
		ftk_widget_set_text(widget, ftk_xul_translate_text(info->priv->callbacks, info->value));
	}

	if(info->alignment > 0)
	{
	    ftk_label_set_alignment(widget, info->alignment);
	}

	return widget;
}

static FtkWidget* ftk_xul_button_create(FtkWidgetCreateInfo* info)
{
	FtkWidget* widget = NULL;

	widget = ftk_button_create(info->parent, info->x, info->y, info->w, info->h);
	if(info->value != NULL)
	{
		ftk_widget_set_text(widget, ftk_xul_translate_text(info->priv->callbacks, info->value));
	}

	return widget;
}

static FtkWidget* ftk_xul_entry_create(FtkWidgetCreateInfo* info)
{
	FtkWidget* widget = NULL;

	widget = ftk_entry_create(info->parent, info->x, info->y, info->w, info->h);
	if(info->value != NULL)
	{
		ftk_entry_set_text(widget, ftk_xul_translate_text(info->priv->callbacks, info->value));
	}

    if(info->tips != NULL)
    {
        ftk_entry_set_tips(widget, ftk_xul_translate_text(info->priv->callbacks, info->tips));
    }

    if(info->type > 0)
    {
        ftk_entry_set_input_type(widget, info->type);
    }

	return widget;
}

static FtkWidget* ftk_xul_wait_box_create(FtkWidgetCreateInfo* info)
{
	FtkWidget* widget = NULL;

	widget = ftk_wait_box_create(info->parent, info->x, info->y, 0, 0);
	if(info->value != NULL && atoi(info->value) > 0)
	{
		ftk_wait_box_start_waiting(widget);
	}

	return widget;
}

static FtkWidget* ftk_xul_progress_bar_create(FtkWidgetCreateInfo* info)
{
	FtkWidget* widget = NULL;
	int value = 0;

	widget = ftk_progress_bar_create(info->parent, info->x, info->y, info->w, info->h);
    if(info->value != NULL && (value = atoi(info->value)) > 0)
	{
		ftk_progress_bar_set_percent(widget, value);
	}

    if(info->tips != NULL)
    {
        ftk_widget_set_text(widget, ftk_xul_translate_text(info->priv->callbacks, info->tips));
    }

	return widget;
}

static FtkWidget* ftk_xul_group_box_create(FtkWidgetCreateInfo* info)
{
	FtkWidget* widget = NULL;

	widget = ftk_group_box_create(info->parent, info->x, info->y, info->w, info->h);
    if(info->value != NULL)
    {
        ftk_widget_set_text(widget, ftk_xul_translate_text(info->priv->callbacks, info->value));
    }

	return widget;
}

static FtkWidget* ftk_xul_radio_button_create(FtkWidgetCreateInfo* info)
{
	FtkWidget* widget = NULL;

	widget = ftk_check_button_create_radio(info->parent, info->x, info->y, info->w, info->h);
	if(info->value != NULL)
	{
		ftk_widget_set_text(widget, ftk_xul_translate_text(info->priv->callbacks, info->value));
	}
	
    if(info->icon_position > 0)
    {
        ftk_check_button_set_icon_position(widget, info->icon_position);
    }

	if(info->checked)
	{
		ftk_check_button_set_checked(widget, info->checked);
	}

	return widget;
}

static FtkWidget* ftk_xul_check_button_create(FtkWidgetCreateInfo* info)
{
	FtkWidget* widget = NULL;

	widget = ftk_check_button_create(info->parent, info->x, info->y, info->w, info->h);
	if(info->value != NULL)
	{
		ftk_widget_set_text(widget, ftk_xul_translate_text(info->priv->callbacks, info->value));
	}
	
	if(info->icon_position > 0)
	{
	    ftk_check_button_set_icon_position(widget, info->icon_position);
	}

	if(info->checked)
	{
		ftk_check_button_set_checked(widget, info->checked);
	}

	return widget;
}

static FtkWidget* ftk_xul_image_create(FtkWidgetCreateInfo* info)
{
	FtkWidget* widget = NULL;

	widget = ftk_image_create(info->parent, info->x, info->y, info->w, info->h);
	if(info->value != NULL)
	{
		ftk_image_set_image(widget, ftk_xul_load_image(info->priv->callbacks, info->value));
	}

	return widget;
}

static FtkWidget* ftk_xul_scroll_bar_create(FtkWidgetCreateInfo* info)
{
	FtkWidget* widget = NULL;
	int value = 0;

	widget = ftk_scroll_bar_create(info->parent, info->x, info->y, info->w, info->h);
    if(info->value != NULL && (value = atoi(info->value)) > 0)
	{
		ftk_scroll_bar_set_param(widget, value, info->max_value, info->page_delta);
	}

	return widget;
}

static FtkWidget* ftk_xul_list_view_create(FtkWidgetCreateInfo* info)
{
	FtkWidget* widget = NULL;

	widget = ftk_list_view_create(info->parent, info->x, info->y, info->w, info->h);
	
	return widget;
}

static FtkWidget* ftk_xul_icon_view_create(FtkWidgetCreateInfo* info)
{
	FtkWidget* widget = NULL;
    int value = 0;

	widget = ftk_icon_view_create(info->parent, info->x, info->y, info->w, info->h);
    if(info->value != NULL && (value = atoi(info->value)) > 0)
	{
		ftk_icon_view_set_item_size(widget, value);
	}

	return widget;
}

static FtkWidget* ftk_xul_icon_view_item_create(FtkWidgetCreateInfo* info)
{
    DECL_PRIV(info, priv);
    FtkIconViewItem item;

    item.text = (char *)info->value;
    item.icon = info->icon;
    item.user_data = NULL;

    ftk_icon_view_add(priv->current, &item);

    return NULL;
}

static FtkWidget* ftk_xul_window_create(FtkWidgetCreateInfo* info)
{
	FtkWidget* widget = NULL;

	widget = ftk_app_window_create();
	if(info->value != NULL)
	{
		ftk_widget_set_text(widget, ftk_xul_translate_text(info->priv->callbacks, info->value));
	}

	return widget;
}

static FtkWidget* ftk_xul_dialog_create(FtkWidgetCreateInfo* info)
{
	FtkWidget* widget = NULL;

	widget = ftk_dialog_create(info->x, info->y, info->w, info->h);
	if(info->value != NULL)
	{
		ftk_widget_set_text(widget, ftk_xul_translate_text(info->priv->callbacks, info->value));
	}

	if(info->hide_title)
	{
	    ftk_dialog_hide_title(widget);
	}

	if(info->icon != NULL)
	{
	    ftk_dialog_set_icon(widget, info->icon);
	}

	return widget;
}

static FtkWidget* ftk_xul_combo_box_create(FtkWidgetCreateInfo* info)
{
    FtkWidget* widget = NULL;

    widget = ftk_combo_box_create(info->parent, info->x, info->y, info->w, info->h);
    if(info->value != NULL)
    {
        ftk_combo_box_set_text(widget, ftk_xul_translate_text(info->priv->callbacks, info->value));
    }

    return widget;
}

static FtkWidget* ftk_xul_combo_box_item_create(FtkWidgetCreateInfo* info)
{
    DECL_PRIV(info, priv);

    ftk_combo_box_append(priv->current, info->icon, info->value);

    return NULL;
}

static FtkWidget* ftk_xul_file_browser_create(FtkWidgetCreateInfo* info)
{
    FtkWidget* widget = NULL;

    widget = ftk_file_browser_create(info->type);

    if(info->value != NULL)
    {
        ftk_file_browser_set_path(widget, info->value);
    }

    if(info->filter != NULL)
    {
        ftk_file_browser_set_filter(widget, info->filter);
    }

    return widget;
}

static FtkWidget* ftk_xul_menu_panel_create(FtkWidgetCreateInfo* info)
{
    FtkWidget* widget = NULL;

    widget = ftk_menu_panel_create();

    return widget;
}

static FtkWidget* ftk_xul_menu_item_create(FtkWidgetCreateInfo* info)
{
    DECL_PRIV(info, priv);
    FtkWidget* widget = NULL;

    widget = ftk_menu_item_create(info->parent);
    if(info->value != NULL)
    {
        ftk_widget_set_text(widget, ftk_xul_translate_text(priv->callbacks, info->value));
    }

    ftk_menu_panel_relayout(info->parent);

    return widget;
}

static FtkWidget* ftk_xul_painter_create(FtkWidgetCreateInfo* info)
{
    FtkWidget* widget = NULL;

    widget = ftk_painter_create(info->parent, info->x, info->y, info->w, info->h);

    return widget;
}

static FtkWidget* ftk_xul_status_item_create(FtkWidgetCreateInfo* info)
{
    FtkWidget* widget = NULL;

    widget = ftk_status_item_create(ftk_default_status_panel(), info->x, info->w);
    if(info->value != NULL)
    {
        ftk_widget_set_text(widget, ftk_xul_translate_text(info->priv->callbacks, info->value));
    }

    return widget;
}

static FtkWidget* ftk_xul_tab_create(FtkWidgetCreateInfo* info)
{
    FtkWidget* widget = NULL;

    widget = ftk_tab_create(info->parent, info->x, info->y, info->w, info->h);

    return widget;
}

static FtkWidget* ftk_xul_page_create(FtkWidgetCreateInfo* info)
{
    FtkWidget* widget = NULL;

    widget = ftk_tab_add_page(info->parent, info->value, info->icon);

    return widget;
}

static FtkWidget* ftk_xul_text_view_create(FtkWidgetCreateInfo* info)
{
    FtkWidget* widget = NULL;

    widget = ftk_text_view_create(info->parent, info->x, info->y, info->w, info->h);
    if(info->value != NULL)
    {
        const char *text = ftk_xul_translate_text(info->priv->callbacks, info->value);

        ftk_text_view_set_text(widget, text, strlen(text));
    }

    if(info->readonly)
    {
        ftk_text_view_set_readonly(widget, info->readonly);
    }

    return widget;
}

static const WidgetCreator s_widget_creaters[] = 
{
	{"label",              ftk_xul_label_create,               1},
	{"entry",              ftk_xul_entry_create,               1},
	{"button",             ftk_xul_button_create,              1},
	{"wait_box",           ftk_xul_wait_box_create,            1},
	{"progress_bar",       ftk_xul_progress_bar_create,        1},
	{"group_box",          ftk_xul_group_box_create,           1},
	{"radio_button",       ftk_xul_radio_button_create,        1},
	{"check_button",       ftk_xul_check_button_create,        1},
	{"image",              ftk_xul_image_create,               1},
	{"scroll_bar",         ftk_xul_scroll_bar_create,          1},
	{"list_view",          ftk_xul_list_view_create,           1},
	{"icon_view",          ftk_xul_icon_view_create,           1},
	{"icon_view_item",     ftk_xul_icon_view_item_create,      0},
	{"window",             ftk_xul_window_create,              1},
	{"dialog",             ftk_xul_dialog_create,              1},
    {"combo_box",          ftk_xul_combo_box_create,           1},
    {"combo_box_item",     ftk_xul_combo_box_item_create,      0},
    {"file_browser",       ftk_xul_file_browser_create,        1},
    {"menu_panel",         ftk_xul_menu_panel_create,          1},
    {"menu_item",          ftk_xul_menu_item_create,           1},
    {"painter",            ftk_xul_painter_create,             1},
    {"status_item",        ftk_xul_status_item_create,         1},
    {"tab",                ftk_xul_tab_create,                 1},
    {"page",               ftk_xul_page_create,                1},
    {"text_view",          ftk_xul_text_view_create,           1},
	{NULL, NULL},
};

static const WidgetCreator *ftk_xul_find_creator(const char* name)
{
	int i = 0;
	return_val_if_fail(name != NULL, NULL);

	for(i = 0; s_widget_creaters[i].name != NULL; i++)
	{
		if(strcmp(s_widget_creaters[i].name, name) == 0)
		{
			return &s_widget_creaters[i];
		}
	}

	return NULL;
}

typedef int (*FtkXulVarGetter)(FtkXmlBuilder* thiz);

typedef struct _VarGetter
{
	const char* name;
	FtkXulVarGetter get;
}VarGetter;

static int ftk_xul_builder_get_parent_width(FtkXmlBuilder* thiz)
{
	DECL_PRIV(thiz, priv);
	
	return ftk_widget_width(priv->current);
}

static int ftk_xul_builder_get_parent_height(FtkXmlBuilder* thiz)
{
	DECL_PRIV(thiz, priv);

	return ftk_widget_height(priv->current);
}

static int ftk_xul_builder_get_window_width(FtkXmlBuilder* thiz)
{
	DECL_PRIV(thiz, priv);

	return ftk_widget_width(priv->root);
}

static int ftk_xul_builder_get_window_height(FtkXmlBuilder* thiz)
{
	DECL_PRIV(thiz, priv);

	return ftk_widget_height(priv->root);
}

static int ftk_xul_builder_get_display_width(FtkXmlBuilder* thiz)
{
	return ftk_display_width(ftk_default_display());
}

static int ftk_xul_builder_get_display_height(FtkXmlBuilder* thiz)
{
	return ftk_display_height(ftk_default_display());
}

static const VarGetter s_var_getters[] = 
{
	{"pw",             ftk_xul_builder_get_parent_width},
	{"parent_width",   ftk_xul_builder_get_parent_width},
	{"ph",             ftk_xul_builder_get_parent_height},
	{"parent_height",  ftk_xul_builder_get_parent_height},
	{"ww",             ftk_xul_builder_get_window_width},
	{"window_width",   ftk_xul_builder_get_window_width},
	{"wh",             ftk_xul_builder_get_window_height},
	{"window_height",  ftk_xul_builder_get_window_height},
	{"dw",             ftk_xul_builder_get_display_width},
	{"display_width",  ftk_xul_builder_get_display_width},
	{"dh",             ftk_xul_builder_get_display_height},
	{"display_height", ftk_xul_builder_get_display_height},
	{NULL, NULL} 
};

static int ftk_xul_find_getter(const char* name)
{
	int i = 0;
	return_val_if_fail(name != NULL, 0);

	for(i = 0; s_var_getters[i].name != NULL; i++)
	{
		if(strncmp(s_var_getters[i].name, name, strlen(s_var_getters[i].name)) == 0)
		{
			return i;
		}
	}

	return -1;
}

typedef struct _VarConst
{
	const char* name;
	int value;
}VarConst;

static const VarConst s_var_conts[] =
{
	{"FTK_ATTR_TRANSPARENT",     FTK_ATTR_TRANSPARENT},
	{"FTK_ATTR_IGNORE_CLOSE",    FTK_ATTR_IGNORE_CLOSE},
	{"FTK_ATTR_BG_CENTER",       FTK_ATTR_BG_CENTER},
	{"FTK_ATTR_BG_TILE",         FTK_ATTR_BG_TILE},
	{"FTK_ATTR_BG_FOUR_CORNER",  FTK_ATTR_BG_FOUR_CORNER},
	{"FTK_ATTR_NO_FOCUS",        FTK_ATTR_NO_FOCUS},
	{"FTK_ATTR_INSENSITIVE",     FTK_ATTR_INSENSITIVE},
	{"FTK_ATTR_FOCUSED",         FTK_ATTR_FOCUSED},
	{"FTK_ATTR_QUIT_WHEN_CLOSE", FTK_ATTR_QUIT_WHEN_CLOSE},
	{"FTK_ATTR_FULLSCREEN",      FTK_ATTR_FULLSCREEN},

    {"FTK_ALIGN_LEFT",           FTK_ALIGN_LEFT},
    {"FTK_ALIGN_RIGHT",          FTK_ALIGN_RIGHT},
    {"FTK_ALIGN_CENTER",         FTK_ALIGN_CENTER},

    {"FTK_INPUT_NORMAL",         FTK_INPUT_NORMAL},
    {"FTK_INPUT_DIGIT",          FTK_INPUT_DIGIT},
    {"FTK_INPUT_ALPHA",          FTK_INPUT_ALPHA},
    {"FTK_INPUT_URL",            FTK_INPUT_URL},
    {"FTK_INPUT_EMAIL",          FTK_INPUT_EMAIL},
    {"FTK_INPUT_ALL",            FTK_INPUT_ALL},

    {"FTK_FILE_BROWER_APP",                     FTK_FILE_BROWER_APP},
    {"FTK_FILE_BROWER_SINGLE_CHOOSER",          FTK_FILE_BROWER_SINGLE_CHOOSER},
    {"FTK_FILE_BROWER_MULTI_CHOOSER",           FTK_FILE_BROWER_MULTI_CHOOSER},

	{NULL, 0},
};

static int ftk_xul_find_const(const char* name)
{
	int i = 0;
	for(i = 0; s_var_conts[i].name != NULL; i++)
	{
		if(strncmp(s_var_conts[i].name, name, strlen(s_var_conts[i].name)) == 0)
		{
			return i;
		}
	}

	return -1;
}

static int ftk_xul_find_const_value(const char* name)
{
    int i = 0;

    if (name[0] == '$')
    {
        name++;
    }
    i = ftk_xul_find_const(name);
    if (i < 0)
    {
        return 0;
    }
    else
    {
        return s_var_conts[i].value;
    }
}

static const char* ftk_xul_builder_preprocess_value(FtkXmlBuilder* thiz, const char* value)
{
	int i = 0;
	int dst = 0;
	DECL_PRIV(thiz, priv);
	const char* iter = NULL;
	return_val_if_fail(value != NULL, NULL);

	for(iter = value; *iter && dst < sizeof(priv->processed_value); iter++)
	{
		if(*iter == '$')
		{
			char value[32] = {0};
			int len = sizeof(priv->processed_value)-dst;
			if((i = ftk_xul_find_getter(iter+1)) >= 0)
			{
				ftk_itoa(value, sizeof(value), s_var_getters[i].get(thiz));
				if(len > (int)strlen(value))
				{
					ftk_strcpy(priv->processed_value+dst, value);
					dst += strlen(priv->processed_value+dst);
					iter += strlen(s_var_getters[i].name);
				}
				continue;
			}
			else if((i = ftk_xul_find_const(iter+1)) >= 0)
			{
				ftk_itoa(value, sizeof(value), s_var_conts[i].value);
				if(len > (int)strlen(value))
				{
					ftk_strcpy(priv->processed_value+dst, value);
					dst += strlen(priv->processed_value+dst);
					iter += strlen(s_var_conts[i].name);
				}
				continue;
			}
		}

		priv->processed_value[dst++] = *iter;
	}
	priv->processed_value[dst] = '\0';

	return priv->processed_value;
}

static void ftk_xul_builder_init_widget_info(FtkXmlBuilder* thiz, const char** attrs, FtkWidgetCreateInfo* info)
{
	int i = 0;
	const char* name = NULL;
	const char* value = NULL;
	DECL_PRIV(thiz, priv);
	return_if_fail(attrs != NULL && info != NULL);

	info->priv = priv;
	info->parent = priv->current;
	for(i = 0; attrs[i] != NULL; i += 2)
	{
		name = attrs[i];
		value = attrs[i+1];

		switch(name[0])
		{
			case 'i':
			{
	            if(name[1] == 'd')
	            {
	                /*id*/
	                info->id = atoi(value);
	            }
	            else if(name[1] == 'c')
	            {
	                if(name[4] == '\0')
	                {
	                    /*icon*/
	                    info->icon = ftk_xul_load_image(info->priv->callbacks, value);
	                }
	                else if (name[4] == '_')
	                {
	                    /*icon_position*/
	                    info->icon_position = atoi(value);
	                }
	            }
	            break;
			}
			case 'x':
			{
				value = ftk_xul_builder_preprocess_value(thiz, value);
				info->x = (int)ftk_expr_eval(value);
				break;
			}
			case 'y':
			{
				value = ftk_xul_builder_preprocess_value(thiz, value);
				info->y = (int)ftk_expr_eval(value);
				break;
			}
			case 'w':
			{
				/*width*/
				value = ftk_xul_builder_preprocess_value(thiz, value);
				info->w = (int)ftk_expr_eval(value);
				break;
			}
			case 'h':
			{
	            if(name[1] == '\0' || name[1] == 'e')
	            {
                    /*height*/
                    value = ftk_xul_builder_preprocess_value(thiz, value);
                    info->h = (int)ftk_expr_eval(value);
	            }
	            else if(name[1] == 'i')
	            {
	                /*hide_title*/
	                info->hide_title = atoi(value);
	            }
				break;
			}
			case 'a':
			{
				if(name[1] == 't')
				{
					/*attr*/
					info->attr = ftk_xul_find_const_value(value);
				}
				else if(name[1] == 'n')
				{
					/*anim_hint*/
					value = ftk_xul_builder_preprocess_value(thiz, value);
					ftk_strncpy(info->anim_hint, value, sizeof(info->anim_hint)-1);
				}
				else if (name[1] == 'l')
				{
				    /*alignment*/
                    info->alignment = ftk_xul_find_const_value(value);
				}
				break;
			}
			case 'v':
			{
				if(name[1] == 'a')
				{
					/*value*/
					info->value = value;
				}
				else if(name[1] == 'i')
				{
					/*visiable*/
					info->visible = atoi(value);
				}
				break;
			}
			case 'm':
			{
				/*max*/
				info->max_value = atoi(value);
				break;
			}
			case 'p':
			{
                /*page_delta*/
                info->page_delta = atoi(value);
				break;
			}
			case 'c':
			{
				/*checked*/
				info->checked = atoi(value);
				break;
			}
			case 'b':
			{
				if(strcmp(name, "bg[normal]") == 0)
				{
					info->gc[FTK_WIDGET_NORMAL].mask |= FTK_GC_BG;
					info->gc[FTK_WIDGET_NORMAL].bg = ftk_parse_color(value);
				}
				else if(strcmp(name, "bg[active]") == 0)
				{
					info->gc[FTK_WIDGET_ACTIVE].mask |= FTK_GC_BG;
					info->gc[FTK_WIDGET_ACTIVE].bg = ftk_parse_color(value);
				}
				else if(strcmp(name, "bg[focused]") == 0)
				{
					info->gc[FTK_WIDGET_FOCUSED].mask |= FTK_GC_BG;
					info->gc[FTK_WIDGET_FOCUSED].bg = ftk_parse_color(value);
				}
				else if(strcmp(name, "bg[disable]") == 0)
				{
					info->gc[FTK_WIDGET_INSENSITIVE].mask |= FTK_GC_BG;
					info->gc[FTK_WIDGET_INSENSITIVE].bg = ftk_parse_color(value);
				}
				else if(strcmp(name, "bg_image[normal]") == 0)
				{
					info->gc[FTK_WIDGET_NORMAL].mask |= FTK_GC_BITMAP;
					info->gc[FTK_WIDGET_NORMAL].bitmap = ftk_xul_load_image(info->priv->callbacks,value);
				}
				else if(strcmp(name, "bg_image[disable]") == 0)
				{
					info->gc[FTK_WIDGET_INSENSITIVE].mask |= FTK_GC_BITMAP;
					info->gc[FTK_WIDGET_INSENSITIVE].bitmap = ftk_xul_load_image(info->priv->callbacks,value);
				}
				else if(strcmp(name, "bg_image[active]") == 0)
				{
					info->gc[FTK_WIDGET_ACTIVE].mask |= FTK_GC_BITMAP;
					info->gc[FTK_WIDGET_ACTIVE].bitmap = ftk_xul_load_image(info->priv->callbacks,value);
				}
				else if(strcmp(name, "bg_image[focused]") == 0)
				{
					info->gc[FTK_WIDGET_FOCUSED].mask |= FTK_GC_BITMAP;
					info->gc[FTK_WIDGET_FOCUSED].bitmap = ftk_xul_load_image(info->priv->callbacks,value);
				}
				else
				{
					ftk_logd("%s: unknown %s\n", __func__, name);
				}
				break;
			}
			case 'f':
			{
				if(strcmp(name, "fg[normal]") == 0)
				{
					info->gc[FTK_WIDGET_NORMAL].mask |= FTK_GC_FG;
					info->gc[FTK_WIDGET_NORMAL].fg = ftk_parse_color(value);
				}
				else if(strcmp(name, "fg[active]") == 0)
				{
					info->gc[FTK_WIDGET_ACTIVE].mask |= FTK_GC_FG;
					info->gc[FTK_WIDGET_ACTIVE].fg = ftk_parse_color(value);
				}
				else if(strcmp(name, "fg[focused]") == 0)
				{
					info->gc[FTK_WIDGET_FOCUSED].mask |= FTK_GC_FG;
					info->gc[FTK_WIDGET_FOCUSED].fg = ftk_parse_color(value);
				}
				else if(strcmp(name, "fg[disable]") == 0)
				{
					info->gc[FTK_WIDGET_INSENSITIVE].mask |= FTK_GC_FG;
					info->gc[FTK_WIDGET_INSENSITIVE].fg = ftk_parse_color(value);
				}
				else if(strcmp(name, "filter") == 0)
				{
				    info->filter = value;
				}
                else if(strcmp(name, "font") == 0)
                {
                    info->font = value;
                }
				else
				{
					ftk_logd("%s: unknown %s\n", __func__, name);
				}
				break;
			}

			case 't':
			{
			    if (name[1] == 'i')
			    {
                    /*tips*/
                    info->tips = value;
			    }
			    else if (name[1] == 'y')
			    {
			        /*type*/
                    info->type = ftk_xul_find_const_value(value);
			    }
			    break;
			}

			default:break;/*TODO: handle other attrs*/
		}
	}

	return;
}

static void ftk_xul_builder_reset_widget_info(FtkXmlBuilder* thiz, FtkWidgetCreateInfo* info)
{
	int i = 0;
	for(i = 0; i < FTK_WIDGET_STATE_NR;i++)
	{
		ftk_gc_reset(info->gc+i);
	}

	return;
}

static void ftk_xul_builder_on_start(FtkXmlBuilder* thiz, const char* tag, const char** attrs)
{
	DECL_PRIV(thiz, priv);
	FtkWidget* widget = NULL;
	FtkWidgetCreateInfo info = {0};
	const WidgetCreator* creator = ftk_xul_find_creator(tag);
	
	priv->meet_start_tag = 0;
	return_if_fail(creator != NULL && creator->create != NULL && attrs != NULL && thiz != NULL);

	ftk_xul_builder_init_widget_info(thiz, attrs, &info);
	if (creator->is_widget)
	{
	    if (!priv->prev_is_widget && priv->current != NULL)
	    {
	        info.parent = ftk_widget_parent(priv->current);
	    }

        if((widget = creator->create(&info)) != NULL)
        {
            ftk_widget_set_id(widget, info.id);
            ftk_widget_set_attr(widget, info.attr);
            if(info.gc[FTK_WIDGET_NORMAL].mask & FTK_GC_BG)
            {
                ftk_widget_unset_attr(widget, FTK_ATTR_TRANSPARENT);
            }
            ftk_widget_set_gc(widget, FTK_WIDGET_NORMAL, info.gc+FTK_WIDGET_NORMAL);
            ftk_widget_set_gc(widget, FTK_WIDGET_FOCUSED, info.gc+FTK_WIDGET_FOCUSED);
            ftk_widget_set_gc(widget, FTK_WIDGET_ACTIVE, info.gc+FTK_WIDGET_ACTIVE);
            ftk_widget_set_gc(widget, FTK_WIDGET_INSENSITIVE, info.gc+FTK_WIDGET_INSENSITIVE);

            if(info.anim_hint[0])
            {
                ftk_window_set_animation_hint(widget, info.anim_hint);
            }

            if(info.font != NULL)
            {
                ftk_widget_set_font(widget, info.font);
            }

            ftk_widget_show(widget, info.visible);
        }
        ftk_xul_builder_reset_widget_info(thiz, &info);

        return_if_fail(widget != NULL);

        priv->current = widget;
        priv->meet_start_tag = 1;
        priv->prev_is_widget = 1;

        if(priv->root == NULL)
        {
            priv->root = widget;
        }
	}
	else
	{
	    creator->create(&info);

        ftk_xul_builder_reset_widget_info(thiz, &info);

        priv->meet_start_tag = 0;

        priv->prev_is_widget = 0;
	}

    return;
}

static void ftk_xul_builder_on_end(FtkXmlBuilder* thiz, const char* tag)
{
	DECL_PRIV(thiz, priv);

	if(priv->meet_start_tag)
	{
		priv->current = ftk_widget_parent(priv->current);
	}

	return;
}

static void ftk_xul_builder_destroy(FtkXmlBuilder* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(FtkXmlBuilder) + sizeof(PrivInfo));
	}

	return;
}

static FtkXmlBuilder* ftk_xul_builder_create(void)
{
	FtkXmlBuilder* thiz = FTK_NEW_PRIV(FtkXmlBuilder);

	if(thiz != NULL)
	{
		thiz->on_start_element = ftk_xul_builder_on_start;
		thiz->on_end_element   = ftk_xul_builder_on_end;
		thiz->destroy          = ftk_xul_builder_destroy;
	}

	return thiz;
}

static const char* ftk_text_no_translate(void* ctx, const char* text)
{
	return text;
}

static FtkBitmap* ftk_default_load_image(void* ctx, const char* filename)
{
	return ftk_bitmap_factory_load(ftk_default_bitmap_factory(), filename);
}

static const FtkXulCallbacks default_callbacks = 
{
	NULL,
	ftk_text_no_translate,
	ftk_default_load_image
};

FtkWidget* ftk_xul_load_ex(const char* xml, int length, FtkXulCallbacks* callbacks)
{
	FtkWidget* widget = NULL;
	FtkXmlParser* parser = NULL;
	FtkXmlBuilder* builder = NULL;
	return_val_if_fail(xml != NULL, NULL);

	parser  = ftk_xml_parser_create();
	return_val_if_fail(parser != NULL, NULL);

	builder = ftk_xul_builder_create();
	if(builder != NULL)
	{
		PrivInfo* priv = (PrivInfo*)builder->priv;
		priv->callbacks = callbacks;
		priv->prev_is_widget = 0;
		ftk_xml_parser_set_builder(parser, builder);
		ftk_xml_parser_parse(parser, xml, length);
		widget = priv->root;
		
	}
	ftk_xml_builder_destroy(builder);
	ftk_xml_parser_destroy(parser);

	return widget;
}

FtkWidget* ftk_xul_load(const char* xml, int length)
{
	return ftk_xul_load_ex(xml, length, (FtkXulCallbacks*)&default_callbacks);	
}

FtkWidget* ftk_xul_load_file(const char* filename, FtkXulCallbacks* callbacks)
{
	FtkMmap* m = NULL;
	FtkWidget* widget = NULL;
	return_val_if_fail(filename != NULL, NULL);

	if((m = ftk_mmap_create(filename, 0, -1)) != NULL)
	{
		widget = ftk_xul_load_ex((const char*)ftk_mmap_data(m), ftk_mmap_length(m), callbacks);
		ftk_mmap_destroy(m);
	}

	ftk_logd("%s: %p %s\n", __func__, widget, filename);

	return widget;
}

