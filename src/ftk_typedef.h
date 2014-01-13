/*
 * File: ftk_typedef.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:  common used type definition. 
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

#ifndef FTK_TYPEDEF_H
#define FTK_TYPEDEF_H

#include "ftk_platform.h"

typedef enum _FtkBool
{
	FTK_TRUE = 1,
	FTK_FALSE = 0
}FtkBool;

typedef struct _FtkPoint
{
	int x;
	int y;
}FtkPoint;

typedef struct _FtkRect
{
	int x;
	int y;
	int width;
	int height;
}FtkRect;

typedef struct _FtkRegion
{
	FtkRect rect;
	struct _FtkRegion* next;
}FtkRegion;

#ifdef FTK_COLOR_RGBA
typedef struct _FtkColor
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
}FtkColor;
#else
typedef struct _FtkColor
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char a;
}FtkColor;
#endif

typedef enum _Ret
{
	RET_OK,
	RET_FAIL,
	RET_REMOVE,
	RET_CONTINUE,
	RET_FOUND,
	RET_EOF,
	RET_NEXT,
	RET_QUIT,
	RET_EXIST,
	RET_AGAIN,
	RET_IGNORED,
	RET_NO_TARGET,
	RET_NOT_FOUND,
	RET_OUT_OF_SPACE
}Ret;

typedef enum _FtkMarqueeAttr
{
	FTK_MARQUEE_NONE = 0,
	FTK_MARQUEE_AUTO    = 1,    /*determine by the text size automatically.*/
	FTK_MARQUEE_ALWAYS  = 2,    /*always marquee the text*/
	FTK_MARQUEE_ONCE    = 4,    /*run once and then stop*/
	FTK_MARQUEE_FOREVER = 8,    /*run forever*/
	FTK_MARQUEE_DOWN2UP = 16,   /*scroll down to up*/
	FTK_MARQUEE_RIGHT2LEFT = 32 /*scroll right to left*/
}FtkMarqueeAttr;

typedef enum _FtkAlignment
{
	FTK_ALIGN_LEFT = 0,
	FTK_ALIGN_RIGHT,
	FTK_ALIGN_CENTER
}FtkAlignment;

typedef enum _FtkPixelFormat
{
	FTK_PIXEL_NONE   = 0,
	FTK_PIXEL_BGR24  = 1,
	FTK_PIXEL_BGRA32 = 2,
	FTK_PIXEL_RGB565 = 3,
	FTK_PIXEL_RGBA32 = 4,
}FtkPixelFormat;

typedef enum _FtkWidgetType
{
	FTK_WIDGET_NONE   = 0,
	FTK_WINDOW = 1,
	FTK_DIALOG,
	FTK_WINDOW_MISC,
	FTK_STATUS_PANEL,
	FTK_MENU_PANEL,
	FTK_MENU_ITEM,
	FTK_LABEL,
	FTK_BUTTON,
	FTK_IMAGE,
	FTK_ENTRY,
	FTK_PAINTER,
	FTK_WAIT_BOX,
	FTK_STATUS_ITEM,
	FTK_PROGRESS_BAR,
	FTK_GROUP_BOX,
	FTK_RADIO_BUTTON,
	FTK_CHECK_BUTTON,
	FTK_SCROLL_VBAR,
	FTK_SCROLL_HBAR,
	FTK_LIST_VIEW,
	FTK_ICON_VIEW,
	FTK_TEXT_VIEW,
	FTK_COMBO_BOX,
	FTK_TAB,
	FTK_TAB_PAGE,
	FTK_KEY_BOARD,
	FTK_WIDGET_TYPE_NR
}FtkWidgetType;

typedef enum _FtkLogLevel
{
	FTK_LOG_V = 0,
	FTK_LOG_D = 1,
	FTK_LOG_I = 2,
	FTK_LOG_W = 3,
	FTK_LOG_E = 4
}FtkLogLevel;

typedef enum _FtkWidgetState
{
	FTK_WIDGET_NORMAL      = 0,
	FTK_WIDGET_FOCUSED     = 1,
	FTK_WIDGET_ACTIVE      = 2,
	FTK_WIDGET_INSENSITIVE = 3,
	FTK_WIDGET_STATE_NR    = 4
}FtkWidgetState;

typedef enum _FtkWidgetAttr
{
	FTK_ATTR_TRANSPARENT     = 1,
	FTK_ATTR_NO_FOCUS        = 1<<1,
	FTK_ATTR_BG_CENTER       = 1<<2,
	FTK_ATTR_BG_TILE         = 1<<3,
	FTK_ATTR_BG_FOUR_CORNER  = 1<<4,
	FTK_ATTR_INSENSITIVE     = 1<<5,
	FTK_ATTR_FOCUSED         = 1<<6,
	FTK_ATTR_IGNORE_CLOSE    = 1<<7, /*for window only*/
	FTK_ATTR_QUIT_WHEN_CLOSE = 1<<8, /*for window only*/
	FTK_ATTR_AUTO_LAYOUT     = 1<<9,  /*for dialog only*/
	FTK_ATTR_POPUP           = 1<<10,  /*for dialog only*/
	FTK_ATTR_FULLSCREEN      = 1<<11  /*for window only*/
}FtkWidgetAttr;

typedef enum _FtkRotate
{
	FTK_ROTATE_0 = 0,
	FTK_ROTATE_90,
	FTK_ROTATE_180,
	FTK_ROTATE_270,
	FTK_ROTATE_NR
}FtkRotate;

typedef enum _FtkWrapMode
{
	FTK_WRAP_NONE,
	FTK_WRAP_CHAR,
	FTK_WRAP_WORD,
	FTK_WRAP_NR
}FtkWrapMode;


typedef void (*FtkDestroy)(void* user_data);
typedef Ret  (*FtkIdle)(void* user_data);
typedef Ret  (*FtkTimer)(void* user_data);
typedef int  (*FtkCompare)(const void* obj1, const void* obj2);
typedef Ret  (*FtkListener)(void* user_data, void* obj);

#define FTK_KEY_LEN 31
#define FTK_VALUE_LEN 63

#define FTK_IM_WORD_LENGTH 31
#define FTK_IM_RAW_TEXT_LENGTH 31
#define FTK_IM_CANDIDATE_BUFF_LENGTH 1023

typedef struct _FtkCommitInfo
{
	int candidate_nr;
	char raw_text[FTK_IM_RAW_TEXT_LENGTH + 1];
	char candidates[FTK_IM_CANDIDATE_BUFF_LENGTH + 1];
}FtkCommitInfo;

#define FTK_INHERITE_FROM(parent) extern int a;

#define FTK_CALL_LISTENER(listener, u, o) listener != NULL ? listener(u, o) : RET_OK

#define return_if_fail(p)          if(!(p)) { ftk_logw("%s:%d "#p" failed.\n", __func__, __LINE__); return;}
#define return_val_if_fail(p, val) if(!(p)) { ftk_logw("%s:%d "#p" failed.\n", __func__, __LINE__); return (val);}

#define DECL_PRIV(thiz, priv) PrivInfo* priv = (PrivInfo*)(thiz->priv)
#define DECL_PRIV0(thiz, priv) PrivInfo* priv = thiz != NULL ? (PrivInfo*)thiz->priv_subclass[0] : NULL
#define DECL_PRIV1(thiz, priv) PrivInfo* priv = thiz != NULL ? (PrivInfo*)thiz->priv_subclass[1] : NULL
#define DECL_PRIV2(thiz, priv) PrivInfo* priv = thiz != NULL ? (PrivInfo*)thiz->priv_subclass[2] : NULL

#define FTK_HALF(a)    ((a)>>1)
#define FTK_MIN(a, b) ((a) < (b) ? (a) : (b))
#define FTK_MAX(a, b) ((a) < (b) ? (b) : (a))
#define FTK_ABS(a) ((a) < 0 ? -(a) : (a))

#define FTK_MASK_BITS(val32, index) (((val32) << ((index)%32)) & 0x80000000)

#define FTK_ALPHA_1(s, d, a) (d) = ((unsigned int)((d) * (0xff - (a)) + (s) * (a)) + 0xff) >> 8
#define FTK_ALPHA(sc, dc, a) FTK_ALPHA_1(sc->r, dc->r, a); \
	     FTK_ALPHA_1(sc->g, dc->g, a); \
	     FTK_ALPHA_1(sc->b, dc->b, a);

#define FTK_ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#define FTK_STR_SIZE(s) (sizeof(s)-1)

/*for customize*/
#define FTK_H_MARGIN            3
#define FTK_V_MARGIN            3
#define FTK_MAX_PATH            260
#define FTK_MAX_WINDOWS         32
#define FTK_SPACE_WIDTH         5
#define FTK_MAX_IMAGE_DECODERS  6
#define FTK_STATUS_PANEL_HEIGHT 36
#define FTK_DIALOG_MARGIN       (ftk_display_width(ftk_default_display()) >> 5)
#define FTK_DIALOG_BORDER       2
#define FTK_SCROLL_BAR_WIDTH    10
#define FTK_MENU_MAX_ITEM       16
#define FTK_MENU_ITEM_HEIGHT    48
#define FTK_MENU_ITEM_WIDTH     100
#define FTK_POPUP_MENU_ITEM_HEIGHT 40
#define FTK_STOCK_IMG_SUFFIX       ".png"
#define FTK_LABEL_LEFT_MARGIN      3
#define FTK_LABEL_TOP_MARGIN       1
#define FTK_CLIPBOARD_SIZE         1024
#define FTK_TITLE_LENGTH           15
#define FTK_IM_PREEDITOR_MAX_ROW   5
#define FTK_TAB_WIDTH              4

#ifndef FTK_FONT_SIZE
#define FTK_FONT_SIZE              16
#endif

#ifdef __cplusplus
#define FTK_BEGIN_DECLS extern "C" {
#define FTK_END_DECLS }
#else
#define FTK_BEGIN_DECLS
#define FTK_END_DECLS
#endif

#ifdef FTK_AS_PLUGIN 
#define FTK_MAIN ftk_main
#define FTK_INIT(argc, argv) 
#define FTK_RUN() 
#define FTK_QUIT()
#define FTK_QUIT_WHEN_WIDGET_CLOSE(win) 
#elif defined(USE_FTK_MAIN)
#define FTK_MAIN ftk_main
#define FTK_INIT ftk_init
#define FTK_RUN() ftk_run()
#define FTK_QUIT() ftk_quit()
#define FTK_QUIT_WHEN_WIDGET_CLOSE(win) ftk_widget_set_attr(win, FTK_ATTR_QUIT_WHEN_CLOSE)
#else
#define FTK_MAIN main
#define FTK_INIT ftk_init
#define FTK_RUN() ftk_run()
#define FTK_QUIT() ftk_quit()
#define FTK_QUIT_WHEN_WIDGET_CLOSE(win) ftk_widget_set_attr(win, FTK_ATTR_QUIT_WHEN_CLOSE)
#endif

#ifdef USE_STD_MALLOC
#define FTK_ALLOC(s)       malloc(s)
#define FTK_REALLOC(p, s)  realloc(p, s)
#define FTK_ZALLOC(s)      calloc(1, s)
#define FTK_FREE(p)        if(p) {free(p); p = NULL;}
#define FTK_ZFREE(p, size) if(p) {memset((p), 0x00, (size)); free(p); p = NULL;}
#else
#define FTK_ALLOC(s)       ftk_allocator_alloc(ftk_default_allocator(), s)
#define FTK_REALLOC(p, s)  ftk_allocator_realloc(ftk_default_allocator(), p, s)
#define FTK_ZALLOC(s)      ftk_allocator_zalloc(ftk_default_allocator(), s)
#define FTK_FREE(p)        ftk_allocator_free(ftk_default_allocator(), p); p = NULL
#define FTK_ZFREE(p, size) ftk_allocator_zfree(ftk_default_allocator(), p, size); p = NULL
#define FTK_STRDUP(p)      p != NULL ? ftk_strdup(p) : NULL
#endif

#define FTK_NEW(Type)      (Type*)FTK_ZALLOC(sizeof(Type))
#define FTK_NEW_PRIV(Type) (Type*)FTK_ZALLOC(sizeof(Type) + sizeof(PrivInfo))

#ifdef HAVE_CONFIG_FTK_H
#include "config_ftk.h"
#endif

#ifdef ENABLE_NLS
#  include <locale.h>
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  define Q_(String) g_strip_context ((String), gettext (String))
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define Q_(String) g_strip_context ((String), (String))
#  define N_(String) (String)
#endif

#ifdef FTK_PROFILE
static size_t profile_begin;
#define PROFILE_START() profile_begin = ftk_get_relative_time();
#define PROFILE_END(what) ftk_logd("%s:%d %s cost %d ms\n", __func__, __LINE__, \
	what, ftk_get_relative_time() - profile_begin);
#define PROFILE_TIME(what) ftk_logd("%s:%d %s time=%d\n", __func__, __LINE__, what, ftk_get_relative_time());
#else
#define PROFILE_START()
#define PROFILE_END(what)
#define PROFILE_TIME(what) 
#endif

#if __GNUC__ >= 3
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define ZERO_LEN_ARRAY 0
#else
#define likely(x) x
#define unlikely(x) x
#define ZERO_LEN_ARRAY 1
#endif

#endif/*FTK_TYPEDEF_H*/

