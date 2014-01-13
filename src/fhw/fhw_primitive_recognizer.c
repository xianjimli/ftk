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

#include "ftk_allocator.h"
#include "fhw_primitive_recognizer.h"

#define FHW_MAX_PRIMITIVES 255

struct _FhwPrimitiveRecognizer
{
	int nr;
	FtkRect rect;
	char results[FHW_MAX_PRIMITIVES + 1];
};

FhwPrimitiveRecognizer* fhw_primitive_recognizer_create(FtkRect* rect)
{
	FhwPrimitiveRecognizer* thiz = FTK_ZALLOC(sizeof(FhwPrimitiveRecognizer));

	if(thiz != NULL)
	{
		thiz->rect = *rect;
	}

	return thiz;
}

Ret  fhw_primitive_recognizer_reset(FhwPrimitiveRecognizer* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	thiz->nr = 0;
	thiz->results[0] = '\0';

	return RET_OK;
}

Ret  fhw_primitive_recognizer_flush(FhwPrimitiveRecognizer* thiz)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);
	/*XXX: do nothing now.*/

	return RET_OK;
}

typedef struct _FhwPointsInfo
{
	/*some pre-computed values.*/
	int x;
	int y;
	int w;
	int h;
	int min_x;
	int min_y;
	int max_x;
	int max_y;
	int delta_x;
	int delta_y;
	int dot_range;
	
	int nr;
	FtkPoint* points;
}FhwPointsInfo;

#define FHW_MAX_MISS  3
#define FHW_TOLERANCE 5

static FhwPrimitiveType fhw_primitive_recognizer_dian(FhwPointsInfo* info)
{
	if(info->delta_x > info->dot_range || info->delta_y > info->dot_range)
	{
		return FHW_PRIMITIVE_NONE;
	}

	if(info->delta_y > 3 * info->delta_x || info->delta_x > 3 * info->delta_y)
	{
		return FHW_PRIMITIVE_NONE;
	}

	return FHW_PRIMITIVE_DIAN;
}

static FhwPrimitiveType fhw_primitive_recognizer_heng(FhwPointsInfo* info)
{
	int i = 0;
	int miss = 0;

	if(4*(info->delta_y) > (info->delta_x))
	{
		return FHW_PRIMITIVE_NONE;
	}

	for(i = 1; i < info->nr ; i++)
	{
		if(miss >= FHW_MAX_MISS)
		{
			return FHW_PRIMITIVE_NONE;
		}
	
		if((info->points[i].x + FHW_TOLERANCE) < info->points[i-1].x)
		{
			return FHW_PRIMITIVE_NONE;
		}

		miss = (info->points[i].x < info->points[i-1].x) ? miss + 1 : 0;
	}

	return FHW_PRIMITIVE_HENG;
}

static FhwPrimitiveType fhw_primitive_recognizer_shu(FhwPointsInfo* info)
{
	int i = 0;
	int miss = 0;

	if(4 * info->delta_x > info->delta_y)
	{
		return FHW_PRIMITIVE_NONE;
	}

	for(i = 1; i < info->nr ; i++)
	{
		if(miss >= FHW_MAX_MISS)
		{
			return FHW_PRIMITIVE_NONE;
		}

		if((info->points[i].y + FHW_TOLERANCE) < info->points[i-1].y)
		{
			return FHW_PRIMITIVE_NONE;
		}

		miss = info->points[i].y < info->points[i-1].y ? miss + 1 : 0;
	}

	return FHW_PRIMITIVE_SHU;
}

static FhwPrimitiveType fhw_primitive_recognizer_pie(FhwPointsInfo* info)
{
	int i = 0;
	int miss = 0;

	if(info->delta_x < info->dot_range || info->delta_y < info->dot_range)
	{
		return FHW_PRIMITIVE_NONE;
	}

	for(i = 1; i < info->nr ; i++)
	{
		if(miss >= FHW_MAX_MISS)
		{
			return FHW_PRIMITIVE_NONE;
		}

		if((info->points[i].y + FHW_TOLERANCE < info->points[i-1].y) 
			|| (info->points[i].x - FHW_TOLERANCE > info->points[i-1].x))
		{
			return FHW_PRIMITIVE_NONE;
		}

		miss = ((info->points[i].y < info->points[i-1].y) || (info->points[i].x > info->points[i-1].x)) ?
			miss + 1 : 0;
	}

	return FHW_PRIMITIVE_PIE;
}

static FhwPrimitiveType fhw_primitive_recognizer_na(FhwPointsInfo* info)
{
	int i = 0;
	int miss = 0;

	if(info->delta_x < info->dot_range || info->delta_y < info->dot_range)
	{
		return FHW_PRIMITIVE_NONE;
	}

	for(i = 1; i < info->nr ; i++)
	{
		if(miss >= FHW_MAX_MISS)
		{
			return FHW_PRIMITIVE_NONE;
		}

		if((info->points[i].y + FHW_TOLERANCE < info->points[i-1].y) 
			|| (info->points[i].x + FHW_TOLERANCE < info->points[i-1].x))
		{
			return FHW_PRIMITIVE_NONE;
		}

		miss = ((info->points[i].y < info->points[i-1].y) || (info->points[i].x < info->points[i-1].x)) ?
			miss + 1 : 0;
	}

	return FHW_PRIMITIVE_NA;
}

static FhwPrimitiveType fhw_primitive_recognizer_heng_zhe(FhwPointsInfo* info)
{
	int i = 0;
	int miss = 0;

	if(2*(info->delta_y) < (info->delta_x) || 2*(info->delta_x) < (info->delta_y))
	{
		return FHW_PRIMITIVE_NONE;
	}

	/*check heng*/
	for(i = 1; i < info->nr ; i++)
	{
		if(miss >= FHW_MAX_MISS)
		{
			return FHW_PRIMITIVE_NONE;
		}
	
		if((info->points[i].x + FHW_TOLERANCE) < info->points[i-1].x)
		{
			return FHW_PRIMITIVE_NONE;
		}

		miss = (info->points[i].x < info->points[i-1].x) ? miss + 1 : 0;

		if((info->points[i].y - info->min_y) > info->dot_range)
		{
			break;
		}
	}

	for(; i < info->nr ; i++)
	{
		if(miss >= FHW_MAX_MISS)
		{
			return FHW_PRIMITIVE_NONE;
		}

		if((info->points[i].y + FHW_TOLERANCE) < info->points[i-1].y)
		{
			return FHW_PRIMITIVE_NONE;
		}

		miss = info->points[i].y < info->points[i-1].y ? miss + 1 : 0;
	}

	return FHW_PRIMITIVE_HENG_ZHE;
}

typedef FhwPrimitiveType (*FhwPrimitiveRecognizerFunc)(FhwPointsInfo* info);

static const FhwPrimitiveRecognizerFunc g_recognizers[] = 
{
	fhw_primitive_recognizer_dian,
	fhw_primitive_recognizer_heng,
	fhw_primitive_recognizer_heng_zhe,
	fhw_primitive_recognizer_shu,
	fhw_primitive_recognizer_pie,
	fhw_primitive_recognizer_na,
	NULL
};

Ret  fhw_primitive_recognizer_add_stroke(FhwPrimitiveRecognizer* thiz, FtkPoint* points, size_t nr)
{
	int i = 0;
	FhwPointsInfo info = {0};
	FhwPrimitiveType result = FHW_PRIMITIVE_NONE;
	return_val_if_fail(thiz != NULL && points != NULL, RET_FAIL);
	return_val_if_fail(thiz->nr < FHW_MAX_PRIMITIVES, RET_FAIL);

	info.nr = nr;
	info.points = points;
	
	info.x = thiz->rect.x;
	info.y = thiz->rect.y;
	info.w = thiz->rect.width;
	info.h = thiz->rect.height;

	info.min_x = points[0].x;
	info.min_y = points[0].y;
	info.max_x = points[0].x;
	info.max_y = points[0].y;
	info.dot_range = (info.w + info.h) >> 3;
	for(i = 1; i < nr; i++)
	{
		info.min_x = FTK_MIN(info.min_x, points[i].x);
		info.min_y = FTK_MIN(info.min_y, points[i].y);
		info.max_x = FTK_MAX(info.max_x, points[i].x);
		info.max_y = FTK_MAX(info.max_y, points[i].y);
	}

	info.delta_x = info.max_x - info.min_x;
	info.delta_y = info.max_y - info.min_y;

	for(i = 0; g_recognizers[i] != NULL; i++)
	{
		if((result = g_recognizers[i](&info)) != FHW_PRIMITIVE_NONE)
		{
			thiz->results[thiz->nr++] = result;
			thiz->results[thiz->nr] = '\0';
			break;
		}
	}

	return RET_OK;
}

Ret  fhw_primitive_recognizer_get_result(FhwPrimitiveRecognizer* thiz, const char** results)
{
	return_val_if_fail(thiz != NULL && results != NULL, RET_FAIL);

	*results = thiz->results;

	return RET_OK;
}

void fhw_primitive_recognizer_destroy(FhwPrimitiveRecognizer* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(FhwPrimitiveRecognizer));
	}

	return;
}

