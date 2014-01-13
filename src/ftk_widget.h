/*
 * File: ftk_widget.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   interface for all widget.
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
 * 2009-10-03 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_WIDGET_H
#define FTK_WIDGET_H

#include "ftk_key.h"
#include "ftk_event.h"
#include "ftk_canvas.h"
#include "ftk_display.h"
#include "ftk_allocator.h"

FTK_BEGIN_DECLS

struct _FtkWidget;
typedef struct _FtkWidget FtkWidget;

struct _FtkWidgetInfo;
typedef struct _FtkWidgetInfo FtkWidgetInfo;

typedef Ret  (*FtkWidgetOnEvent)(FtkWidget* thiz, FtkEvent* event);
typedef Ret  (*FtkWidgetOnPaint)(FtkWidget* thiz);
typedef void (*FtkWidgetDestroy)(FtkWidget* thiz);

struct _FtkWidget
{
	int ref;
	FtkWidgetOnEvent on_event;
	FtkWidgetOnPaint on_paint;
	FtkWidgetDestroy destroy;
	
	FtkWidget* prev;
	FtkWidget* next;
	FtkWidget* parent;
	FtkWidget* children;
	FtkWidgetInfo* priv;
	/*private data for subclass*/
	void* priv_subclass[5];
};

void ftk_widget_init(FtkWidget* thiz, int type, int id, int x, int y, 
	int width, int height, unsigned int attr);
int ftk_widget_type(FtkWidget* thiz);
int ftk_widget_top(FtkWidget* thiz);
int ftk_widget_left(FtkWidget* thiz);
int ftk_widget_top_abs(FtkWidget* thiz);
int ftk_widget_left_abs(FtkWidget* thiz);
int ftk_widget_top_in_window(FtkWidget* thiz);
int ftk_widget_left_in_window(FtkWidget* thiz);
int ftk_widget_width(FtkWidget* thiz);
int ftk_widget_height(FtkWidget* thiz);
int ftk_widget_is_insensitive(FtkWidget* thiz);
int ftk_widget_is_visible(FtkWidget* thiz);
int ftk_widget_is_focused(FtkWidget* thiz);
int ftk_widget_is_active(FtkWidget* thiz);
int ftk_widget_id(FtkWidget* thiz);
Ret ftk_widget_update(FtkWidget* thiz);
Ret ftk_widget_update_rect(FtkWidget* thiz, FtkRect* rect);
FtkGc* ftk_widget_get_gc(FtkWidget* thiz);
FtkCanvas* ftk_widget_canvas(FtkWidget* thiz);
int ftk_widget_has_attr(FtkWidget* thiz, FtkWidgetAttr attr);
FtkWidgetState ftk_widget_state(FtkWidget* thiz);
void* ftk_widget_user_data(FtkWidget* thiz);
const char* ftk_widget_get_text(FtkWidget* thiz);
Ret ftk_widget_invalidate(FtkWidget* thiz);
FtkWrapMode ftk_widget_get_wrap_mode(FtkWidget* thiz);

void ftk_widget_show(FtkWidget* thiz, int visible);
void ftk_widget_show_all(FtkWidget* thiz, int visible);
void ftk_widget_set_visible(FtkWidget* thiz, int visible);
void ftk_widget_move(FtkWidget* thiz, int x, int y);
void ftk_widget_resize(FtkWidget* thiz, int width, int height);
void ftk_widget_move_resize(FtkWidget* thiz, int x, int y, int width, int height);
void ftk_widget_set_type(FtkWidget* thiz, int type);
void ftk_widget_set_insensitive(FtkWidget* thiz, int insensitive);
/*ftk_widget_set_focused is internal used. user should call ftk_window_set_focus*/
void ftk_widget_set_focused(FtkWidget* thiz, int focused);
void ftk_widget_set_active(FtkWidget* thiz, int active);
void ftk_widget_set_id(FtkWidget* thiz, int id);
void ftk_widget_set_canvas(FtkWidget* thiz, FtkCanvas* canvas);
void ftk_widget_set_parent(FtkWidget* thiz, FtkWidget* parent);
void ftk_widget_append_child(FtkWidget* thiz, FtkWidget* child);
void ftk_widget_append_sibling(FtkWidget* thiz, FtkWidget* next);
void ftk_widget_remove_child(FtkWidget* thiz, FtkWidget* child);
void ftk_widget_set_attr(FtkWidget* thiz, unsigned int attr);
void ftk_widget_unset_attr(FtkWidget* thiz, FtkWidgetAttr attr);
void ftk_widget_set_user_data(FtkWidget* thiz, FtkDestroy destroy, void* data);
void ftk_widget_set_gc(FtkWidget* thiz, FtkWidgetState state, FtkGc* gc);
int  ftk_widget_get_font_size(FtkWidget* thiz);
void ftk_widget_set_font_size(FtkWidget* thiz, int font_size);
void ftk_widget_set_font(FtkWidget* thiz, const char* font_desc);
void ftk_widget_reset_gc(FtkWidget* thiz, FtkWidgetState state, FtkGc* gc);
void ftk_widget_set_text(FtkWidget* thiz, const char* text);
void ftk_widget_set_event_listener(FtkWidget* thiz, FtkListener listener, void* ctx);
void ftk_widget_set_wrap_mode(FtkWidget* thiz, FtkWrapMode mode);

FtkWidget* ftk_widget_toplevel(FtkWidget* thiz);
FtkWidget* ftk_widget_parent(FtkWidget* thiz);
FtkWidget* ftk_widget_prev(FtkWidget* thiz);
FtkWidget* ftk_widget_next(FtkWidget* thiz);
FtkWidget* ftk_widget_child(FtkWidget* thiz);
FtkWidget* ftk_widget_last_child(FtkWidget* thiz);
FtkWidget* ftk_widget_lookup(FtkWidget* thiz, int id);
FtkWidget* ftk_widget_find_target(FtkWidget* thiz, int x, int y, int only_sensitive);
void ftk_widget_hide_self(FtkWidget* thiz, int hide); 

void ftk_widget_paint(FtkWidget* thiz, FtkRect *rects, int rect_nr);
void ftk_widget_destroy(FtkWidget* thiz);

void ftk_widget_ref(FtkWidget* thiz);
void ftk_widget_unref(FtkWidget* thiz);

Ret ftk_widget_paint_self(FtkWidget* thiz, FtkRect *rects, int rect_nr);
void ftk_widget_ref_self(FtkWidget* thiz);
void ftk_widget_unref_self(FtkWidget* thiz);
Ret  ftk_widget_event(FtkWidget* thiz, FtkEvent* event);

#define FTK_GET_PAINT_RECT(widget, r) \
	r.x = ftk_widget_left_abs(widget);\
	r.y = ftk_widget_top_abs(widget);\
	r.width  = ftk_widget_width(widget);\
	r.height = ftk_widget_height(widget);

#define FTK_BEGIN_PAINT(x, y, width, height, canvas) \
	int x = ftk_widget_left_abs(thiz);\
	int y = ftk_widget_top_abs(thiz);\
	int width  = ftk_widget_width(thiz);\
	int height = ftk_widget_height(thiz);\
	FtkCanvas* canvas = ftk_widget_canvas(thiz);

#define FTK_PAINT_X(thiz) ftk_widget_left_abs(thiz)
#define FTK_PAINT_Y(thiz) ftk_widget_top_abs(thiz)

#define FTK_END_PAINT() \
	return ftk_widget_update(thiz);

#define FTK_POINT_IN_WIDGET(x, y, widget) \
	((x >= ftk_widget_left_abs(widget)) \
	&& (y >= ftk_widget_top_abs(widget)) \
	&& (x < (ftk_widget_left_abs(widget) + ftk_widget_width(widget))) \
	&& (y < (ftk_widget_top_abs(widget) + ftk_widget_height(widget))))

FTK_END_DECLS

#endif/*FTK_WIDGET_H*/

