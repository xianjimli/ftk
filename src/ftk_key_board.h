/*
 * File: ftk_key_board.h    
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

#ifndef FTK_KEY_BOARD_H
#define FTK_KEY_BOARD_H

#include "ftk_widget.h"

FTK_INHERITE_FROM(Widget)

FTK_BEGIN_DECLS

typedef Ret (*FtkKeyBoardCellAction)(FtkWidget* keyboard, const char* text, const char* args);

FtkWidget* ftk_key_board_create(FtkWidget* parent, int x, int y, int width, int height);
Ret ftk_key_board_load(FtkWidget* thiz, const char* description_filename);
Ret ftk_key_board_get_min_size(FtkWidget* thiz, size_t* width, size_t* height);
Ret ftk_key_board_select_view(FtkWidget* thiz, size_t index);
Ret ftk_key_board_set_custom_action(FtkWidget* thiz, FtkKeyBoardCellAction action);
Ret ftk_key_board_reset_candidates(FtkWidget* thiz);
Ret ftk_key_board_add_candidate(FtkWidget* thiz, const char* text);
Ret ftk_key_board_set_editor(FtkWidget* thiz, FtkWidget* editor);

void ftk_key_board_test(const char* filename);

FTK_END_DECLS

#endif/*FTK_KEY_BOARD_H*/

