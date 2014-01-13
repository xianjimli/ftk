/*
 * File:    ftk_sprite.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   screen sprite
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
 * 2009-11-21 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_SPRITE_H
#define FTK_SPRITE_H

#include "ftk_bitmap.h"

FTK_BEGIN_DECLS

struct _FtkSprite;
typedef struct _FtkSprite FtkSprite;

FtkSprite* ftk_sprite_create(void);
int  ftk_sprite_get_x(FtkSprite* thiz);
int  ftk_sprite_get_y(FtkSprite* thiz);
int  ftk_sprite_is_visible(FtkSprite* thiz);
Ret  ftk_sprite_show(FtkSprite* thiz, int show);
Ret  ftk_sprite_move(FtkSprite* thiz, int x, int y);
Ret  ftk_sprite_set_icon(FtkSprite* thiz, FtkBitmap* icon);
Ret  ftk_sprite_set_move_listener(FtkSprite* thiz, FtkListener listener, void* ctx);
void ftk_sprite_destroy(FtkSprite* thiz);

FTK_END_DECLS

#endif/*FTK_SPRITE_H*/

