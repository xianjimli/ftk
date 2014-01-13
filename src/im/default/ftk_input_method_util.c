/*
 * File: ftk_input_method_util.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   some util functions used by input method.
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
 * 2010-02-15 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_input_method_util.h"

Ret  ftk_im_candidate_info_parse(FtkCommitInfo* info, const char* matched, FtkCompare compare)
{
	int len = 0;
	int candidate_size = 0;
	const char* end   = matched;
	const char* start = matched;
	const char* line  = matched;
	
	info->candidate_nr = 0;
	info->candidates[0] = '\0';

	for(;line != NULL;)
	{
		if(compare(line, info->raw_text) != 0)
		{
			break;
		}

		start = strchr(line, ' ');
		if(start == NULL) break; else start++;

		end   = start;
		while(*end && *end != ' ' && *end != '\n') end++;
		
		len = end - start;
		if((candidate_size + len + 1) < FTK_IM_CANDIDATE_BUFF_LENGTH)
		{
			memcpy(info->candidates + candidate_size, start, len);
			info->candidates[candidate_size + len] ='\0';
			candidate_size += len + 1;
			info->candidate_nr++;	
		}
		else
		{
			break;
		}

		line = strchr(end, '\n');
		if(line != NULL) line++;

		if(info->candidate_nr > FTK_IM_MAX_CANDIDATES) break;
	}

	return RET_OK;
}

