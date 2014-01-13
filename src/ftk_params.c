/*
 * File: ftk_param.c
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

#include "ftk_allocator.h"
#include "ftk_log.h"
#include "ftk_util.h"
#include "ftk_expr.h"
#include "ftk_pairs.h"
#include "ftk_params.h"

struct _FtkParams
{
	FtkPairs* vars;
	FtkPairs* params;
	char eval[FTK_VALUE_LEN << 2];
};

FtkParams* ftk_params_create(int max_params_nr, int max_vars_nr)
{
	FtkParams* thiz = (FtkParams*)FTK_ZALLOC(sizeof(FtkParams));

	if(thiz != NULL)
	{
		thiz->vars = ftk_pairs_create(max_vars_nr, (FtkCompare)strcmp);
		thiz->params = ftk_pairs_create(max_params_nr, (FtkCompare)strcmp);
	}

	return thiz; 
}

Ret ftk_params_set_param(FtkParams* thiz, const char* name, const char* value)
{
	return_val_if_fail(thiz != NULL && name != NULL && value != NULL, RET_FAIL);

	return ftk_pairs_set(thiz->params, name, value);
}

Ret ftk_params_set_var(FtkParams* thiz, const char* name, const char* value)
{
	return_val_if_fail(thiz != NULL && name != NULL && value != NULL, RET_FAIL);

	return ftk_pairs_set(thiz->vars, name, value);
}

int ftk_params_eval_int(FtkParams* thiz, const char* name, int defval)
{
	int ret = defval;
	const char* value = ftk_params_eval_string(thiz, name);

	if(value != NULL)
	{
		ret = (int)ftk_expr_eval(value);
	}

	return ret;
}

float ftk_params_eval_float(FtkParams* thiz, const char* name, float defval)
{
	float ret = defval;
	const char* value = ftk_params_eval_string(thiz, name);

	if(value != NULL)
	{
		ret = (float)ftk_expr_eval(value);
	}

	return ret;
}

#define FTK_IS_ID_CHAR(c) (isalpha(c) || isdigit(c))
const char* ftk_params_eval_string(FtkParams* thiz, const char* name)
{
	const char* value = ftk_pairs_find(thiz->params, name);

	if(value != NULL && strstr(value, "$") != NULL)
	{
		int i = 0;
		int dst = 0;
		char var[FTK_KEY_LEN+1] = {0};

		for(i = 0; value[i]; i++)
		{
			if(value[i] != '$')
			{
				thiz->eval[dst++] = value[i];
			}
			else
			{
				int j = 0;
				const char* var_value = NULL;
				for(i = i + 1; FTK_IS_ID_CHAR(value[i]); i++, j++)
				{
					var[j] = value[i];
				}

				i--;
				var[j] = '\0';

				var_value = ftk_pairs_find(thiz->vars, var);
				if(var_value != NULL)
				{
					ftk_strcpy(thiz->eval+dst, var_value);
					dst += strlen(var_value);
				}
			}
		}

		value = thiz->eval;
	}

	return value;
}

void ftk_params_dump(FtkParams* thiz)
{
	if(thiz != NULL)
	{
		ftk_logd("-------------param------------------\n");
		ftk_pairs_dump(thiz->params);
		ftk_logd("-------------var------------------\n");
		ftk_pairs_dump(thiz->vars);
	}
}

void ftk_params_destroy(FtkParams* thiz)
{
	if(thiz != NULL)
	{
		ftk_pairs_destroy(thiz->params);
		ftk_pairs_destroy(thiz->vars);

		FTK_FREE(thiz);
	}

	return;
}


