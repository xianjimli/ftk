/*
 * File: ftk_stroke_painter.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   handwrite stroke painter.
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

#ifndef FTK_STROKE_PAINTER_H
#define FTK_STROKE_PAINTER_H

#include "ftk_typedef.h"

FTK_BEGIN_DECLS

struct _FtkStrokePainter;
typedef struct _FtkStrokePainter FtkStrokePainter;

FtkStrokePainter* ftk_stroke_painter_create(void);

Ret ftk_stroke_painter_init(FtkStrokePainter* thiz);

Ret ftk_stroke_painter_begin(FtkStrokePainter* thiz);
Ret ftk_stroke_painter_end(FtkStrokePainter* thiz);
Ret ftk_stroke_painter_clear(FtkStrokePainter* thiz);
Ret ftk_stroke_painter_update(FtkStrokePainter* thiz, FtkRect* rect);
Ret ftk_stroke_painter_move_to(FtkStrokePainter* thiz, int x, int y);
Ret ftk_stroke_painter_line_to(FtkStrokePainter* thiz, int x, int y);
Ret ftk_stroke_painter_show(FtkStrokePainter* thiz, int visible);
Ret ftk_stroke_painter_set_rect(FtkStrokePainter* thiz, FtkRect* rect);
Ret ftk_stroke_painter_set_pen_width(FtkStrokePainter* thiz, int width);
Ret ftk_stroke_painter_set_pen_color(FtkStrokePainter* thiz, FtkColor color);

void ftk_stroke_painter_destroy(FtkStrokePainter* thiz);

FTK_END_DECLS

#endif/*FTK_STROKE_PAINTER_H*/

