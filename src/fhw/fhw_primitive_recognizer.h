/*
 * File: ftk_input_method.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   FTK handwrite primtives
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
 * 2010-02-13 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef FHW_PRIMITIVE_RECOGNIZER_H
#define FHW_PRIMITIVE_RECOGNIZER_H

#include "ftk_typedef.h"

FTK_BEGIN_DECLS

typedef enum _FhwPrimitiveType
{
	FHW_PRIMITIVE_NONE     = '\0',
	FHW_PRIMITIVE_DIAN     = 'a',
	FHW_PRIMITIVE_HENG     = 'b',
	FHW_PRIMITIVE_SHU      = 'c',
	FHW_PRIMITIVE_PIE      = 'd',
	FHW_PRIMITIVE_NA       = 'e',
	FHW_PRIMITIVE_HENG_ZHE = 'f',
	FHW_PRIMITIVE_SHU_GOU  = 'g',
	FHW_PRIMITIVE_NR
}FhwPrimitiveType;

struct _FhwPrimitiveRecognizer;
typedef struct _FhwPrimitiveRecognizer FhwPrimitiveRecognizer;

FhwPrimitiveRecognizer* fhw_primitive_recognizer_create(FtkRect* rect);

Ret  fhw_primitive_recognizer_reset(FhwPrimitiveRecognizer* thiz);
Ret  fhw_primitive_recognizer_flush(FhwPrimitiveRecognizer* thiz);
Ret  fhw_primitive_recognizer_add_stroke(FhwPrimitiveRecognizer* thiz, FtkPoint* points, size_t nr);
Ret  fhw_primitive_recognizer_get_result(FhwPrimitiveRecognizer* thiz, const char** results);
void fhw_primitive_recognizer_destroy(FhwPrimitiveRecognizer* thiz);

FTK_END_DECLS

#endif/*FHW_PRIMITIVE_RECOGNIZER_H*/

