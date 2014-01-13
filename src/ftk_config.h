/*
 * File: ftk_config.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: configs
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
 * 2010-03-16 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_CONFIG_H
#define FTK_CONFIG_H

#include "ftk_typedef.h"

FTK_BEGIN_DECLS

struct _FtkConfig;
typedef struct _FtkConfig FtkConfig;

FtkConfig* ftk_config_create();
Ret  ftk_config_init(FtkConfig* thiz, int argc, char* argv[]);
Ret ftk_config_load(FtkConfig* thiz, const char* progname);
Ret  ftk_config_parse(FtkConfig* thiz, const char* xml, int length);

const char* ftk_config_get_theme(FtkConfig* thiz);
const char* ftk_config_get_data_dir(FtkConfig* thiz);
const char* ftk_config_get_data_root_dir(FtkConfig* thiz);
const char* ftk_config_get_test_data_dir(FtkConfig* thiz);
FtkRotate   ftk_config_get_rotate(FtkConfig* thiz);
int ftk_config_get_enable_cursor(FtkConfig* thiz);
int ftk_config_get_enable_status_bar(FtkConfig* thiz);

Ret ftk_config_set_theme(FtkConfig* thiz, const char* theme);
Ret ftk_config_set_data_dir(FtkConfig* thiz, const char* data_dir);
Ret ftk_config_set_test_data_dir(FtkConfig* thiz, const char* test_data_dir);
Ret ftk_config_set_enable_cursor(FtkConfig* thiz, int enable_cursor);
Ret ftk_config_set_enable_status_bar(FtkConfig* thiz, int enable_status_bar);

void ftk_config_destroy(FtkConfig* thiz);

FTK_END_DECLS

#endif/*FTK_CONFIG_H*/

