/*
 * File: ftk_key_board.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   soft key_board
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
 * 2011-08-04 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_util.h"
#include "ftk_theme.h"
#include "ftk_mmap.h"
#include "ftk_window.h"
#include "ftk_globals.h"
#include "ftk_key_board.h"
#include "ftk_xml_parser.h"
#include "ftk_xml_builder.h"

struct _FtkKeyBoardCell;
typedef struct _FtkKeyBoardCell FtkKeyBoardCell;

typedef Ret (*FtkCellAction)(FtkWidget* keyboard, FtkKeyBoardCell* cell);

#define FTK_MAX_CANDIDATE_NR 32
#define FTK_MAX_CANDIDATE_LEN 32

struct _FtkKeyBoardCell
{
	size_t x;
	size_t width;/*in pixels*/
	size_t width_weight;
	char text[8];
	char action_str[16];
	char action_args[16];
	FtkCellAction action;
	FtkBitmap* icon;
	FtkBitmap* bg_normal;
	FtkBitmap* bg_active;
};

typedef struct _FtkKeyBoardRow
{
	size_t y;
	size_t height;/*in pixels*/
	size_t height_weight;
	size_t cell_nr;
	size_t current_cell;
	FtkKeyBoardCell* cells;
}FtkKeyBoardRow;

typedef struct _FtkKeyBoardView
{
	size_t row_nr;
	size_t current_row;
	FtkKeyBoardRow* rows;

	FtkBitmap* bg;
	FtkBitmap* cell_bg_normal;
	FtkBitmap* cell_bg_active;
	FtkPoint last_click_pos;
	FtkKeyBoardCell* focus;
}FtkKeyBoardView;

typedef struct _FtkKeyBoardCandidate
{
	char text[FTK_MAX_CANDIDATE_LEN];
	size_t extent;
}FtkKeyBoardCandidate;

typedef struct _FtkKeyBoardDesc
{
	size_t min_width;
	size_t min_height;
	size_t view_nr;
	size_t current_view;
	FtkKeyBoardView* views;
	FtkKeyBoardCellAction custom_action;

	size_t candidate_nr;
	size_t candidate_start;
	size_t candidate_focus;
	FtkKeyBoardCandidate candidates[FTK_MAX_CANDIDATE_NR];
	FtkBitmap* candidates_vertical_line;
	size_t candidate_space;
}FtkKeyBoardDesc;

typedef struct _KeyBoardPrivInfo
{
	FtkWidget* editor;
	FtkKeyBoardDesc* desc;
}PrivInfo;

typedef struct _BuilderInfo
{
	FtkKeyBoardDesc* desc;
}BuilderInfo;

static void ftk_key_board_desc_destroy(FtkKeyBoardDesc* desc);

static Ret ftk_key_board_choose_input_method(FtkWidget* thiz, FtkKeyBoardCell* cell)
{
	const char* im = cell->action_args;
	int im_index = ftk_atoi(im);

	ftk_input_method_manager_set_current(ftk_default_input_method_manager(), im_index);

	return RET_OK;
}

static Ret ftk_key_board_send_key(FtkWidget* thiz, FtkKeyBoardCell* cell)
{
	FtkEvent event;
	const char* key = NULL;
	FtkKey code = FTK_KEY_0;

	key = cell->action_args;
	if(strlen(key) == 1)
	{
		code = (FtkKey)key[0];
	}
	else
	{
		/*FIXME*/
		if(strcmp(key, "BACKSPACE") == 0)
		{
			code = FTK_KEY_BACKSPACE;
		}
		else if(strcmp(key, "DELETE") == 0)
		{
			code = FTK_KEY_DELETE;
		}
		else if(strcmp(key, "ENTER") == 0)
		{
			code = FTK_KEY_ENTER;
		}
		else if(strcmp(key, "LEFT") == 0)
		{
		    code = FTK_KEY_LEFT;
		}
		else if(strcmp(key, "RIGHT") == 0)
		{
		    code = FTK_KEY_RIGHT;
		}
		else if(strcmp(key, "CLR") == 0)
		{
		    code = FTK_KEY_CLR;
		}
		else
		{
			assert(!"not supported key.");
		}
	}

	ftk_event_init(&event, FTK_EVT_KEY_DOWN);
	event.u.key.code = code;
	ftk_wnd_manager_dispatch_event(ftk_default_wnd_manager(), &event);
	
	ftk_event_init(&event, FTK_EVT_KEY_UP);
	event.u.key.code = code;
	ftk_wnd_manager_dispatch_event(ftk_default_wnd_manager(), &event);

	ftk_logd("%s:%d %s\n", __func__, __LINE__, cell->text);

	return RET_OK;
}

static Ret ftk_key_board_switch_view(FtkWidget* thiz, FtkKeyBoardCell* cell)
{
	DECL_PRIV0(thiz, priv);
	FtkKeyBoardDesc* desc = priv->desc;
	const char* im_str = strchr(cell->action_args, ',');
	
	desc->current_view = ftk_atoi(cell->action_args);

	/*The second args is optional, means input method.*/
	if(im_str != NULL)
	{
		int im_index = ftk_atoi(im_str + 1);
		ftk_input_method_manager_set_current(ftk_default_input_method_manager(), im_index);
		ftk_logd("%s: input method change to %d\n", __func__, im_index);
	}
	ftk_widget_invalidate(thiz);

	ftk_logd("%s:%d %s %s\n", __func__, __LINE__, cell->text, cell->action_args);

	return RET_OK;
}

static Ret ftk_key_board_candidate_prev(FtkWidget* thiz, FtkKeyBoardCell* cell)
{
	//DECL_PRIV0(thiz, priv);
	//FtkKeyBoardDesc* desc = priv->desc;
	//FtkKeyBoardCandidate* candidate = NULL;

	/*TODO*/

	return RET_OK;
}

static Ret ftk_key_board_candidate_text(FtkWidget* thiz, FtkKeyBoardCell* cell)
{
	FtkEvent event;
	DECL_PRIV0(thiz, priv);
	FtkKeyBoardDesc* desc = priv->desc;
	FtkKeyBoardCandidate* candidate = NULL;
	return_val_if_fail(desc->candidate_focus < desc->candidate_nr, RET_FAIL);

	if(priv->editor != NULL)
	{
		candidate = desc->candidates + desc->candidate_focus;

		ftk_event_init(&event,     FTK_EVT_IM_COMMIT);
		event.widget   = priv->editor;
		event.u.extra  = (char*)candidate->text;
		ftk_widget_event(priv->editor, &event);
	}

	ftk_logd("%s:%d %s\n", __func__, __LINE__, cell->text);

	return RET_OK;
}

static Ret ftk_key_board_candidate_next(FtkWidget* thiz, FtkKeyBoardCell* cell)
{
	/*TODO*/
	ftk_logd("%s:%d %s\n", __func__, __LINE__, cell->text);

	return RET_OK;
}

static Ret ftk_key_board_call_custom_action(FtkWidget* thiz, FtkKeyBoardCell* cell)
{
	DECL_PRIV0(thiz, priv);
	FtkKeyBoardDesc* desc = priv->desc;
	return_val_if_fail(desc != NULL, RET_FAIL);

	if(desc->custom_action != NULL)
	{
		desc->custom_action(thiz, cell->text, cell->action_args);
	}

	ftk_logd("%s:%d %s\n", __func__, __LINE__, cell->text);

	return RET_OK;
}

static Ret ftk_key_board_cell_init_action(FtkKeyBoardCell* cell, const char* action_str)
{
	char* p = NULL;
	char* args = NULL;
	char* action = ftk_strdup(action_str);
	return_val_if_fail(action != NULL, RET_FAIL);

	p = strrchr(action, ')');
	return_val_if_fail(p != NULL, RET_FAIL);
	*p = '\0';
	
	args = strchr(action, '(');
	return_val_if_fail(args != NULL, RET_FAIL);
	*args = '\0';
	args += 1;

	ftk_strncpy(cell->action_str, action, sizeof(cell->action_str));
	ftk_strncpy(cell->action_args, args, sizeof(cell->action_args));
	if(strcmp(action, "send_key") == 0)
	{
		cell->action = ftk_key_board_send_key;
	}
	else if(strcmp(action, "switch_view") == 0)
	{
		cell->action = ftk_key_board_switch_view;
	}
	else if(strcmp(action, "choose_input_method") == 0)
	{
		cell->action = ftk_key_board_choose_input_method;
	}
	else if(strcmp(action, "call_custom") == 0)
	{
		cell->action = ftk_key_board_call_custom_action;
	}
	else if(strcmp(action, "candidate") == 0)
	{
		if(strcmp(args, "prev") == 0)
		{
			cell->action = ftk_key_board_candidate_prev;
		}
		else if(strcmp(args, "text") == 0)
		{
			cell->action = ftk_key_board_candidate_text;
		}
		else if(strcmp(args, "next") == 0)
		{
			cell->action = ftk_key_board_candidate_next;
		}
		else
		{
			assert(!"invalid args.");
		}
	}
	else
	{
		ftk_logw("%s:%d illegal action: %s\n", __func__, __LINE__, action_str);
	}

	FTK_FREE(action);

	return RET_OK;
}

static void ftk_key_board_desc_builder_on_start(FtkXmlBuilder* thiz, const char* tag, const char** attrs)
{
	size_t i = 0;
	const char* key = NULL;
	const char* value = NULL;
	BuilderInfo* info = (BuilderInfo*)thiz->priv;
	FtkKeyBoardDesc* desc = info->desc;

	if(strcmp(tag, "keyboard") == 0)
	{
		for(i = 0; attrs[i] != NULL; i += 2)
		{
			key = attrs[i];
			value = attrs[i+1];
			if(strcmp(key, "min_width") == 0)
			{
				desc->min_width = ftk_atoi(value);
			}
			else if(strcmp(key, "min_height") == 0)
			{
				desc->min_height = ftk_atoi(value);
			}
			else if(strcmp(key, "views") == 0)
			{
				desc->view_nr = ftk_atoi(value);
			}
			else
			{
				ftk_logw("%s:%d unknown attr: %s\n", __func__, __LINE__, key);
			}
		}
		assert(desc->view_nr > 0);
		desc->current_view = 0;
		desc->views = (FtkKeyBoardView*)FTK_ZALLOC(desc->view_nr * sizeof(FtkKeyBoardView));
	}
	else if(strcmp(tag, "view") == 0)
	{
		FtkKeyBoardView* view = desc->views + desc->current_view;

		for(i = 0; attrs[i] != NULL; i += 2)
		{
			key = attrs[i];
			value = attrs[i+1];
			if(strcmp(key, "rows") == 0)
			{
				view->row_nr = ftk_atoi(value);
			}
			else if(strcmp(key, "bg") == 0)
			{
				view->bg = ftk_theme_load_image(ftk_default_theme(), value);
			}
			else if(strcmp(key, "cell_bg_image[normal]") == 0)
			{
				view->cell_bg_normal = ftk_theme_load_image(ftk_default_theme(), value);
			}
			else if(strcmp(key, "cell_bg_image[active]") == 0)
			{
				view->cell_bg_active = ftk_theme_load_image(ftk_default_theme(), value);
			}
			else
			{
				ftk_logw("%s:%d unknown attr: %s\n", __func__, __LINE__, key);
			}
		}

		assert(view->row_nr > 0);
		view->current_row = 0;
		view->rows = (FtkKeyBoardRow*)FTK_ZALLOC(view->row_nr * sizeof(FtkKeyBoardRow));
	}
	else if(strcmp(tag, "row") == 0)
	{
		FtkKeyBoardView* view = desc->views + desc->current_view;
		FtkKeyBoardRow* row = view->rows + view->current_row;

		for(i = 0; attrs[i] != NULL; i += 2)
		{
			key = attrs[i];
			value = attrs[i+1];
			if(strcmp(key, "cols") == 0)
			{
				row->cell_nr = ftk_atoi(value);
			}
			else if(strcmp(key, "height_weight") == 0)
			{
				row->height_weight = ftk_atoi(value);
			}
			else
			{
				ftk_logw("%s:%d unknown attr: %s\n", __func__, __LINE__, key);
			}
		}
		assert(row->cell_nr > 0);
		row->cells = (FtkKeyBoardCell*)FTK_ZALLOC(sizeof(FtkKeyBoardCell) * row->cell_nr);
	}
	else if(strcmp(tag, "spacer") == 0)
	{
		FtkKeyBoardView* view = desc->views + desc->current_view;
		FtkKeyBoardRow* row = view->rows + view->current_row;
		FtkKeyBoardCell* cell = row->cells + row->current_cell;

		for(i = 0; attrs[i] != NULL; i += 2)
		{
			key = attrs[i];
			value = attrs[i+1];

			if(strcmp(key, "width_weight") == 0)
			{
				cell->width_weight = ftk_atoi(value);
			}
		}
	}
	else if(strcmp(tag, "cell") == 0)
	{
		FtkKeyBoardView* view = desc->views + desc->current_view;
		FtkKeyBoardRow* row = view->rows + view->current_row;
		FtkKeyBoardCell* cell = row->cells + row->current_cell;

		for(i = 0; attrs[i] != NULL; i += 2)
		{
			key = attrs[i];
			value = attrs[i+1];
			if(strcmp(key, "text") == 0)
			{
				ftk_strncpy(cell->text, value, sizeof(cell->text));
			}
			else if(strcmp(key, "action") == 0)
			{
				ftk_key_board_cell_init_action(cell, value);
			}
			else if(strcmp(key, "width_weight") == 0)
			{
				cell->width_weight = ftk_atoi(value);
			}
			else if(strcmp(key, "icon") == 0)
			{
				cell->icon = ftk_theme_load_image(ftk_default_theme(), value);
			}
			else if(strcmp(key, "bg_image[normal]") == 0)
			{
				cell->bg_normal = ftk_theme_load_image(ftk_default_theme(), value);
			}
			else if(strcmp(key, "bg_image[active]") == 0)
			{
				cell->bg_active = ftk_theme_load_image(ftk_default_theme(), value);
			}
			else
			{
				ftk_logw("%s:%d unknown attr: %s\n", __func__, __LINE__, key);
			}
		}

		if(cell->bg_normal == NULL)
		{
			cell->bg_normal = view->cell_bg_normal;
			ftk_bitmap_ref(cell->bg_normal);
		}

		if(cell->bg_active == NULL)
		{
			cell->bg_active = view->cell_bg_active;
			ftk_bitmap_ref(cell->bg_active);
		}
	}
	else
	{
		ftk_logw("%s:%d unknown tag: %s\n", __func__, __LINE__, tag);
	}

	return;
}

static void ftk_key_board_desc_builder_on_end(FtkXmlBuilder* thiz, const char* tag)
{
	BuilderInfo* info = (BuilderInfo*)thiz->priv;
	FtkKeyBoardDesc* desc = info->desc;

	if(strcmp(tag, "view") == 0)
	{
		desc->current_view++;
		assert(desc->current_view <= desc->view_nr);
	}
	else if(strcmp(tag, "row") == 0)
	{
		FtkKeyBoardView* view = desc->views + desc->current_view;
		view->current_row++;
		assert(view->current_row <= view->row_nr);
	}
	else if(strcmp(tag, "cell") == 0 || strcmp(tag, "spacer") == 0)
	{
		FtkKeyBoardView* view = desc->views + desc->current_view;
		FtkKeyBoardRow* row = view->rows + view->current_row;

		row->current_cell++;
		assert(row->current_cell <= row->cell_nr);
	}

	return;
}

static void ftk_key_board_desc_builder_destroy(FtkXmlBuilder* thiz)
{
	FTK_FREE(thiz);

	return;
}

static FtkXmlBuilder* ftk_key_board_desc_builder_create(void)
{
	FtkXmlBuilder* thiz = (FtkXmlBuilder*)FTK_ZALLOC(sizeof(FtkXmlBuilder) + sizeof(BuilderInfo));

	if(thiz != NULL)
	{
		thiz->on_start_element = ftk_key_board_desc_builder_on_start;
		thiz->on_end_element   = ftk_key_board_desc_builder_on_end;
		thiz->destroy          = ftk_key_board_desc_builder_destroy;
	}

	return thiz;
}

static Ret ftk_key_board_desc_load(FtkKeyBoardDesc* desc, const char* xml, size_t length)
{
	FtkXmlParser* parser = NULL;
	FtkXmlBuilder* builder = NULL;
	return_val_if_fail(desc != NULL && xml != NULL, RET_FAIL);

	parser  = ftk_xml_parser_create();
	return_val_if_fail(parser != NULL, RET_FAIL);

	builder = ftk_key_board_desc_builder_create();
	if(builder != NULL)
	{
		BuilderInfo* priv = (BuilderInfo*)builder->priv;
		priv->desc = desc;
		ftk_xml_parser_set_builder(parser, builder);
		ftk_xml_parser_parse(parser, xml, length);
	}
	ftk_xml_builder_destroy(builder);
	ftk_xml_parser_destroy(parser);

	desc->candidates_vertical_line = ftk_theme_load_image(ftk_default_theme(), 
		"key_board_candidates_vertical_line"FTK_STOCK_IMG_SUFFIX);
	desc->candidate_space = ftk_bitmap_width(desc->candidates_vertical_line) + 2;

	return RET_OK;
}

static FtkKeyBoardDesc* ftk_key_board_desc_create(const char* filename)
{
	FtkMmap* m = NULL;
	FtkKeyBoardDesc* desc = NULL;
	return_val_if_fail(filename != NULL, NULL);

	if((m = ftk_mmap_create(filename, 0, -1)) != NULL)
	{
		desc = (FtkKeyBoardDesc*)FTK_ZALLOC(sizeof(FtkKeyBoardDesc));
		ftk_key_board_desc_load(desc, (const char*)ftk_mmap_data(m), ftk_mmap_length(m));
		ftk_mmap_destroy(m);
	}

	return desc;
}

static void ftk_key_board_desc_dump(FtkKeyBoardDesc* desc)
{
	size_t v = 0;
	size_t r = 0;
	size_t c = 0;
	if(desc != NULL)
	{
		ftk_logd("<key_board min_width=\"%d\" min_height=\"%d\" views=\"%d\">\n",
			desc->min_width, desc->min_height, desc->view_nr);
		for(v = 0 ; v < desc->view_nr; v++)
		{
			FtkKeyBoardView* view = desc->views+v;
			ftk_logd("<view rows=\"%d\">\n", view->row_nr);
			for(r = 0; r < view->row_nr; r++)
			{
				FtkKeyBoardRow* row = view->rows+r;
				ftk_logd("<row cols=\"%d\" height_weight=\"%d\" y=\"%d\" height=\"%d\">\n", 
					row->cell_nr, row->height_weight, row->y, row->height);
				for(c = 0; c < row->cell_nr; c++)
				{
					FtkKeyBoardCell* cell = row->cells+c;

					if(cell->action == NULL)
					{
						ftk_logd("<spacer width_weight=\"%d\" x=\"%d\" width=\"%d\"/>\n", 
							cell->width_weight, cell->x, cell->width);
					}
					else
					{
						ftk_logd("<cell text=\"%s\" action=\"%s(%s)\" width_weight=\"%d\" x=\"%d\" width=\"%d\"/>\n",
							cell->text, cell->action_str, cell->action_args, cell->width_weight,
							cell->x, cell->width);
					}
				}
				ftk_logd("</row>\n");
			}
			ftk_logd("</view>\n");
		}
		ftk_logd("</key_board>\n");
	}

	return;
}

static void ftk_key_board_desc_layout(FtkKeyBoardDesc* desc, size_t width, size_t height)
{
	size_t x = 0;
	size_t y = 0;
	size_t v = 0;
	size_t r = 0;
	size_t c = 0;
	size_t total_height_weight = 0;
	size_t total_width_weight = 0;

	if(desc != NULL)
	{
		for(v = 0 ; v < desc->view_nr; v++)
		{
			FtkKeyBoardView* view = desc->views+v;
			for(r = 0, total_height_weight = 0; r < view->row_nr; r++)
			{
				FtkKeyBoardRow* row = view->rows+r;
				total_height_weight += row->height_weight;
			}

			y = 0;
			for(r = 0; r < view->row_nr; r++)
			{	
				FtkKeyBoardRow* row = view->rows+r;
				row->y = y;
				row->height = height * row->height_weight / total_height_weight;
				y += row->height;

				for(c = 0, total_width_weight = 0; c < row->cell_nr; c++)
				{
					FtkKeyBoardCell* cell = row->cells+c;
					total_width_weight += cell->width_weight;
				}

				x = 0;
				for(c = 0; c < row->cell_nr; c++)
				{
					FtkKeyBoardCell* cell = row->cells+c;
					
					cell->x = x;
					cell->width = cell->width_weight * width / total_width_weight;
					x += cell->width;
				}
			}
		}
	}

	return;
}

void ftk_key_board_test(const char* filename)
{
	FtkKeyBoardDesc* desc = ftk_key_board_desc_create(filename);
	ftk_key_board_desc_layout(desc, 240, 160);
	ftk_key_board_desc_destroy(desc);
	
	return;
}

static void ftk_key_board_desc_destroy(FtkKeyBoardDesc* desc)
{
	size_t v = 0;
	size_t r = 0;
	size_t c = 0;
	if(desc != NULL)
	{
		ftk_key_board_desc_dump(desc);

		for(v = 0 ; v < desc->view_nr; v++)
		{
			FtkKeyBoardView* view = desc->views+v;
			for(r = 0; r < view->row_nr; r++)
			{
				FtkKeyBoardRow* row = view->rows+r;
				for(c = 0; c < row->cell_nr; c++)
				{
					FtkKeyBoardCell* cell = row->cells+c;
					ftk_bitmap_unref(cell->icon);
					ftk_bitmap_unref(cell->bg_active);
					ftk_bitmap_unref(cell->bg_normal);
				}
				FTK_FREE(row->cells);
			}
			FTK_FREE(view->rows);
			ftk_bitmap_unref(view->bg);
			ftk_bitmap_unref(view->cell_bg_active);
			ftk_bitmap_unref(view->cell_bg_normal);
		}
		FTK_FREE(desc->views);
		ftk_bitmap_unref(desc->candidates_vertical_line);
		FTK_FREE(desc);
	}

	return;
}

static FtkKeyBoardCell* ftk_key_board_find_cell(FtkWidget* thiz, size_t x, size_t y)
{
	size_t r = 0;
	size_t c = 0;
	DECL_PRIV0(thiz, priv);
	FtkKeyBoardRow* row = NULL;
	FtkKeyBoardCell* cell = NULL;
	FtkKeyBoardView* view = priv->desc->views + priv->desc->current_view;

	for(r = 0; r < view->row_nr; r++)
	{
		row = view->rows + r;

		if(y < row->y || y >= (row->y + row->height)) continue;

		for(c = 0; c < row->cell_nr; c++)
		{
			cell = row->cells + c;
			if(x >= cell->x && x < (cell->x + cell->width))
			{
				return cell;
			}
		}
	}

	return NULL;
}

static Ret ftk_key_board_on_mouse_event(FtkWidget* thiz, FtkEventType event, size_t x, size_t y)
{
	DECL_PRIV0(thiz, priv);
	FtkKeyBoardCell* cell = NULL;
	FtkKeyBoardDesc* desc = priv->desc;	
	FtkKeyBoardView* view = desc->views + desc->current_view;

	x -= ftk_widget_left_abs(thiz);
	y -= ftk_widget_top_abs(thiz);

	view->last_click_pos.x = x;
	view->last_click_pos.y = y;
	cell = ftk_key_board_find_cell(thiz, x, y);

	if(cell == NULL)
	{
		return RET_OK;
	}

	if(cell->action == ftk_key_board_candidate_text)
	{
		size_t i = 0;
		size_t x_c = cell->x;

		for(i = desc->candidate_start; i < desc->candidate_nr; i++)
		{
			FtkKeyBoardCandidate* candidate = desc->candidates+i;
			if(x >= x_c && x < (x_c + candidate->extent))
			{
				desc->candidate_focus = i;
				ftk_logd("%s:%d on %s\n", __func__, __LINE__, candidate->text);
				break;
			}
			x_c += candidate->extent + desc->candidate_space;
		}
		ftk_widget_invalidate(thiz);
	}

	switch(event)
	{
		case FTK_EVT_MOUSE_DOWN:
		{
			view->focus = cell;
			ftk_window_grab(ftk_widget_toplevel(thiz), thiz);
			break;
		}	
		case FTK_EVT_MOUSE_UP:
		{
			if(cell->action != NULL)
			{
				cell->action(thiz, cell);
			}
		
			view->focus = NULL;
			desc->candidate_focus = -1;
			ftk_window_ungrab(ftk_widget_toplevel(thiz), thiz);
		}
		default:
		{
			if(view->focus == cell)
			{
				return RET_OK;
			}
			
			if(view->focus != NULL)
			{
				view->focus = cell;
			}

		}
	}
	
	if(cell->action != NULL)
	{
		ftk_widget_invalidate(thiz);
	}

	return RET_OK;
}

static Ret ftk_key_board_on_event(FtkWidget* thiz, FtkEvent* event)
{
	Ret ret = RET_OK;
	DECL_PRIV0(thiz, priv);

	switch(event->type)
	{
		case FTK_EVT_MOUSE_MOVE:
		case FTK_EVT_MOUSE_DOWN:
		case FTK_EVT_MOUSE_UP:
		{
			size_t x = event->u.mouse.x;
			size_t y = event->u.mouse.y;

			ret = ftk_key_board_on_mouse_event(thiz, event->type, x, y);

			break;
		}
		case FTK_EVT_RESIZE:
		case FTK_EVT_MOVE_RESIZE:
		{
			ftk_key_board_desc_layout(priv->desc, ftk_widget_width(thiz), ftk_widget_height(thiz));
			break;
		}
		default:break;
	}

	return ret;
}

static Ret ftk_key_board_on_paint(FtkWidget* thiz)
{
	size_t r = 0;
	size_t c = 0;
	size_t xoffset = 0;
	size_t yoffset = 0;
	DECL_PRIV0(thiz, priv);
	FtkKeyBoardView* view = NULL;
	FtkKeyBoardDesc* desc = priv->desc;
	FTK_BEGIN_PAINT(x, y, width, height, canvas);
	return_val_if_fail(desc != NULL && desc->view_nr > 0, RET_FAIL);

	ftk_canvas_reset_gc(canvas, ftk_widget_get_gc(thiz)); 
	view = desc->views+desc->current_view;
	
	for(r = 0; r < view->row_nr; r++)
	{
		FtkKeyBoardRow* row = view->rows+r;
		for(c = 0; c < row->cell_nr; c++)
		{
			FtkKeyBoardCell* cell = row->cells+c;

			width = cell->width;
			height = row->height;
			yoffset = row->y + y;
			xoffset = cell->x + x;

			if(cell->action == NULL)
			{
				/*It is a spacer*/
				continue;
			}
		
			/*Draw candidates */
			if(cell->action == ftk_key_board_candidate_text)
			{
				size_t i = 0;
				size_t x_c = 0;
				FtkBitmap* bmp = cell->bg_normal;
				ftk_canvas_draw_bg_image(canvas, bmp, FTK_BG_FOUR_CORNER, xoffset, yoffset, width, height);

				for(i = desc->candidate_start; i < desc->candidate_nr; i++)
				{
					FtkKeyBoardCandidate* candidate = desc->candidates+i;
					size_t extent = candidate->extent;

					if((x_c + extent) > width) break;

					if(i == desc->candidate_focus)
					{
						size_t bg_width = (extent + 1) & 0xfffe;
						ftk_canvas_draw_bg_image(canvas, cell->bg_active, FTK_BG_FOUR_CORNER, 
							xoffset + x_c, yoffset, bg_width, height);
					}

					ftk_canvas_draw_string(canvas, xoffset + x_c, yoffset + FTK_HALF(height), 
						candidate->text, strlen(candidate->text), 1);

					x_c += candidate->extent + 1;
					bmp = desc->candidates_vertical_line;
					ftk_canvas_draw_bg_image(canvas, bmp, FTK_BG_NORMAL, 
						xoffset + x_c, yoffset + 2, ftk_bitmap_width(bmp), height - 4); 
					x_c += ftk_bitmap_width(bmp) + 1;
				}
			}
			else
			{
				FtkBitmap* bg = (cell == view->focus) ? cell->bg_active : cell->bg_normal;
				ftk_canvas_draw_bg_image(canvas, bg, FTK_BG_FOUR_CORNER, xoffset, yoffset, width, height);

				if(cell->icon != NULL)
				{
					ftk_canvas_draw_bg_image(canvas, cell->icon, FTK_BG_CENTER, 
						xoffset, yoffset, width, height);
				}

				if(cell->text[0])
				{
					size_t len = strlen(cell->text);
					size_t extent = ftk_canvas_get_str_extent(canvas, cell->text, len);

					if(extent <= width)
					{
						ftk_canvas_draw_string(canvas,
							xoffset + FTK_HALF(width - extent),
							yoffset + FTK_HALF(height),
							cell->text, len, 1);
					}
					else
					{
						ftk_logw("%s:%d %s is too long to show\n", __func__, __LINE__, cell->text);
					}
				}
			}
		}
	}

	FTK_END_PAINT();
}

static void ftk_key_board_destroy(FtkWidget* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV0(thiz, priv);
		ftk_key_board_desc_destroy(priv->desc);
		FTK_ZFREE(priv, sizeof(PrivInfo));
	}

	return;
}

FtkWidget* ftk_key_board_create(FtkWidget* parent, int x, int y, int width, int height)
{
	FtkWidget* thiz = (FtkWidget*)FTK_ZALLOC(sizeof(FtkWidget));
	return_val_if_fail(thiz != NULL, NULL);
	
	thiz->priv_subclass[0] = (PrivInfo*)FTK_ZALLOC(sizeof(PrivInfo));
	if(thiz->priv_subclass[0] != NULL)
	{
		thiz->on_event = ftk_key_board_on_event;
		thiz->on_paint = ftk_key_board_on_paint;
		thiz->destroy  = ftk_key_board_destroy;

		ftk_widget_init(thiz, FTK_KEY_BOARD, 0, x, y, width, height, FTK_ATTR_BG_FOUR_CORNER|FTK_ATTR_NO_FOCUS);
		ftk_widget_append_child(parent, thiz);
	}
	else
	{
		FTK_FREE(thiz);
	}

	return thiz;
}

Ret ftk_key_board_load(FtkWidget* thiz, const char* filename)
{
	DECL_PRIV0(thiz, priv);
	
	return_val_if_fail(thiz != NULL && filename != NULL && priv->desc == NULL, RET_FAIL);
	
	priv->desc = ftk_key_board_desc_create(filename);
	return_val_if_fail(priv->desc != NULL, RET_FAIL);

	priv->desc->current_view = 0;
	ftk_key_board_desc_layout(priv->desc, ftk_widget_width(thiz), ftk_widget_height(thiz));
	ftk_widget_invalidate(thiz);

	return RET_OK;
}

Ret ftk_key_board_get_min_size(FtkWidget* thiz, size_t* width, size_t* height)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL && priv->desc != NULL, RET_FAIL);

	if(width != NULL)
	{
		*width = priv->desc->min_width;
	}

	if(height != NULL)
	{
		*height = priv->desc->min_height;
	}

	return RET_OK;
}

Ret ftk_key_board_select_view(FtkWidget* thiz, size_t index)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL && priv->desc != NULL, RET_FAIL);
	return_val_if_fail(index < priv->desc->view_nr, RET_FAIL);

	priv->desc->current_view = index;

	return RET_OK;
}

Ret ftk_key_board_set_custom_action(FtkWidget* thiz, FtkKeyBoardCellAction action)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL && priv->desc != NULL, RET_FAIL);

	priv->desc->custom_action = action;

	return RET_OK;
}

Ret ftk_key_board_add_candidate(FtkWidget* thiz, const char* text)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(text != NULL, RET_FAIL);
	return_val_if_fail(priv != NULL && priv->desc != NULL, RET_FAIL);

	if((priv->desc->candidate_nr + 1) < FTK_MAX_CANDIDATE_NR)
	{
		FtkKeyBoardCandidate* c = priv->desc->candidates + priv->desc->candidate_nr;
		ftk_strncpy(c->text, text, FTK_MAX_CANDIDATE_LEN);
		c->extent = ftk_canvas_get_str_extent(ftk_widget_canvas(thiz), c->text, strlen(c->text));

		priv->desc->candidate_nr++;
	}

	return RET_OK;
}

Ret ftk_key_board_reset_candidates(FtkWidget* thiz)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL && priv->desc != NULL, RET_FAIL);

	priv->desc->candidate_nr = 0;
	priv->desc->candidate_focus = -1;

	return RET_OK;
}

Ret ftk_key_board_set_editor(FtkWidget* thiz, FtkWidget* editor)
{
	DECL_PRIV0(thiz, priv);
	return_val_if_fail(priv != NULL && priv->desc != NULL, RET_FAIL);

	priv->editor = editor;

	return RET_OK;
}
