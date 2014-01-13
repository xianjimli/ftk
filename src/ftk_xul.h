/*
 * File: ftk_xul.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: create ui from xml.
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
 * 2009-11-23 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_XUL_H
#define FTK_XUL_H

#include "ftk_widget.h"

FTK_BEGIN_DECLS

typedef const char* (*FtkXulTranslateText)(void* ctx, const char* text);
typedef FtkBitmap*  (*FtkXulLoadImage)(void* ctx, const char* filename);

typedef struct _FtkXulCallbacks
{
	void* ctx;
	FtkXulTranslateText translate_text;
	FtkXulLoadImage load_image;
}FtkXulCallbacks;

FtkWidget* ftk_xul_load(const char* xml, int length);
FtkWidget* ftk_xul_load_file(const char* filename, FtkXulCallbacks* callbacks);
FtkWidget* ftk_xul_load_ex(const char* xml, int length, FtkXulCallbacks* callbacks);

FTK_END_DECLS

#endif/*FTK_XUL_H*/

