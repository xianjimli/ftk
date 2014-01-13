/*
 * File: ftk_param.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:  params manager
 *
 * Copyright (c) 2009 - 2011  Li XianJing <xianjimli@hotmail.com>
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
 * 2011-03-23 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FTK_PARAMS_H
#define FTK_PARAMS_H

#include "ftk_typedef.h"

FTK_BEGIN_DECLS

struct _FtkParams;
typedef struct _FtkParams FtkParams;

FtkParams* ftk_params_create(int max_params_nr, int max_vars_nr);
Ret ftk_params_set_param(FtkParams* thiz, const char* name, const char* value);
Ret ftk_params_set_var(FtkParams* thiz, const char* name, const char* value);
int ftk_params_eval_int(FtkParams* thiz, const char* name, int defval);
float ftk_params_eval_float(FtkParams* thiz, const char* name, float defval);
const char* ftk_params_eval_string(FtkParams* thiz, const char* name);

void ftk_params_dump(FtkParams* thiz);

void ftk_params_destroy(FtkParams* thiz);

FTK_END_DECLS

#endif/*FTK_PARAMS_H*/

