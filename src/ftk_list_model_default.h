/*
 * File: ftk_list_model_default.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   default list model
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
 * 2009-11-28 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_LIST_MODEL_DEFAULT_H
#define FTK_LIST_MODEL_DEFAULT_H

#include "ftk_bitmap.h"
#include "ftk_list_model.h"

FTK_BEGIN_DECLS

typedef enum _FtkListItemType
{
	FTK_LIST_ITEM_NORMAL = 0,
	FTK_LIST_ITEM_RADIO,  /*draw radio button icon at right side.*/
	FTK_LIST_ITEM_CHECK,  /*draw check button icon at right side.*/
	FTK_LIST_ITEM_MORE    /*draw MORE icon at right side, which means click this item will lead to more UI. */
}FtkListItemType;

#define FTK_LIST_TEXT_LENGTH 31
typedef struct _FtkListItemInfo
{
	char* text;
	int   disable;
	int   value; /*to store other info.*/
	int   state; /*used for FTK_LIST_ITEM_RADIO and FTK_LIST_ITEM_CHECK*/
	FtkListItemType type;
	FtkBitmap* left_icon;
	FtkBitmap* right_icon;
	void*      user_data;
	void*      extra_user_data;
}FtkListItemInfo;

FtkListModel* ftk_list_model_default_create(size_t init_nr);

FTK_END_DECLS

#endif/*FTK_LIST_MODEL_DEFAULT_H*/

