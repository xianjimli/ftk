/*
 * File: ftk_interpolator_bounce.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   bounce interpolator for animation.
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
 * 2011-03-20 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_interpolator.h"

static float ftk_interpolator_bounce_calc(float percent)
{
	return 8 * percent * percent;
}

static float ftk_interpolator_bounce_get(FtkInterpolator* thiz, float percent)
{
	percent *= 1.1226f;

	if (percent < 0.3535f) 
		return ftk_interpolator_bounce_calc(percent);
	else if (percent < 0.7408f) 
		return ftk_interpolator_bounce_calc(percent - 0.54719f) + 0.7f;
	else if (percent < 0.9644f) 
		return ftk_interpolator_bounce_calc(percent - 0.8526f) + 0.9f;
	else 
		return ftk_interpolator_bounce_calc(percent - 1.0435f) + 0.95f;
}

const static FtkInterpolator s_interpolator_bounce = 
{
	ftk_interpolator_bounce_get,
	NULL
};

FtkInterpolator* ftk_interpolator_bounce_create(void)
{
	return (FtkInterpolator*)&s_interpolator_bounce;
}

