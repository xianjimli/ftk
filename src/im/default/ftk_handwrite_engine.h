/*
 * File: ftk_handwrite_engine.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   handwrite engine
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
 * 2010-02-10 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_HANDWRITE_ENGINE_H
#define FTK_HANDWRITE_ENGINE_H

#include "ftk_typedef.h"

FTK_BEGIN_DECLS

struct _FtkHandWriteEngine;
typedef struct _FtkHandWriteEngine FtkHandWriteEngine;

typedef Ret (*FtkHandWriteResult)(void* ctx, FtkCommitInfo* info);

FtkHandWriteEngine* ftk_hand_write_engine_create(FtkHandWriteResult on_result, void* ctx);
Ret  ftk_hand_write_engine_activate(FtkHandWriteEngine* thiz);
Ret  ftk_hand_write_engine_deactivate(FtkHandWriteEngine* thiz);
Ret  ftk_hand_write_engine_reset(FtkHandWriteEngine* thiz);
Ret  ftk_hand_write_engine_set_rect(FtkHandWriteEngine* thiz, FtkRect* rect);
Ret  ftk_hand_write_engine_add_stroke(FtkHandWriteEngine* thiz, FtkPoint* points, size_t points_nr);
Ret  ftk_hand_write_engine_flush(FtkHandWriteEngine* thiz);
void ftk_hand_write_engine_destroy(FtkHandWriteEngine* thiz);

FTK_END_DECLS

#endif/*FTK_HANDWRITE_ENGINE_H*/

