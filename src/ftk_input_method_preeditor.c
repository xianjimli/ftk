/*
 * File: ftk_input_method_preeditor.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   input candidate selector.
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
 * 2010-02-11 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_button.h"
#include "ftk_window.h"
#include "ftk_globals.h"
#include "ftk_list_view.h"
#include "ftk_text_buffer.h"
#include "ftk_list_model_default.h"
#include "ftk_list_render_default.h"
#include "ftk_input_method_preeditor.h"

Ret ftk_im_show_preeditor(FtkWidget* editor, FtkPoint* caret_pos, FtkCommitInfo* info)
{
	int i = 0;
	const char* item = NULL;
	FtkImPreeditor* preeditor = ftk_default_input_method_preeditor();

	return_val_if_fail(info != NULL && preeditor != NULL, RET_FAIL);


	if(info->raw_text[0] == '\0' && info->candidate_nr == 0)
	{
		ftk_input_method_preeditor_hide(preeditor);
		return RET_OK;
	}

	ftk_input_method_preeditor_reset(preeditor);
	ftk_input_method_preeditor_set_editor(preeditor, editor);
	ftk_input_method_preeditor_set_raw_text(preeditor, info->raw_text);
	
	item = info->candidates;
	for(i = 0; i < info->candidate_nr; i++)
	{
		ftk_input_method_preeditor_add_candidate(preeditor, item);
		item += strlen(item) + 1;
	}
	
	ftk_input_method_preeditor_show(preeditor, caret_pos);

	return RET_OK;
}

