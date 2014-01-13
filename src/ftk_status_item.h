/*
 * File: ftk_status_item.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   item on the status panel. 
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
 * 2009-10-20 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_STATUS_ITEM_H
#define FTK_STATUS_ITEM_H

#include "ftk_widget.h"

FTK_INHERITE_FROM(Widget)

FTK_BEGIN_DECLS

FtkWidget* ftk_status_item_create(FtkWidget* parent, int pos, int width);

/*
 * the position on the panel:
 *  =0: N/A
 *  >0: from first to last. eg. 1 means the first place. 2 means the second place.
 *  <0: from last to first. eg. -1 means the last place.
 **/
Ret ftk_status_item_set_position(FtkWidget* thiz, int pos);
int ftk_status_item_get_position(FtkWidget* thiz);
Ret ftk_status_item_set_clicked_listener(FtkWidget* thiz, FtkListener listener, void* ctx);

FTK_END_DECLS

#endif/*FTK_STATUS_ITEM_H*/

