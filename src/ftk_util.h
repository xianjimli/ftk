/*
 * File: ftk_util.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   common used functions.
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

#ifndef FTK_UTIL_H
#define FTK_UTIL_H

#include "ftk_log.h"
#include "ftk_allocator.h"

FTK_BEGIN_DECLS

char* utf16_to_utf8 (const unsigned short  *str, long len, char* utf8, int out_len);
unsigned short utf8_get_char (const char *p, const char** next);
unsigned short utf8_get_prev_char (const char *p, const char** prev);
int utf8_count_char(const char *begin, int length);
const char* utf8_move_forward(const char* str, int nr);
const char* ftk_line_break(const char* start, const char* end);

int ftk_str2bool(const char* str);
FtkColor ftk_parse_color( const char* value);
const char* ftk_normalize_path(char path[FTK_MAX_PATH+1]);

int    ftk_atoi(const char* str);
double ftk_atof(const char* str);
const char* ftk_itoa(char* str, int len, int n);
const char* ftk_ftoa(char* str, int len, double f);
long  ftk_strtol(const char* str, const char **end, int base);
char* ftk_strs_cat(char* str, int len, const char* first, ...);
char* ftk_strcpy(char* dst, const char* src);

FTK_END_DECLS

#endif/*FTK_UTIL_H*/

