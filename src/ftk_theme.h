/*
 * File: ftk_theme.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   theme
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
 * 2009-12-05 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_THEME_H
#define FTK_THEME_H

#include "ftk_bitmap.h"
#include "ftk_animation_trigger.h"

FTK_BEGIN_DECLS

struct _FtkTheme;
typedef struct _FtkTheme FtkTheme;

FtkTheme*  ftk_theme_create(int init_default);
Ret        ftk_theme_parse_file(FtkTheme* thiz, const char* filename);
Ret        ftk_theme_parse_data(FtkTheme* thiz, const char* data, size_t length);
FtkBitmap* ftk_theme_load_image(FtkTheme* thiz, const char* filename);
FtkBitmap* ftk_theme_get_bg(FtkTheme* thiz, FtkWidgetType type, FtkWidgetState state);
FtkColor   ftk_theme_get_bg_color(FtkTheme* thiz, FtkWidgetType type, FtkWidgetState state);
FtkColor   ftk_theme_get_fg_color(FtkTheme* thiz, FtkWidgetType type, FtkWidgetState state);
FtkColor   ftk_theme_get_border_color(FtkTheme* thiz, FtkWidgetType type, FtkWidgetState state);
FtkFontDesc*   ftk_theme_get_font(FtkTheme* thiz, FtkWidgetType type);
FtkAnimationTrigger* ftk_theme_get_animation_trigger(FtkTheme* thiz);

void       ftk_theme_destroy(FtkTheme* thiz);

FTK_END_DECLS

#endif/*FTK_THEME_H*/

